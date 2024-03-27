#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "adios2.h"

int main(int argc, char **argv)
{

    adios2::ADIOS ad;

    std::string bpPath = "/Users/zw1/Downloads/ptj.small.field.bp5";

    std::string varName = "species";

    adios2::IO reader_io = ad.DeclareIO("Input");
    // reader_io.SetEngine("BP5");

    adios2::Engine reader = reader_io.Open(bpPath, adios2::Mode::Read);

    int data_sz = 128 * 128 * 128;

    adios2::Variable<double> var_ad2;
    //std::vector<double> data_out(data_sz);
    std::cout << "data_sz is: " << data_sz << std::endl;
    int step = 0;
    while (true)
    {
        // Begin step
        adios2::StepStatus read_status = reader.BeginStep(adios2::StepMode::Read, 10.0f);
        if (read_status != adios2::StepStatus::OK)
        {
            break;
        }
        std::cout << "start adios step " << step << "\n";
        std::cout << "read varName" << "\n";
        std::cout.flush();
        var_ad2 = reader_io.InquireVariable<double>(varName);
        std::cout << "var_ad2 " << var_ad2.Shape()[0] << std::endl;
        // if ((shape[0] > var_ad2.Shape()[0]) || (shape[1] > var_ad2.Shape()[1]) || (shape[2] > var_ad2.Shape()[2]))
        // {
        //     std::cout << "error in requested data size\n";
        //     continue;
        // }
        std::vector<double> var_in;
        var_ad2.SetSelection(adios2::Box<adios2::Dims>({0, 0, 0, 0}, {0, 128, 128, 128}));
        reader.Get<double>(var_ad2, var_in, adios2::Mode::Sync);
        reader.PerformGets();

        // // adios2::Variable<double>var_out = writer_io.DefineVariable<double>(species_name[ivar],
        // //              shape, {0, 0, 0}, shape);
        // // var_out.SetSelection(adios2::Box<adios2::Dims>({0, 0, 0}, {shape[0], shape[1], shape[2]}));

        FILE *fp = fopen("species_0_output.raw", "wb");
        fwrite(var_in.data(), sizeof(double), data_sz, fp);
        fclose(fp);

        reader.EndStep();
        step++;
        if (step == 1)
        {
            break;
        }
    }
    reader.Close();

    return 0;
}

// TODO, there is block information in bp
// but the block info is missing after writtering out