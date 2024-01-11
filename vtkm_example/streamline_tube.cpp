#include <vtkm/cont/Initialize.h>
#include <vtkm/io/VTKDataSetReader.h>
#include <vtkm/io/VTKDataSetWriter.h>
#include <vtkm/filter/geometry_refinement/Tube.h>
#include <vtkm/filter/flow/WarpXStreamline.h>
#include <vtkm/worklet/WorkletMapField.h>

#include <vtkm/rendering/Actor.h>
#include <vtkm/rendering/CanvasRayTracer.h>
#include <vtkm/rendering/MapperPoint.h>
#include <vtkm/rendering/MapperQuad.h>
#include <vtkm/rendering/MapperRayTracer.h>
#include <vtkm/rendering/MapperConnectivity.h>
#include <vtkm/rendering/MapperWireframer.h>
#include <vtkm/rendering/MapperVolume.h>
#include <vtkm/rendering/MapperGlyphScalar.h>


#include <vtkm/rendering/View3D.h>

#include <iostream>
#include <string>

template<typename T>
class MemSetWorklet : public vtkm::worklet::WorkletMapField
{
protected:
  T Value;
public:
  VTKM_CONT
  MemSetWorklet(const T value)
    : Value(value)
  {
  }

  typedef void ControlSignature(FieldOut);
  typedef void ExecutionSignature(_1);

  VTKM_EXEC
  void operator()(T &value) const
  {
    value = Value;
  }
}; //class MemSetWorklet

template<typename T>
void MemSet(vtkm::cont::ArrayHandle<T> &array, const T value, const vtkm::Id num_values)
{
  array.Allocate(num_values);
  vtkm::worklet::DispatcherMapField<MemSetWorklet<T>>(MemSetWorklet<T>(value))
    .Invoke(array);
}

void GenerateChargedParticles(const vtkm::cont::ArrayHandle<vtkm::Vec3f> &pos,
                              const vtkm::cont::ArrayHandle<vtkm::Vec3f> &mom,
                              const vtkm::cont::ArrayHandle<vtkm::Float64> &mass,
                              const vtkm::cont::ArrayHandle<vtkm::Float64> &charge,
                              const vtkm::cont::ArrayHandle<vtkm::Float64> &weight,
                              vtkm::cont::ArrayHandle<vtkm::ChargedParticle> &seeds)
{
    auto pPortal = pos.ReadPortal();
    auto uPortal = mom.ReadPortal();
    auto mPortal = mass.ReadPortal();
    auto qPortal = charge.ReadPortal();
    auto wPortal = weight.ReadPortal();

    auto numValues = pos.GetNumberOfValues();

    seeds.Allocate(numValues);
    auto sPortal = seeds.WritePortal();

    for (vtkm::Id i = 0; i < numValues; i++)
    {
        vtkm::ChargedParticle electron(
            pPortal.Get(i), i, mPortal.Get(i), qPortal.Get(i), wPortal.Get(i), uPortal.Get(i));
        sPortal.Set(i, electron);
    }
}

