#include "possion2d.hpp"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <math.h>
#include <cstring>

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

double u_exact(double x, double y)
{
    double r8_pi = 3.141592653589793;
    double value;

    value = sin(r8_pi * x * y);

    return value;
}

double uxxyy_exact(double x, double y)
{
    double r8_pi = 3.141592653589793;
    double value;

    value = -r8_pi * r8_pi * (x * x + y * y) * sin(r8_pi * x * y);

    return value;
}

//init the boundry value
void Possion2d::init(MPI_Comm &cart_comm)
{
    double phy_x = m_dx * m_x * 1.0;
    double phy_y = m_dy * m_y * 1.0;
    //init the values in ghost
    //it is larger than one than current value
    if (m_x == 0 || m_x == m_len - 1 || m_y == 0 || m_y == m_len - 1)
    {

        this->m_cellValue = u_exact(phy_x, phy_y);
        //std::cout << "uexact x " << phy_x << " y " << phy_y << std::endl;
        this->m_fb = u_exact(phy_x, phy_y);
    }
    else
    {
        this->m_cellValue = 0.0;
        this->m_fb = -uxxyy_exact(phy_x, phy_y);
    }

    //std::cout << "m_x " << m_x << " m_y " << m_y << " m_l " << this->m_left << " m_r " << this->m_right << " m_d " << this->m_down << " m_u " << this->m_up << " fb " << this->m_fb << std::endl;

    MPI_Cart_shift(cart_comm, 1, 1, &m_leftId, &m_rightId);
    MPI_Cart_shift(cart_comm, 0, 1, &m_downId, &m_upId);

    //std::cout << " x " << m_x << " y " << m_y << " lid " << m_leftId << " m_rightId " << m_rightId << " m_downId " << m_downId << " m_upId " << m_upId << std::endl;
    if (m_rank == 0)
    {
        //use original declared pointer, instead of a new pointer
        int totalProcess = m_len * m_len;
        m_rcvBufferLast = (double *)calloc(totalProcess, sizeof(double));
        m_rcvBufferCurrent = (double *)calloc(totalProcess, sizeof(double));
    }
}

