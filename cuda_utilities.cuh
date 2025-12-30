#ifndef CUDA_UTILITIES_CUH_
#define CUDA_UTILITIES_CUH_

#include <cuda_runtime.h>
#include <cstdio>
#include <cstdlib>

namespace CudaUtilities {
    int getDeviceCount();
    void queryDeviceCount();
    void querySpecificDevice(int deviceIndex);
    void queryGPU();
    template <typename T>
    void check(T result, char const *const func, const char *const file, int const line) {
        if (result != cudaSuccess) {
            fprintf(stderr, "CUDA error at %s:%d code=%d(%s) \"%s\" \n", file, line,
                    static_cast<unsigned int>(result), cudaGetErrorString(result), func);
            exit(EXIT_FAILURE);
        }
    }

}

#define CHECK_CUDA_ERROR(val) CudaUtilities::check((val), #val, __FILE__, __LINE__)


#endif // CUDA_UTILITIES_CUH_
