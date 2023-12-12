

#include <vtkm/io/VTKDataSetWriter.h>
#include <vtkm/io/VTKDataSetReader.h>
#include <vtkm/cont/DataSetBuilderUniform.h>
#include <vtkm/worklet/DispatcherReduceByKey.h>
#include <vtkm/worklet/DispatcherMapTopology.h>
#include <vtkm/cont/ArrayHandleSOA.h>
#include <vtkm/cont/ArrayCopy.h>
#include <vtkm/cont/Initialize.h>

#include <vtkm/cont/ArrayHandleRuntimeVec.h>
#include <vtkm/cont/ArrayHandleGroupVec.h>
#include <vtkm/worklet/WorkletMapField.h>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

struct CheckEnsemblesWorklet : vtkm::worklet::WorkletMapField
{
    using ControlSignature = void(FieldIn sourceArray);
    using ExecutionSignature = void(WorkIndex, _1);

    template <typename InVecType>
    VTKM_EXEC void operator()(vtkm::Id index, const InVecType &inVec) const
    {
        printf("point index %d, total component: %d\n", index, inVec.GetNumberOfComponents());
        for (vtkm::IdComponent comp = 0; comp < inVec.GetNumberOfComponents(); ++comp)
        {
            printf("check ensemble value %lf\n", static_cast<vtkm::FloatDefault>(inVec[comp]));
        }
        // Do some other operations based on the ensmeble data
    }
};

int main(int argc, char *argv[])
{
    vtkm::cont::InitializeResult initResult = vtkm::cont::Initialize(
        argc, argv, vtkm::cont::InitializeOptions::DefaultAnyDevice);

    if (argc != 3)
    {
        std::cout << "<executable> <num of ensembles> <size of dim>" << std::endl;
        exit(0);
    }

    int numEnsembles = std::stoi(argv[1]);
    int sizeDim = std::stoi(argv[2]);
    int numPoints = sizeDim * sizeDim;

    // assuming there are numPoints
    // the field assocaited with each point contains numEnsmebles values

    // init runtime vec
    vtkm::cont::ArrayHandleRuntimeVec<vtkm::FloatDefault> runtimeVecArray(numEnsembles);
    runtimeVecArray.Allocate(numPoints);

    // go through each element
    auto portal = runtimeVecArray.WritePortal();

    // the out layer can be wrapped into a worklet if it is possible
    for (int i = 0; i < numPoints; i++)
    {
        // for each ensemble value
        auto value = portal.Get(i);
        for (int j = 0; j < numEnsembles; j++)
        {
            // Setting value to each element in the vector, this operation is similar to common array handle
            // the actual value is loaded from vtk file, we set some dummy values here for testing
            value[j] = j * 0.1f + i * 0.01f;
        }
    }
    // Attaching runtimeVecArray to a data set
    const vtkm::Id2 dims(sizeDim, sizeDim);
    vtkm::cont::DataSetBuilderUniform dataSetBuilder;
    vtkm::cont::DataSet vtkmDataSet = dataSetBuilder.Create(dims);
    vtkmDataSet.AddPointField("ensembles", runtimeVecArray);
    vtkmDataSet.PrintSummary(std::cout);

    // Do some operations ...

    // Calling a worklet on the runtimeVecArray
    using ComponentType = vtkm::FloatDefault;
    vtkm::cont::Invoker invoke;
    invoke(CheckEnsemblesWorklet{},
           vtkmDataSet.GetField("ensembles").GetData().ExtractArrayFromComponents<ComponentType>());

    // Using CastAndCallWithExtractedArray (get RecombineVec as the return results) 
    // and make_ArrayHandleView
    std::cout << "---Try CastAndCallWithExtractedArray " << std::endl;
    auto resolveType = [&](auto &concreteArray)
    {
        printSummary_ArrayHandle(concreteArray, std::cout);
        auto viewOut = vtkm::cont::make_ArrayHandleView(concreteArray, 0, numPoints);
        auto portal = viewOut.ReadPortal();
        for (int i = 0; i < numPoints; i++)
        {
            std::cout << "point index " << i << std::endl;
            // for each ensemble value
            auto vecValue = portal.Get(i);
            for (int j = 0; j < numEnsembles; j++)
            {
                std::cout << vecValue[j] << " ";
            }
            std::cout << std::endl;
        }
    };
    vtkmDataSet.GetField("ensembles")
        .GetData()
        .CastAndCallWithExtractedArray(resolveType);

    return 0;
}