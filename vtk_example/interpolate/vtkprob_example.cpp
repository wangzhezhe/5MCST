

// refer to https://kitware.github.io/vtk-examples/site/Cxx/PolyData/InterpolateTerrain/
// load a polydata and then use the image data to do the interpolation

// another example to set differnet interpolation kernel
// https://kitware.github.io/vtk-examples/site/Cxx/Meshes/PointInterpolator/

//https://kitware.github.io/vtk-examples/site/Cxx/Meshes/InterpolateFieldDataDemo/
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

int main()
{
    vtkNew<vtkImageData> image;
    image->SetExtent(0, 9, 0, 9, 0, 0);
    image->AllocateScalars(VTK_DOUBLE, 1);

    // Create a random set of heights on a grid. This is often called a
    //"terrain map"
    vtkNew<vtkPoints> points;

    vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
    randomSequence->SetSeed(8775070);
    unsigned int GridSize = 10;
    for (unsigned int x = 0; x < GridSize; x++)
    {
        for (unsigned int y = 0; y < GridSize; y++)
        {
            double val = randomSequence->GetRangeValue(-1, 1);
            randomSequence->Next();
            points->InsertNextPoint(x, y, val);
            image->SetScalarComponentFromDouble(x, y, 0, 0, val);
        }
    }

    // add the grid points to a polydata object
    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);

    // triangulate the grid points
    // get the polygonal data by this way
    vtkNew<vtkDelaunay2D> delaunay;
    delaunay->SetInputData(polydata);
    delaunay->Update();

    vtkNew<vtkXMLPolyDataWriter> writer;
    writer->SetFileName("surface.vtp");
    writer->SetInputConnection(delaunay->GetOutputPort());
    writer->Write();

    // Add some points to interpolate
    vtkNew<vtkPoints> probePoints;
    probePoints->InsertNextPoint(5.2, 3.2, 0);
    probePoints->InsertNextPoint(5.0, 3.0, 0);
    probePoints->InsertNextPoint(0.0, 0.0, 0);

    vtkNew<vtkPolyData> probePolyData;
    probePolyData->SetPoints(probePoints);
    
    //VTK/Filters/Core
    vtkNew<vtkProbeFilter> probe;
    probe->SetSourceData(image);
    probe->SetInputData(probePolyData);
    probe->Update();

    // write out the image data
    vtkSmartPointer<vtkXMLImageDataWriter> imgwriter = vtkSmartPointer<vtkXMLImageDataWriter>::New();
    imgwriter->SetFileName("surface_image_sample.vtk");

    // get the specific image and check the results
    imgwriter->SetInputData(image);
    // writer->SetInputData(importer->GetOutputPort());
    // Optional - set the mode. The default is binary.
    imgwriter->SetDataModeToBinary();
    // writer->SetDataModeToAscii();
    imgwriter->Write();
}