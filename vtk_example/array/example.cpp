
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkTypeInt32Array.h>

void basicIntArray() {
  std::cout << __FUNCTION__ << std::endl;
  std::vector<int32_t> source;
  for (int32_t i = 0; i < 12; i++) {
    source.push_back(i);
  }

  auto darray = vtkSmartPointer<vtkTypeInt32Array>::New();
  darray->SetNumberOfComponents(3);
  for (vtkIdType i = 0; i < 4; i++) {
    // there are four tuples
    darray->SetValue(i,source[i]);
  }

  // check properties of the darray
  // the return value of the getPointer bind with a particular type
  int32_t* parray = darray->GetPointer(0);
  int datalen = darray->GetNumberOfComponents() * darray->GetNumberOfTuples();
  if (datalen != 12) {
    throw std::runtime_error("failed for data length check");
  }
  for (int i = 0; i < datalen; i++) {
    int32_t v = *parray;
    if (v != 0.1 * i) {
      throw std::runtime_error("failed for data value checking");
    }
    parray++;
  }

  int32_t* parray2 = (int32_t*)(darray->GetVoidPointer(0));

  for (int i = 0; i < datalen; i++) {
    int32_t v = *parray2;
    if (v != 0.1 * i) {
      throw std::runtime_error("failed for data value checking parray2");
    }
    parray2++;
  }
}

void testdoubleArray() {
  std::cout << __FUNCTION__ << std::endl;
  std::vector<double> source;
  for (int i = 0; i < 12; i++) {
    source.push_back(0.1 * i);
  }

  auto darray = vtkSmartPointer<vtkDoubleArray>::New();
  darray->SetNumberOfComponents(3);
  for (vtkIdType i = 0; i < 4; i++) {
    // there are four tuples
    darray->InsertNextTuple(&source[i * 3]);
  }

  // check properties of the darray
  // the return value of the getPointer bind with a particular type
  double* parray = darray->GetPointer(0);
  int datalen = darray->GetNumberOfComponents() * darray->GetNumberOfTuples();
  if (datalen != 12) {
    throw std::runtime_error("failed for data length check");
  }
  for (int i = 0; i < datalen; i++) {
    double v = *parray;
    if (v != 0.1 * i) {
      throw std::runtime_error("failed for data value checking");
    }
    parray++;
  }

  double* parray2 = (double*)(darray->GetVoidPointer(0));

  for (int i = 0; i < datalen; i++) {
    double v = *parray2;
    if (v != 0.1 * i) {
      throw std::runtime_error("failed for data value checking parray2");
    }
    parray2++;
  }
}

// specilised array with the component equals to 3
// check here https://vtk.org/doc/nightly/html/vtkPoints_8h_source.html
void testPointsArray() {
  std::cout << __FUNCTION__ << std::endl;

  std::vector<double> source;
  for (int i = 0; i < 12; i++) {
    source.push_back(0.1 * i);
  }

  // test point array
  // the default type is the float
  // this is usually managed by point set
  auto pointarray = vtkSmartPointer<vtkPoints>::New();

  // number of the points is basically the value of the tuple
  pointarray->SetNumberOfPoints(4);
  // the position have been allocated at this point

  for (vtkIdType i = 0; i < 4; i++) {
    pointarray->SetPoint(i, &source[i * 3]);
  }

  // check the point
  double v[3];
  for (vtkIdType i = 0; i < 4; i++) {
    pointarray->GetPoint(i, v);
    std::cout << "tuple: " << i << " value " << v[0] << " " << v[1] << " "
              << v[2] << std::endl;
  }

  // check properties of the darray
  // the return value is a null
  // this part is error prone, when we cacaulate the size of element
  // why this error???
  float* ptr = (float*)(pointarray->GetVoidPointer(0));
  int datalen = pointarray->GetNumberOfPoints() * 3;
  if (datalen != 12) {
    throw std::runtime_error("failed for data length check");
  }
  // there are errors here ???
  for (int i = 0; i < datalen; i++) {
    float v = *ptr;
    std::cout << "value " << v << std::endl;
    if (v != float(0.1 * i)) {
      throw std::runtime_error("failed for data value checking");
    }
    ptr++;
  }
}

int main() {
  // test int array
  basicIntArray();
  // test double array
  testdoubleArray();
  // test points
  testPointsArray();
  // test cell array
  // the cell array contains two arrays internally
  // the offset and the connectivity
  // the obsolete design may combine these two parts together
  // testCellArray();
}