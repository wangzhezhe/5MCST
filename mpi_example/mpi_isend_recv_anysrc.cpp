#include <mpi.h>
#include <unistd.h>
#include <iostream>
#include <vector>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int rank, procs;
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &procs);
    int iterNum = 5;
    int step = 0;
    int tag = 12345;
    MPI_Request sendreq;
    while (step < iterNum)
    {

        // synthetic computing step
        sleep(1);
        int elementNum = step + 1;
        std::vector<int> sendBuffer;
        sendBuffer.resize(elementNum);
        for (int i = 0; i < elementNum; i++)
        {
            sendBuffer[i] = i;
        }
        int dest = (rank + 1) % procs;
        // send messages, not block
        MPI_Isend(sendBuffer.data(), elementNum, MPI_INT, dest, tag, comm, &sendreq);
        std::cout << "rank " << rank << " isend ok for step " << step << std::endl;

        // recv messages
        // the source can be the any source
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, tag, comm, &status);
        int recvNumElement = 0;
        MPI_Get_count(&status, MPI_INT, &recvNumElement);
        std::vector<int> recvBuffer;
        recvBuffer.resize(recvNumElement);
        MPI_Recv(recvBuffer.data(), recvNumElement, MPI_INT, MPI_ANY_SOURCE, tag, comm, &status);

        // check the correctness of recv values
        if (recvNumElement != elementNum)
        {
            std::cout << "recv Num element is wrong" << std::endl;
            exit(-1);
        }

        for (int i = 0; i < recvBuffer.size(); i++)
        {
            if (recvBuffer[i] != i)
            {
                std::cout << "recv wrong value" << std::endl;
                exit(-1);
            }
        }

        std::cout << "rank " << rank << " recv ok for step " << step << std::endl;
        step++;
    }

    MPI_Finalize();
    return 0;
}

//TODO, update, 1 send 1 recv to multiple sender, multiple recievers
//scenario: one req is not arrive we can still process other req
//need to decide, how many recvs, the buffer size of recvs, if each time, these info is different