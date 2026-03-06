#include <simulator_kernels.cuh>
#include <cuda_runtime.h>
#include <cuda_utilities.cuh>
#include <iostream>
#include <simulator.h>
#include <fstream>
#include <vector>
#include <cooperative_groups.h>

namespace FDTD {

// -----------------------------------------------------------------------------

#define BLOCK_SIZE_X 16
#define BLOCK_SIZE_Y 16
#define TILE_SIZE 4

__global__ void
updateVxKernel(const float* pres, float* vx, int presRows, int presCols, float courantNb)
{
    int i = blockIdx.y * blockDim.y + threadIdx.y;
    int j = blockIdx.x * blockDim.x + threadIdx.x;
    const int vxRows = presRows - 1;
    const int vxCols = presCols - 2;
    if (i < vxRows && j < vxCols) {
         //column j of vx corresponds to column j + 1 of pres
        int vxIdx = i * vxCols + j;
        int presIdx1 = (i + 1) * presCols + j + 1;
        int presIdx2 = i * presCols + j + 1;
        vx[vxIdx] -= courantNb * (pres[presIdx1] - pres[presIdx2]);
    }
}

// -----------------------------------------------------------------------------

__global__ void
updateVyKernel(const float* pres, float* vy, int presRows, int presCols, float courantNb)
{
    int i = blockIdx.y * blockDim.y + threadIdx.y;
    int j = blockIdx.x * blockDim.x + threadIdx.x;
    const int vyRows = presRows - 2;
    const int vyCols = presCols - 1;
    if (i < vyRows && j < vyCols) {
        //row i of vy corresponds to row i + 1 of pres
        int vyIdx = i * vyCols + j;
        int presIdx1 = (i + 1) * presCols + (j + 1);
        int presIdx2 = (i + 1) * presCols + j;
        vy[vyIdx] -= courantNb * (pres[presIdx1] - pres[presIdx2]);
    }
}

// -----------------------------------------------------------------------------

__global__ void
updatePressureKernel(const float* vx, const float* vy, float* pres, int presRows, int presCols, float crSquareTimesCourantNb)
{
    int i = blockIdx.y * blockDim.y + threadIdx.y;
    int j = blockIdx.x * blockDim.x + threadIdx.x;
    const int vxCols = presCols - 2;
    const int vyCols = presCols - 1;
    // Boundaries are assumed to have Dirichlet condition (skip boundary points)
    if (i < presRows - 2 && j < presCols - 2) {
        //row i of vy corresponds to row i + 1 of pres
        //column j of vx corresponds to column j + 1 of pres
        int presIdx = (i + 1) * presCols + j + 1;
        int vxIdx1 = (i + 1) * vxCols + j;
        int vxIdx2 = i * vxCols + j;
        int vyIdx1 = i * vyCols + j + 1;
        int vyIdx2 = i * vyCols + j;
        pres[presIdx] -= crSquareTimesCourantNb * (vx[vxIdx1] - vx[vxIdx2] + vy[vyIdx1] - vy[vyIdx2]);
    }
}

// -----------------------------------------------------------------------------

__global__ void
updateVelocityKernelShared(const float* pres, float* vx, float* vy,
                           int presRows, int presCols,
                           float courantNb)
{
    
    const int vxRows = presRows - 1;
    const int vxCols = presCols - 2;
    const int vyRows = presRows - 2;
    const int vyCols = presCols - 1;
    __shared__ float s_pres[BLOCK_SIZE_Y][BLOCK_SIZE_X + 1];
    const int iOffset = blockIdx.y * (blockDim.y - 1);
    const int jOffset = blockIdx.x * (blockDim.x - 1);
    int i = threadIdx.y;
    int j = threadIdx.x;
    if (i + iOffset < presRows && j + jOffset < presCols) {
        s_pres[i][j] 
        = pres[(i + iOffset) * presCols + (j + jOffset)];
    }

    __syncthreads();
    if (i + iOffset < vxRows && j + jOffset< vxCols && i < BLOCK_SIZE_Y - 1 && j < BLOCK_SIZE_X - 1) {
        vx[(i + iOffset) * vxCols + j + jOffset] -= courantNb * (s_pres[i+1][j+1] - s_pres[i][j+1]);
    }

    if (i + iOffset < vyRows && j + jOffset< vyCols && i < BLOCK_SIZE_Y - 1 && j < BLOCK_SIZE_X - 1) {
        vy[(i + iOffset) * vyCols + j + jOffset] -= courantNb * (s_pres[i+1][j+1] - s_pres[i+1][j]);
    }

}

// -----------------------------------------------------------------------------

__global__ void
updateVelocityKernel(const float* pres, float* vx, float* vy,
                     int presRows, int presCols,
                     float courantNb)
{
    int i = blockIdx.y * blockDim.y + threadIdx.y;
    int j = blockIdx.x * blockDim.x + threadIdx.x;
    const int vxRows = presRows - 1;
    const int vxCols = presCols - 2;
    const int vyRows = presRows - 2;
    const int vyCols = presCols - 1;
    
    if (i < vxRows && j < vxCols) {
         //column j of vx corresponds to column j + 1 of pres
        int vxIdx = i * vxCols + j;
        int presIdx1 = (i + 1) * presCols + j + 1;
        int presIdx2 = i * presCols + j + 1;
        vx[vxIdx] -= courantNb * (pres[presIdx1] - pres[presIdx2]);
    }

    if (i < vyRows && j < vyCols) {
        //row i of vy corresponds to row i + 1 of pres
        int vyIdx = i * vyCols + j;
        int presIdx1 = (i + 1) * presCols + (j + 1);
        int presIdx2 = (i + 1) * presCols + j;
        vy[vyIdx] -= courantNb * (pres[presIdx1] - pres[presIdx2]);
    }
}

// -----------------------------------------------------------------------------

void
saveLaunchParameters(const std::string& name, const dim3& BlockDim, const dim3& ElementDimension, const void* func, const dim3& GridDim) 
{
    std::cout << "************************************************" << std::endl;
    std::cout << "Saving launch parameters for " << name << std::endl;
    std::cout << "BlockDim: " << BlockDim.x << " " << BlockDim.y << " " << BlockDim.z << std::endl;
    std::cout << "ElementDimension: " << ElementDimension.x << " " << ElementDimension.y << " " << ElementDimension.z << std::endl;
    std::cout << "GridDim: " << GridDim.x << " " << GridDim.y << " " << GridDim.z << std::endl;
    const size_t NbBlocksLaunched = CudaUtilities::getSize(GridDim);
    std::cout << "#Blocks launched: " << NbBlocksLaunched << "\n";
    size_t BlockSize = CudaUtilities::getSize(BlockDim);
    int NbOfBlocksMaxOccupancy = CudaUtilities::getNbOfBlocksMaxOccupancy(func, BlockSize, /*size_t dynamicSMemSize*/0);
    std::cout << "#Blocks per SM for Max Occupancy: " << NbOfBlocksMaxOccupancy << "\n";
    std::cout << "Max Occupancy: " << CudaUtilities::computeMaxOccupancy(NbOfBlocksMaxOccupancy, BlockSize) << "\n";
    const size_t NbOfBlocksForDeviceMaxOccupancy = CudaUtilities::getDeviceProperties()[0].multiProcessorCount * NbOfBlocksMaxOccupancy;
    std::cout << "Waves of Blocks launched: " << float(NbBlocksLaunched) / float(NbOfBlocksForDeviceMaxOccupancy) << "\n"; 
}

// -----------------------------------------------------------------------------

void
saveLaunchParameters(const std::string& name, const dim3& BlockDim, const dim3& ElementDimension, const void* func) 
{
    const dim3 GridDim{CudaUtilities::getGridDimension(ElementDimension, BlockDim)};
    saveLaunchParameters(name, BlockDim, ElementDimension, func, GridDim);
}

// -----------------------------------------------------------------------------

dim3
CudaWorkSpace::getPressureDimension() const
{
    return dim3(m_Pres.cols()-2, m_Pres.rows()-2,1);
}

// -----------------------------------------------------------------------------

dim3 
CudaWorkSpace::getVxDimension() const
{
    return dim3(m_Vx.cols(), m_Vx.rows(),1);
}   

// -----------------------------------------------------------------------------

dim3
CudaWorkSpace::getVyDimension() const
{
    return dim3(m_Vy.cols(), m_Vy.rows(),1);
}

// -----------------------------------------------------------------------------

dim3
CudaWorkSpace::getUpdateVelocityDimension() const
{
    return dim3(m_Pres.cols()-1, m_Pres.rows()-1,1);
}

// -----------------------------------------------------------------------------

bool
CudaWorkSpace::initialize(size_t numRows, size_t numCols)
{
    m_Pres.resize(numRows, numCols);
    m_Vx.resize(numRows - 1, numCols - 2); //column j of vx corresponds to column j + 1 of pres
    m_Vy.resize(numRows - 2, numCols - 1); //row i of vy corresponds to row i + 1 of pres
    
    // Initialize all matrices to zero using cudaMemset
    if (m_Pres.size() > 0) {
        CHECK_CUDA_ERROR(cudaMemset(m_Pres.data(), 0, m_Pres.size() * sizeof(float)));
    }
    
    if (m_Vx.size() > 0) {
        CHECK_CUDA_ERROR(cudaMemset(m_Vx.data(), 0, m_Vx.size() * sizeof(float)));
    }
    
    if (m_Vy.size() > 0) {
        CHECK_CUDA_ERROR(cudaMemset(m_Vy.data(), 0, m_Vy.size() * sizeof(float)));
    }
    if (m_UpdateAllTogether) {
        saveLaunchParameters("Velocity kernel", getBlockDimension(), getUpdateVelocityDimension(), (void*)updateVelocityKernel, getGridDimensionForVelocity());
        saveLaunchParameters("Pres", getBlockDimension(), getPressureDimension(), (void*)updatePressureKernel);
    }
    else {
        saveLaunchParameters("Pres", getBlockDimension(), getPressureDimension(), (void*)updatePressureKernel);
        saveLaunchParameters("Vx", getBlockDimension(), getVxDimension(), (void*)updateVxKernel);
        saveLaunchParameters("Vy", getBlockDimension(), getVyDimension(), (void*)updateVyKernel);
    }
    return true;
}

// -----------------------------------------------------------------------------

constexpr dim3 
CudaWorkSpace::getBlockDimension()
{
    return dim3(BLOCK_SIZE_X, BLOCK_SIZE_Y);
}

// -----------------------------------------------------------------------------


constexpr dim3 
CudaWorkSpace::getBlockDimensionMinusOne()
{
    return dim3(BLOCK_SIZE_X-1, BLOCK_SIZE_Y-1);
}

// -----------------------------------------------------------------------------

void
CudaWorkSpace::updateVx(float courantNb)
{
    if (m_Vx.size() == 0 || m_Pres.size() == 0) {
        return;
    }
    
    // Configure kernel launch parameters
    const dim3 BlockDim{getBlockDimension()};
    const dim3 ElementDimension(getVxDimension());
    const dim3 GridDim{CudaUtilities::getGridDimension(ElementDimension, BlockDim)};
    
    // Launch kernel
    updateVxKernel<<<GridDim, BlockDim>>>(
        m_Pres.data(),
        m_Vx.data(),
        m_Pres.rows(),
        m_Pres.cols(),
        courantNb
    );

    CHECK_CUDA_ERROR(cudaGetLastError());

}

// -----------------------------------------------------------------------------

void
CudaWorkSpace::updateVy(float courantNb)
{
    if (m_Vy.size() == 0 || m_Pres.size() == 0) {
        return;
    }
    
    // Configure kernel launch parameters
    const dim3 BlockDim{getBlockDimension()};
    const dim3 ElementDimension(getVyDimension());
    const dim3 GridDim{CudaUtilities::getGridDimension(ElementDimension, BlockDim)};
    
    // Launch kernel
    updateVyKernel<<<GridDim, BlockDim>>>(
        m_Pres.data(),
        m_Vy.data(),
        m_Pres.rows(),
        m_Pres.cols(),
        courantNb
    );
    
    // Check for errors
    CHECK_CUDA_ERROR(cudaGetLastError());
    
}

// -----------------------------------------------------------------------------

void
CudaWorkSpace::updatepressure(float crSquareTimesCourantNb)
{
    if (m_Pres.size() == 0 || m_Vx.size() == 0 || m_Vy.size() == 0) {
        return;
    }
    
    // Configure kernel launch parameters
    const dim3 BlockDim{getBlockDimension()};
    const dim3 ElementDimension(getPressureDimension());
    const dim3 GridDim{CudaUtilities::getGridDimension(ElementDimension, BlockDim)};
    
    // Launch kernel
    updatePressureKernel<<<GridDim, BlockDim>>>(
        m_Vx.data(),
        m_Vy.data(),
        m_Pres.data(),
        m_Pres.rows(),
        m_Pres.cols(),
        crSquareTimesCourantNb
    );
    
    // Check for errors
    CHECK_CUDA_ERROR(cudaGetLastError());

}

// -----------------------------------------------------------------------------

dim3 
CudaWorkSpace::getGridDimensionForVelocity() const
{
    if (m_bShared) {
        return CudaUtilities::getGridDimension(getUpdateVelocityDimension(), getBlockDimensionMinusOne());
    }
    else {
        return CudaUtilities::getGridDimension(getUpdateVelocityDimension(), getBlockDimension());
    }
}

// -----------------------------------------------------------------------------
void
CudaWorkSpace::updateVelocity(float courantNb)
{
    if (m_Pres.size() == 0 || m_Vx.size() == 0 || m_Vy.size() == 0) {
        return;
    }
    
    // Configure kernel launch parameters
    const dim3 BlockDim{getBlockDimension()};
    const dim3 GridDim{getGridDimensionForVelocity()};
    
    // Launch kernel
    if (m_bShared) {
        updateVelocityKernelShared<<<GridDim, BlockDim>>>(
            m_Pres.data(),
            m_Vx.data(),
            m_Vy.data(),
            m_Pres.rows(),
            m_Pres.cols(),
            courantNb
        );
    }
    else {
        updateVelocityKernel<<<GridDim, BlockDim>>>(
            m_Pres.data(),
            m_Vx.data(),
            m_Vy.data(),
            m_Pres.rows(),
            m_Pres.cols(),
            courantNb
        );
    }
    // Check for errors
    CHECK_CUDA_ERROR(cudaGetLastError());
}

// -----------------------------------------------------------------------------


void
CudaWorkSpace::UpdateForSource(unsigned GridIndexX, unsigned GridIndexY, float val)
{
    if (m_Pres.empty()) {
        return;
    }
    if (GridIndexX >= m_Pres.rows() || GridIndexY >= m_Pres.cols()) {
        return;
    }
    std::size_t index = GridIndexX * m_Pres.cols() + GridIndexY;
    CHECK_CUDA_ERROR(cudaMemcpy(m_Pres.data() + index, &val, sizeof(float), cudaMemcpyHostToDevice));
}

// -----------------------------------------------------------------------------

void CudaWorkSpace::updateFields(float courantNb, float crSquareTimesCourantNb) {
    if (m_UpdateAllTogether) {
        updateVelocity(courantNb);
        CHECK_CUDA_ERROR(cudaDeviceSynchronize());
        updatepressure(crSquareTimesCourantNb);
        CHECK_CUDA_ERROR(cudaDeviceSynchronize());
    }
    else {
        updateVx(courantNb);
        updateVy(courantNb);
        CHECK_CUDA_ERROR(cudaDeviceSynchronize());
        updatepressure(crSquareTimesCourantNb);
        CHECK_CUDA_ERROR(cudaDeviceSynchronize());
    }
}

// -----------------------------------------------------------------------------

template< int BlockSizeX, int BlockSizeY>
struct UnifiedKernelParams {
    static constexpr int m_BlockSizeX = BlockSizeX;
    static constexpr int m_BlockSizeY = BlockSizeY;
    static constexpr dim3 getBlockDimension() {
        return dim3(m_BlockSizeX, m_BlockSizeY);
    }
    static constexpr dim3 getBlockDimensionMinusTwo() {
        return dim3(m_BlockSizeX-2, m_BlockSizeY-2);
    }
};

// -----------------------------------------------------------------------------


template< typename Params>
__global__ void
updateUnifiedKernel(const float* prev, float* presdelta, float* pres,
                    int presRows, int presCols,
                    int nbColsWithoutPadding,
                    float factor)
{
    //presCols is the number of columns for the matrix without padding
    //numColsWithPadding.

    __shared__ float s_pres[Params::m_BlockSizeY][Params::m_BlockSizeX + 1];
    __shared__ float s_presdelta[Params::m_BlockSizeY][Params::m_BlockSizeX + 1];
    const int iOffset = blockIdx.y * (blockDim.y - 2);
    const int jOffset = blockIdx.x * (blockDim.x - 2);
    int i = threadIdx.y;
    int j = threadIdx.x;
    if (i + iOffset < presRows && j + jOffset < nbColsWithoutPadding) {
        s_pres[i][j] 
        = prev[(i + iOffset) * presCols + (j + jOffset)];
        s_presdelta[i][j] 
        = presdelta[(i + iOffset) * presCols + (j + jOffset)];
    }

    __syncthreads();
    if (i > 0 && j > 0 && i + iOffset < presRows - 1 && j + jOffset< nbColsWithoutPadding - 1 && i < Params::m_BlockSizeY - 1 && j < Params::m_BlockSizeX - 1) {
        float delta = factor * (s_pres[i+1][j] + s_pres[i-1][j] + s_pres[i][j+1] + s_pres[i][j-1] - 4 * s_pres[i][j]);
        s_presdelta[i][j] -= delta;
        presdelta[(i + iOffset) * presCols + j + jOffset] = s_presdelta[i][j];
        pres[(i + iOffset) * presCols + j + jOffset] = s_pres[i][j] - s_presdelta[i][j];
    }

}

// -----------------------------------------------------------------------------

dim3
CudaWorkSpaceUnified::getPressureDimension() const
{
    return dim3(m_PresA.cols()-2, m_PresA.rows()-2,1);
}

// -----------------------------------------------------------------------------

template< typename Params>
bool
CudaWorkSpaceUnified::initializeTemplated(size_t numRows, size_t numCols)
{
    m_nbColsWithoutPadding = numCols;
    if (m_ShouldPad) {
        numCols = (numCols  + 31) / 32 * 32; //That will make this the number of columns with padding
    }
    m_PresA.resize(numRows, numCols);
    m_PresB.resize(numRows, numCols);
    m_PresDelta.resize(numRows, numCols);
    
    // Initialize all matrices to zero using cudaMemset
    if (m_PresA.size() > 0) {
        CHECK_CUDA_ERROR(cudaMemset(m_PresA.data(), 0, m_PresA.size() * sizeof(float)));
    }

    if (m_PresB.size() > 0) {
        CHECK_CUDA_ERROR(cudaMemset(m_PresB.data(), 0, m_PresB.size() * sizeof(float)));
    }
    
    if (m_PresDelta.size() > 0) {
        CHECK_CUDA_ERROR(cudaMemset(m_PresDelta.data(), 0, m_PresDelta.size() * sizeof(float)));
    }
    
    m_CurrentPres = &m_PresA;
    m_PreviousPres = &m_PresB;

    saveLaunchParameters("Pres", Params::getBlockDimension(), getPressureDimension(), (void*)updateUnifiedKernel<Params>);
    
    return true;
}

// -----------------------------------------------------------------------------

#define ListOfUnifiedKernelParams \
    UnifiedKernelParams<4, 4>, \
    UnifiedKernelParams<4, 8>, \
    UnifiedKernelParams<4, 16>, \
    UnifiedKernelParams<4, 32>, \
    UnifiedKernelParams<4, 64>, \
    UnifiedKernelParams<4, 128>, \
    UnifiedKernelParams<4, 256>, \
    UnifiedKernelParams<4, 512>, \
    UnifiedKernelParams<8, 4>, \
    UnifiedKernelParams<8, 8>, \
    UnifiedKernelParams<8, 16>, \
    UnifiedKernelParams<8, 32>, \
    UnifiedKernelParams<8, 64>, \
    UnifiedKernelParams<8, 128>, \
    UnifiedKernelParams<8, 256>, \
    UnifiedKernelParams<16, 4>, \
    UnifiedKernelParams<16, 8>, \
    UnifiedKernelParams<16, 16>, \
    UnifiedKernelParams<16, 32>, \
    UnifiedKernelParams<16, 64>, \
    UnifiedKernelParams<16, 128>, \
    UnifiedKernelParams<16, 256>, \
    UnifiedKernelParams<32, 4>, \
    UnifiedKernelParams<32, 8>, \
    UnifiedKernelParams<32, 16>, \
    UnifiedKernelParams<32, 32>, \
    UnifiedKernelParams<32, 64>, \
    UnifiedKernelParams<32, 128>, \
    UnifiedKernelParams<64, 4>, \
    UnifiedKernelParams<64, 8>, \
    UnifiedKernelParams<64, 16>, \
    UnifiedKernelParams<64, 32>, \
    UnifiedKernelParams<64, 64>, \
    UnifiedKernelParams<128, 4>, \
    UnifiedKernelParams<128, 8>, \
    UnifiedKernelParams<128, 16>, \
    UnifiedKernelParams<128, 32>, \
    UnifiedKernelParams<256, 4>, \
    UnifiedKernelParams<256, 8>, \
    UnifiedKernelParams<256, 16>, \
    UnifiedKernelParams<512, 4>, \
    UnifiedKernelParams<1024, 4>
    
bool
CudaWorkSpaceUnified::initialize(size_t numRows, size_t numCols)
{
    return variadicInitialize<ListOfUnifiedKernelParams>(numRows, numCols);
}

// -----------------------------------------------------------------------------

template< typename Params>
void
CudaWorkSpaceUnified::updateFieldsTemplated(float courantNb, float crSquareTimesCourantNb)
{
    if (m_PresA.size() == 0 || m_PresB.size() == 0 || m_PresDelta.size() == 0) {
        return;
    }
    std::swap(m_CurrentPres, m_PreviousPres);
    const float factor = courantNb * crSquareTimesCourantNb;
    // Configure kernel launch parameters
    constexpr dim3 BlockDim{Params::getBlockDimension()};
    const dim3 ElementDimension(getPressureDimension());
    const dim3 GridDim{CudaUtilities::getGridDimension(ElementDimension, Params::getBlockDimensionMinusTwo())};
    
    // Launch kernel
    updateUnifiedKernel<Params><<<GridDim, BlockDim>>>(
        m_PreviousPres->data(),
        m_PresDelta.data(),
        m_CurrentPres->data(),
        m_CurrentPres->rows(),
        m_CurrentPres->cols(),
        m_nbColsWithoutPadding,
        factor
    );
    
    // Check for errors
    CHECK_CUDA_ERROR(cudaGetLastError());
}

// -----------------------------------------------------------------------------

void
CudaWorkSpaceUnified::updateFields(float courantNb, float crSquareTimesCourantNb)
{
    // Try dispatching to template instances - add more UnifiedKernelParams as needed
    variadicUpdateFields<ListOfUnifiedKernelParams>(courantNb, crSquareTimesCourantNb);
    
}

// -----------------------------------------------------------------------------


void
CudaWorkSpaceUnified::UpdateForSource(unsigned GridIndexX, unsigned GridIndexY, float val)
{
    if (m_CurrentPres->empty()) {
        return;
    }
    if (GridIndexX >= m_CurrentPres->rows() || GridIndexY >= m_CurrentPres->cols()) {
        return;
    }
    std::size_t index = GridIndexX * m_CurrentPres->cols() + GridIndexY;
    CHECK_CUDA_ERROR(cudaMemcpy(m_CurrentPres->data() + index, &val, sizeof(float), cudaMemcpyHostToDevice));
}

// -----------------------------------------------------------------------------

// Template specializations for CUDA workspace (PT=GPU)
// These are defined here so they can access the full CudaWorkSpace definition

// -----------------------------------------------------------------------------

void
CudaWorkSpaceUnified::setBlockSize(int blockSizeX, int blockSizeY)
{
    m_BlockSizeX = blockSizeX;
    m_BlockSizeY = blockSizeY;
}

// -----------------------------------------------------------------------------

template<>
bool
Simulator::initializeMatrices<GPU>()
{
    std::size_t numRows = m_Grids.get<X>().size();
    std::size_t numCols = m_Grids.get<Y>().size();
    
    if (!m_CudaWorkSpace) {
        m_CudaWorkSpace = new CudaWorkSpaceUnified();
        if (m_SimulationParam.getBlockSizeX() != 0 && m_SimulationParam.getBlockSizeY() != 0) {
            m_CudaWorkSpace->setBlockSize(m_SimulationParam.getBlockSizeX(), m_SimulationParam.getBlockSizeY());
        }
    }
    
    return m_CudaWorkSpace->initialize(numRows, numCols);
}

// -----------------------------------------------------------------------------

template<>
void
Simulator::updateFields<GPU>()
{
    if (m_CudaWorkSpace) {
        m_CudaWorkSpace->updateFields(m_CourantNb, m_CrSquareTimesCourantNb);
    }
}

// -----------------------------------------------------------------------------

template<>
void
Simulator::UpdateForSource<GPU>(unsigned GridIndexX, unsigned GridIndexY, float val) {
    m_CudaWorkSpace->UpdateForSource(GridIndexX, GridIndexY, val);
}

// -----------------------------------------------------------------------------

void
Simulator::potentiallyTransferToDevice(DenseMatrix<float>& To)
{
    if (m_CudaWorkSpace) {
        m_CudaWorkSpace->getPres().transferTo(To);
        To.eraseLastCols(m_CudaWorkSpace->getNbColsWithPadding());
    }
}

// -----------------------------------------------------------------------------




} // namespace FDTD

