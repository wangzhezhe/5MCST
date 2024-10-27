/// Custom VTK to VTK-m data set conversion. This is to handle fields with arbitrary
/// tuple sizes. This is fixed in VTK with MR !10763
/// (https://gitlab.kitware.com/vtk/vtk/-/merge_requests/10763).
/// Later versions of ParaView will not need this and can use the standard VTK features.

#include "VTKToVTKm.h"

#include <vtkAOSDataArrayTemplate.h>
#include <vtkCellData.h>
#include <vtkDataObjectTypes.h>
#include <vtkImageData.h>
#include <vtkLogger.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkRectilinearGrid.h>
#include <vtkSOADataArrayTemplate.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>

#include <vtkm/Swap.h>
#include <vtkm/cont/Algorithm.h>
#include <vtkm/cont/ArrayCopy.h>
#include <vtkm/cont/ArrayHandleCartesianProduct.h>
#include <vtkm/cont/ArrayHandleGroupVec.h>
#include <vtkm/cont/DataSetBuilderUniform.h>
#include <vtkm/worklet/WorkletMapField.h>

namespace
{

struct ReorderHex : vtkm::worklet::WorkletMapField
{
  using ControlSignature = void(FieldInOut);

  VTKM_EXEC void operator()(vtkm::Vec<vtkm::Id, 8>& indices) const
  {
    vtkm::Swap(indices[2], indices[3]);
    vtkm::Swap(indices[6], indices[7]);
  }
};

struct SupportedCellShape
{
  VTKM_EXEC_CONT
  bool operator()(vtkm::UInt8 shape) const
  {
    return (shape < vtkm::NUMBER_OF_CELL_SHAPES) && (shape != 2) && (shape != 6) && (shape != 8) &&
      (shape != 11);
  }
};

template <typename T>
vtkm::cont::ArrayHandleBasic<T> vtkAOSDataArrayToFlatArrayHandle(vtkAOSDataArrayTemplate<T>* input)
{
  // Register a reference to the input here to make sure the array cannot
  // be deleted before the `ArrayHandle` is done with it. (Note that you
  // will still get problems if the `vtkAOSDataArrayTemplate` gets resized.
  input->Register(nullptr);

  auto deleter = [](void* container)
  {
    vtkAOSDataArrayTemplate<T>* vtkArray = reinterpret_cast<vtkAOSDataArrayTemplate<T>*>(container);
    vtkArray->UnRegister(nullptr);
  };
  auto reallocator =
    [](void*& memory, void*& container, vtkm::BufferSizeType oldSize, vtkm::BufferSizeType newSize)
  {
    vtkAOSDataArrayTemplate<T>* vtkArray = reinterpret_cast<vtkAOSDataArrayTemplate<T>*>(container);
    if ((vtkArray->GetVoidPointer(0) != memory) || (vtkArray->GetNumberOfValues() != oldSize))
    {
      vtkLog(ERROR,
        "Dangerous inconsistency found between pointers for VTK and VTK-m. "
        "Was the VTK array resized outside of VTK-m?");
    }
    vtkArray->SetNumberOfValues(newSize);
    memory = vtkArray->GetVoidPointer(0);
  };

  return vtkm::cont::ArrayHandleBasic<T>(
    input->GetPointer(0), input, input->GetNumberOfValues(), deleter, reallocator);
}

template <typename T>
vtkm::cont::ArrayHandleBasic<T> vtkSOADataArrayToComponentArrayHandle(
  vtkSOADataArrayTemplate<T>* input, int componentIndex)
{
  // Register for each component (as each will have the deleter call to
  // unregister).
  input->Register(nullptr);

  using ContainerPair = std::pair<vtkSOADataArrayTemplate<T>*, int>;
  ContainerPair* componentInput = new ContainerPair(input, componentIndex);

  auto deleter = [](void* container)
  {
    ContainerPair* containerPair = reinterpret_cast<ContainerPair*>(container);
    containerPair->first->UnRegister(nullptr);
    delete containerPair;
  };
  auto reallocator = [](void*& memory, void*& container, vtkm::BufferSizeType vtkNotUsed(oldSize),
                       vtkm::BufferSizeType newSize)
  {
    ContainerPair* containerPair = reinterpret_cast<ContainerPair*>(container);
    containerPair->first->SetNumberOfTuples(newSize);
    memory = containerPair->first->GetComponentArrayPointer(containerPair->second);
  };

  return vtkm::cont::ArrayHandleBasic<T>(input->GetComponentArrayPointer(componentIndex),
    componentInput, input->GetNumberOfTuples(), deleter, reallocator);
}

template <typename T>
vtkm::cont::ArrayHandleRuntimeVec<T> vtkDataArrayToArrayHandle(vtkAOSDataArrayTemplate<T>* input)
{
  auto flatArray = vtkAOSDataArrayToFlatArrayHandle(input);
  return vtkm::cont::make_ArrayHandleRuntimeVec(input->GetNumberOfComponents(), flatArray);
}

template <typename T>
vtkm::cont::ArrayHandleRecombineVec<T> vtkDataArrayToArrayHandle(vtkSOADataArrayTemplate<T>* input)
{
  // Wrap each component array in a basic array handle, convert that to a
  // strided array, and then add that as a component to the returned
  // recombined vec.
  vtkm::cont::ArrayHandleRecombineVec<T> output;

  for (int componentIndex = 0; componentIndex < input->GetNumberOfComponents(); ++componentIndex)
  {
    auto componentArray = vtkSOADataArrayToComponentArrayHandle(input, componentIndex);
    output.AppendComponentArray(
      vtkm::cont::ArrayExtractComponent(componentArray, 0, vtkm::CopyFlag::Off));
  }

  return output;
}

struct BuildSingleTypeCellSetVisitor
{
  template <typename CellStateT>
  vtkm::cont::UnknownCellSet operator()(
    CellStateT& state, vtkm::UInt8 cellType, vtkm::IdComponent cellSize, vtkIdType numPoints)
  {
    using VTKIdT = typename CellStateT::ValueType; // might not be vtkIdType...
    using VTKArrayT = vtkAOSDataArrayTemplate<VTKIdT>;
    static constexpr bool IsVtkmIdType = std::is_same<VTKIdT, vtkm::Id>::value;

    // Construct an arrayhandle that holds the connectivity array
    auto connHandleDirect = vtkAOSDataArrayToFlatArrayHandle(state.GetConnectivity());

    // Cast if necessary:
    auto connHandle = IsVtkmIdType ? connHandleDirect
                                   : vtkm::cont::make_ArrayHandleCast<vtkm::Id>(connHandleDirect);

    using ConnHandleType = typename std::decay<decltype(connHandle)>::type;
    using ConnStorageTag = typename ConnHandleType::StorageTag;
    using CellSetType = vtkm::cont::CellSetSingleType<ConnStorageTag>;

    CellSetType cellSet;
    cellSet.Fill(static_cast<vtkm::Id>(numPoints), cellType, cellSize, connHandle);
    return cellSet;
  }
};

struct BuildSingleTypeVoxelCellSetVisitor
{
  template <typename CellStateT>
  vtkm::cont::UnknownCellSet operator()(CellStateT& state, vtkIdType numPoints)
  {
    vtkm::cont::ArrayHandle<vtkm::Id> connHandle;
    {
      auto* conn = state.GetConnectivity();
      const auto* origData = conn->GetPointer(0);
      const vtkm::Id numIds = conn->GetNumberOfValues();
      vtkm::cont::ArrayCopy(
        vtkm::cont::make_ArrayHandle(origData, numIds, vtkm::CopyFlag::Off), connHandle);

      // reorder cells from voxel->hex
      vtkm::cont::Invoker invoke;
      invoke(ReorderHex{}, vtkm::cont::make_ArrayHandleGroupVec<8>(connHandle));
    }

    using CellSetType = vtkm::cont::CellSetSingleType<>;

    CellSetType cellSet;
    cellSet.Fill(numPoints, vtkm::CELL_SHAPE_HEXAHEDRON, 8, connHandle);
    return cellSet;
  }
};

vtkm::cont::UnknownCellSet ConvertSingleType(
  vtkCellArray* cells, int cellType, vtkIdType numberOfPoints)
{
  switch (cellType)
  {
    case VTK_LINE:
      return cells->Visit(
        BuildSingleTypeCellSetVisitor{}, vtkm::CELL_SHAPE_LINE, 2, numberOfPoints);

    case VTK_HEXAHEDRON:
      return cells->Visit(
        BuildSingleTypeCellSetVisitor{}, vtkm::CELL_SHAPE_HEXAHEDRON, 8, numberOfPoints);

    case VTK_VOXEL:
      // Note that this is a special case that reorders ids voxel to hex:
      return cells->Visit(BuildSingleTypeVoxelCellSetVisitor{}, numberOfPoints);

    case VTK_QUAD:
      return cells->Visit(
        BuildSingleTypeCellSetVisitor{}, vtkm::CELL_SHAPE_QUAD, 4, numberOfPoints);

    case VTK_TETRA:
      return cells->Visit(
        BuildSingleTypeCellSetVisitor{}, vtkm::CELL_SHAPE_TETRA, 4, numberOfPoints);

    case VTK_TRIANGLE:
      return cells->Visit(
        BuildSingleTypeCellSetVisitor{}, vtkm::CELL_SHAPE_TRIANGLE, 3, numberOfPoints);

    case VTK_VERTEX:
      return cells->Visit(
        BuildSingleTypeCellSetVisitor{}, vtkm::CELL_SHAPE_VERTEX, 1, numberOfPoints);

    case VTK_WEDGE:
      return cells->Visit(
        BuildSingleTypeCellSetVisitor{}, vtkm::CELL_SHAPE_WEDGE, 6, numberOfPoints);

    case VTK_PYRAMID:
      return cells->Visit(
        BuildSingleTypeCellSetVisitor{}, vtkm::CELL_SHAPE_PYRAMID, 5, numberOfPoints);

    default:
      break;
  }

  throw vtkm::cont::ErrorBadType("Unsupported VTK cell type in "
                                 "CellSetSingleType converter.");
}

struct BuildExplicitCellSetVisitor
{
  template <typename CellStateT, typename S>
  vtkm::cont::UnknownCellSet operator()(CellStateT& state,
    const vtkm::cont::ArrayHandle<vtkm::UInt8, S>& shapes, vtkm::Id numPoints) const
  {
    using VTKIdT = typename CellStateT::ValueType; // might not be vtkIdType...
    using VTKArrayT = vtkAOSDataArrayTemplate<VTKIdT>;
    static constexpr bool IsVtkmIdType = std::is_same<VTKIdT, vtkm::Id>::value;

    // Construct arrayhandles to hold the arrays
    auto offsetsHandleDirect = vtkAOSDataArrayToFlatArrayHandle(state.GetOffsets());
    auto connHandleDirect = vtkAOSDataArrayToFlatArrayHandle(state.GetConnectivity());

    // Cast if necessary:
    auto connHandle = IsVtkmIdType ? connHandleDirect
                                   : vtkm::cont::make_ArrayHandleCast<vtkm::Id>(connHandleDirect);
    auto offsetsHandle = IsVtkmIdType
      ? offsetsHandleDirect
      : vtkm::cont::make_ArrayHandleCast<vtkm::Id>(offsetsHandleDirect);

    using ShapesStorageTag = typename std::decay<decltype(shapes)>::type::StorageTag;
    using ConnStorageTag = typename decltype(connHandle)::StorageTag;
    using OffsetsStorageTag = typename decltype(offsetsHandle)::StorageTag;
    using CellSetType =
      vtkm::cont::CellSetExplicit<ShapesStorageTag, ConnStorageTag, OffsetsStorageTag>;

    CellSetType cellSet;
    cellSet.Fill(numPoints, shapes, connHandle, offsetsHandle);
    return cellSet;
  }
};

// convert a cell array of mixed types to a vtkm CellSetExplicit
vtkm::cont::UnknownCellSet ConvertCells(
  vtkUnsignedCharArray* types, vtkCellArray* cells, vtkIdType numberOfPoints)
{
  auto shapes = vtkAOSDataArrayToFlatArrayHandle(types);
  if (!vtkm::cont::Algorithm::Reduce(
        vtkm::cont::make_ArrayHandleTransform(shapes, SupportedCellShape{}), true,
        vtkm::LogicalAnd()))
  {
    throw vtkm::cont::ErrorBadType("Unsupported VTK cell type in CellSet converter.");
  }

  return cells->Visit(BuildExplicitCellSetVisitor{}, shapes, numberOfPoints);
}

vtkm::cont::DataSet ConvertImpl(vtkUnstructuredGrid* input)
{
  // This will need to use the custom storage and portals so that
  // we can efficiently map between VTK and VTKm
  vtkm::cont::DataSet dataset;

  // first step convert the points over to an array handle
  vtkm::cont::CoordinateSystem coords = vtktovtkm::ConvertCoordinates(input->GetPoints());
  dataset.AddCoordinateSystem(coords);
  // last

  // Use our custom explicit cell set to do the conversion
  const vtkIdType numPoints = input->GetNumberOfPoints();
  if (input->IsHomogeneous())
  {
    int cellType = input->GetCellType(0); // get the celltype
    auto cells = ConvertSingleType(input->GetCells(), cellType, numPoints);
    dataset.SetCellSet(cells);
  }
  else
  {
    auto cells = ConvertCells(input->GetCellTypesArray(), input->GetCells(), numPoints);
    dataset.SetCellSet(cells);
  }

  vtktovtkm::ProcessFields(input, dataset);

  return dataset;
}
vtkm::cont::DataSet ConvertImpl(vtkStructuredGrid* input)
{
  const int dimensionality = input->GetDataDimension();
  int dims[3];
  input->GetDimensions(dims);

  vtkm::cont::DataSet dataset;

  // first step convert the points over to an array handle
  vtkm::cont::CoordinateSystem coords = vtktovtkm::ConvertCoordinates(input->GetPoints());
  dataset.AddCoordinateSystem(coords);

  // second step is to create structured cellset that represe
  if (dimensionality == 1)
  {
    vtkm::cont::CellSetStructured<1> cells;
    cells.SetPointDimensions(dims[0]);
    dataset.SetCellSet(cells);
  }
  else if (dimensionality == 2)
  {
    vtkm::cont::CellSetStructured<2> cells;
    cells.SetPointDimensions(vtkm::make_Vec(dims[0], dims[1]));
    dataset.SetCellSet(cells);
  }
  else
  { // going to presume 3d for everything else
    vtkm::cont::CellSetStructured<3> cells;
    cells.SetPointDimensions(vtkm::make_Vec(dims[0], dims[1], dims[2]));
    dataset.SetCellSet(cells);
  }

  vtktovtkm::ProcessFields(input, dataset);

  return dataset;
}

struct SetGlobalPointIndexStart
{
  template <vtkm::IdComponent Dim>
  void operator()(const vtkm::cont::CellSetStructured<Dim>&, const vtkm::Id3& structuredCoordsDims,
    const int extent[6], vtkm::cont::UnknownCellSet& dcs) const
  {
    typename vtkm::cont::CellSetStructured<Dim>::SchedulingRangeType extStart{};
    for (int i = 0, ii = 0; i < 3; ++i)
    {
      if (structuredCoordsDims[i] > 1)
      {
        vtkm::VecTraits<decltype(extStart)>::SetComponent(extStart, ii++, extent[2 * i]);
      }
    }

    vtkm::cont::CellSetStructured<Dim> cs;
    dcs.AsCellSet(cs);
    cs.SetGlobalPointIndexStart(extStart);
  }
};

vtkm::cont::DataSet ConvertImpl(vtkImageData* input)
{
  int extent[6];
  input->GetExtent(extent);
  double vorigin[3];
  input->GetOrigin(vorigin);
  double vspacing[3];
  input->GetSpacing(vspacing);
  int vdims[3];
  input->GetDimensions(vdims);

  vtkm::Vec<vtkm::FloatDefault, 3> origin(
    static_cast<vtkm::FloatDefault>((static_cast<double>(extent[0]) * vspacing[0]) + vorigin[0]),
    static_cast<vtkm::FloatDefault>((static_cast<double>(extent[2]) * vspacing[1]) + vorigin[1]),
    static_cast<vtkm::FloatDefault>((static_cast<double>(extent[4]) * vspacing[2]) + vorigin[2]));
  vtkm::Vec<vtkm::FloatDefault, 3> spacing(static_cast<vtkm::FloatDefault>(vspacing[0]),
    static_cast<vtkm::FloatDefault>(vspacing[1]), static_cast<vtkm::FloatDefault>(vspacing[2]));
  vtkm::Id3 dims(vdims[0], vdims[1], vdims[2]);

  vtkm::cont::DataSet dataset = vtkm::cont::DataSetBuilderUniform::Create(dims, origin, spacing);

  using ListCellSetStructured = vtkm::List<vtkm::cont::CellSetStructured<1>,
    vtkm::cont::CellSetStructured<2>, vtkm::cont::CellSetStructured<3>>;
  auto cellSet = dataset.GetCellSet().ResetCellSetList(ListCellSetStructured{});
  vtkm::cont::CastAndCall(cellSet, SetGlobalPointIndexStart{}, dims, extent, dataset.GetCellSet());

  vtktovtkm::ProcessFields(input, dataset);

  return dataset;
}

struct build_type_array
{
  template <typename CellStateT>
  void operator()(CellStateT& state, vtkUnsignedCharArray* types) const
  {
    const vtkIdType size = state.GetNumberOfCells();
    for (vtkIdType i = 0; i < size; ++i)
    {
      auto cellSize = state.GetCellSize(i);
      unsigned char cellType;
      switch (cellSize)
      {
        case 3:
          cellType = VTK_TRIANGLE;
          break;
        case 4:
          cellType = VTK_QUAD;
          break;
        default:
          cellType = VTK_POLYGON;
          break;
      }
      types->SetValue(i, cellType);
    }
  }
};

vtkm::cont::DataSet ConvertImpl(vtkPolyData* input)
{
  // the poly data is an interesting issue with the fact that the
  // vtk datastructure can contain multiple types.
  // we should look at querying the cell types, so we can use single cell
  // set where possible
  vtkm::cont::DataSet dataset;

  // Only set coordinates if they exists in the vtkPolyData
  if (input->GetPoints())
  {
    // first step convert the points over to an array handle
    vtkm::cont::CoordinateSystem coords = vtktovtkm::ConvertCoordinates(input->GetPoints());
    dataset.AddCoordinateSystem(coords);
  }

  // first check if we only have polys/lines/verts
  bool filled = false;
  const bool onlyPolys = (input->GetNumberOfCells() == input->GetNumberOfPolys());
  const bool onlyLines = (input->GetNumberOfCells() == input->GetNumberOfLines());
  const bool onlyVerts = (input->GetNumberOfCells() == input->GetNumberOfVerts());

  const vtkIdType numPoints = input->GetNumberOfPoints();
  if (onlyPolys)
  {
    vtkCellArray* cells = input->GetPolys();
    const vtkIdType homoSize = cells->IsHomogeneous();
    if (homoSize == 3)
    {
      // We are all triangles
      auto dcells = ConvertSingleType(cells, VTK_TRIANGLE, numPoints);
      dataset.SetCellSet(dcells);
      filled = true;
    }
    else if (homoSize == 4)
    {
      // We are all quads
      auto dcells = ConvertSingleType(cells, VTK_QUAD, numPoints);
      dataset.SetCellSet(dcells);
      filled = true;
    }
    else
    {
      // need to construct a vtkUnsignedCharArray* types mapping for our zoo data
      // we can do this by mapping number of points per cell to the type
      // 3 == tri, 4 == quad, else polygon
      vtkNew<vtkUnsignedCharArray> types;
      types->SetNumberOfComponents(1);
      types->SetNumberOfTuples(static_cast<vtkIdType>(cells->GetNumberOfCells()));

      cells->Visit(build_type_array{}, types.GetPointer());

      auto dcells = ConvertCells(types, cells, numPoints);
      dataset.SetCellSet(dcells);
      filled = true;
    }
  }
  else if (onlyLines)
  {
    vtkCellArray* cells = input->GetLines();
    const vtkIdType homoSize = cells->IsHomogeneous();
    if (homoSize == 2)
    {
      // We are all lines
      auto dcells = ConvertSingleType(cells, VTK_LINE, numPoints);
      dataset.SetCellSet(dcells);
      filled = true;
    }
    else
    {
      throw vtkm::cont::ErrorBadType("VTK-m does not currently support PolyLine cells.");
    }
  }
  else if (onlyVerts)
  {
    vtkCellArray* cells = input->GetVerts();
    const vtkIdType homoSize = cells->IsHomogeneous();
    if (homoSize == 1)
    {
      // We are all single vertex
      auto dcells = ConvertSingleType(cells, VTK_VERTEX, numPoints);
      dataset.SetCellSet(dcells);
      filled = true;
    }
    else
    {
      throw vtkm::cont::ErrorBadType("VTK-m does not currently support PolyVertex cells.");
    }
  }
  else
  {
    throw vtkm::cont::ErrorBadType(
      "VTK-m does not currently support mixed cell types or triangle strips in vtkPolyData.");
  }

  if (!filled)
  {
    // todo: we need to convert a mixed type which
  }

  vtktovtkm::ProcessFields(input, dataset);

  return dataset;
}

vtkm::cont::DataSet ConvertImpl(vtkRectilinearGrid* input)
{
  const int dimensionality = input->GetDataDimension();
  int dims[3];
  input->GetDimensions(dims);

  int extent[6];
  input->GetExtent(extent);

  vtkm::cont::DataSet dataset;

  // first step, convert the points x, y aqnd z arrays over
  dataset.AddCoordinateSystem(vtktovtkm::ConvertCoordinates(
    input->GetXCoordinates(), input->GetYCoordinates(), input->GetZCoordinates()));

  // second step is to create structured cellset that represe
  if (dimensionality == 1)
  {
    vtkm::cont::CellSetStructured<1> cells;
    if (dims[0] > 1)
    {
      cells.SetPointDimensions(dims[0]);
      cells.SetGlobalPointIndexStart(extent[0]);
    }
    else if (dims[1] > 1)
    {
      cells.SetPointDimensions(dims[1]);
      cells.SetGlobalPointIndexStart(extent[2]);
    }
    else
    {
      cells.SetPointDimensions(dims[2]);
      cells.SetGlobalPointIndexStart(extent[4]);
    }
    dataset.SetCellSet(cells);
  }
  else if (dimensionality == 2)
  {
    vtkm::cont::CellSetStructured<2> cells;
    if (dims[0] == 1)
    {
      cells.SetPointDimensions(vtkm::make_Vec(dims[1], dims[2]));
      cells.SetGlobalPointIndexStart(vtkm::make_Vec(extent[2], extent[4]));
    }
    else if (dims[1] == 1)
    {
      cells.SetPointDimensions(vtkm::make_Vec(dims[0], dims[2]));
      cells.SetGlobalPointIndexStart(vtkm::make_Vec(extent[0], extent[4]));
    }
    else
    {
      cells.SetPointDimensions(vtkm::make_Vec(dims[0], dims[1]));
      cells.SetGlobalPointIndexStart(vtkm::make_Vec(extent[0], extent[2]));
    }

    dataset.SetCellSet(cells);
  }
  else // going to presume 3d for everything else
  {
    vtkm::cont::CellSetStructured<3> cells;
    cells.SetPointDimensions(vtkm::make_Vec(dims[0], dims[1], dims[2]));
    cells.SetGlobalPointIndexStart(vtkm::make_Vec(extent[0], extent[2], extent[4]));
    dataset.SetCellSet(cells);
  }

  vtktovtkm::ProcessFields(input, dataset);

  return dataset;
}

} // anonymous namespace

