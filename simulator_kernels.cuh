#ifndef SIMULATOR_KERNELS_CUH
#define SIMULATOR_KERNELS_CUH

#include <cuda_runtime.h>
#include <CudaDenseMatrix.h>
#include <cstddef>

namespace FDTD {

class CudaWorkSpace {
    CudaDenseMatrix<float> m_Pres;
    CudaDenseMatrix<float> m_Vx;
    CudaDenseMatrix<float> m_Vy;
    void updateVx(float courantNb);
    void updateVy(float courantNb);
    void updatepressure(float crSquareTimesCourantNb);
    static dim3 getBlockDimension();
public:
    const CudaDenseMatrix<float>& getPres() const {
        return m_Pres;
    }
    bool initialize(size_t numRows, size_t numCols);
    void UpdateForSource(unsigned GridIndexX, unsigned GridIndexY, float val);
    void updateFields(float courantNb, float crSquareTimesCourantNb);
};


} // namespace FDTD

#endif // SIMULATOR_KERNELS_CUH

