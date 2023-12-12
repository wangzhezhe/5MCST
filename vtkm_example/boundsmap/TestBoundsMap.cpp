#include "BoundsMap.h"
#include <vtkm/cont/Initialize.h>
#include <vtkm/cont/DataSetBuilderUniform.h>
#include <vector>
#include <mpi.h>
vtkm::cont::DataSet CreateUniformData(vtkm::Vec2f origin)
{
    vtkm::cont::DataSetBuilderUniform dsb;
    vtkm::Id2 dimensions(3, 2);
    vtkm::cont::DataSet dataSet = dsb.Create(dimensions, origin, vtkm::Vec2f(1, 1));
    constexpr vtkm::Id nVerts = 6;
    constexpr vtkm::Float32 var[nVerts] = {10.1f, 20.1f, 30.1f, 40.1f, 50.1f, 60.1f};
    dataSet.AddPointField("pointVar", var, nVerts);
    constexpr vtkm::Float32 cellvar[2] = {100.1f, 200.1f};
    dataSet.AddCellField("cellVar", cellvar, 2);
    return dataSet;
}

int main(int argc, char *argv[])
{
    // MPI init
    MPI_Init(&argc, &argv);

    // VTKm init
    vtkm::cont::InitializeResult initResult = vtkm::cont::Initialize(
        argc, argv, vtkm::cont::InitializeOptions::DefaultAnyDevice);

    auto comm = vtkm::cont::EnvironmentTracker::GetCommunicator();
    int Rank = comm.rank();
    int Size = comm.size();

    if (Rank == 0)
    {
        std::cout << "comm size: " << Size << std::endl;
    }

    // test data sets with automatic bounds map
    std::vector<vtkm::cont::DataSet> inputDataSets;

    vtkm::cont::DataSet dataSet0 = CreateUniformData(vtkm::Vec2f(3.0 * Rank, 0.0));
    vtkm::cont::DataSet dataSet1 = CreateUniformData(vtkm::Vec2f(3.0 * Rank + 2, 0.0));

    inputDataSets.push_back(dataSet0);
    inputDataSets.push_back(dataSet1);

    // builds bounds map in an distributed way
    vtkm::filter::flow::internal_test::BoundsMap boundsMap(inputDataSets);

    // get global bounds
    auto bounds = boundsMap.GetGlobalBounds();
    if (Rank == 0)
    {
        int totalBlockNum = boundsMap.GetTotalNumBlocks();
        std::cout << "TotalBlockNum is " << totalBlockNum << std::endl;
        std::cout << "global bounds " << bounds << std::endl;
        for (int i = 0; i < 8; i++)
        {
            auto blockBounds = boundsMap.GetBlockBounds(i);
            std::cout << "blockBounds for block id " << i << " is " << blockBounds << std::endl;
        }
    }

    auto localBlockid = boundsMap.GetLocalBlockId(0);
    std::cout << "Rank " << Rank << " local block id(local view) 0, the global id is " << localBlockid << std::endl;
    localBlockid = boundsMap.GetLocalBlockId(1);
    std::cout << "Rank " << Rank << " local block id(local view) 1, the global id is " << localBlockid << std::endl;

    if (Rank == 0)
    {
        std::vector<vtkm::Id> blockids = boundsMap.FindBlocks({11.0, 0.5, 0});
        for (int i = 0; i < blockids.size(); i++)
        {
            std::cout << "Find block id (in global view) that contain current point: " << blockids[i] << std::endl;
        }

        blockids = boundsMap.FindBlocks({11.0, 0.5, 0}, 6);
        for (int i = 0; i < blockids.size(); i++)
        {
            std::cout << "Find block id (in global view) that contain current point, with ignore block: " << blockids[i] << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}