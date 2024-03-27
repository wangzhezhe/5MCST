// Be careful the dims output by the adios
// the {2560, 960, 3456} means the quickest changed dim is 3456 (x) then the y then the z(2560)
// the data can have more than 3 dims, such as s3d
// species           {19, 2560, 960, 3456}
// there are 19 species, for each species, its z dim is 2560, y dim is 960 and x dim is 3456
// sample command line
//./adiosToRawCommon <path to data> ptj.field.bp 1 temp 2560 960 3456 
// when setting the dims of adios selection, should it be dz dy dx, keep it same with input?

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
    std::string dpath(argv[1]);
    std::string fname(argv[2]);

    std::cout << "readin: " << dpath + fname << "\n";

    std::cout.flush();
    int n_vars = std::stoi(argv[3]);
    std::cout << "number of variables: " << n_vars << "\n";

    // we have parsed three parameters, so the cnt_argv start from 3
    int cnt_argv = 3;
    std::vector<std::string> species_name(n_vars);
    for (int i = 0; i < n_vars; i++)
    {
        species_name[i] = argv[++cnt_argv];
        std::cout << "species_name " << i << " " << species_name[i] << std::endl;
    }
    std::vector<std::size_t> shape(3);
    size_t data_sz = std::stoi(argv[++cnt_argv]);
    shape[0] = std::stoi(argv[cnt_argv]);
    for (int i = 0; i < 2; i++)
    {
        data_sz = data_sz * std::stoi(argv[++cnt_argv]);
        shape[i + 1] = std::stoi(argv[cnt_argv]);
    }

    std::cout << "ok to get all output" << std::endl;

    adios2::IO reader_io = ad.DeclareIO("Input");
    // reader_io.SetEngine("BP5");

    adios2::IO writer_io = ad.DeclareIO("Output");
    adios2::Engine reader = reader_io.Open(dpath + fname, adios2::Mode::Read);
    adios2::Engine writer = writer_io.Open("./" + fname + ".bp", adios2::Mode::Write);

    adios2::Variable<double> var_ad2;
    std::vector<double> data_out(data_sz);
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
        for (int ivar = 0; ivar < n_vars; ivar++)
        {
            std::cout << species_name[ivar] << "\n";
            std::cout.flush();
            var_ad2 = reader_io.InquireVariable<double>(species_name[ivar]);
            std::cout << "var_ad2 shape" << var_ad2.Shape()[0] << "," << var_ad2.Shape()[1] << "," << var_ad2.Shape()[2] << std::endl;
            if ((shape[0] > var_ad2.Shape()[0]) || (shape[1] > var_ad2.Shape()[1]) || (shape[2] > var_ad2.Shape()[2]))
            {
                std::cout << "error in requested data size\n";
                continue;
            }
            std::vector<double> var_in;
            var_ad2.SetSelection(adios2::Box<adios2::Dims>({0, 0, 0}, {shape[0], shape[1], shape[2]}));
            reader.Get<double>(var_ad2, var_in, adios2::Mode::Sync);
            reader.PerformGets();

            // adios2::Variable<double>var_out = writer_io.DefineVariable<double>(species_name[ivar],
            //              shape, {0, 0, 0}, shape);
            // var_out.SetSelection(adios2::Box<adios2::Dims>({0, 0, 0}, {shape[0], shape[1], shape[2]}));
            // writer.Put<double>(var_out, var_in.data(), adios2::Mode::Sync);
            // writer.PerformPuts();

            // sample the data
            int sampleRate = 8;
            int reducedx = shape[0] / sampleRate;
            int reducedy = shape[1] / sampleRate;
            int reducedz = shape[2] / sampleRate;

            std::cout << "reduced x y z " << reducedx << "," << reducedy << "," << reducedz << std::endl;

            int reduced_data_num = reducedx * reducedy * reducedz;

            std::cout << "reduced data num " << reduced_data_num << " original data num " << data_sz << std::endl;

            std::vector<double> data_sampled;
            // data_sampled.reserve(reduced_data_num);
            std::cout << "var in size " << var_in.size() << std::endl;
            // the first one change slowest
            for (int z = 0; z < shape[0]; z++)
            {
                if (z % sampleRate != 0)
                    continue;
                for (int y = 0; y < shape[1]; y++)
                {
                    if (y % sampleRate != 0)
                        continue;
                    for (int x = 0; x < shape[2]; x++)
                    {
                        if (x % sampleRate != 0)
                            continue;
                        // std::cout << "test x " << x << std::endl;
                        size_t index = z * shape[2] * shape[1] + y * shape[2] + x;
                        // std::cout << "index " << index  << std::endl;
                        data_sampled.push_back(var_in[index]);
                    }
                }
            }

            // try to get first layer
            // for(size_t i==0;i<)

            std::cout << "check data sampled size " << data_sampled.size() << std::endl;

            FILE *fp = fopen((fname + species_name[ivar] + ".raw").c_str(), "wb");
            fwrite(data_sampled.data(), sizeof(double), reduced_data_num, fp);
            fclose(fp);
        }
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
