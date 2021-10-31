#include <vtkImageData.h>
#include <vtkMarchingCubes.h>
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
  vtkPolyData *polySurface = surface->GetOutput();
  std::cout << "------isosurface------" << std::endl;
  polySurface->Print(std::cout);

  // Write the file by vtkXMLDataSetWriter
  vtkSmartPointer<vtkXMLPolyDataWriter> writer =
      vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName("./iso_wavelet.vtp");

  // get the specific polydata and check the results
  writer->SetInputData(polySurface);
  writer->Write();

  return 0;
}