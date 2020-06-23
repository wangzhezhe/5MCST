#include "possion2dhybrid.hpp"

#include <math.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>

/*
  for (j = 0; j < ny; j++)
  {
    y = (double)(j) / (double)(ny - 1);
    for (i = 0; i < nx; i++)
    {
      x = (double)(i) / (double)(nx - 1);
      if (i == 0 || i == nx - 1 || j == 0 || j == ny - 1)
      {
        f[i][j] = u_exact(x, y);
      }
      else
      {
        f[i][j] = -uxxyy_exact(x, y);
      }
    }
  }
*/

double u_exact(double x, double y) {
  double r8_pi = 3.141592653589793;
  double value;

  value = sin(r8_pi * x * y);

  return value;
}

double uxxyy_exact(double x, double y) {
  double r8_pi = 3.141592653589793;
  double value;

  value = -r8_pi * r8_pi * (x * x + y * y) * sin(r8_pi * x * y);

  return value;
}

// Convert the local coordinates to the global index

// init the boundry value
void Possion2dHybrid::init(int rank, MPI_Comm &cart_comm) {
  int globalx, globaly;
  double phy_x, phy_y;
  for (int j = 1; j <= m_localLen; j++) {
    for (int i = 1; i <= m_localLen; i++) {
      globalx = offset_x + (i - 1);
      globaly = offset_y + (j - 1);
      phy_x = 1.0 * m_dx * globalx;
      phy_y = 1.0 * m_dy * globaly;
      if (globalx == 0 || globalx == m_globalLen - 1 || globaly == 0 ||
          globaly == m_globalLen - 1) {
        // the exacat value for cell
        m_cellValue[l2i(i, j)] = u_exact(phy_x, phy_y);
        // the exact value for f
        m_fb[l2i(i, j)] = u_exact(phy_x, phy_y);
      } else {
        // the uxxyy exact value for f
        m_fb[l2i(i, j)] = -uxxyy_exact(phy_x, phy_y);
      }
      m_exact[l2i(i, j)] = u_exact(phy_x, phy_y);
      // std::cout << "rank " << rank << " j " << j << " i " << i
      //          << " error coords " << phy_y << "," << phy_x << std::endl;
    }
  }

  // debug the m_exact
  //std::cout << "debug exact ";
  //for (int i = 0; i < m_exact.size(); i++) {
  //  std::cout << m_exact[i] << ", ";
  //}
  //std::cout << std::endl;

  MPI_Cart_shift(cart_comm, 1, 1, &m_leftBlockId, &m_rightBlockId);
  MPI_Cart_shift(cart_comm, 0, 1, &m_downBlockId, &m_upBlockId);

  // refer to
  // http://www.math-cs.gordon.edu/courses/cps343/presentations/MPI_Derived_datatypes.pdf
  // refer to
  // https://pages.tacc.utexas.edu/~eijkhout/pcse/html/mpi-data.html#Vectortype
  // X edge
  MPI_Type_vector(m_localLen, 1, 1, MPI_DOUBLE, &x_edge_type);
  MPI_Type_commit(&x_edge_type);

  // Y edge
  MPI_Type_vector(m_localLen, 1, (m_localLen + 2), MPI_DOUBLE, &y_edge_type);
  MPI_Type_commit(&y_edge_type);

  // std::cout << " x " << m_x << " y " << m_y << " lid " << m_leftId << "
  // m_rightId " << m_rightId << " m_downId " << m_downId << " m_upId " <<
  // m_upId
  // << std::endl;
  // if (m_rank == 0) {
  // use original declared pointer, instead of a new pointer
  //  int totalProcess = m_len * m_len;
  //  m_rcvBufferLast = (double *)calloc(totalProcess, sizeof(double));
  //  m_rcvBufferCurrent = (double *)calloc(totalProcess, sizeof(double));
  //}
}

