#include <iostream>
#include <vector>
#include <vtkAbstractArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkFlyingEdges3D.h>
#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLDataSetWriter.h>
#include <vtkXMLImageDataWriter.h>

void testimage()
{
  std::cout << "------test " << __FUNCTION__ << "\n";

  std::vector<double> array(1000, 0);
  for (int i = 0; i < array.size(); i++)
  {
    if (i % 2 == 0)
    {
      array[i] = i;
    }
    else
    {
      array[i] = i - 1 + 100;
    }
  }

  std::array<int, 3> indexlb = { { 0, 0, 0 } };
  std::array<int, 3> indexub = { { 9, 9, 9 } };

  auto importer = vtkSmartPointer<vtkImageImport>::New();
  importer->SetDataSpacing(1, 1, 1);
  importer->SetDataOrigin(0, 0, 0);
  // double anaStep1 = tl::timer::wtime();

  // still not sure the difference between the whole extent and the buffered extent
  importer->SetWholeExtent(indexlb[0], indexub[0], indexlb[1], indexub[1], indexlb[2], indexub[2]);
  importer->SetDataExtentToWholeExtent();
  importer->SetDataScalarTypeToDouble();
  importer->SetNumberOfScalarComponents(1);
  // u_nonghost is a function that takes long time
  // it basically reoranize the data
  // get the same value here direactly to avoid the copy
  importer->SetImportVoidPointer((double*)(array.data()));
  // !!!the actual image data of the image is empty if we do not execute the update
  importer->Update();
  vtkSmartPointer<vtkImageData> imagedata = importer->GetOutput();
  int* extentArray = imagedata->GetExtent();
  std::cout << extentArray[0] << "," << extentArray[1] << "," << extentArray[2] << ","
            << extentArray[3] << "," << extentArray[4] << "," << extentArray[5] << std::endl;

  int arrayNum = imagedata->GetPointData()->GetNumberOfArrays();
  std::cout << "GetScalarType " << imagedata->GetScalarType() << " elem size "
            << imagedata->GetScalarSize() << " elem num " << imagedata->GetNumberOfPoints()
            << " point array num " << arrayNum << std::endl;

  if (arrayNum > 0)
  {
    //the scarlar is the default array associated with the point
    std::cout << "array name: " << imagedata->GetPointData()->GetAbstractArray(0)->GetName() << std::endl;
  }

  double* datapointer = (double*)imagedata->GetScalarPointer();
  // for (int i = 0; i < imagedata->GetNumberOfPoints(); i++)
  //{
  //  std::cout << "index " << i << " value " << datapointer[i] << std::endl;
  //}

  // write image
  // Write the file by vtkXMLDataSetWriter
  std::string fileName = "test_vtkimage_1component.vti";
  vtkSmartPointer<vtkXMLImageDataWriter> writer = vtkSmartPointer<vtkXMLImageDataWriter>::New();
  writer->SetFileName(fileName.data());

  // get the specific image and check the results
  writer->SetInputData(imagedata);
  // writer->SetInputData(importer->GetOutputPort());
  // Optional - set the mode. The default is binary.
  writer->SetDataModeToBinary();
  // writer->SetDataModeToAscii();
  writer->Write();
}

void testimagetopoly()
{
  std::cout << "------test " << __FUNCTION__ << "\n";
  std::vector<double> array(1000, 0);
  for (int i = 0; i < array.size(); i++)
  {
    if (i % 2 == 0)
    {
      array[i] = i;
    }
    else
    {
      array[i] = i - 1 + 100;
    }
  }

  std::array<int, 3> indexlb = { { 0, 0, 0 } };
  std::array<int, 3> indexub = { { 9, 9, 9 } };
  // for the imageimporter
  // !! If we use the image importer to create the image data
  // The array applies on scalar point data only, not on cell data.
  // Scalars are single data values associated with each point/cell in the dataset.
  auto importer = vtkSmartPointer<vtkImageImport>::New();
  importer->SetDataSpacing(1, 1, 1);
  importer->SetDataOrigin(0, 0, 0);
  // double anaStep1 = tl::timer::wtime();

  // still not sure the difference between the whole extent and the buffered extent
  importer->SetWholeExtent(indexlb[0], indexub[0], indexlb[1], indexub[1], indexlb[2], indexub[2]);
  importer->SetDataExtentToWholeExtent();
  importer->SetDataScalarTypeToDouble();
  importer->SetNumberOfScalarComponents(1);
  // u_nonghost is a function that takes long time
  // it basically reoranize the data
  // get the same value here direactly to avoid the copy
  importer->SetImportVoidPointer((double*)(array.data()));
  // !!!the actual image data of the image is empty if we do not execute the update
  importer->Update();
  vtkSmartPointer<vtkImageData> imagedata = importer->GetOutput();

  auto isoExtraction = vtkSmartPointer<vtkFlyingEdges3D>::New();
  isoExtraction->SetInputData(imagedata);
  isoExtraction->ComputeNormalsOn();
  isoExtraction->SetValue(0, 100);
  isoExtraction->Update();
  vtkSmartPointer<vtkPolyData> polydata = isoExtraction->GetOutput();

  // write poly data
  // we only have the scarlar in this case, the output of the poly data also contains the scarlar,
  // and the normal
  std::string fileName = "test_vtkpoly.vtp";
  vtkSmartPointer<vtkXMLDataSetWriter> writer = vtkSmartPointer<vtkXMLDataSetWriter>::New();
  writer->SetFileName(fileName.data());
  // get the specific polydata and check the results
  writer->SetInputData(polydata);
  // Optional - set the mode. The default is binary.
  writer->SetDataModeToBinary();
  // writer->SetDataModeToAscii();
  writer->Write();
}

