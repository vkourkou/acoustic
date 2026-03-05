#ifndef SIMULATOR_KERNELS_CUH
#define SIMULATOR_KERNELS_CUH

#include <cuda_runtime.h>
#include <cuda_dense_matrix.h>
#include <cstddef>

namespace FDTD {

class CudaWorkSpace {
    CudaDenseMatrix<float> m_Pres;
    CudaDenseMatrix<float> m_Vx;
    CudaDenseMatrix<float> m_Vy;
    void updateVx(float courantNb);
    void updateVy(float courantNb);
    void updatepressure(float crSquareTimesCourantNb);
    void updateVelocity(float courantNb);
    constexpr static dim3 getBlockDimension();
    constexpr static dim3 getBlockDimensionMinusOne();
    dim3 getGridDimensionForVelocity() const;
public:
    const CudaDenseMatrix<float>& getPres() const {
        return m_Pres;
    }
    bool initialize(size_t numRows, size_t numCols);
    void UpdateForSource(unsigned GridIndexX, unsigned GridIndexY, float val);
    void updateFields(float courantNb, float crSquareTimesCourantNb);
    dim3 getPressureDimension() const;
    dim3 getVxDimension() const;
    dim3 getVyDimension() const;
    dim3 getUpdateVelocityDimension() const;
    const bool m_UpdateAllTogether{true};
    const bool m_bShared{false};
};


class CudaWorkSpaceUnified {
    CudaDenseMatrix<float> m_PresA;
    CudaDenseMatrix<float> m_PresB;
    CudaDenseMatrix<float>* m_CurrentPres;
    CudaDenseMatrix<float>* m_PreviousPres;
    CudaDenseMatrix<float> m_PresDelta;
    int m_BlockSizeX{128};
    int m_BlockSizeY{4};
    size_t m_nbColsWithoutPadding;
    bool m_ShouldPad{true};
    dim3 getPressureDimension() const;
    dim3 getGridDimension() const;
    template< typename Params>
    void updateFieldsTemplated(float courantNb, float crSquareTimesCourantNb);
    template<typename FirstParams, typename... RestParams>
    bool variadicUpdateFields(float courantNb, float crSquareTimesCourantNb);
    template<typename FirstParams, typename... RestParams>
    bool variadicInitialize(size_t numRows, size_t numCols);
    template< typename Params>
    bool initializeTemplated(size_t numRows, size_t numCols);
public:
    int getNbColsWithPadding() const {
        return m_PresA.cols() - m_nbColsWithoutPadding;
    }
    const CudaDenseMatrix<float>& getPres() const {
        return *m_CurrentPres;
    }
    bool initialize(size_t numRows, size_t numCols);
    void UpdateForSource(unsigned GridIndexX, unsigned GridIndexY, float val);
    void updateFields(float courantNb, float crSquareTimesCourantNb);
    void setBlockSize(int blockSizeX, int blockSizeY);
};

// Variadic template function to try multiple UnifiedKernelParams
// Takes UnifiedKernelParams as template parameters
template<typename FirstParams, typename... RestParams>
inline bool
CudaWorkSpaceUnified::variadicUpdateFields(float courantNb, float crSquareTimesCourantNb) 
{
    // Try current UnifiedKernelParams
    if (m_BlockSizeX == FirstParams::m_BlockSizeX && m_BlockSizeY == FirstParams::m_BlockSizeY) {
        updateFieldsTemplated<FirstParams>(courantNb, crSquareTimesCourantNb);
        return true;
    }
    // Try remaining UnifiedKernelParams if any
    if constexpr (sizeof...(RestParams) > 0) {
        return variadicUpdateFields<RestParams...>(courantNb, crSquareTimesCourantNb);
    }
    return false;
}

// -----------------------------------------------------------------------------

// Variadic template function to try multiple UnifiedKernelParams for initialization
// Takes UnifiedKernelParams as template parameters
template<typename FirstParams, typename... RestParams>
inline bool
CudaWorkSpaceUnified::variadicInitialize(size_t numRows, size_t numCols)
{
    // Try current UnifiedKernelParams
    if (m_BlockSizeX == FirstParams::m_BlockSizeX && m_BlockSizeY == FirstParams::m_BlockSizeY) {
        return initializeTemplated<FirstParams>(numRows, numCols);
    }
    // Try remaining UnifiedKernelParams if any
    if constexpr (sizeof...(RestParams) > 0) {
        return variadicInitialize<RestParams...>(numRows, numCols);
    }
    return false;
}

// -----------------------------------------------------------------------------


void
cleanupCudaWorkSpace(CudaWorkSpaceUnified* ws) {
    if (ws) {
        delete ws;
    }
}

void
cleanupCudaWorkSpace(CudaWorkSpace* ws) {
    if (ws) {
        delete ws;
    }
}


} // namespace FDTD

#endif // SIMULATOR_KERNELS_CUH