/*

    double *send_x = new double[m_s.ndx + 2];
    double *recv_x = new double[m_s.ndx + 2];

  // send to left + receive from right
    int tag = 1;
    MPI_Status status;
    if (m_s.rank_left >= 0)
    {
        // std::cout << "Rank " << m_s.rank << " send left to rank "
        //          << m_s.rank_left << std::endl;
        for (unsigned int i = 0; i < m_s.ndx + 2; ++i)
            send_x[i] = m_TCurrent[i][1];
        MPI_Send(send_x, m_s.ndx + 2, MPI_REAL8, m_s.rank_left, tag, comm);
    }
    if (m_s.rank_right >= 0)
    {
        // std::cout << "Rank " << m_s.rank << " receive from right from rank "
        //          << m_s.rank_right << std::endl;
        MPI_Recv(recv_x, m_s.ndx + 2, MPI_REAL8, m_s.rank_right, tag, comm,
                 &status);
        for (unsigned int i = 0; i < m_s.ndx + 2; ++i)
            m_TCurrent[i][m_s.ndy + 1] = recv_x[i];
    }

    // send to right + receive from left
    tag = 2;
    if (m_s.rank_right >= 0)
    {
        // std::cout << "Rank " << m_s.rank << " send right to rank "
        //          << m_s.rank_right << std::endl;
        for (unsigned int i = 0; i < m_s.ndx + 2; ++i)
            send_x[i] = m_TCurrent[i][m_s.ndy];
        MPI_Send(send_x, m_s.ndx + 2, MPI_REAL8, m_s.rank_right, tag, comm);
    }
    if (m_s.rank_left >= 0)
    {
        // std::cout << "Rank " << m_s.rank << " receive from left from rank "
        //          << m_s.rank_left << std::endl;
        MPI_Recv(recv_x, m_s.ndx + 2, MPI_REAL8, m_s.rank_left, tag, comm,
                 &status);
        for (unsigned int i = 0; i < m_s.ndx + 2; ++i)
            m_TCurrent[i][0] = recv_x[i];
    }

    // send down + receive from above
    tag = 3;
    if (m_s.rank_down >= 0)
    {
        // std::cout << "Rank " << m_s.rank << " send down to rank "
        //          << m_s.rank_down << std::endl;
        MPI_Send(m_TCurrent[m_s.ndx], m_s.ndy + 2, MPI_REAL8, m_s.rank_down,
                 tag, comm);
    }
    if (m_s.rank_up >= 0)
    {
        // std::cout << "Rank " << m_s.rank << " receive from above from rank "
        //          << m_s.rank_up << std::endl;
        MPI_Recv(m_TCurrent[0], m_s.ndy + 2, MPI_REAL8, m_s.rank_up, tag, comm,
                 &status);
    }

    // send up + receive from below
    tag = 4;
    if (m_s.rank_up >= 0)
    {
        // std::cout << "Rank " << m_s.rank << " send up to rank " <<
        // m_s.rank_up
        //          << std::endl;
        MPI_Send(m_TCurrent[1], m_s.ndy + 2, MPI_REAL8, m_s.rank_up, tag, comm);
    }
    if (m_s.rank_down >= 0)
    {
        // std::cout << "Rank " << m_s.rank << " receive from below from rank "
        //          << m_s.rank_down << std::endl;
        MPI_Recv(m_TCurrent[m_s.ndx + 1], m_s.ndy + 2, MPI_REAL8, m_s.rank_down,
                 tag, comm, &status);
    }

*/

// get the value from all neigobor cells
void Possion2dHybrid::exchange() {
  // attention if element at the boundy positions the coresponding position is
  // zero
  // dim 0 is y dim 1 is x
  // get the id for neigobors

  // send current value to neigobors
  // get value from curent values
  // MPI_Sendrecv(buffer, 10, MPI_INT, left, 123, buffer2, 10, MPI_INT, right,
  // 123, MPI_COMM_WORLD, &status); MPI_SENDRECV(sendbuf, sendcount, sendtype,
  // dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status)
  // MPI_SENDRECV(&m_cellValue, 1, MPI_DOUBLE, upId, 0, &m_down, 1, MPI_DOUBLE,
  // downId, 1, cart_comm, &status); send current to east and rcv from the west

  // send current to up and rcv from down

  MPI_Status status;

  // use differnet tag for different exchange

  // send to left + receive from right
  int tag = 1;

  // inorder make the data continuous

  // for left edge, there is no element need to send to left
  if (m_blockx != 0) {
    // send to left
    MPI_Send(&m_cellValue[l2i(1, 0)], 1, y_edge_type, m_leftBlockId, tag,
             MPI_COMM_WORLD);
  }
  if (m_blockx != m_blockNumInOneD - 1) {
    // rcv from right
    MPI_Recv(&m_cellValue[l2i(m_localLen + 1, 0)], 1, y_edge_type,
             m_rightBlockId, tag, MPI_COMM_WORLD, &status);
  }

  // send to right + receive from left
  tag = 2;
  // get right value
  if (m_blockx != m_blockNumInOneD - 1) {
    // send to right
    MPI_Send(&m_cellValue[l2i(m_localLen, 0)], 1, y_edge_type, m_rightBlockId,
             tag, MPI_COMM_WORLD);
  }

  if (m_blockx != 0) {
    // rcv from left
    MPI_Recv(&m_cellValue[l2i(0, 0)], 1, y_edge_type, m_leftBlockId, tag,
             MPI_COMM_WORLD, &status);
  }

  // send up + receive from below
  tag = 3;
  if (m_blocky != m_blockNumInOneD - 1) {
    // send to up (use the real value)
    MPI_Send(&m_cellValue[l2i(0, m_localLen)], 1, x_edge_type, m_upBlockId, tag,
             MPI_COMM_WORLD);
  }

  // rcv down value
  if (m_blocky != 0) {
    MPI_Recv(&m_cellValue[l2i(0, 0)], 1, x_edge_type, m_downBlockId, tag,
             MPI_COMM_WORLD, &status);
  }

  // send down + receive from above
  tag = 4;
  if (m_blocky != 0) {
    // send to down
    MPI_Send(&m_cellValue[l2i(0, 0)], 1, x_edge_type, m_downBlockId, tag,
             MPI_COMM_WORLD);
  }
  if (m_blocky != m_blockNumInOneD - 1) {
    // rcv from up
    MPI_Recv(&m_cellValue[l2i(0, m_localLen + 1)], 1, x_edge_type, m_upBlockId,
             tag, MPI_COMM_WORLD, &status);
  }
}

