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

    MPI_Request sendreq1;
    MPI_Request sendreq2;

    std::vector<MPI_Request> irecvRequestList;

    while (step < iterNum)
    {
        // recv message
        // assuming the src rank may from any source
        // go through each possible sender
        std::vector<std::vector<int>> recvBuffer;
        int bufferCount = 0;
        for (int srcid = 0; srcid < rank; srcid++)
        {
            int flag = 0;
            MPI_Status status;

            MPI_Iprobe(srcid, tag, comm, &flag, &status);
            // if flag is 0,
            // the src may not send data
            // the src may send data but it is not ready
            // how to know which is the right situation here?
            if (flag)
            {
                // when the messge from potential src is avalibale
                int recvNumElement = 0;
                MPI_Get_count(&status, MPI_INT, &recvNumElement);
                std::vector<int> recvBufferInner;
                recvBufferInner.resize(recvNumElement);
                recvBuffer.push_back(recvBufferInner);
                MPI_Request request;
                MPI_Irecv(recvBuffer[bufferCount].data(), recvNumElement, MPI_INT, MPI_ANY_SOURCE, tag, comm, &request);
                bufferCount++;
                irecvRequestList.push_back(request);
            }
        }

        // if there is no avalibale requests
        // flag is 0
        // MPI_Irecv will not be called, how to execute wait operation here
        if (bufferCount > 0)
        {
            // wait for the completion of assocaited isend/irecv
            for (int i = 0; i < bufferCount; i++)
            {
                MPI_Status status;
                MPI_Wait(&irecvRequestList[i], &status);
            }
            irecvRequestList.clear();
        }

        // check resutls
        std::cout << "rank " << rank << " step " << step << " rcv buffer size " << recvBuffer.size() << std::endl;

        // synthetic computing step
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // preparing sending data after synthetic computation
        int elementNum = step + 1;
        std::vector<int> sendBuffer;
        sendBuffer.resize(elementNum);
        for (int i = 0; i < elementNum; i++)
        {
            sendBuffer[i] = i;
        }
        // send messages, assume there are multiple dest
        int dest1 = (rank + 1) % procs;
        int dest2 = (rank + 2) % procs;
        MPI_Isend(sendBuffer.data(), elementNum, MPI_INT, dest1, tag, comm, &sendreq1);
        // std::cout << "rank " << rank << " isend to " << dest1 << " ok for step " << step << std::endl;
        MPI_Isend(sendBuffer.data(), elementNum, MPI_INT, dest2, tag, comm, &sendreq2);
        // std::cout << "rank " << rank << " isend to " << dest2 << " ok for step " << step << std::endl;

        step++;
    }

    MPI_Finalize();
    return 0;
}

// TODO, update, 1 send 1 recv to multiple sender, multiple recievers
// scenario: one req is not arrive we can still process other req
// need to decide, how many recvs, the buffer size of recvs, if each time, these info is different
// iprob for one reciver might not usefule a lot (maybe overlapping with computation)
// iprov for multiple recievers might be useful
// MPI_Wait or MPI_TestSome can be used for mutiple send recivers