#include <vtkDataSetReader.h>
#include <vtkDataSet.h>

// communicator marshal
#include <vtkCommunicator.h>
#include <vtkCharArray.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkStructuredPoints.h>

// vtkm
#include <vtkm/cont/DataSet.h>
#include <vtkm/cont/DataSetBuilderUniform.h>
#include <vtkm/cont/ArrayHandle.h>

// convert vtk to vtkm
vtkm::cont::DataSet ConvertToVtkmDS(vtkStructuredPoints *outDataSet, std::string filedName)
{
    std::cout << "--start ConvertToVtkmDS" << std::endl;
    int dimensions[3];
    outDataSet->GetDimensions(dimensions);
    std::cout << "get dims " << dimensions[0] << " " << dimensions[1] << " " << dimensions[2] << std::endl;
    double spacijk[3];
    outDataSet->GetSpacing(spacijk);
    // get space and bounds
    std::cout << "get spaces " << spacijk[0] << " " << spacijk[1] << " " << spacijk[2] << std::endl;
    double origin[3];
    outDataSet->GetOrigin(origin);
    std::cout << "get origin " << origin[0] << " " << origin[1] << " " << origin[2] << std::endl;

    int arrayNum = outDataSet->GetPointData()->GetNumberOfArrays();
    std::cout << "get number of point array " << arrayNum << std::endl;
    vtkDataArray *dataArray = outDataSet->GetPointData()->GetArray(filedName.c_str());
    dataArray->Print(std::cout);

    // Number of component is 3
    // Number Of Tuples: 2097152

    vtkIdType numTuples = dataArray->GetNumberOfTuples();

    vtkm::cont::DataSetBuilderUniform dsb;
    vtkm::Id3 vtkmDimensions(dimensions[0], dimensions[1], dimensions[2]);
    vtkm::Vec<double, 3> vtkmSpacing(spacijk[0], spacijk[1], spacijk[2]);
    vtkm::Vec<double, 3> vtkmOrigin(origin[0], origin[1], origin[2]);

    vtkm::cont::DataSet ds = dsb.Create(vtkmDimensions, vtkmOrigin, vtkmSpacing);

    // adding vector field into vtkm data set from vtkDoubleArray
    vtkm::cont::ArrayHandle<vtkm::Vec3f> vecField;
    vecField.Allocate(numTuples);
    auto wPortal = vecField.WritePortal();

    std::array<double, 3> tuple;
    for (vtkIdType t = 0; t < numTuples; ++t)
    {
        dataArray->GetTuple(t, tuple.data());
        //if(t<10){
        //std::cout << tuple[0]<<" " <<tuple[1] << " " << tuple[2] << std::endl;
        //}
        //add to array handle
        wPortal.Set(vtkm::Id(t),vtkm::Vec<double, 3>(tuple[0],tuple[1],tuple[2]));
    }
    ds.AddPointField("velocity",vecField);
    return ds;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "<binary> <file path>" << std::endl;
        exit(0);
    }
    std::string filePath = argv[1];
    // load the vtk from local file, such as image
    vtkSmartPointer<vtkDataSetReader> reader =
        vtkSmartPointer<vtkDataSetReader>::New();
    reader->SetFileName(filePath.c_str());
    reader->Update();

    vtkDataSet *inData = reader->GetOutput();
    std::cout << "debug vtk inData" << std::endl;
    inData->Print(std::cout);

    // marshal the vtk object based on communicator marshal (this requires ParallelMPI)
    vtkSmartPointer<vtkCharArray> buffer = vtkSmartPointer<vtkCharArray>::New();
    bool oktoMarshal = vtkCommunicator::MarshalDataObject(inData, buffer);

    if (oktoMarshal == false)
    {
        throw std::runtime_error("failed to marshal vtk data");
    }

    std::cout << "---Marshal buffer info" << std::endl;
    buffer->PrintSelf(std::cout, vtkIndent(5));

    // assuming transfering the object through network
    // try to recv the data
    vtkSmartPointer<vtkCharArray> recvbuffer = vtkSmartPointer<vtkCharArray>::New();

    vtkIdType numTuples = buffer->GetNumberOfTuples();
    int numComponents = buffer->GetNumberOfComponents();

    size_t dataSize = numTuples * numComponents;

    // set key properties, type, numTuples, numComponents, name,
    recvbuffer->SetNumberOfComponents(numComponents);
    recvbuffer->SetNumberOfTuples(numTuples);

    size_t recvSize = dataSize;

    // try to simulate the data recv process
    // when the memory of the vtkchar array is allocated???
    memcpy(recvbuffer->GetPointer(0), buffer->GetPointer(0), recvSize);

    // unmarshal to vtk Data Object
    vtkSmartPointer<vtkDataObject> recvbj = vtkCommunicator::UnMarshalDataObject(recvbuffer);

    // Converting the vtk object into vtkm object using the SafeDownCast
    // Assuming we know the dedicated results
    vtkStructuredPoints *outDataSet = vtkStructuredPoints::SafeDownCast(recvbj);

    std::cout << "\n---Unmarshal object:" << std::endl;
    outDataSet->PrintSelf(std::cout, vtkIndent(5));

    // TODO, construct vtkm data manually

    // //Converting the vtk data set to vtkm data set
    // vtkm::cont::DataSet vtkmInDs = vtktovtkm::Convert(outDataSet);

    // std::cout << "---vtkm data object is" << std::endl;
    // vtkmInDs.PrintSummary(std::cout);
    vtkm::cont::DataSet vtkmInDs = ConvertToVtkmDS(outDataSet, "mesh_mesh/velocity");
    vtkmInDs.PrintSummary(std::cout);

    return 0;
}