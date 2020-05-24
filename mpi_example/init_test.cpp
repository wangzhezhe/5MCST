

#include <cstdlib>
#include <iostream>
#include <string>
#include <mpi.h>

void init_mpi(size_t gridL)
{
    // Dimension of process grid
    size_t npx, npy, npz;
    // Coordinate of this rank in process grid
    size_t px, py, pz;
    // Dimension of local array
    size_t size_x, size_y, size_z;
    // Offset of local array in the global array
    size_t offset_x, offset_y, offset_z;

    int rank, procs;
    int west, east, up, down, north, south;
    MPI_Comm comm = MPI_COMM_WORLD;

    MPI_Comm cart_comm;
    // MPI datatypes for halo exchange
    MPI_Datatype xy_face_type;
    MPI_Datatype xz_face_type;
    MPI_Datatype yz_face_type;

    int dims[3] = {};
    const int periods[3] = {1, 1, 1};
    int coords[3] = {};

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &procs);
    MPI_Dims_create(procs, 3, dims);
    npx = dims[0];
    npy = dims[1];
    npz = dims[2];

    MPI_Cart_create(comm, 3, dims, periods, 0, &cart_comm);
    MPI_Cart_coords(cart_comm, rank, 3, coords);
    px = coords[0];
    py = coords[1];
    pz = coords[2];

    size_x = (gridL + npx - 1) / npx;
    size_y = (gridL + npy - 1) / npy;
    size_z = (gridL + npz - 1) / npz;

    offset_x = size_x * px;
    offset_y = size_y * py;
    offset_z = size_z * pz;

    //std::cout << "bf_size_x " << size_x << " bf_size_y " << size_y << " bf_size_z " << size_z << std::endl;

    //current size_x is the case with the adjusted value
    //we need to decrease the adjusted value
    //caculate the reminder
    //size_x = size_x - [(size_x*npx)-gridL]
    if (px == npx - 1)
    {
        size_x -= size_x * npx - gridL;
    }
    if (py == npy - 1)
    {
        size_y -= size_y * npy - gridL;
    }
    if (pz == npz - 1)
    {
        size_z -= size_z * npz - gridL;
    }

    MPI_Cart_shift(cart_comm, 0, 1, &west, &east);
    MPI_Cart_shift(cart_comm, 1, 1, &down, &up);
    MPI_Cart_shift(cart_comm, 2, 1, &south, &north);

    // XY faces: size_x * (size_y + 2)
    MPI_Type_vector(size_y + 2, size_x, size_x + 2, MPI_DOUBLE, &xy_face_type);
    MPI_Type_commit(&xy_face_type);

    // XZ faces: size_x * size_z
    MPI_Type_vector(size_z, size_x, (size_x + 2) * (size_y + 2), MPI_DOUBLE,
                    &xz_face_type);
    MPI_Type_commit(&xz_face_type);

    // YZ faces: (size_y + 2) * (size_z + 2)
    MPI_Type_vector((size_y + 2) * (size_z + 2), 1, size_x + 2, MPI_DOUBLE,
                    &yz_face_type);
    MPI_Type_commit(&yz_face_type);

    //if(rank==0){
    std::cout << "rank " << rank << std::endl;
    std::cout << "npx " << npx << " npy " << npy << " npz " << npz << std::endl;
    std::cout << "size_x " << size_x << " size_y " << size_y << " size_z " << size_z << std::endl;
    std::cout << "offset_x " << offset_x << " offset_y " << offset_y << " offset_z " << offset_z << std::endl;
    std::cout << "west " << west << " east " << east << 
    " down " << down <<" up " << up <<
    " south " << south <<" north " << north <<std::endl;
    //}

}

int main(int argc, char **argv)
{

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    size_t L = 0;
    if(argc!=2){
        std::cout << "init_test <gridSize>\n";
        exit(0); 
    }
    L = (size_t)std::stoi(argv[1]);
    init_mpi(L);
}