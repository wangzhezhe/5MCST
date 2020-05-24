
#include "mpi.h"
#include <iostream>
int main(int argc, char *argv[])
{

    int rank, procs;
    MPI_Comm cart_comm;
    int reorder;
    int coord[3], id;
    int dim=3;
    int xl, xr, yl, yr, zl, zr;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    int periodical[3] = {1, 1, 1};
    reorder = 0;

    //update the dim according to nproc
    int dims[3]={};
    MPI_Dims_create(procs, 3, dims);

    if (rank == 0)
    {
        printf("dims %d,%d,%d\n", dims[0], dims[1], dims[2]);
    }

    MPI_Cart_create(MPI_COMM_WORLD, dim, dims, periodical, reorder, &cart_comm);
    MPI_Cart_coords(cart_comm, rank, dim, coord);

    printf("rank %d x %d y %d z %d\n", rank, coord[0], coord[1], coord[2]);

}
