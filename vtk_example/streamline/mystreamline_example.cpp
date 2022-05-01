#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include "./myvtkStreamTracer.h"
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkPolyDataWriter.h>
#include <vtkUnstructuredGridWriter.h>

#include <vtkPolyData.h>

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cerr << "Required arguments: xyzFile qFile e.g. combxyz.bin combq.bin"
              << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkNamedColors> namedColors;

  std::string xyzFile = argv[1]; // "combxyz.bin";
  std::string qFile = argv[2];   // "combq.bin";

  vtkNew<vtkMultiBlockPLOT3DReader> pl3d;
  pl3d->SetXYZFileName(xyzFile.c_str());
  pl3d->SetQFileName(qFile.c_str());
  pl3d->SetScalarFunctionNumber(100);
  pl3d->SetVectorFunctionNumber(202);
  pl3d->Update();

  // Source of the streamlines
  // create the grid from the plane source
  // each location represent one point on the plane
  // there is 4*4 cells for this setting
  // when there is 4 segments at each direaction
  // there is 5 points for each direaction
  // the total number of points are 5*5=25
  vtkNew<vtkPlaneSource> seeds;
  seeds->SetXResolution(4);
  seeds->SetYResolution(4);
  seeds->SetOrigin(2, -2, 26);
  seeds->SetPoint1(2, 2, 26);
  seeds->SetPoint2(2, -2, 32);

  // Streamline itself
  vtkNew<myvtkStreamTracer> streamLine;
  streamLine->SetInputData(pl3d->GetOutput()->GetBlock(0));
  streamLine->SetSourceConnection(seeds->GetOutputPort());
  streamLine->SetMaximumPropagation(1000);
  streamLine->SetInitialIntegrationStep(0.01);
  streamLine->SetIntegrationDirectionToBoth();
  
  // Print into

  streamLine->PrintSelf(std::cout, vtkIndent(0));

  // output the data to vtk file
  vtkSmartPointer<vtkPolyDataWriter> polyWriter =
        vtkSmartPointer<vtkPolyDataWriter>::New();
  
  polyWriter->SetFileName("./streamline_example.vtk");
  polyWriter->SetInputConnection(streamLine->GetOutputPort());
  polyWriter->Update();
  
  return EXIT_SUCCESS;
}