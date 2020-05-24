
#include "possion2d.hpp"
#include "mpi.h"
#include <iostream>
#include <stdexcept>

int main(int argc, char *argv[])
{

    int rank, procs;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    if (argc != 2)
    {
        throw std::runtime_error("<possion2d> <gridLen>");
    }
    //the len of the grid should larger than 3
    int len = std::stoi(argv[1]);

    if (len * len != procs)
    {
        throw std::runtime_error("the total number of process should equal to len*len");
    }

    //generate dims
    //assign it manully if you want a square
    int dims[2] = {len, len};
    //MPI_Dims_create(procs, 2, dims);
    if (rank == 0)
    {
        std::cout << "dim 1(y) " << dims[0] << " dim 2(x) " << dims[1] << std::endl;
    }
    int periodical[2] = {1, 1};
    int reorder = 0;

    //generate coordinates
    int coord[2] = {};
    MPI_Comm cart_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periodical, reorder, &cart_comm);
    MPI_Cart_coords(cart_comm, rank, 2, coord);

    int x = coord[1];
    int y = coord[0];

    //std::cout << "rank " << rank << " x " << x << " y " << y << std::endl;

    //init the dimention and the cartition
    Possion2d p2d(rank, len, x, y);

    //init the possion2d

    p2d.init(cart_comm);

    //call the iteration
    for (int i = 1; i < 10; i++)
    {
        if (rank == 0)
        {
            std::cout << "iterate step " << i << std::endl;
        }

        //error is valid only for the rank 0 process
        double diff = p2d.printInfoGetDiff(rank, procs, len, i);
        if (rank == 0)
        {
            std::cout << "diff " << diff << std::endl;
            //if (error < 0.001)
            //{
            //    break;
            //}
        }

        //exchange value firstly
        p2d.exchange();
        //update the vlaue in current cell
        p2d.iterate();

        //if (i > 16)
        //{

        //}
        MPI_Barrier(MPI_COMM_WORLD);
    }

    //output the data
    return 0;
}