int main(int argc, char *argv[])
{
    vtkm::cont::InitializeResult initResult = vtkm::cont::Initialize(
        argc, argv, vtkm::cont::InitializeOptions::DefaultAnyDevice);

    if (argc != 3)
    {
        std::cout << "<executable> <particle file path> <field file path>" << std::endl;
        exit(0);
    }

    std::string particleFile = argv[1];
    std::string fieldFile = argv[2];

    using SeedsType = vtkm::cont::ArrayHandle<vtkm::ChargedParticle>;

    SeedsType seeds;
    vtkm::io::VTKDataSetReader seedsReader(particleFile);
    vtkm::cont::DataSet seedsData = seedsReader.ReadDataSet();
    vtkm::cont::ArrayHandle<vtkm::Vec3f> pos, mom;
    vtkm::cont::ArrayHandle<vtkm::Float64> mass, charge, w;

    seedsData.GetCoordinateSystem().GetDataAsDefaultFloat().AsArrayHandle(pos);
    seedsData.GetField("Momentum").GetDataAsDefaultFloat().AsArrayHandle(mom);
    seedsData.GetField("Mass").GetData().AsArrayHandle(mass);
    seedsData.GetField("Charge").GetData().AsArrayHandle(charge);
    seedsData.GetField("Weighting").GetData().AsArrayHandle(w);

    GenerateChargedParticles(pos, mom, mass, charge, w, seeds);

    vtkm::io::VTKDataSetReader dataReader(fieldFile);
    vtkm::cont::DataSet dataset = dataReader.ReadDataSet();
    vtkm::cont::UnknownCellSet cells = dataset.GetCellSet();
    vtkm::cont::CoordinateSystem coords = dataset.GetCoordinateSystem();

    auto bounds = coords.GetBounds();
    std::cout << "Bounds : " << bounds << std::endl;
    using Structured3DType = vtkm::cont::CellSetStructured<3>;
    Structured3DType castedCells;
    cells.AsCellSet(castedCells);
    auto dims = castedCells.GetSchedulingRange(vtkm::TopologyElementTagPoint());
    vtkm::Vec3f spacing = {static_cast<vtkm::FloatDefault>(bounds.X.Length()) / (dims[0] - 1),
                           static_cast<vtkm::FloatDefault>(bounds.Y.Length()) / (dims[1] - 1),
                           static_cast<vtkm::FloatDefault>(bounds.Z.Length()) / (dims[2] - 1)};
    std::cout << spacing << std::endl;
    constexpr static vtkm::FloatDefault SPEED_OF_LIGHT =
        static_cast<vtkm::FloatDefault>(2.99792458e8);
    spacing = spacing * spacing;

    vtkm::Id steps = 50;
    vtkm::FloatDefault length = static_cast<vtkm::FloatDefault>(
        1.0 / (SPEED_OF_LIGHT * vtkm::Sqrt(1. / spacing[0] + 1. / spacing[1] + 1. / spacing[2])));
    std::cout << "CFL length : " << length << std::endl;

    vtkm::filter::flow::WarpXStreamline streamline;
    streamline.SetStepSize(length);
    streamline.SetNumberOfSteps(steps);
    streamline.SetSeeds(seeds);
    streamline.SetEField("E");
    streamline.SetBField("B");

    auto streamLineOutput = streamline.Execute(dataset);

    // output to vtk data
    vtkm::io::VTKDataSetWriter writer("test_warpx_streamline_output.vtk");
    writer.WriteDataSet(streamLineOutput);

    // adding tube results
    vtkm::filter::geometry_refinement::Tube tubeFilter;
    tubeFilter.SetCapping(false);
    tubeFilter.SetNumberOfSides(3);
    tubeFilter.SetRadius(0.00000003);

    auto tubeOut = tubeFilter.Execute(streamLineOutput);
    vtkm::io::VTKDataSetWriter writerTube("test_warpx_tube_output.vtk");
    writerTube.WriteDataSet(tubeOut);

    std::cout << "debug tubeOut dataset" << std::endl;
    tubeOut.PrintSummary(std::cout);

    //add constant point field
    vtkm::Id num_points = tubeOut.GetCoordinateSystem().GetData().GetNumberOfValues();
    vtkm::cont::ArrayHandle<vtkm::FloatDefault> array;
    MemSet(array, 1.0, num_points);
    vtkm::cont::Field field("constantField", vtkm::cont::Field::Association::Points, array);
    tubeOut.AddField(field);

    std::cout << "debug tubeOut dataset after adding constant" << std::endl;
    tubeOut.PrintSummary(std::cout);

    vtkm::io::VTKDataSetWriter writerTubeConstant("test_warpx_tube_constant_output.vtk");
    writerTubeConstant.WriteDataSet(tubeOut);
    
    std::cout << "debug constantField" << std::endl;
    tubeOut.GetField("constantField").PrintSummary(std::cout);

    // test rendering
    vtkm::rendering::Camera camera2;
    camera2.ResetToBounds(tubeOut.GetCoordinateSystem().GetBounds());
    camera2.Azimuth(90.f);
    vtkm::cont::ColorTable colorTable("inferno");

    vtkm::rendering::Color bg(0.2f, 0.2f, 0.2f, 1.0f);
    vtkm::rendering::Actor actor(
        tubeOut.GetCellSet(),
        tubeOut.GetCoordinateSystem(),
        tubeOut.GetField("constantField"));
        //colorTable);
    vtkm::rendering::Scene scene;
    scene.AddActor(actor);
    vtkm::rendering::CanvasRayTracer canvas(1024, 512);

    //vtkm::rendering::MapperPoint mapper;
    
    //mapper.SetRadius(5e-7f);
    //vtkm::rendering::MapperConnectivity mapper;
    
    //vtkm::rendering::MapperWireframer mapper; //this output could get the right output
    vtkm::rendering::MapperGlyphScalar mapper; //this output could get the right output
    
    //mapper.SetRadius(5e-7f);
    //vtkm::rendering::MapperVolume mapper; //this output could get the right output
    vtkm::rendering::View3D view(scene, mapper, canvas, camera2, bg);
    //vtkm::rendering::View3D view(scene, mapper, canvas);
    //view.Initialize();
    view.Paint();
    view.SaveAs("vtkm_warpx.png");
}