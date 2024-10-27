#include <vtkm/cont/Initialize.h>
#include <vtkm/cont/DataSetBuilderUniform.h>
#include <vtkm/cont/Serialization.h>
#include <vtkm/thirdparty/diy/diy.h>
#include <vtkm/cont/DIYMemoryManagement.h>
#include <vtkm/cont/EnvironmentTracker.h>

using CellSetTypes = vtkm::List<vtkm::cont::CellSetExplicit<>,
                                vtkm::cont::CellSetSingleType<>,
                                vtkm::cont::CellSetStructured<1>,
                                vtkm::cont::CellSetStructured<2>,
                                vtkm::cont::CellSetStructured<3>>;

using DataSetWrapper = vtkm::cont::DataSetWithCellSetTypes<CellSetTypes>;

vtkm::cont::DataSet createDataSet()
{
    vtkm::cont::DataSetBuilderUniform dsb;
    constexpr vtkm::Id2 dimensions(3, 2);
    vtkm::cont::DataSet dataSet = dsb.Create(dimensions);

    constexpr vtkm::Id nVerts = 6;
    constexpr vtkm::Float32 var[nVerts] = {10.1f, 20.1f, 30.1f, 40.1f, 50.1f, 60.1f};

    dataSet.AddPointField("pointvar", var, nVerts);

    constexpr vtkm::Float32 cellvar[2] = {100.1f, 200.1f};
    dataSet.AddCellField("cellvar", cellvar, 2);

    return dataSet;
}

using CellSetTypes = vtkm::List<vtkm::cont::CellSetExplicit<>,
                                vtkm::cont::CellSetSingleType<>,
                                vtkm::cont::CellSetStructured<1>,
                                vtkm::cont::CellSetStructured<2>,
                                vtkm::cont::CellSetStructured<3>>;

int main(int argc, char *argv[])
{
    vtkm::cont::InitializeResult initResult = vtkm::cont::Initialize(
        argc, argv, vtkm::cont::InitializeOptions::DefaultAnyDevice);

    // create a data set
    vtkm::cont::DataSet inData = createDataSet();

    // serilizae data set
    std::cout << "---Original dataset before serilization" << std::endl;
    inData.PrintSummary(std::cout);

    std::cout << "---Testing data serilization" << std::endl;

    // mangled_diy_namespace::MemoryBuffer bb;
    mangled_diy_namespace::MemoryBuffer bb;
    mangled_diy_namespace::Serialization<vtkm::cont::DataSet> serialization;

    vtkmdiy::save(bb, inData);

    // transfer data to other programs

    vtkm::cont::DataSet outData;

    vtkmdiy::load(bb, outData);
    // checking the deserilized data
    outData.PrintSummary(std::cout);
    return 0;
}
