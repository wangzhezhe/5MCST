/// Custom VTK to VTK-m data set conversion. This is to handle fields with arbitrary
/// tuple sizes. This is fixed in VTK with MR !10763
/// (https://gitlab.kitware.com/vtk/vtk/-/merge_requests/10763).
/// Later versions of ParaView will not need this and can use the standard VTK features.

#ifndef VTKTOVTKM_H
#define VTKTOVTKM_H

#include <vtkm/cont/DataSet.h>

class vtkDataArray;
class vtkDataSet;
class vtkPoints;

namespace vtktovtkm
{

vtkm::cont::UnknownArrayHandle Convert(vtkDataArray* input);

vtkm::cont::Field ConvertField(vtkDataArray* input, int association);
vtkm::cont::CoordinateSystem ConvertCoordinates(vtkPoints* input);
vtkm::cont::CoordinateSystem ConvertCoordinates(
  vtkDataArray* xArray, vtkDataArray* yArray, vtkDataArray* zArray);

void ProcessFields(vtkDataSet* input, vtkm::cont::DataSet& dataset);

vtkm::cont::DataSet Convert(vtkDataSet* input);

} // namespace vtktovtkm

/// This is new code that needs to be pushed to VTK's fromvtkm namespace.
namespace vtkmtovtk
{

vtkDataArray* Convert(const vtkm::cont::UnknownArrayHandle& input, const std::string& name);

vtkDataArray* Convert(const vtkm::cont::Field& input);

} // namespace vtkmtovtk

#endif // VTKTOVTKM_H