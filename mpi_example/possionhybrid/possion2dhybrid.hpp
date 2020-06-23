#ifndef POSSION2D
#define POSSION2D

#include <mpi.h>

#include <fstream>
#include <iostream>
#include <vector>

typedef struct Possion2dHybrid {
  Possion2dHybrid(int rank, int globalLen, int localLen, int partitionNum1D,
                  int blockx, int blocky)
      : m_rank(rank),
        m_globalLen(globalLen),
        m_localLen(localLen),
        m_blockNumInOneD(partitionNum1D),
        m_blockx(blockx),
        m_blocky(blocky) {
    m_dx = 1.0 / (1.0 * (globalLen - 1));
    m_dy = 1.0 / (1.0 * (globalLen - 1));
    if (m_rank == 0) {
      std::cout << "m_dx " << m_dx << " m_dy " << m_dy << std::endl;
    }

    // this offset is for the global mesh
    offset_x = localLen * blockx;
    offset_y = localLen * blocky;

    //std::cout << "rank " << rank << " m_blockx " << m_blockx << " m_blocky "
    //          << m_blocky << " offset_x " << offset_x << " offset_y "
    //          << offset_y << std::endl;

    // init the cellData, include the ghost values
    for (int j = 0; j < localLen + 2; j++) {
      for (int i = 0; i < localLen + 2; i++) {
        m_cellValue.push_back(0);
        m_cellValueNew.push_back(0);
        m_fb.push_back(0);
        m_exact.push_back(0);
      }
    }
  };

  // global coor -> local coor no ghost -> local coor ghost -> local index
  // Convert global coordinate to local index
  // the global coordinates does not contains the ghost value
  inline int g2i(int gx, int gy) const {
    int x = gx - this->offset_x;
    int y = gy - this->offset_y;
    // the 1 here is for the ghost position at the boundries
    return l2i(x + 1, y + 1);
  }
  // Convert local coordinate to local index ( local array contains the ghost
  // value)
  inline int l2i(int x, int y) const { return x + y * (this->m_localLen + 2); }
  // local index (including the ghost value) to the global mesh index
  inline int l2gi(int x, int y) const {
    int globalx = offset_x + (x - 1);
    int globaly = offset_y + (y - 1);
    return globalx + globaly * this->m_globalLen;
  }

  // init the boundry value
  void init(int rank, MPI_Comm &cart_comm);
  // get the value from all neigobor cells
  void exchange();
  // iterate to the next step
  void iterate();
  // void output
  void output(int myrank, int totalProcess, int len, int step);
  // compute diff of two iterations and the error compared with acurate values
  void printInfoGetDiff(int myrank, int procNum);

  void printInfo(int myrank);

  // cell value and the ghost value
  // (Attention to the data depedency) for each iteration
  // use the m_cellValue to update the m_cellValueNew
  // use the m_cellValueNew to update the ghost value
  std::vector<double> m_cellValue;
  std::vector<double> m_cellValueNew;

  // fb store the right side of the function
  std::vector<double> m_fb;
  std::vector<double> m_exact;

  // MPI datatypes for halo exchange
  MPI_Datatype x_edge_type;
  MPI_Datatype y_edge_type;

  // this represents the global mesh len
  int m_globalLen;
  // this represents the locak mesh len
  int m_localLen;
  // block number in each direaction
  int m_blockNumInOneD;

  // m_x and m_y are coords for mesh
  // it needs another level of transformation from this into the actual coords
  // in physical domain
  const int m_blockx;
  const int m_blocky;

  int offset_x;
  int offset_y;

  double m_omega = 0.8;
  int m_rank;

  double m_dx;
  double m_dy;

  // neighbor block id
  int m_leftBlockId, m_rightBlockId, m_downBlockId, m_upBlockId;

  // this is only meningful for rank 0
  double *m_rcvBufferLast = NULL;
  double *m_rcvBufferCurrent = NULL;

} Possion2d;

#endif