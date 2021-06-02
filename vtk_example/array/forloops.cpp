#include <vtkAOSDataArrayTemplate.h>
#include <vtkArrayDispatch.h>
#include <vtkDataArrayAccessor.h>
#include <vtkDataArrayRange.h>
#include <vtkDoubleArray.h>
#include <vtkGenericDataArray.h>
#include <vtkSmartPointer.h>

#include <iostream>
#include <type_traits>
#include <vector>

// refer to https://blog.kitware.com/c11-for-range-support-in-vtk/
// refer to https://blog.kitware.com/new-data-array-layouts-in-vtk-7-1/
// refer to https://blog.kitware.com/working-with-vtkdataarrays-2019-edition/
// refer to https://discourse.vtk.org/t/get-the-raw-pointer-from-the-vtkpoints/4894/12

// using naive way to go through the array
void naivemag3(vtkDataArray *vectors, vtkDataArray *magnitudes) {
  std::cout << "------test naivemag3 naive" << std::endl;
  const vtkIdType numTuples = vectors->GetNumberOfTuples();
  std::array<double, 3> tuple;
  for (vtkIdType t = 0; t < numTuples; ++t) {
    vectors->GetTuple(t, tuple.data());
    double mag = 0.;
    for (double comp : tuple) {
      // std::cout << "comp " << comp << std::endl;
      mag += comp * comp;
    }
    mag = std::sqrt(mag);
    std::cout << "mag " << mag << std::endl;

    // assume space is allocated
    magnitudes->SetTuple(t, &mag);
  }
}

// GetVoidPointer, mag3GetPointer call mag3Trampoline then call mag3Helper
template <typename VecType, typename MagType>
void mag3Helper(VecType *vecs, MagType *mags, vtkIdType numTuples) {
  for (vtkIdType t = 0; t < numTuples; ++t) {
    MagType mag = 0;
    for (size_t i = 0; i < 3; ++i) {
      // std::cout << "vecs " << *vecs << std::endl;
      mag += static_cast<MagType>((*vecs) * (*vecs));
      ++vecs;
    }
    *mags = std::sqrt(mag);
    std::cout << "mag " << *mags << std::endl;
    ++mags;
  }
}

template <typename VecType>
void mag3Trampoline(VecType *vecs, vtkDataArray *mags, vtkIdType numTuples) {
  // Resolve mags data type:
  switch (mags->GetDataType()) {
    vtkTemplateMacro(mag3Helper(
        vecs, static_cast<VTK_TT *>(mags->GetVoidPointer(0)), numTuples));
    default:
      std::cout << "error at mag3Trampoline" << std::endl;
  }
}

void mag3GetPointer(vtkDataArray *vecs, vtkDataArray *mags) {
  std::cout << "------test mag3GetPointer" << std::endl;
  const vtkIdType numTuples = vecs->GetNumberOfTuples();
  // Resolve vecs data type:
  switch (vecs->GetDataType()) {
    vtkTemplateMacro(mag3Trampoline(
        static_cast<VTK_TT *>(vecs->GetVoidPointer(0)), mags, numTuples));
    default:
      std::cout << "error at mag3GetPointer" << std::endl;
  }
}

// mag3 explicit types
// not sure how to create the vtkGenericDataArray
template <typename ArrayT1, typename ArrayT2>
void mag3Explicit(ArrayT1 *vectors, ArrayT2 *magnitudes)

{
  std::cout << "------test mag3Explicit" << std::endl;
  using VecType = typename ArrayT1::ValueType;
  using MagType = typename ArrayT2::ValueType;

  const vtkIdType numTuples = vectors->GetNumberOfTuples();

  for (vtkIdType t = 0; t < numTuples; ++t) {
    MagType mag = 0;
    for (int c = 0; c < 3; ++c) {
      VecType comp = vectors->GetTypedComponent(t, c);
      mag += static_cast<MagType>(comp * comp);
    }
    mag = std::sqrt(mag);
    std::cout << "mag " << mag << std::endl;
    magnitudes->SetTypedComponent(t, 0, mag);
  }
}

struct Mag3Worker1 {
  template <typename VecArray, typename MagArray>
  void operator()(VecArray *vecs, MagArray *mags) {
    // The Accessor types:
    using VecAccess = vtkDataArrayAccessor<VecArray>;
    using MagAccess = vtkDataArrayAccessor<MagArray>;

    // The "APITypes"
    // (explicit-type when possible, double for plain vtkDataArrays)
    using VecType = typename VecAccess::APIType;
    using MagType = typename MagAccess::APIType;

    // Tell the compiler the tuple sizes to enable optimizations:
    VTK_ASSUME(vecs->GetNumberOfComponents() == 3);
    VTK_ASSUME(mags->GetNumberOfComponents() == 1);

    const vtkIdType numTuples = vecs->GetNumberOfTuples();

    VecAccess vecAccess{vecs};
    MagAccess magAccess{mags};

    for (vtkIdType t = 0; t < numTuples; ++t) {
      MagType mag = 0;
      for (int c = 0; c < 3; ++c) {
        VecType comp = vecAccess.Get(t, c);
        mag += static_cast<MagType>(comp * comp);
      }
      mag = std::sqrt(mag);
      std::cout << "worker1 mag " << mag << std::endl;
      magAccess.Set(t, 0, mag);
    }
  }
};


