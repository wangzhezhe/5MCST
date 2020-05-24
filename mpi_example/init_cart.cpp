//the example comes from https://www.youtube.com/watch?v=eMnjwohg-sg

#include "mpi.h"
#include <iostream>
int main(int argc, char *argv[])
{

    int rank, procs;
    MPI_Comm cart_comm;
    int reorder;
    int coord[2], id;
    int up, down, left, right;
    int dim1_src, dim1_dest, dim2_src, dim2_dest;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    if (procs != 12)
    {
        std::cout << "the process number should be 12" << std::endl;
        exit(0);
    }

    int dim[2];
    dim[0] = 4;
    dim[1] = 3;
    int periodical[2] = {1, 1};
    reorder = 0;

    MPI_Cart_create(MPI_COMM_WORLD, 2, dim, periodical, reorder, &cart_comm);
    MPI_Cart_coords(cart_comm, rank, 2, coord);

    printf("rank %d coor1 %d coor2 %d\n", rank, coord[0], coord[1]);
    
    if (rank == 7)
    {
        MPI_Cart_shift(cart_comm, 0, 1, &dim1_src, &dim1_dest);
        MPI_Cart_shift(cart_comm, 1, 1, &dim2_src, &dim2_dest);
        printf("neighbors for rank 7 dim1_src %d dim1_dest %d dim2_src %d dim2_dest %d\n",
               dim1_src, dim1_dest, dim2_src, dim2_dest);
    }
}


/*
output:
rank 0 coor1 0 coor2 0
rank 3 coor1 1 coor2 0
rank 4 coor1 1 coor2 1
rank 6 coor1 2 coor2 0
rank 8 coor1 2 coor2 2
rank 10 coor1 3 coor2 1
rank 11 coor1 3 coor2 2
rank 1 coor1 0 coor2 1
rank 5 coor1 1 coor2 2
rank 7 coor1 2 coor2 1
neighbors for rank 7 dim1_src 4 dim1_dest 10 dim2_src 6 dim2_dest 8
rank 9 coor1 3 coor2 0
rank 2 coor1 0 coor2 2
*/