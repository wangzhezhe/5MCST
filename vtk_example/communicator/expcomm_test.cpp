
//this exmaple show how to use MPI communicator to transfer the VTK object
//for mpi controller
#include <vtkMPIController.h>
#include "mpi.h"
#include <vtkMPI.h>

//for sphere
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

int main(int argc, char *argv[])
{
    // This is here to avoid false leak messages from vtkDebugLeaks when
    // using mpich. It appears that the root process which spawns all the
    // main processes waits in MPI_Init() and calls exit() when
    // the others are done, causing apparent memory leaks for any objects
    // created before MPI_Init().
    MPI_Init(&argc, &argv);

    // Note that this will create a vtkMPIController if MPI
    // is configured, vtkThreadedController otherwise.
    // init the controller
    vtkMPIController *controller = vtkMPIController::New();
    controller->Initialize(&argc, &argv, 1);
    int rank = controller->GetLocalProcessId();
    std::cout << "current rank is " << rank << std::endl;

    if (rank == 0)
    {
        //create sphere source
        vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
        sphereSource->SetThetaResolution(8);
        sphereSource->SetPhiResolution(8);
        sphereSource->SetStartTheta(0.0);
        sphereSource->SetEndTheta(360.0);
        sphereSource->SetStartPhi(0.0);
        sphereSource->SetEndPhi(180.0);
        sphereSource->LatLongTessellationOff();

        sphereSource->Update();
        vtkSmartPointer<vtkPolyData> polyData = sphereSource->GetOutput();

        // send this poly data
        // vtkDataObject *data, int remoteId, int tag
        std::cout << "---generate sphere source---\n";
        polyData->PrintSelf(std::cout, vtkIndent(0));
        std::cout << "---rank0 send sphere source---\n";
        controller->Send(polyData, 1, 12345);
    }
    if (rank == 1)
    {
        //recieve the poly data
        vtkNew<vtkPolyData> rcvpData;
        int tag = 12345;
        controller->Receive(rcvpData, 0, 12345);

        //print the results
        std::cout << "---rank1 recieve sphere source---\n";
        rcvpData->PrintSelf(std::cout, vtkIndent(5));
    }

    return 0;
}