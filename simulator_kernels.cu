#include <simulator_kernels.cuh>
#include <cuda_runtime.h>
#include <cuda_utilities.cuh>
#include <iostream>
#include <Simulator.h>
#include <fstream>
#include <vector>
#include <cooperative_groups.h>

namespace FDTD {

// -----------------------------------------------------------------------------

#define BLOCK_SIZE 8
#define TILE_SIZE 4

__global__ void
updateVxKernel(const float* pres, float* vx, std::size_t presRows, std::size_t presCols, float courantNb)
{
    std::size_t i = blockIdx.y * blockDim.y + threadIdx.y;
    std::size_t j = blockIdx.x * blockDim.x + threadIdx.x;
    const std::size_t vxRows = presRows - 1;
    const std::size_t vxCols = presCols - 2;
    if (i < vxRows && j < vxCols) {
         //column j of vx corresponds to column j + 1 of pres
        std::size_t vxIdx = i * vxCols + j;
        std::size_t presIdx1 = (i + 1) * presCols + j + 1;
        std::size_t presIdx2 = i * presCols + j + 1;
        vx[vxIdx] -= courantNb * (pres[presIdx1] - pres[presIdx2]);
    }
}

// -----------------------------------------------------------------------------

__global__ void
updateVyKernel(const float* pres, float* vy, std::size_t presRows, std::size_t presCols, float courantNb)
{
    std::size_t i = blockIdx.y * blockDim.y + threadIdx.y;
    std::size_t j = blockIdx.x * blockDim.x + threadIdx.x;
    const std::size_t vyRows = presRows - 2;
    const std::size_t vyCols = presCols - 1;
    if (i < vyRows && j < vyCols) {
        //row i of vy corresponds to row i + 1 of pres
        std::size_t vyIdx = i * vyCols + j;
        std::size_t presIdx1 = (i + 1) * presCols + (j + 1);
        std::size_t presIdx2 = (i + 1) * presCols + j;
        vy[vyIdx] -= courantNb * (pres[presIdx1] - pres[presIdx2]);
    }
}

// -----------------------------------------------------------------------------

__global__ void
updatePressureKernel(const float* vx, const float* vy, float* pres, std::size_t presRows, std::size_t presCols, float crSquareTimesCourantNb)
{
    std::size_t i = blockIdx.y * blockDim.y + threadIdx.y;
    std::size_t j = blockIdx.x * blockDim.x + threadIdx.x;
    const std::size_t vxCols = presCols - 2;
    const std::size_t vyCols = presCols - 1;
    // Boundaries are assumed to have Dirichlet condition (skip boundary points)
    if (i < presRows - 2 && j < presCols - 2) {
        //row i of vy corresponds to row i + 1 of pres
        //column j of vx corresponds to column j + 1 of pres
        std::size_t presIdx = (i + 1) * presCols + j + 1;
        std::size_t vxIdx1 = (i + 1) * vxCols + j;
        std::size_t vxIdx2 = i * vxCols + j;
        std::size_t vyIdx1 = i * vyCols + j + 1;
        std::size_t vyIdx2 = i * vyCols + j;
        pres[presIdx] -= crSquareTimesCourantNb * (vx[vxIdx1] - vx[vxIdx2] + vy[vyIdx1] - vy[vyIdx2]);
    }
}

// -----------------------------------------------------------------------------

__global__ void
updateVelocityKernelShared(const float* pres, float* vx, float* vy,
                           std::size_t presRows, std::size_t presCols,
                           float courantNb)
{
    
    const std::size_t vxRows = presRows - 1;
    const std::size_t vxCols = presCols - 2;
    const std::size_t vyRows = presRows - 2;
    const std::size_t vyCols = presCols - 1;
    __shared__ float s_pres[BLOCK_SIZE][BLOCK_SIZE + 1];
    const size_t iOffset = blockIdx.y * (blockDim.y - 1);
    const size_t jOffset = blockIdx.x * (blockDim.x - 1);
    std::size_t i = threadIdx.y;
    std::size_t j = threadIdx.x;
    if (i + iOffset < presRows && j + jOffset < presCols) {
        s_pres[i][j] 
        = pres[(i + iOffset) * presCols + (j + jOffset)];
    }

    __syncthreads();
    if (i + iOffset < vxRows && j + jOffset< vxCols && i < BLOCK_SIZE - 1 && j < BLOCK_SIZE - 1) {
        vx[(i + iOffset) * vxCols + j + jOffset] -= courantNb * (s_pres[i+1][j+1] - s_pres[i][j+1]);
    }

    if (i + iOffset < vyRows && j + jOffset< vyCols && i < BLOCK_SIZE - 1 && j < BLOCK_SIZE - 1) {
        vy[(i + iOffset) * vyCols + j + jOffset] -= courantNb * (s_pres[i+1][j+1] - s_pres[i+1][j]);
    }

}

// -----------------------------------------------------------------------------

__global__ void
updateVelocityKernel(const float* pres, float* vx, float* vy,
                     std::size_t presRows, std::size_t presCols,
                     float courantNb)
{
    std::size_t i = blockIdx.y * blockDim.y + threadIdx.y;
    std::size_t j = blockIdx.x * blockDim.x + threadIdx.x;
    const std::size_t vxRows = presRows - 1;
    const std::size_t vxCols = presCols - 2;
    const std::size_t vyRows = presRows - 2;
    const std::size_t vyCols = presCols - 1;
    
    if (i < vxRows && j < vxCols) {
         //column j of vx corresponds to column j + 1 of pres
        std::size_t vxIdx = i * vxCols + j;
        std::size_t presIdx1 = (i + 1) * presCols + j + 1;
        std::size_t presIdx2 = i * presCols + j + 1;
        vx[vxIdx] -= courantNb * (pres[presIdx1] - pres[presIdx2]);
    }

    if (i < vyRows && j < vyCols) {
        //row i of vy corresponds to row i + 1 of pres
        std::size_t vyIdx = i * vyCols + j;
        std::size_t presIdx1 = (i + 1) * presCols + (j + 1);
        std::size_t presIdx2 = (i + 1) * presCols + j;
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
    return dim3(BLOCK_SIZE, BLOCK_SIZE);
}

// -----------------------------------------------------------------------------


constexpr dim3 
CudaWorkSpace::getBlockDimensionMinusOne()
{
    return dim3(BLOCK_SIZE-1, BLOCK_SIZE-1);
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

// Template specializations for CUDA workspace (PT=GPU)
// These are defined here so they can access the full CudaWorkSpace definition

// -----------------------------------------------------------------------------

template<>
bool
Simulator::initializeMatrices<GPU>()
{
    std::size_t numRows = m_Grids.get<X>().size();
    std::size_t numCols = m_Grids.get<Y>().size();
    
    if (!m_CudaWorkSpace) {
        m_CudaWorkSpace = new CudaWorkSpace();
    }
    
    return m_CudaWorkSpace->initialize(numRows, numCols);
}

// -----------------------------------------------------------------------------

// Helper function to cleanup CUDA workspace
// This is called from Simulator destructor when CUDA workspace exists
void
cleanupCudaWorkSpace(CudaWorkSpace* ws)
{
    if (ws) {
        delete ws;
    }
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
    }
}

// -----------------------------------------------------------------------------



} // namespace FDTD

