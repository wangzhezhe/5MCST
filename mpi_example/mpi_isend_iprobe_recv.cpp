#include <mpi.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

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
        // preparing sending data
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

        // synthetic computing step
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // recv messages
        // the source can be the any source

        MPI_Status status;
        int flag = 0;
        while (!flag)
        {
            // spin here when the flag is not ready
            MPI_Iprobe(MPI_ANY_SOURCE, tag, comm, &flag, &status);
        }
        int recvNumElement = 0;
        MPI_Get_count(&status, MPI_INT, &recvNumElement);
        std::vector<int> recvBuffer;
        recvBuffer.resize(recvNumElement);
        MPI_Recv(recvBuffer.data(), recvNumElement, MPI_INT, MPI_ANY_SOURCE, tag, comm, &status);

        // check the correctness of recieved data
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