// refer to this
// https://stackoverflow.com/questions/61395352/add-color-array-to-vtkimagedata-c
void testAddArrayToImage()
{
  std::cout << "------test " << __FUNCTION__ << "\n";

  // get image data
  std::vector<double> array(1000, 0);
  for (int i = 0; i < array.size(); i++)
  {
    if (i % 2 == 0)
    {
      array[i] = i;
    }
    else
    {
      array[i] = i - 1 + 100;
    }
  }

  std::array<int, 3> indexlb = { { 0, 0, 0 } };
  std::array<int, 3> indexub = { { 9, 9, 9 } };
  // for the imageimporter
  // !! If we use the image importer to create the image data
  // The array applies on scalar point data only, not on cell data.
  // Scalars are single data values associated with each point/cell in the dataset.
  auto importer = vtkSmartPointer<vtkImageImport>::New();
  importer->SetDataSpacing(1, 1, 1);
  importer->SetDataOrigin(0, 0, 0);
  // double anaStep1 = tl::timer::wtime();

  // still not sure the difference between the whole extent and the buffered extent
  importer->SetWholeExtent(indexlb[0], indexub[0], indexlb[1], indexub[1], indexlb[2], indexub[2]);
  importer->SetDataExtentToWholeExtent();
  importer->SetDataScalarTypeToDouble();
  importer->SetNumberOfScalarComponents(1);
  // u_nonghost is a function that takes long time
  // it basically reoranize the data
  // get the same value here direactly to avoid the copy
  importer->SetImportVoidPointer((double*)(array.data()));
  // !!!the actual image data of the image is empty if we do not execute the update
  importer->Update();
  vtkSmartPointer<vtkImageData> imagedata = importer->GetOutput();

  // add new array to the point
  vtkNew<vtkDoubleArray> pointattributeArray;
  pointattributeArray->SetName("newpointAttrbute1");
  for (int i = 0; i < 1000; i++)
  {
    pointattributeArray->InsertNextValue(0.12345 * i);
  }

  imagedata->GetPointData()->AddArray(pointattributeArray);

  // the data is associated with cells
  vtkNew<vtkDoubleArray> cellAttributeArray;
  cellAttributeArray->SetName("newCellAttrbute1");
  for (int i = 0; i < imagedata->GetNumberOfCells(); i++)
  {
    cellAttributeArray->InsertNextValue(0.12345 * i);
  }

  // the data is associated with point
  imagedata->GetCellData()->AddArray(cellAttributeArray);

  // write image data
  std::string fileName = "test_vtkimage_newattribute.vti";
  vtkSmartPointer<vtkXMLImageDataWriter> writer = vtkSmartPointer<vtkXMLImageDataWriter>::New();
  writer->SetFileName(fileName.data());

  // get the specific image and check the results
  writer->SetInputData(imagedata);
  // writer->SetInputData(importer->GetOutputPort());
  // Optional - set the mode. The default is binary.
  writer->SetDataModeToBinary();
  // writer->SetDataModeToAscii();
  writer->Write();
}

// things about the vtknew and vtksmartpointer
// refer to https://discourse.vtk.org/t/vtknew-and-vtksmartpointer/469/9
void testDireactImageCreate()
{
  std::cout << "------test " << __FUNCTION__ << "\n";
  // Create an image data
  // by this way, we do not need to have scarlar data
  vtkNew<vtkImageData> imageData;

  // Specify the size of the image data
  int nx = 10;
  int ny = 10;
  int nz = 10;
  // imageData->SetDimensions(nx, ny, nz);
  // Same as setting extents such as SetExtent(0, i-1, 0, j-1, 0, k-1)
  imageData->SetDimensions(nx, ny, nz);
  imageData->SetSpacing(1.0, 1.0, 1.0);
  imageData->SetOrigin(0.0, 0.0, 0.0);

  vtkNew<vtkDoubleArray> array;
  array->SetName("TestAttribute1");
  double val[1000];
  for (int i = 0; i < 1000; i++)
  {
    array->InsertNextValue(i);
  }

  imageData->GetPointData()->AddArray(array);
  std::cout << "num of arrays: " << imageData->GetPointData()->GetNumberOfArrays() << std::endl;

  // try to get array information
  vtkAbstractArray* innerarray0 = imageData->GetPointData()->GetAbstractArray(0);
  if (innerarray0 == nullptr)
  {
    std::cout << "innerarray0 is empty" << std::endl;
  }
  else
  {
    std::cout << "innerarray0 name:" << innerarray0->GetName() << std::endl;
  }
  // try to get array information
  vtkAbstractArray* innerarray1 = imageData->GetPointData()->GetAbstractArray(1);
  if (innerarray1 == nullptr)
  {
    std::cout << "innerarray1 is empty" << std::endl;
  }
  else
  {
    std::cout << "array name:" << innerarray1->GetName() << std::endl;
  }
}

int main()
{
  testimage();
  testimagetopoly(); // only scarlar varaibe
  testAddArrayToImage();
  testDireactImageCreate();
}