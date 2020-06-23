//refer to https://vtk.org/Wiki/VTK/Examples/Cxx/IO/ReadPolyData
//refer to https://vtk.org/doc/nightly/html/classvtkDataReader.html
//refer to https://github.com/Kitware/VTK/tree/master/IO/Legacy

//if xml, using xml https://public.kitware.com/pipermail/vtkusers/2017-April/098521.html
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkUnstructuredGrid.h>

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
    vtkSmartPointer<vtkXMLUnstructuredGridReader> reader =
        vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
    reader->SetFileName(filename.c_str());
    reader->Update();
    
    // get the specific unstructureGridData and check the results
    vtkUnstructuredGrid *unstructureGridData = reader->GetOutput();
    unstructureGridData->Print(std::cout);

    //get the z dim 

    double bounds[6];
    unstructureGridData->GetBounds(bounds);
    for(int i=0;i<6;i++){
        std::cout << "index " << i << "value " << bounds[i] << std::endl;
    }

    std::cout <<"set cells" <<std::endl;
    vtkCellArray *cells=nullptr;
    int *types=nullptr;
    unstructureGridData->SetCells(types,cells);
    std::cout <<"ok to set cells" <<std::endl;
    unstructureGridData->Print(std::cout);

    //unstructureGridData->GetBounds(bounds);
    //for(int i=0;i<6;i++){
    //    std::cout << "index " << i << "value " << bounds[i] << std::endl;
    //}


    return 0;
}