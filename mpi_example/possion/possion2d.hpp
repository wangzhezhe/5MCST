#ifndef POSSION2D
#define POSSION2D

#include <mpi.h>
#include <fstream>
#include <iostream>

typedef struct Possion2d{
    Possion2d(int rank, int len, int x, int y):m_rank(rank), m_len(len),m_x(x),m_y(y){
        m_dx = 1.0/(1.0*(len-1));
        m_dy = 1.0/(1.0*(len-1));
        if(m_rank==0){
            std::cout << "m_dx " << m_dx << " m_dy " << m_dy << std::endl;
        }
    };
    
    //init the boundry value
    void init(MPI_Comm& cart_comm);
    //get the value from all neigobor cells
    void exchange();
    //iterate to the next step
    void iterate();
    //void output
    void output(int myrank, int totalProcess, int len, int step);
    //compute diff of two iterations and the error compared with acurate values
    double printInfoGetDiff(int myrank, int totalProcess, int len, int step);

    //id
    int m_leftId, m_rightId, m_downId, m_upId;
    
    //value

    double m_up,m_down,m_left,m_right=0;
    double m_cellValue = 0;
    
    //this represents the global size
    int m_len;
    //m_x and m_y are coords for mesh 
    //it needs another level of transformation from this into the actual coords in physical domain
    const int m_x;
    const int m_y;
    double m_omega = 0.8; 
    int m_rank;

    double m_dx;
    double m_dy;

    double m_fb;

    //this is only meningful for rank 0
    double *m_rcvBufferLast = NULL;
    double *m_rcvBufferCurrent = NULL;


}Possion2d;



#endif