namespace vtktovtkm
{

vtkm::cont::UnknownArrayHandle Convert(vtkDataArray* input)
{
  // Investigate using vtkArrayDispatch, AOS for all types, and than SOA for
  // just
  // float/double
  vtkm::cont::UnknownArrayHandle output;
  switch (input->GetDataType())
  {
    vtkTemplateMacro(
      vtkAOSDataArrayTemplate<VTK_TT>* typedIn1 =
        vtkAOSDataArrayTemplate<VTK_TT>::FastDownCast(input);
      if (typedIn1) { output = vtkDataArrayToArrayHandle(typedIn1); } else {
        vtkSOADataArrayTemplate<VTK_TT>* typedIn2 =
          vtkSOADataArrayTemplate<VTK_TT>::FastDownCast(input);
        if (typedIn2)
        {
          output = vtkDataArrayToArrayHandle(typedIn2);
        }
        else
        {
          vtkmDataArray<VTK_TT>* typedIn3 = vtkmDataArray<VTK_TT>::SafeDownCast(input);
          if (typedIn3)
          {
            output = typedIn3->GetVtkmUnknownArrayHandle();
          }
        }
      });
    // end vtkTemplateMacro
  }
  if (!output.IsValid())
  {
    throw vtkm::cont::ErrorBadType("Unsupported VTK array type.");
  }
  return output;
}

vtkm::cont::Field ConvertField(vtkDataArray* input, int association)
{
  switch (association)
  {
    case vtkDataObject::FIELD_ASSOCIATION_POINTS:
      return vtkm::cont::Field{ input->GetName(), vtkm::cont::Field::Association::Points,
        Convert(input) };
    case vtkDataObject::FIELD_ASSOCIATION_CELLS:
      return vtkm::cont::Field{ input->GetName(), vtkm::cont::Field::Association::Cells,
        Convert(input) };
    default:
      // Invalid field association. Ignore.
      return vtkm::cont::Field{};
  }
}

vtkm::cont::CoordinateSystem ConvertCoordinates(vtkPoints* input)
{
  auto vtkarray = input->GetData();
  return vtkm::cont::CoordinateSystem{ "coords", Convert(vtkarray) };
}

vtkm::cont::CoordinateSystem ConvertCoordinates(
  vtkDataArray* xArray, vtkDataArray* yArray, vtkDataArray* zArray)
{
  vtkm::cont::UnknownArrayHandle xUntyped = Convert(xArray);
  vtkm::cont::UnknownArrayHandle yUntyped = Convert(yArray);
  vtkm::cont::UnknownArrayHandle zUntyped = Convert(zArray);

  using ArrayType = vtkm::cont::ArrayHandle<vtkm::Float64>;
  if (!xUntyped.CanConvert<ArrayType>() || !yUntyped.CanConvert<ArrayType>() ||
    !zUntyped.CanConvert<ArrayType>())
  {
    throw vtkm::cont::ErrorBadType("Unexpected array type for rectilinear grid.");
  }

  auto product = vtkm::cont::make_ArrayHandleCartesianProduct(xUntyped.AsArrayHandle<ArrayType>(),
    yUntyped.AsArrayHandle<ArrayType>(), zUntyped.AsArrayHandle<ArrayType>());
  return vtkm::cont::CoordinateSystem{ "coord", product };
}

void ProcessFields(vtkDataSet* input, vtkm::cont::DataSet& dataset)
{
  vtkPointData* pointData = input->GetPointData();
  for (int i = 0; i < pointData->GetNumberOfArrays(); i++)
  {
    vtkDataArray* array = pointData->GetArray(i);
    if (array == nullptr)
    {
      continue;
    }

    vtkm::cont::Field pfield =
      vtktovtkm::ConvertField(array, vtkDataObject::FIELD_ASSOCIATION_POINTS);
    dataset.AddField(pfield);
  }

  vtkCellData* cellData = input->GetCellData();
  for (int i = 0; i < cellData->GetNumberOfArrays(); i++)
  {
    vtkDataArray* array = cellData->GetArray(i);
    if (array == nullptr)
    {
      continue;
    }

    vtkm::cont::Field pfield =
      vtktovtkm::ConvertField(array, vtkDataObject::FIELD_ASSOCIATION_CELLS);
    dataset.AddField(pfield);
  }
}

vtkm::cont::DataSet Convert(vtkDataSet* input)
{
  auto typeId = input->GetDataObjectType();
  switch (typeId)
  {
    case VTK_UNSTRUCTURED_GRID:
      return ConvertImpl(vtkUnstructuredGrid::SafeDownCast(input));
    case VTK_STRUCTURED_GRID:
      return ConvertImpl(vtkStructuredGrid::SafeDownCast(input));
    case VTK_UNIFORM_GRID:
    case VTK_IMAGE_DATA:
      return ConvertImpl(vtkImageData::SafeDownCast(input));
    case VTK_POLY_DATA:
      return ConvertImpl(vtkPolyData::SafeDownCast(input));
    case VTK_RECTILINEAR_GRID:
      return ConvertImpl(vtkRectilinearGrid::SafeDownCast(input));

    case VTK_UNSTRUCTURED_GRID_BASE:
    case VTK_STRUCTURED_POINTS:
    default:
      const std::string typeStr = vtkDataObjectTypes::GetClassNameFromTypeId(typeId);
      const std::string errMsg = "Unable to convert " + typeStr + " to vtkm::cont::DataSet";
      throw vtkm::cont::ErrorBadType(errMsg);
  }
}

} // namespace vtktovtkm

