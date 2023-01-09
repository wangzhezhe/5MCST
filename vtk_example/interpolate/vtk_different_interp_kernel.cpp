
// use different interpolation kernel instead of the
// default one specified by specific cellset

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
#include <vtkPointInterpolator.h>
#include <vtkGaussianKernel.h>
#include <vtkDataSetWriter.h>
#include <vtkShepardKernel.h>
#include <vtkVoronoiKernel.h>

void GaussianKernel(vtkSmartPointer<vtkDataSet> sourceData)
{

    // new grid
    vtkNew<vtkStructuredPoints> newImage;
    // there are 19 points
    int newGridSize = 19;
    // newImage->SetExtent(0, newGridSize-1, 0, newGridSize-1, 0, 0);
    //  either set the dim or the extent
    //  if the dim is gridsize, the extent is from 0 to gridsize
    //  the last dim should at lest be 1, otherwise, there is not output!
    newImage->SetDimensions(newGridSize, newGridSize, 1);
    newImage->SetSpacing(0.5, 0.5, 0.5);
    newImage->SetOrigin(0.0, 0.0, 0.0);

    // VTK/Filters/Core
    // Gaussian kernel
    vtkNew<vtkGaussianKernel> gaussianKernel;
    gaussianKernel->SetSharpness(2.0);
    // this radius influences results a lot
    // if we set unproper value here, this results change a lot
    gaussianKernel->SetRadius(1.0);

    // the dataset must contains the point data set
    vtkNew<vtkPointInterpolator> interpolator;
    // input is the empty grid which we need to fill in
    // the source is the original grid which we try to sample from
    interpolator->SetSourceData(sourceData);
    interpolator->SetInputData(newImage);

    interpolator->SetKernel(gaussianKernel);
    interpolator->Update();

    // getoutput data from the prob
    vtkDataSet *interpData = interpolator->GetOutput();
    if (interpData == nullptr)
    {
        std::cout << "failed to get probed data" << std::endl;
        exit(0);
    }

    std::cout << "summary for the interpData (gaussian kernel):" << std::endl;
    interpData->Print(std::cout);

    // write out the interpoted data data
    // this is another way to do the data writer
    vtkNew<vtkDataSetWriter> interpWriter;
    interpWriter->SetInputData(interpData);
    interpWriter->SetFileName("interp_image_gaussian.vtk");
    interpWriter->Write();
}

void ShepardKernel(vtkSmartPointer<vtkDataSet> sourceData)
{
    // new grid
    vtkNew<vtkStructuredPoints> newImage;
    // there are 19 points
    int newGridSize = 19;
    // newImage->SetExtent(0, newGridSize-1, 0, newGridSize-1, 0, 0);
    //  either set the dim or the extent
    //  if the dim is gridsize, the extent is from 0 to gridsize
    //  the last dim should at lest be 1, otherwise, there is not output!
    newImage->SetDimensions(newGridSize, newGridSize, 1);
    newImage->SetSpacing(0.5, 0.5, 0.5);
    newImage->SetOrigin(0.0, 0.0, 0.0);

    // VTK/Filters/Core
    // Gaussian kernel
    vtkNew<vtkVoronoiKernel> vorononiKernel;

    // the dataset must contains the point data set
    vtkNew<vtkPointInterpolator> interpolator;
    // input is the empty grid which we need to fill in
    // the source is the original grid which we try to sample from
    interpolator->SetSourceData(sourceData);
    interpolator->SetInputData(newImage);

    interpolator->SetKernel(vorononiKernel);
    interpolator->Update();

    // getoutput data from the prob
    vtkDataSet *interpData = interpolator->GetOutput();
    if (interpData == nullptr)
    {
        std::cout << "failed to get probed data" << std::endl;
        exit(0);
    }

    std::cout << "summary for the interpData (shepard kernel):" << std::endl;
    interpData->Print(std::cout);

    // write out the interpoted data data
    // this is another way to do the data writer
    vtkNew<vtkDataSetWriter> interpWriter;
    interpWriter->SetInputData(interpData);
    interpWriter->SetFileName("interp_image_shepard.vtk");
    interpWriter->Write();
}

void VoronoiKernel(vtkSmartPointer<vtkDataSet> sourceData)
{
    // new grid
    vtkNew<vtkStructuredPoints> newImage;
    // there are 19 points
    int newGridSize = 19;
    // newImage->SetExtent(0, newGridSize-1, 0, newGridSize-1, 0, 0);
    //  either set the dim or the extent
    //  if the dim is gridsize, the extent is from 0 to gridsize
    //  the last dim should at lest be 1, otherwise, there is not output!
    newImage->SetDimensions(newGridSize, newGridSize, 1);
    newImage->SetSpacing(0.5, 0.5, 0.5);
    newImage->SetOrigin(0.0, 0.0, 0.0);

    // VTK/Filters/Core
    // Gaussian kernel
    vtkNew<vtkShepardKernel> shepardKernel;

    // the dataset must contains the point data set
    vtkNew<vtkPointInterpolator> interpolator;
    // input is the empty grid which we need to fill in
    // the source is the original grid which we try to sample from
    interpolator->SetSourceData(sourceData);
    interpolator->SetInputData(newImage);

    interpolator->SetKernel(shepardKernel);
    interpolator->Update();

    // getoutput data from the prob
    vtkDataSet *interpData = interpolator->GetOutput();
    if (interpData == nullptr)
    {
        std::cout << "failed to get probed data" << std::endl;
        exit(0);
    }

    std::cout << "summary for the interpData (Voronoi kernel):" << std::endl;
    interpData->Print(std::cout);

    // write out the interpoted data data
    // this is another way to do the data writer
    vtkNew<vtkDataSetWriter> interpWriter;
    interpWriter->SetInputData(interpData);
    interpWriter->SetFileName("interp_image_voronoi.vtk");
    interpWriter->Write();
}

int main()
{
    vtkNew<vtkStructuredPoints> image;
    int GridSize = 10;
    image->SetExtent(0, GridSize - 1, 0, GridSize - 1, 0, 0);
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

    vtkNew<vtkStructuredPointsWriter> writer;
    writer->SetFileName("source_image.vtk");
    writer->SetInputData(image);
    writer->Write();

    GaussianKernel(image);

    ShepardKernel(image);

    VoronoiKernel(image);

    return 0;
}