// iterate to the next step
void Possion2dHybrid::iterate() {
  int globalx, globaly;
  double phy_x, phy_y;
  // only iterate the actual value instead of the ghost value
  for (unsigned int j = 1; j <= m_localLen; ++j) {
    for (unsigned int i = 1; i <= m_localLen; ++i) {
      globalx = offset_x + (i - 1);
      globaly = offset_y + (j - 1);
      phy_x = 1.0 * m_dx * globalx;
      phy_y = 1.0 * m_dy * globaly;
      if (globalx == 0 || globalx == (m_globalLen - 1) || globaly == 0 ||
          globaly == (m_globalLen - 1)) {
        m_cellValueNew[l2i(i, j)] = u_exact(phy_x, phy_y);
      } else {
        m_cellValueNew[l2i(i, j)] =
            0.25 * (m_cellValue[l2i(i - 1, j)] + m_cellValue[l2i(i + 1, j)] +
                    m_cellValue[l2i(i, j - 1)] + m_cellValue[l2i(i, j + 1)] +
                    m_fb[l2i(i, j)] * m_dx * m_dy);
      }

      /*
      if (i == 3 && j == 2) {
        std::cout << "debug " << m_cellValue[l2i(i - 1, j)] << ","
                  << m_cellValue[l2i(i + 1, j)] << ","
                  << m_cellValue[l2i(i, j - 1)] << ","
                  << m_cellValue[l2i(i, j + 1)] << "," << m_fb[l2i(i, j)] << ","
                  << m_dx << "," << m_dy << std::endl;
      }
      */
    }
  }

  // make sure the value is the latest one
  m_cellValue = m_cellValueNew;

  return;
}

void Possion2dHybrid::printInfoGetDiff(int myrank, int procNum) {
  // caculate value at each process then gather
  // debug, output the data

  double error = 0;
  int globalx, globaly;
  for (int j = 1; j <= m_localLen; j++) {
    for (int i = 1; i <= m_localLen; i++) {
      error = error + pow((m_cellValue[l2i(i, j)] - m_exact[l2i(i, j)]), 2);
      globalx = offset_x + (i - 1);
      globaly = offset_y + (j - 1);
      // std::cout << "iter " << iter <<" globaly " << globaly + 1 << " globalx
      // " << globalx + 1
      //          << " value " << m_cellValue[l2i(i, j)] << std::endl;
    }
  }
  error = sqrt(error / (double)(m_localLen * m_localLen));
  double totalError = 0;
  // MPI reduce to cacaulte the avg value
  MPI_Reduce(&error, &totalError, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (myrank == 0) {
    std::cout << "avg error for is " << totalError / (1.0 * procNum)
              << std::endl;
  }
  return;
}

// output the data
void Possion2dHybrid::printInfo(int myrank) {
  // this is used to check the results after the exchange operation

  for (int j = 0; j <= m_localLen + 1; j++) {
    for (int i = 0; i <= m_localLen + 1; i++) {
      // std::cout << "debug exchange rank " << myrank << " j " << j << " i " <<
      // i << " value "
      //          << m_cellValue[l2i(i, j)] << std::endl;
    }
  }
  return;
}