namespace vtkmtovtk
{

vtkDataArray* Convert(const vtkm::cont::UnknownArrayHandle& input, const std::string& name)
{
  vtkDataArray* data = nullptr;

  auto converter = [&](auto componentExample) {
    using ComponentType = std::decay_t<decltype(componentExample)>;

    if ((data != nullptr) || !input.IsBaseComponentType<ComponentType>()) { return; }

    // The current implementation of fromvtkm will copy data to a vtkAOSDataArrayTemplate if
    // possible. That is not great because it means we always copy to the CPU and loose the
    // GPU version of the data. The vtkmDataArray class should be updated to do all of this
    // lazily.
    using BasicArrayType = vtkm::cont::ArrayHandleRuntimeVec<ComponentType>;
    if (input.CanConvert<BasicArrayType>())
    {
      BasicArrayType concreteInput;
      input.AsArrayHandle(concreteInput);

      vtkm::cont::ArrayHandleBasic<ComponentType> componentsArray =
        concreteInput.GetComponentsArray();
      VTKM_ASSERT(componentsArray.GetBuffers().size() == 1);

      using VTKArrayType = vtkAOSDataArrayTemplate<ComponentType>;
      VTKArrayType* vtkArray = VTKArrayType::New();
      vtkArray->SetNumberOfComponents(concreteInput.GetNumberOfComponents());

      // BOOOOOO!!!!!!
      componentsArray.SyncControlArray();
      auto bufferInfo = componentsArray.GetBuffers()[0].GetHostBufferInfo();

      vtkm::cont::internal::TransferredBuffer transfer = bufferInfo.TransferOwnership();
      ComponentType* srcMemory = reinterpret_cast<ComponentType*>(transfer.Memory);
      vtkm::Id size = componentsArray.GetNumberOfValues();
      if (transfer.Memory == transfer.Container)
      { // transfer the memory ownership over to VTK instead of copy
        vtkArray->SetVoidArray(srcMemory, size, 0, vtkAbstractArray::VTK_DATA_ARRAY_USER_DEFINED);
        vtkArray->SetArrayFreeFunction(transfer.Delete);
      }
      else
      {
        // deep copy the memory to VTK as the memory coming from
        // a source that VTK can't represent
        ComponentType* dataBuffer = new ComponentType[size];
        std::copy(srcMemory, srcMemory + size, dataBuffer);

        vtkArray->SetVoidArray(dataBuffer, size, 0, vtkAbstractArray::VTK_DATA_ARRAY_DELETE);
        transfer.Delete(transfer.Container);
      }

      data = vtkArray;
    }
    else
    {
      //data = make_vtkmDataArray(input.ExtractArrayFromComponents<ComponentType>());
      throw vtkm::cont::ErrorBadType("VTK-m data array not supported to convert to VTK.");
    }
  };

  vtkm::ListForEach(converter, vtkm::TypeListScalarAll{});

  if (data)
  {
    data->SetName(name.c_str());
  }

  return data;
}

vtkDataArray* Convert(const vtkm::cont::Field& input)
{
  return Convert(input.GetData(), input.GetName());
}

} // namespace vtkmtovtk