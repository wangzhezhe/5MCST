// refer to https://kitware.github.io/vtk-examples/site/Cxx/PolyData/InterpolateTerrain/
// load a polydata and then use the image data to do the interpolation

// another example to set differnet interpolation kernel
// https://kitware.github.io/vtk-examples/site/Cxx/Meshes/PointInterpolator/

// https://kitware.github.io/vtk-examples/site/Cxx/Meshes/InterpolateFieldDataDemo/

// this is the example that shows how to use the prob func to do the interpolation
#include <vtkCellArray.h>
#include <vtkCellLocator.h>
#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkImageData.h>
#include <vtkLine.h>
#include <vtkMath.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProbeFilter.h>
#include <vtkTriangle.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkStructuredPointsWriter.h>
#include <vtkStructuredPoints.h>

int main()
{
    vtkNew<vtkStructuredPoints> image;
    int GridSize = 10;
    image->SetExtent(0, GridSize-1, 0, GridSize-1, 0, 0);
    // set dims is same with set extent
    // image->SetDimensions(dimx, dimy, dimz);
    image->SetSpacing(1.0, 1.0, 1.0);
    image->SetOrigin(0.0, 0.0, 0.0);

    // Create a random set of heights on a grid. This is often called a
    // "terrain map"
    vtkNew<vtkDoubleArray> VarArray;

    VarArray->SetName("v_sin");
    VarArray->SetNumberOfComponents(1);

    double center[3] = {GridSize / 2.0, GridSize / 2.0, 0.0};

    // the number of point in each dim is gridsize
    for (unsigned int x = 0; x < GridSize; x++)
    {
        for (unsigned int y = 0; y < GridSize; y++)
        {
            // compute the value based on the xyz
            double distToCenter = sqrt(pow(x - center[0], 2) + pow(y - center[1], 2));
            VarArray->InsertNextValue(distToCenter);
        }
    }

    auto dataset = image->GetPointData();
    dataset->AddArray(VarArray);

    // add the grid points to a polydata object
    // vtkNew<vtkPolyData> polydata;
    // polydata->SetPoints(points);

    // triangulate the grid points
    // get the polygonal data by this way
    // vtkNew<vtkDelaunay2D> delaunay;
    // delaunay->SetInputData(polydata);
    // delaunay->Update();

    vtkNew<vtkStructuredPointsWriter> writer;
    writer->SetFileName("source_image.vtk");
    writer->SetInputData(image);
    writer->Write();

    // new grid
    vtkNew<vtkStructuredPoints> newImage;
    //there are 19 points
    int newGridSize = 19;
    newImage->SetExtent(0, newGridSize-1, 0, newGridSize-1, 0, 0);
    // either set the dim or the extent
    // if the dim is gridsize, the extent is from 0 to gridsize
    // when we set dims instead of extent, the last one should be 1
    //newImage->SetDimensions(newGridSize, newGridSize, 1);
    newImage->SetSpacing(0.5,0.5,0.5);
    newImage->SetOrigin(0.0, 0.0, 0.0);

    // VTK/Filters/Core
    vtkNew<vtkProbeFilter> probe;

    // input is the empty grid which we need to fill in
    // the source is the original grid which we try to sample from
    probe->SetSourceData(image);
    probe->SetInputData(newImage);
    probe->Update();

    // getoutput data from the prob
    vtkDataSet* probedData = probe->GetOutput();
    if (probedData==nullptr){
        std::cout << "failed to get probed data" << std::endl;
        exit(0);
    }
    
    probedData->Print(std::cout);


    // auto newDataset = newImage->GetPointData();
    // newDataset->AddArray(doubleDataArray);

    // write out the image data
    vtkSmartPointer<vtkStructuredPointsWriter> probDataWriter = vtkSmartPointer<vtkStructuredPointsWriter>::New();
    probDataWriter->SetFileName("interp_image.vtk");

    // get the specific image and check the results
    // imgwriter->SetInputData(image);
    probDataWriter->SetInputData(probedData);
    // Optional - set the mode. The default is binary.
    // imgwriter->SetDataModeToBinary();
    // writer->SetDataModeToAscii();
    probDataWriter->Write();
}