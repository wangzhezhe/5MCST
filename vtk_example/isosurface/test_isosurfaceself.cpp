#include <vtkImageData.h>
#include "vtkMarchingCubesSelf.h"
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLImageDataReader.h>
// refer to this to get data from the vtk source
// https://kitware.github.io/vtk-examples/site/Cxx/Modelling/MarchingCubes/
int main(int argc, char *argv[]) {
  // Parse command line arguments
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " Filename" << std::endl;
    return EXIT_FAILURE;
  }

  std::string filename = argv[1];

  // Read the image data from the file
  vtkSmartPointer<vtkXMLImageDataReader> reader =
      vtkSmartPointer<vtkXMLImageDataReader>::New();
  reader->SetFileName(filename.c_str());
  reader->Update();

  // get the waveletdata and check the results
  vtkImageData *waveletdata = reader->GetOutput();
  waveletdata->Print(std::cout);

  // marching cube filter
  vtkNew<vtkMarchingCubes> surface;

  surface->SetInputData(waveletdata);
  surface->ComputeNormalsOn();
  surface->SetValue(0, 157);
  surface->SetValue(1, 160);

  // get output data
  surface->Update();
  //vtkPolyData *polySurface = surface->GetOutput();
  //std::cout << "------isosurface------" << std::endl;
  //polySurface->Print(std::cout);


  return 0;
}