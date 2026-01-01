#ifndef SIMULATOR_KERNELS_CUH
#define SIMULATOR_KERNELS_CUH

#include <cuda_runtime.h>
#include <CudaDenseMatrix.h>
#include <cstddef>

namespace FDTD {

struct CudaWorkSpace {
    CudaDenseMatrix<float> m_Pres;
    CudaDenseMatrix<float> m_Vx;
    CudaDenseMatrix<float> m_Vy;
    bool initialize(size_t numRows, size_t numCols);
    void updateVx(float courantNb);
    void updateVy(float courantNb);
    void updatepressure(float crSquareTimesCourantNb);
    void UpdateForSource(unsigned GridIndexX, unsigned GridIndexY, float val);
    static dim3 getBlockDimension();
};


} // namespace FDTD

#endif // SIMULATOR_KERNELS_CUH

