//refer to https://vtk.org/Wiki/VTK/Examples/Cxx/IO/ReadPolyData
//refer to https://vtk.org/doc/nightly/html/classvtkDataReader.html
//refer to https://github.com/Kitware/VTK/tree/master/IO/Legacy

#include <vtkPolyDataReader.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

int main(int argc, char *argv[])
{
    // Parse command line arguments
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0]
                  << " Filename" << std::endl;
        return EXIT_FAILURE;
    }

    std::string filename = argv[1];

    // Read all the data from the file
    vtkSmartPointer<vtkPolyDataReader> reader =
        vtkSmartPointer<vtkPolyDataReader>::New();
    reader->SetFileName(filename.c_str());
    reader->Update();
    
    // get the specific polydata and check the results
    vtkPolyData *polydata = reader->GetOutput();
    polydata->Print(std::cout);

    return 0;
}