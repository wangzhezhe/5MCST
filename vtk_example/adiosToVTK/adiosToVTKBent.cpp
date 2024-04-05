#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "adios2.h"

#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>

#include <vtkUnstructuredGrid.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkResampleToImage.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkUniformGrid.h>
#include <vtkProbeFilter.h>

int main(int argc, char **argv)
{

    adios2::ADIOS ad;

    std::string bpPath = "/lustre/orion/csc143/proj-shared/gongq/frontier/dataset/bent_pipe.bp/";

    std::string connectivity = "connectivity";
    std::string points = "points";
    std::string VX = "VX";
    std::string VY = "VY";
    std::string VZ = "VZ";

    adios2::IO reader_io = ad.DeclareIO("Input");
    adios2::Engine reader = reader_io.Open(bpPath, adios2::Mode::Read);

    int pointElement = 235008000;
    int numCells = 235008000;

    adios2::Variable<float> pointsAdiosVar;
    adios2::Variable<int32_t> connAdiosVar;
    adios2::Variable<double> vxVar;
    adios2::Variable<double> vyVar;
    adios2::Variable<double> vzVar;

    auto unsturcturedData = vtkSmartPointer<vtkUnstructuredGrid>::New();

    // std::vector<double> data_out(data_sz);
    int step = 0;
    while (true)
    {
        // Begin step
        adios2::StepStatus read_status = reader.BeginStep(adios2::StepMode::Read, 10.0f);
        // jump out first step

        if (read_status != adios2::StepStatus::OK)
        {
            break;
        }

        std::cout << "start adios step " << step << "\n";
        std::cout.flush();

        // get the point array
        if (step == 0)
        {
            pointsAdiosVar = reader_io.InquireVariable<float>(points);
            std::cout << "pointsAdiosVar: " << pointsAdiosVar.Shape()[0] << "," << pointsAdiosVar.Shape()[1] << std::endl;

            pointsAdiosVar.SetSelection(adios2::Box<adios2::Dims>({0, 0}, {235008000, 3}));
            std::vector<float> pointsVarIn;
            reader.Get<float>(pointsAdiosVar, pointsVarIn, adios2::Mode::Sync);
            reader.PerformGets();

            // set points to vtk array
            std::cout << "elements in of pointsVarIn " << pointsVarIn.size() << std::endl;
            auto vtkPointArray = vtkSmartPointer<vtkPoints>::New();
            vtkPointArray->SetNumberOfPoints(pointElement);
            for (int i = 0; i < pointElement; i++)
            {
                int pointStart = i * 3;
                float coods[3];
                coods[0] = pointsVarIn[pointStart];
                coods[1] = pointsVarIn[pointStart + 1];
                coods[2] = pointsVarIn[pointStart + 2];

                vtkPointArray->SetPoint(i, coods);
            }

            vtkPointArray->PrintSelf(std::cout, vtkIndent(5));
            unsturcturedData->SetPoints(vtkPointArray);

            // extracting connectivity
            connAdiosVar = reader_io.InquireVariable<int32_t>(connectivity);
            connAdiosVar.SetSelection(adios2::Box<adios2::Dims>({0}, {1880064000}));
            std::vector<int32_t> connVarIn;
            reader.Get<int32_t>(connAdiosVar, connVarIn, adios2::Mode::Sync);
            std::cout << "elements in connVarIn " << connVarIn.size() << std::endl;

            // set cell array
            // auto cellArray = vtkSmartPointer<vtkCellArray>::New();
            for (int i = 0; i < numCells; i++)
            {
                int cellStart = i * 8;
                vtkIdType pointIds[8];
                for (int j = 0; j < 8; j++)
                {
                    pointIds[j] = connVarIn[cellStart + j];
                    // if (i < 10)
                    // {
                    //     std::cout << "i " << i << " j " << j << " point id " << pointIds[j] << std::endl;
                    // }
                }

                // cellArray->InsertNextCell(8, pointIds);
                // oom when adding the cell
                unsturcturedData->InsertNextCell(VTK_HEXAHEDRON, 8, pointIds);
            }

            // std::cout << "Output cellArray:" << std::endl;
            // cellArray->PrintSelf(std::cout, vtkIndent(5));
        }
        // extract the point array
        if (step > 0)
        {
            // load vx
            vxVar = reader_io.InquireVariable<double>(VX);
            vyVar = reader_io.InquireVariable<double>(VY);
            vzVar = reader_io.InquireVariable<double>(VZ);

            std::cout << "vxVar: " << vxVar.Shape()[0] << "," << vxVar.Shape()[1] << std::endl;

            vxVar.SetSelection(adios2::Box<adios2::Dims>({0}, {235008000}));
            vyVar.SetSelection(adios2::Box<adios2::Dims>({0}, {235008000}));
            vzVar.SetSelection(adios2::Box<adios2::Dims>({0}, {235008000}));

            // TODO set selection time step?
            // first step contains all zero values
            std::vector<double> vVarVarIn;
            reader.Get<double>(vxVar, vVarVarIn, adios2::Mode::Sync);
            reader.PerformGets();

            auto velocityx = vtkSmartPointer<vtkDoubleArray>::New();
            velocityx->SetNumberOfComponents(1);
            for (int i = 0; i < pointElement; i++)
            {
                // if(vVarVarIn[i]!=0){
                //     std::cout << vVarVarIn[i] << std::endl;
                // }
                velocityx->InsertNextValue(vVarVarIn[i]);
            }

            double *parray = velocityx->GetPointer(0);
            for (int i = 0; i < 1000; i++)
            {
                double v = *parray;
                parray++;
                std::cout << "debug " << v << std::endl;
            }

            velocityx->PrintSelf(std::cout, vtkIndent(5));

            velocityx->SetName("vx");
            unsturcturedData->GetPointData()->AddArray(velocityx);

            reader.Get<double>(vyVar, vVarVarIn, adios2::Mode::Sync);
            reader.PerformGets();
            auto velocityy = vtkSmartPointer<vtkDoubleArray>::New();
            velocityy->SetNumberOfComponents(1);
            for (int i = 0; i < pointElement; i++)
            {
                velocityy->InsertNextValue(vVarVarIn[i]);
            }
            velocityy->SetName("vy");
            unsturcturedData->GetPointData()->AddArray(velocityy);

            reader.Get<double>(vzVar, vVarVarIn, adios2::Mode::Sync);
            reader.PerformGets();
            auto velocityz = vtkSmartPointer<vtkDoubleArray>::New();
            velocityz->SetNumberOfComponents(1);
            for (int i = 0; i < pointElement; i++)
            {
                velocityz->InsertNextValue(vVarVarIn[i]);
            }
            velocityz->SetName("vz");
            unsturcturedData->GetPointData()->AddArray(velocityz);

    unsturcturedData->GetPointData()->SetActiveScalars("vx");
    unsturcturedData->GetPointData()->SetActiveScalars("vy");
    unsturcturedData->GetPointData()->SetActiveScalars("vz");

            std::cout << "Output unsturcturedData:" << std::endl;
            unsturcturedData->PrintSelf(std::cout, vtkIndent(5));

            // write raw data
            //  vtkSmartPointer<vtkUnstructuredGridWriter> writer =
            //     vtkSmartPointer<vtkUnstructuredGridWriter>::New();
            // writer->SetFileName("./bent.vtk");
            // // sample some points to avoid out of space

            // // output the data into the vtk file

            // // get the specific polydata and check the results
            // writer->SetInputData(unsturcturedData);
            // // Optional - set the mode. The default is binary.
            // // writer->SetDataModeToBinary();
            // // writer->SetDataModeToAscii();
            // writer->Write();

            // reample to image
            int dx = 100;
            int dy = 50;
            int dz = 100;
            auto ugrid = vtkSmartPointer<vtkUniformGrid>::New();
            ugrid->SetOrigin(-20, -0.5, -10);
            ugrid->SetDimensions(100, 50, 100);
            ugrid->SetSpacing(22.0 / (dx * 1.0-1), 1.0 / (dy * 1.0-1), 12.0 / (dz * 1.0-1));

            auto resample = vtkSmartPointer<vtkProbeFilter>::New();
            resample->SetSourceData(unsturcturedData);
            resample->SetInputData(ugrid);
            resample->Update();

            vtkSmartPointer<vtkXMLImageDataWriter> writer = vtkSmartPointer<vtkXMLImageDataWriter>::New();
            std::string writeFileName = std::string("bent_img_") + std::to_string(step) + ".vti";
            writer->SetFileName(writeFileName.data());

            // get the specific image and check the results
            // writer->SetInputData(imagedata);

            auto testoutput = resample->GetOutput();
            std::cout << "test output:" << std::endl;
            testoutput->PrintSelf(std::cout, vtkIndent(5));
            
            //why this is sure  
            auto sampledVxArray = testoutput->GetPointData()->GetArray("vx");
            std::cout << "sampledVxArray:" << std::endl;
            sampledVxArray->PrintSelf(std::cout, vtkIndent(5));

            double *sparray = (double*)sampledVxArray->GetVoidPointer(0);
            for (int i = 0; i < 100; i++)
            {
                double v = *sparray;
                sparray++;
                std::cout << "debug sampledVxArray " << v << std::endl;
            }


            writer->SetInputData(resample->GetOutput());
            // Optional - set the mode. The default is binary.
            // writer->SetDataModeToBinary();
            // writer->SetDataModeToAscii();
            writer->Write();
        }
        reader.EndStep();
        step++;

        if (step == 2)
        {
            break;
        }
    } // while
    reader.Close();
    return 0;
}
