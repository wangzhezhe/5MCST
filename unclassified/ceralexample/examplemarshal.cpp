
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/types/vector.hpp>
#include <iostream>
#include <sstream>
#include <vector>

class Object {
 public:
  Object(){};
  Object(unsigned width, unsigned height, unsigned depth, double z_offset,
         float range = 1.2, unsigned nblocks = 1)
      : m_width(width),
        m_height(height),
        m_depth(depth + 1),
        m_extents(6),
        m_origin(3),
        m_data(width * height * (depth + 1)),
        m_z_offset(z_offset),
        m_range(range),
        m_nblocks(nblocks) {
    // initilization
  }

  Object(const Object&) = default;
  Object(Object&&) = default;
  Object& operator=(const Object&) = default;
  Object& operator=(Object&&) = default;
  ~Object() = default;

  bool compare(Object& other) {
    if (other.m_width != m_width || other.m_height != m_height ||
        other.m_depth != m_depth) {
      std::cout << "unequal part1" << std::endl;
      return false;
    }
    if (other.m_z_offset != m_z_offset || other.m_range != m_range ||
        other.m_nblocks != m_nblocks) {
      std::cout << "unequal part2" << std::endl;
      return false;
    }
    if (other.m_extents.size() != m_extents.size() ||
        other.m_origin.size() != m_origin.size() ||
        other.m_data.size() != m_data.size()) {
      std::cout << "unequal part3" << std::endl;
      return false;
    }
    for (int i = 0; i < m_extents.size(); i++) {
      if (other.m_extents[i] != m_extents[i]) {
        std::cout << "unequal m_extents" << std::endl;
        return false;
      }
    }
    for (int i = 0; i < m_origin.size(); i++) {
      if (other.m_origin[i] != m_origin[i]) {
        std::cout << "unequal m_origin" << std::endl;
        return false;
      }
    }
    for (int i = 0; i < m_data.size(); i++) {
      if (other.m_data[i] != m_data[i]) {
        std::cout << "unequal m_data" << std::endl;
        return false;
      }
    }
    return true;
  }

  void caculateSize() {
    size_t stackVar;
    stackVar = sizeof(m_width) + sizeof(m_height) + sizeof(m_depth) +
               sizeof(m_extents) + sizeof(m_origin) + sizeof(m_data) +
               sizeof(m_z_offset) + sizeof(m_range) + sizeof(m_nblocks);
    std::cout << "major ariables";
    std::cout << " m_width " << sizeof(m_width) << ",";
    std::cout << " m_extents " << sizeof(m_extents) << ",";
    std::cout << " m_origin " << sizeof(m_origin) << ",";
    std::cout << std::endl;

    std::cout << " stackVar " << stackVar << std::endl;

    size_t heapVar = sizeof(int) * m_extents.size() +
                     sizeof(double) * m_origin.size() +
                     sizeof(int) * m_data.size();

    std::cout << " heapVar " << heapVar << std::endl;
  }

  // binary output based on cereal
  // https://uscilab.github.io/cereal/serialization_archives.html
  // the function should be public one!
  template <typename A>
  void serialize(A& ar) {
    ar& m_width;
    ar& m_height;
    ar& m_depth;
    ar& m_extents;
    ar& m_origin;
    ar& m_data;
    ar& m_z_offset;
    ar& m_range;
    ar& m_nblocks;
  }

 private:
  size_t m_width;
  size_t m_height;
  size_t m_depth;
  std::vector<int> m_extents;
  std::vector<double> m_origin;
  std::vector<int> m_data;
  unsigned m_z_offset;
  float m_range;
  unsigned m_nblocks;
};

int main() {
  static unsigned LEN = 10;
  Object ob(LEN, LEN, LEN, 1, 1.2, 5);

  std::stringstream ss;  // any stream can be used

  cereal::BinaryOutputArchive oarchive(ss);  // Create an output archive

  oarchive(ob);  // Write the data to the archive

  // oarchive.saveBinaryValue(mb, sizeof(mb), "mb");

  std::string bufferedstr = ss.str();

  std::cout << "serilized size " << bufferedstr.size() << " original size "
            << sizeof(ob);

  // deserialize
  std::stringstream is(bufferedstr);

  cereal::BinaryInputArchive archive_in(is);
  Object mbnew;
  archive_in(mbnew);

  bool ifequal = ob.compare(mbnew);
  std::cout << " ifequal " << ifequal << std::endl;

  mbnew.caculateSize();

  return 0;
}