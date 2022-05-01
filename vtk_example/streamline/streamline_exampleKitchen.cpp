//the code comes from
//https://kitware.github.io/vtk-examples/site/Cxx/Visualization/Kitchen/
//we do not use the rending and actors in this example

#include <vtkLineSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkStreamTracer.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridReader.h>
#include <vtkPolyDataWriter.h>
#include <array>

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0] << " kitchen.vtk" << std::endl;
    return EXIT_FAILURE;
  }
  double range[2];

  //
  // Read data
  //
  vtkNew<vtkStructuredGridReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update(); // force a read to occur
  reader->GetOutput()->GetLength();

  //
  // regular streamlines
  //
  vtkNew<vtkLineSource> line;
  line->SetResolution(50);
  line->SetPoint1(0.08, 2.50, 0.71);
  line->SetPoint2(0.08, 4.50, 0.71);


  vtkNew<vtkStreamTracer> streamers;
  // streamers->DebugOn();
  streamers->SetInputConnection(reader->GetOutputPort());
  streamers->SetSourceConnection(line->GetOutputPort());
  streamers->SetMaximumPropagation(1000);
  streamers->SetInitialIntegrationStep(.5);
  streamers->SetMinimumIntegrationStep(.1);
  streamers->SetIntegratorType(2);
  streamers->Update();

  // output the data to vtk file
  vtkSmartPointer<vtkPolyDataWriter> polyWriter =
        vtkSmartPointer<vtkPolyDataWriter>::New();
  
  polyWriter->SetFileName("./streamline_kitchen.vtk");
  polyWriter->SetInputConnection(streamers->GetOutputPort());
  polyWriter->Update();

  return EXIT_SUCCESS;
}