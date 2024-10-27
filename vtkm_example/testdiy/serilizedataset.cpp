#include <vtkm/cont/Initialize.h>
#include <vtkm/cont/DataSetBuilderUniform.h>
#include <vtkm/cont/Serialization.h>
#include <vtkm/thirdparty/diy/diy.h>
#include <vtkm/cont/DIYMemoryManagement.h>
#include <vtkm/cont/EnvironmentTracker.h>

vtkm::cont::DataSet createDataSet()
{
    vtkm::cont::DataSetBuilderUniform dsb;
    constexpr vtkm::Id2 dimensions(3, 2);
    vtkm::cont::DataSet dataSet = dsb.Create(dimensions);

    constexpr vtkm::Id nVerts = 6;
    constexpr vtkm::Float32 var[nVerts] = {10.1f, 20.1f, 30.1f, 40.1f, 50.1f, 60.1f};

    dataSet.AddPointField("pointvar", var, nVerts);

    constexpr vtkm::Float32 cellvar[2] = {100.1f, 200.1f};
    dataSet.AddCellField("cellvar", cellvar, 2);

    return dataSet;
}

using CellSetTypes = vtkm::List<vtkm::cont::CellSetExplicit<>,
                                vtkm::cont::CellSetSingleType<>,
                                vtkm::cont::CellSetStructured<1>,
                                vtkm::cont::CellSetStructured<2>,
                                vtkm::cont::CellSetStructured<3>>;

using DataSetWrapper = vtkm::cont::DataSetWithCellSetTypes<CellSetTypes>;

template <typename T>
struct Block
{
    T send;
    T received;
};

template <typename T>
void TestSerialization(const T &obj)
{
    auto comm = vtkm::cont::EnvironmentTracker::GetCommunicator();
    vtkmdiy::Master master(comm);

    auto nblocks = comm.size();
    vtkmdiy::RoundRobinAssigner assigner(comm.size(), nblocks);

    std::vector<int> gids;
    assigner.local_gids(comm.rank(), gids);
    VTKM_ASSERT(gids.size() == 1);
    auto gid = gids[0];

    Block<T> block;
    block.send = obj;

    vtkmdiy::Link *link = new vtkmdiy::Link;
    vtkmdiy::BlockID neighbor;

    // send neighbor
    neighbor.gid = (gid < (nblocks - 1)) ? (gid + 1) : 0;
    neighbor.proc = assigner.rank(neighbor.gid);
    link->add_neighbor(neighbor);

    std::cout << "rank " << comm.rank() << " send gid " << neighbor.gid << " send proc " << neighbor.proc << std::endl;

    // recv neighbor
    neighbor.gid = (gid > 0) ? (gid - 1) : (nblocks - 1);
    neighbor.proc = assigner.rank(neighbor.gid);
    link->add_neighbor(neighbor);

    std::cout << "rank " << comm.rank() << " recv gid " << neighbor.gid << " recv proc " << neighbor.proc << std::endl;

    master.add(gid, &block, link);

    // compute, exchange, compute
    master.foreach ([](Block<T> *b, const vtkmdiy::Master::ProxyWithLink &cp)
                    { cp.enqueue(cp.link()->target(0), b->send); });

    vtkm::cont::DIYMasterExchange(master);

    master.foreach ([](Block<T> *b, const vtkmdiy::Master::ProxyWithLink &cp)
                    { cp.dequeue(cp.link()->target(1).gid, b->received); });

    comm.barrier();

    // check recv
    if (comm.rank() > 0)
    {
        block.received.PrintSummary(std::cout);
    }
}

int main(int argc, char *argv[])
{
    vtkm::cont::InitializeResult initResult = vtkm::cont::Initialize(
        argc, argv, vtkm::cont::InitializeOptions::DefaultAnyDevice);

    // create a data set
    vtkm::cont::DataSet inData = createDataSet();

    // serilizae data set
    std::cout << "---Original dataset before serilization" << std::endl;
    inData.PrintSummary(std::cout);

    std::cout << "---Testing data serilization" << std::endl;

    // mangled_diy_namespace::MemoryBuffer bb;
    mangled_diy_namespace::MemoryBuffer bb;
    mangled_diy_namespace::Serialization<DataSetWrapper> serialization;

    vtkmdiy::save(bb, DataSetWrapper{inData});

    // vtkm::cont::DataSet dataDeserilized;

    DataSetWrapper dataDeserilized;

    // need to set cell type
    vtkmdiy::load(bb, dataDeserilized);
    // dataDeserilized.PrintSummary(std::cout);

    // vtkmdiy::MemoryBuffer bb;
    // vtkmdiy::save(bb, inData);

    // results after the serilization
    // vtkm::cont::DataSet inDataDeserilized;
    // inDataDeserilized.PrintSummary(std::cout);

    // vtkmdiy::load(bb, inDataDeserilized);

    // std::cout << "deserilized dataset:" << std::endl;

    // inDataDeserilized.PrintSummary(std::cout);
    // TestSerialization(inData);
}