struct Mag3Worker2 {
  template <typename VecArray, typename MagArray>
  void operator()(VecArray *vecs, MagArray *mags) {
    // Create range objects:
    // refer to this https://vtk.org/doc/nightly/html/classvtkArrayDispatch.html
    const auto vecRange = vtk::DataArrayTupleRange<3>(vecs);
    auto magRange = vtk::DataArrayValueRange<1>(mags);

    using VecType = typename decltype(vecRange)::ComponentType;
    using MagType = typename decltype(magRange)::ValueType;

    auto magIter = magRange.begin();
    for (const auto &vecTuple : vecRange) {
      MagType mag = 0;
      for (const VecType comp : vecTuple) {
        mag += static_cast<MagType>(comp * comp);
      }
      *magIter = std::sqrt(mag);
      std::cout << "worker2 mag " << *magIter << std::endl;
      magIter++;
    }
  }
};

struct Mag3Worker3 {
  template <typename VecArray, typename MagArray>
  void operator()(VecArray *vecs, MagArray *mags) {
    // Create range objects:
    // should we do things like this?
    // vtk::DataArrayTupleRange<VecArray,3>(vecs);
    // vtk::DataArrayTupleRange<MagArray,3>(vecs);
    const auto vecRange = vtk::DataArrayTupleRange<3>(vecs);
    auto magRange = vtk::DataArrayValueRange<1>(mags);

    // using VecTuple = typename decltype(vecRange)::ComponentType;
    // using MagType = typename decltype(magRange)::ValueType;

    using VecTuple = typename decltype(vecRange)::const_reference;
    using MagType = typename decltype(magRange)::ValueType;

    // Per-tuple magnitude functor for std::transform:
    auto computeMag = [](const VecTuple &tuple) -> MagType {
      MagType mag = 0;
      for (const auto &comp : tuple) {
        mag += static_cast<MagType>(comp * comp);
      }

      std::cout << "worker3 mag " << std::sqrt(mag) << std::endl;
      return std::sqrt(mag);
    };

    std::transform(vecRange.cbegin(), vecRange.cend(), magRange.begin(),
                   computeMag);
  }
};

void mag3Dispatch1(vtkDataArray *vecs, vtkDataArray *mags) {
  std::cout << "------test mag3Dispatch1" << std::endl;
  Mag3Worker1 worker1;

  // Create a dispatcher. We want to generate fast-paths for when
  // vecs and mags both use doubles or floats, but fallback to a
  // slow path for any other situation.
  using Dispatcher =
      vtkArrayDispatch::Dispatch2ByValueType<vtkArrayDispatch::Reals,
                                             vtkArrayDispatch::Reals>;

  if (!Dispatcher::Execute(vecs, mags, worker1)) {
    // Otherwise fallback to using the vtkDataArray API.
    worker1(vecs, mags);
  }
}

void mag3Dispatch2(vtkDataArray *vecs, vtkDataArray *mags) {
  std::cout << "------test mag3Dispatch2" << std::endl;
  Mag3Worker2 worker2;

  // Create a dispatcher. We want to generate fast-paths for when
  // vecs and mags both use doubles or floats, but fallback to a
  // slow path for any other situation.
  using Dispatcher =
      vtkArrayDispatch::Dispatch2ByValueType<vtkArrayDispatch::Reals,
                                             vtkArrayDispatch::Reals>;

  // Generate optimized workers when mags/vecs are both float|double
  if (!Dispatcher::Execute(vecs, mags, worker2)) {
    // Otherwise fallback to using the vtkDataArray API.
    worker2(vecs, mags);
  }
}

void mag3Dispatch3(vtkDataArray *vecs, vtkDataArray *mags) {
  std::cout << "------test mag3Dispatch3" << std::endl;
  Mag3Worker3 worker3;

  // Create a dispatcher. We want to generate fast-paths for when
  // vecs and mags both use doubles or floats, but fallback to a
  // slow path for any other situation.
  using Dispatcher =
      vtkArrayDispatch::Dispatch2ByValueType<vtkArrayDispatch::Reals,
                                             vtkArrayDispatch::Reals>;

  // Generate optimized workers when mags/vecs are both float|double
  if (!Dispatcher::Execute(vecs, mags, worker3)) {
    // Otherwise fallback to using the vtkDataArray API.
    worker3(vecs, mags);
  }
}

int main() {
  // init data
  auto darray = vtkSmartPointer<vtkDoubleArray>::New();
  auto results = vtkSmartPointer<vtkDoubleArray>::New();

  // the number of componbents must be set in advance
  darray->SetNumberOfComponents(3);
  results->SetNumberOfComponents(1);

  int TupleNum = 10;

  darray->SetNumberOfTuples(TupleNum);
  results->SetNumberOfTuples(TupleNum);

  for (vtkIdType i = 0; i < TupleNum; i++) {
    double tuple[3] = {i * 0.1, i * 0.2, i * 0.3};
    // std::array<double, 3> tuple = {{i * 0.1, i * 0.2, i * 0.3}};
    // std::cout << tuple[0] << "," << tuple[1] << "," << tuple[2] << std::endl;

    // if the numer of tuple is not set in advance, we can use InsertTuple
    // darray->InsertTuple(i, tuple.data());
    // darray->InsertTuple(i, tuple);
    darray->SetTuple(i, tuple);
  }

  // using naive API
  naivemag3(darray, results);

  // reset results to zero
  for (vtkIdType i = 0; i < TupleNum; i++) {
    double v = 0;
    results->SetTuple(i, &v);
  }

  // using get raw pointer
  mag3GetPointer(darray, results);

  // reset results to zero
  for (vtkIdType i = 0; i < TupleNum; i++) {
    double v = 0;
    results->SetTuple(i, &v);
  }

  // instantiate with explicit type
  // not sure how to create a genericDataArray explicitly
  mag3Explicit<vtkAOSDataArrayTemplate<double>,
               vtkAOSDataArrayTemplate<double>>(darray, results);

  // worker and dispatcher, there are three different types of worker
  mag3Dispatch1(darray, results);

  mag3Dispatch2(darray, results);

  mag3Dispatch3(darray, results);

  return 0;
}