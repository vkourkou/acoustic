#ifndef SIMULATOR_KERNELS_CUH
#define SIMULATOR_KERNELS_CUH

#include <cuda_runtime.h>
#include <CudaDenseMatrix.h>
#include <cstddef>

namespace FDTD {

// CUDA kernel to update Vx field
__global__ void
updateVxKernel(const float* pres, float* vx, std::size_t vxRows, std::size_t vxCols, std::size_t presCols, float courantNb);

// CUDA kernel to update Vy field
__global__ void
updateVyKernel(const float* pres, float* vy, std::size_t vyRows, std::size_t vyCols, std::size_t presCols, float courantNb);

// CUDA kernel to update pressure field
__global__ void
updatePressureKernel(const float* vx, const float* vy, float* pres, std::size_t presRows, std::size_t presCols, std::size_t vxCols, std::size_t vyCols, float crSquareTimesCourantNb);

struct CudaWorkSpace {
    CudaDenseMatrix<float> m_Pres;
    CudaDenseMatrix<float> m_Vx;
    CudaDenseMatrix<float> m_Vy;
    bool initialize(size_t numRows, size_t numCols);
    void updateVx(float courantNb);
    void updateVy(float courantNb);
    void updatepressure(float crSquareTimesCourantNb);
};


} // namespace FDTD

#endif // SIMULATOR_KERNELS_CUH

