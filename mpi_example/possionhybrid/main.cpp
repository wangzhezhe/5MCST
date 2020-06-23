
#include <unistd.h>

#include <iostream>
#include <stdexcept>

#include "mpi.h"
#include "possion2dhybrid.hpp"
#define BILLION 1000000000L

int main(int argc, char *argv[]) {
  int rank, procs;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &procs);

  if (argc != 3) {
    throw std::runtime_error("./possion2dhybrid <globalLen> <localLen>");
  }

  int globalLen = std::stoi(argv[1]);
  int localLen = std::stoi(argv[2]);
  if (globalLen % localLen != 0) {
    throw std::runtime_error(
        "globalLen should be integer multiples then localLen");
  }

  int partitionNum1D = globalLen / localLen;
  int blockNum = partitionNum1D * partitionNum1D;
  if (blockNum != procs) {
    throw std::runtime_error(
        "the total number of MPI process should equal to the block numbers");
  }

  // generate dims
  // assign it manully if you want a square
  int dims[2] = {partitionNum1D, partitionNum1D};
  // MPI_Dims_create(procs, 2, dims);
  if (rank == 0) {
    std::cout << "dim 1(y) " << dims[0] << " dim 2(x) " << dims[1] << std::endl;
  }
  int periodical[2] = {1, 1};
  int reorder = 0;

  // generate coordinates
  int blockCoord[2] = {};
  MPI_Comm cart_comm;
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periodical, reorder, &cart_comm);
  MPI_Cart_coords(cart_comm, rank, 2, blockCoord);

  int blockx = blockCoord[1];
  int blocky = blockCoord[0];

  // init the dimention and the cartition
  Possion2dHybrid p2d(rank, globalLen, localLen, partitionNum1D, blockx,
                      blocky);

  // init the possion2d
  p2d.init(rank, cart_comm);
  struct timespec start, end;
  double diff;

  // timer
  if (rank == 0) {
    clock_gettime(CLOCK_REALTIME, &start); /* mark start time */
  }

  // call the iteration
  p2d.printInfoGetDiff(rank, procs);

  for (int i = 1; i < 100; i++) {
    MPI_Barrier(MPI_COMM_WORLD);
    // if (rank == 0) {
    //  std::cout << "iterate step " << i << std::endl;
    //}
    // double diff = p2d.printInfoGetDiff(rank, procs, len, i);
    //if (rank == 0) {
    //  std::cout << "iteration " << i << std::endl;
    //}
    // exchange value firstly

    p2d.exchange();

    p2d.printInfo(rank);

    // update the vlaue in current cell
    p2d.iterate();
  }

  p2d.printInfoGetDiff(rank, procs);

  if (rank == 0) {
    // some functions here
    clock_gettime(CLOCK_REALTIME, &end); /* mark end time */
    diff = (end.tv_sec - start.tv_sec) * 1.0 +
           (end.tv_nsec - start.tv_nsec) * 1.0 / BILLION;
    printf("time is %lf seconds \n", diff);
  }

  // output the data
  return 0;
}