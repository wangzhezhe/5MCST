#include <vtkDataSetReader.h>
#include <vtkDataSet.h>
#include <vtkImageData.h>

//communicator marshal
#include <vtkCommunicator.h>
#include <vtkCharArray.h>

#include <vtkmlib/DataSetConverters.h>
#include <vtkm/cont/DataSet.h>

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

    //Converting the vtk data set to vtkm data set
    //Unable to convert vtkStructuredPoints to vtkm::cont::DataSet
    vtkm::cont::DataSet vtkmDS = tovtkm::Convert(inData);
    
    //TODO, maybe just extract each component and conver to vtkm 

    std::cout << "---vtkm data object is" << std::endl;
    vtkmDS.PrintSummary(std::cout);

    return 0;
}