/*
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

//get the value from all neigobor cells
void Possion2d::exchange()
{
    //attention if element at the boundy positions the coresponding position is zero
    // dim 0 is y dim 1 is x
    // get the id for neigobors

    //send current value to neigobors
    //get value from curent values
    //MPI_Sendrecv(buffer, 10, MPI_INT, left, 123, buffer2, 10, MPI_INT, right, 123, MPI_COMM_WORLD, &status);
    //MPI_SENDRECV(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status)
    //MPI_SENDRECV(&m_cellValue, 1, MPI_DOUBLE, upId, 0, &m_down, 1, MPI_DOUBLE, downId, 1, cart_comm, &status);
    //send current to east and rcv from the west

    //send current to up and rcv from down

    MPI_Status status;

    //use differnet tag for different exchange

    // send to left + receive from right
    int tag = 1;
    //for left edge, there is no element need to send to left
    if (m_x != 0)
    {
        //send to left
        MPI_Send(&m_cellValue, 1, MPI_DOUBLE, m_leftId, tag, MPI_COMM_WORLD);
    }
    if (m_x != m_len - 1)
    {
        //rcv from right
        MPI_Recv(&m_right, 1, MPI_DOUBLE, m_rightId, tag, MPI_COMM_WORLD, &status);
    }

    // send to right + receive from left
    tag = 2;
    //get right value
    if (m_x != m_len - 1)
    {
        //send to right
        MPI_Send(&m_cellValue, 1, MPI_DOUBLE, m_rightId, tag, MPI_COMM_WORLD);
    }

    if (m_x != 0)
    {
        //rcv from left
        MPI_Recv(&m_left, 1, MPI_DOUBLE, m_leftId, tag, MPI_COMM_WORLD, &status);
    }

    // send up + receive from below
    tag = 3;
    if (m_y != m_len - 1)
    {
        //send to up
        MPI_Send(&m_cellValue, 1, MPI_DOUBLE, m_upId, tag, MPI_COMM_WORLD);
    }

    //rcv down value
    if (m_y != 0)
    {
        MPI_Recv(&m_down, 1, MPI_DOUBLE, m_downId, tag, MPI_COMM_WORLD, &status);
    }

    // send down + receive from above
    tag = 4;
    if (m_y != 0)
    {
        // send to down
        MPI_Send(&m_cellValue, 1, MPI_DOUBLE, m_downId, tag, MPI_COMM_WORLD);
    }
    if (m_y != m_len - 1)
    {
        //rcv from up
        MPI_Recv(&m_up, 1, MPI_DOUBLE, m_upId, tag, MPI_COMM_WORLD, &status);
    }
}

//iterate to the next step
void Possion2d::iterate()
{
    //update current cell value
    //m_cellValue = (m_omega / 4) * (m_left + m_right + m_down + m_up) + (1.0 - m_omega) * m_cellValue;
    if (m_x == 0 || m_x == m_len - 1 || m_y == 0 || m_y == m_len - 1)
    {
        m_cellValue = this->m_fb;
    }
    else
    {
        m_cellValue = 0.25 * (m_left + m_right + m_down + m_up + this->m_fb * m_dx * m_dy);
    }
    return;
}

double Possion2d::printInfoGetDiff(int myrank, int totalProcess, int len, int step)
{

    MPI_Gather(&m_cellValue, 1, MPI_DOUBLE, m_rcvBufferCurrent, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double diff = 0;
    //these can be caculated at every process separately
    double *tempCurrent = m_rcvBufferCurrent;
    double *tempLast = m_rcvBufferLast;
    double exact = 0;
    double error = 0;
    if (myrank == 0)
    {
        for (int y = 0; y < len; y++)
        {
            for (int x = 0; x < len; x++)
            {

                //std::cout << "x " << x << " y " << y << " v " << *temp << std::endl;
                //std::cout << "debug " << exact << " " << *temp << std::endl;
                diff = diff + pow((*tempCurrent - *tempLast), 2);

                exact = u_exact(m_dx * x * 1.0, m_dy * y * 1.0);
                error = error + pow((*tempCurrent - exact), 2);

                tempCurrent++;
                tempLast++;
            }
        }
        //copy only for rank 0
        std::memcpy(m_rcvBufferLast, m_rcvBufferCurrent, m_len * m_len * sizeof(double));
    }
    //std::cout << "debug " << error << std::endl;
    diff = diff / (len * len * 1.0);
    error = error / (len * len * 1.0);
    if (myrank == 0)
    {
        std::cout << "error " << error << std::endl;
    }

    return diff;
}

//output the data
void Possion2d::output(int myrank, int totalProcess, int len, int step)
{
    //std::cout << " x " << m_x
    //          << " y " << m_y << " value " << m_cellValue << " l " << m_left << " r " << m_right << " down " << m_down << " up " << m_up << std::endl;

    // aggregate all information into one process and write data out maybe in vtk format
    // the data is placed in consecutive groups in rank order in the root process recvbuf.
    // refer to this http://www.math-cs.gordon.edu/courses/cps343/presentations/MPI_Collective.pdf
    double *rcvBuffer = NULL;
    if (myrank == 0)
    {
        //use original declared pointer, instead of a new pointer
        rcvBuffer = (double *)malloc(totalProcess * sizeof(double));
    }

    //address of receive buffer (choice, significant only at root)
    //attehtion, the send and rcv should in same size
    //the rcv aims for one slot, not all slots
    //https://www.mcs.anl.gov/research/projects/mpi/mpi-standard/mpi-report-1.1/node70.htm
    MPI_Gather(&m_cellValue, 1, MPI_DOUBLE, rcvBuffer, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //all rank will call this

    if (myrank == 0)
    {

        std::ofstream fout;

        char countstr[50];
        sprintf(countstr, "%03d", step);
        //generate new file
        std::string fileName = "./output/possion2d_" + std::string(countstr) + ".csv";
        fout.open(fileName);
        double *temp = rcvBuffer;
        fout << "x,y,z" << std::endl;
        //output to disk file
        for (int y = 0; y < len; y++)
        {
            for (int x = 0; x < len; x++)
            {
                fout << x << "," << y << "," << *temp << std::endl;
                temp++;
            }
        }
        if (rcvBuffer != NULL)
        {
            free(rcvBuffer);
        }
        fout.close();
    }

    return;
}