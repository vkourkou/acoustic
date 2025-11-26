

#ifndef ADD_ARRAYS_CUH_
#define ADD_ARRAYS_CUH_

#include <vector>

namespace CudaBasic {


bool addVectors(bool bUseGPU, const std::vector<float>& A, const std::vector<float>& B, std::vector<float>& C);

int launchKernel();

int getDeviceCount();

} // namespace

#endif // ADD_ARRAYS_CUH_