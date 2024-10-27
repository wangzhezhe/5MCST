

#include <vtkCharArray.h>
#include <vtkCommunicator.h>
#include <vtkImageImport.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataWriter.h>

void test_marshal_unmarshal_image()
{
    std::cout << "---test " << __FUNCTION__ << std::endl;

    std::vector<int> rawdata(10 * 10 * 10, 1);
    auto importer = vtkSmartPointer<vtkImageImport>::New();
    importer->SetDataSpacing(1, 1, 1);
    importer->SetDataOrigin(0, 0, 0);
    // from 0 to the shape -1 or from lb to the ub??
    importer->SetWholeExtent(0, 10, 0, 10, 0, 10);
    importer->SetDataExtentToWholeExtent();
    importer->SetDataScalarTypeToDouble();
    importer->SetNumberOfScalarComponents(1);
    importer->SetImportVoidPointer((int *)(rawdata.data()));
    importer->Update();

    vtkSmartPointer<vtkImageData> imageData = importer->GetOutput();
    
    std::cout << "---Original data object" << std::endl;
    imageData->PrintSelf(std::cout, vtkIndent(5));

    // marshal the vtk data into the string
    vtkSmartPointer<vtkCharArray> buffer = vtkSmartPointer<vtkCharArray>::New();
    bool oktoMarshal = vtkCommunicator::MarshalDataObject(imageData, buffer);

    if (oktoMarshal == false)
    {
        throw std::runtime_error("failed to marshal vtk data");
    }

    buffer->PrintSelf(std::cout, vtkIndent(5));

    // array is grouped into smaller subarrays with NumberOfComponents components.
    // These subarrays are called tuples, the array size is tuple size times the components size

    std::cout << "data type: " << buffer->GetDataType() << std::endl;

    // try to send the marshared vtkArray into the remote server
    // get the block summary, type, size, etc
    vtkIdType numTuples = buffer->GetNumberOfTuples();
    int numComponents = buffer->GetNumberOfComponents();

    size_t dataSize = numTuples * numComponents;

    std::cout << "numTuples " << numTuples << " numComponents " << numComponents << std::endl;

    // try to recv the data
    vtkSmartPointer<vtkCharArray> recvbuffer = vtkSmartPointer<vtkCharArray>::New();

    // set key properties, type, numTuples, numComponents, name,
    recvbuffer->SetNumberOfComponents(numComponents);
    recvbuffer->SetNumberOfTuples(numTuples);

    size_t recvSize = dataSize;

    // try to simulate the data recv process
    // when the memory of the vtkchar array is allocated???
    memcpy(recvbuffer->GetPointer(0), buffer->GetPointer(0), recvSize);

    // std::cout << "------check recvbuffer content: ------" << std::endl;
    // for (int i = 0; i < recvSize; i++)
    //{
    //  std::cout << recvbuffer->GetValue(i);
    //}
    // std::cout << "------" << std::endl;

    // unmarshal
    vtkSmartPointer<vtkDataObject> recvbj = vtkCommunicator::UnMarshalDataObject(recvbuffer);

    std::cout << "---Unmarshal object:" << std::endl;
    recvbj->PrintSelf(std::cout, vtkIndent(5));
}

void test_marshal_unmarshal_poly()
{
    std::cout << "---test " << __FUNCTION__ << std::endl;
    // create sphere source
    vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetThetaResolution(800);
    sphereSource->SetPhiResolution(800);
    sphereSource->SetStartTheta(0.0);
    sphereSource->SetEndTheta(360.0);
    sphereSource->SetStartPhi(0.0);
    sphereSource->SetEndPhi(180.0);
    sphereSource->LatLongTessellationOff();

    sphereSource->Update();
    vtkSmartPointer<vtkPolyData> polyData = sphereSource->GetOutput();

    std::cout << "---Original poly data object" << std::endl;
    polyData->PrintSelf(std::cout, vtkIndent(5));

    // marshal the vtk data into the string
    vtkSmartPointer<vtkCharArray> buffer = vtkSmartPointer<vtkCharArray>::New();
    bool oktoMarshal = vtkCommunicator::MarshalDataObject(polyData, buffer);

    if (oktoMarshal == false)
    {
        throw std::runtime_error("failed to marshal vtk data");
    }

    buffer->PrintSelf(std::cout, vtkIndent(5));

    // array is grouped into smaller subarrays with NumberOfComponents components.
    // These subarrays are called tuples, the array size is tuple size times the components size

    std::cout << "data type: " << buffer->GetDataType() << std::endl;

    // try to send the marshared vtkArray into the remote server
    // get the block summary, type, size, etc
    vtkIdType numTuples = buffer->GetNumberOfTuples();
    int numComponents = buffer->GetNumberOfComponents();

    size_t dataSize = numTuples * numComponents;

    std::cout << "numTuples " << numTuples << " numComponents " << numComponents << std::endl;

    // try to recv the data
    vtkSmartPointer<vtkCharArray> recvbuffer = vtkSmartPointer<vtkCharArray>::New();

    // set key properties, type, numTuples, numComponents, name,
    recvbuffer->SetNumberOfComponents(numComponents);
    recvbuffer->SetNumberOfTuples(numTuples);

    size_t recvSize = dataSize;

    // try to simulate the data recv process
    // when the memory of the vtkchar array is allocated???
    memcpy(recvbuffer->GetPointer(0), buffer->GetPointer(0), recvSize);

    // std::cout << "------check recvbuffer content: ------" << std::endl;
    // for (int i = 0; i < recvSize; i++)
    //{
    //  std::cout << recvbuffer->GetValue(i);
    //}
    // std::cout << "------" << std::endl;

    // unmarshal
    vtkSmartPointer<vtkDataObject> recvbj = vtkCommunicator::UnMarshalDataObject(recvbuffer);

    std::cout << "---unmarshal object:" << std::endl;
    recvbj->PrintSelf(std::cout, vtkIndent(5));
    
}

int main()
{
    test_marshal_unmarshal_image();
    test_marshal_unmarshal_poly();
}