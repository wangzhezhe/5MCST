// refer to
// https://stackoverflow.com/questions/61428040/vtk-c-set-points-for-vtkstructuredgrid-from-three-xyz-vectors/61454228#61454228
// https://vtk.org/Wiki/VTK/Supporting_Arrays_With_Arbitrary_Memory_Layouts

#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkSOADataArrayTemplate.h>

int main() {
  vtkSOADataArrayTemplate<double>* pointArray =
      vtkSOADataArrayTemplate<double>::New();

  int nbOfPoints = 10;

  vtkNew<vtkDoubleArray> XArray;
  XArray->SetName("XArray");
  for (int i = 0; i < nbOfPoints; i++) {
    XArray->InsertNextValue(0.12345 * i);
  }

  vtkNew<vtkDoubleArray> YArray;
  YArray->SetName("YArray");
  for (int i = 0; i < nbOfPoints; i++) {
    YArray->InsertNextValue(0.23456 * i);
  }

  vtkNew<vtkDoubleArray> ZArray;
  ZArray->SetName("ZArray");
  for (int i = 0; i < nbOfPoints; i++) {
    ZArray->InsertNextValue(0.34567 * i);
  }

  // the components and the tuples are same with the soa part
  // the difference is the memory layout
  // it contains three components and each components is stored at the separate
  // array
  pointArray->SetNumberOfComponents(3);
  pointArray->SetNumberOfTuples(nbOfPoints);

  // the second parameter should be a vtkType*

  pointArray->SetArray(0, (double*)(XArray->GetVoidPointer(0)), nbOfPoints,
                       false, true);
  pointArray->SetArray(1, (double*)(YArray->GetVoidPointer(0)), nbOfPoints,
                       false, true);
  pointArray->SetArray(2, (double*)(ZArray->GetVoidPointer(0)), nbOfPoints,
                       false, true);

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