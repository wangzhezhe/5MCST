

#include <vtkAOSDataArrayTemplate.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>

int main() {
  vtkAOSDataArrayTemplate<double>* pointArray =
      vtkAOSDataArrayTemplate<double>::New();

  int nbOfPoints = 10;

  // the components and the tuples are same with the soa part
  // the difference is the memory layout
  // it contains three components and each components is stored at the separate
  // array
  pointArray->SetNumberOfComponents(3);
  pointArray->SetNumberOfTuples(nbOfPoints);

  double tuple[3];
  for (int i = 0; i < nbOfPoints; i++) {
    tuple[0] = 0.12345 * i;
    tuple[1] = 0.23456 * i;
    tuple[2] = 0.34567 * i;

    pointArray->SetTuple(i, tuple);
  }

  vtkNew<vtkPoints> points;
  points->SetData(pointArray);
  pointArray->Delete();
  // VTKGrid->SetPoints(points);
  // points->DebugOn();

  // vtkDebugMacro(<< "try debug macro");
  points->PrintSelf(std::cout, vtkIndent(5));

  // check the output value
  double* arrayptr = (double*)pointArray->GetVoidPointer(0);
  for (int i = 0; i < 12; i++) {
    std::cout << "i " << i << " value " << *(arrayptr + i) << std::endl;
  }
}