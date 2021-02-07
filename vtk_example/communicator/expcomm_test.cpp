
// this exmaple show how to use MPI communicator to transfer the VTK object
// for mpi controller
#include <vtkMPI.h>
#include <vtkMPIController.h>

#include "mpi.h"

// for sphere
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  // Note that this will create a vtkMPIController if MPI
  // is configured, vtkThreadedController otherwise.
  // init the controller
  vtkMPIController* controller = vtkMPIController::New();
  controller->Initialize(&argc, &argv, 1);
  int rank = controller->GetLocalProcessId();
  std::cout << "current rank is " << rank << std::endl;
  int tag = 12345;
  if (rank == 0) {
    // create sphere source
    vtkSmartPointer<vtkSphereSource> sphereSource =
        vtkSmartPointer<vtkSphereSource>::New();
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
    controller->Send(polyData, 1, tag);
  }
  if (rank == 1) {
    // recieve the poly data
    vtkNew<vtkPolyData> rcvpData;
    controller->Receive(rcvpData, 0, tag);

    // print the results
    std::cout << "---rank1 recieve sphere source---\n";
    rcvpData->PrintSelf(std::cout, vtkIndent(5));
  }

  return 0;
}