//refer to 
//https://github.com/Kitware/VTK/blob/master/Common/DataModel/vtkOctreePointLocator.cxx

//datasets
//https://github.com/lorensen/VTKExamples/blob/master/src/Testing/Data/treemesh.vtk
//maybe use the slice to get a specific plane

//online example
//https://kitware.github.io/vtk-examples/site/Cxx/#octree

#include <vtkCellArray.h>
#include <vtkDataSetCollection.h>
#include <vtkIdList.h>
#include <vtkNew.h>
#include <vtkOctreePointLocator.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>

#include <vtkXMLUnstructuredGridReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkPolyDataWriter.h>

int main(int argc, char *argv[]){
    // Parse command line arguments
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0]
                  << " Filename" << std::endl;
        return EXIT_FAILURE;
    }

    std::string filename = argv[1];

    //load the unstructured data
    //vtkSmartPointer<vtkUnstructuredGridReader> reader =
    //    vtkSmartPointer<vtkUnstructuredGridReader>::New();
    
    //load the polydata
    vtkSmartPointer<vtkPolyDataReader> reader =
        vtkSmartPointer<vtkPolyDataReader>::New();
    
    reader->SetFileName(filename.c_str());
    reader->Update();
      
    // get the specific unstructureGridData and check the results
    //vtkUnstructuredGrid* grid = reader->GetOutput();
    vtkPolyData* grid = reader->GetOutput();

    grid->Print(std::cout);
    printf("---\n");
    //create the oct tree based on the tree mesh
    vtkNew<vtkOctreePointLocator> octree;
    octree->SetDataSet(grid);
    octree->BuildLocator();
    octree->Print(std::cout);

    //check the representation
    printf("---\n");
    int level = octree->GetLevel();
    printf("Octree level %d\n",level);

    for(int i=0;i<level;i++){
        vtkNew<vtkPolyData> polydata;
        octree->GenerateRepresentation(i,polydata);
        polydata->Print(std::cout);

        vtkSmartPointer<vtkPolyDataWriter> polywriter =
            vtkSmartPointer<vtkPolyDataWriter>::New();
        std::string filename = "./oct_poly"+std::to_string(i)+".vtk";
        polywriter->SetFileName(filename.c_str());

        // get the specific polydata and check the results
        // writer->SetInputData???
        polywriter->SetInputData(polydata);
        //polywriter->SetInputData(delaunay->GetOutputPort());

        // Optional - set the mode. The default is binary.
        //polywriter->SetDataModeToAscii();
        polywriter->Write();
    }

   
    
    return 0;
}