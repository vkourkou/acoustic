#include <simulator_kernels.cuh>
#include <cuda_runtime.h>
#include <cuda_utilities.cuh>
#include <iostream>
#include <Simulator.h>
#include <fstream>
#include <vector>

namespace FDTD {

// -----------------------------------------------------------------------------

__global__ void
updateVxKernel(const float* pres, float* vx, std::size_t vxRows, std::size_t vxCols, std::size_t presCols, float courantNb)
{
    std::size_t i = blockIdx.y * blockDim.y + threadIdx.y;
    std::size_t j = blockIdx.x * blockDim.x + threadIdx.x;

    if (i < vxRows && j < vxCols) {
        std::size_t vxIdx = i * vxCols + j;
        std::size_t presIdx1 = (i + 1) * presCols + j;
        std::size_t presIdx2 = i * presCols + j;
        vx[vxIdx] = vx[vxIdx] - courantNb * (pres[presIdx1] - pres[presIdx2]);
    }
}

// -----------------------------------------------------------------------------

__global__ void
updateVyKernel(const float* pres, float* vy, std::size_t vyRows, std::size_t vyCols, std::size_t presCols, float courantNb)
{
    std::size_t i = blockIdx.y * blockDim.y + threadIdx.y;
    std::size_t j = blockIdx.x * blockDim.x + threadIdx.x;

    if (i < vyRows && j < vyCols) {
        std::size_t vyIdx = i * vyCols + j;
        std::size_t presIdx1 = i * presCols + (j + 1);
        std::size_t presIdx2 = i * presCols + j;
        vy[vyIdx] = vy[vyIdx] - courantNb * (pres[presIdx1] - pres[presIdx2]);
    }
}

// -----------------------------------------------------------------------------

__global__ void
updatePressureKernel(const float* vx, const float* vy, float* pres, std::size_t presRows, std::size_t presCols, std::size_t vxCols, std::size_t vyCols, float crSquareTimesCourantNb)
{
    std::size_t i = blockIdx.y * blockDim.y + threadIdx.y;
    std::size_t j = blockIdx.x * blockDim.x + threadIdx.x;

    // Boundaries are assumed to have Dirichlet condition (skip boundary points)
    if (i >= 1 && i < presRows - 1 && j >= 1 && j < presCols - 1) {
        std::size_t presIdx = i * presCols + j;
        std::size_t vxIdx1 = i * vxCols + j;
        std::size_t vxIdx2 = (i - 1) * vxCols + j;
        std::size_t vyIdx1 = i * vyCols + j;
        std::size_t vyIdx2 = i * vyCols + (j - 1);
        pres[presIdx] = pres[presIdx] - crSquareTimesCourantNb * (vx[vxIdx1] - vx[vxIdx2] + vy[vyIdx1] - vy[vyIdx2]);
    }
}

// -----------------------------------------------------------------------------


void
saveLaunchParameters(const std::string& name, const dim3& BlockDim, CudaDenseMatrix<float>& Mat, const void* func) 
{
    std::cout << "************************************************" << std::endl;
    std::cout << "Saving launch parameters for " << name << std::endl;
    const dim3 ElementDimension(Mat.cols(), Mat.rows(),1);
    const dim3 GridDim{CudaUtilities::getGridDimension(ElementDimension, BlockDim)};
    std::cout << "BlockDim: " << BlockDim.x << " " << BlockDim.y << " " << BlockDim.z << std::endl;
    std::cout << "ElementDimension: " << ElementDimension.x << " " << ElementDimension.y << " " << ElementDimension.z << std::endl;
    std::cout << "GridDim: " << GridDim.x << " " << GridDim.y << " " << GridDim.z << std::endl;
    std::cout << "#Blocks launched: " << CudaUtilities::getSize(GridDim) << "\n";
    size_t BlockSize = CudaUtilities::getSize(BlockDim);
    int NbOfBlocksMaxOccupancy = CudaUtilities::getNbOfBlocksMaxOccupancy(func, BlockSize, /*size_t dynamicSMemSize*/0);
    std::cout << "#Blocks for Max Occupancy: " << NbOfBlocksMaxOccupancy << "\n";
    std::cout << "Max Occupancy: " << CudaUtilities::computeMaxOccupancy(NbOfBlocksMaxOccupancy, BlockSize) << "\n";
}

// -----------------------------------------------------------------------------

bool
CudaWorkSpace::initialize(size_t numRows, size_t numCols)
{
    m_Pres.resize(numRows, numCols);
    m_Vx.resize(numRows - 1, numCols);
    m_Vy.resize(numRows, numCols - 1);
    
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
    
    saveLaunchParameters("Pres", getBlockDimension(), m_Pres, (void*)updatePressureKernel);
    saveLaunchParameters("Vx", getBlockDimension(), m_Vx, (void*)updateVxKernel);
    saveLaunchParameters("Vy", getBlockDimension(), m_Vy, (void*)updateVyKernel);
    return true;
}

// -----------------------------------------------------------------------------

dim3 
CudaWorkSpace::getBlockDimension()
{
    return dim3(16, 16);
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
    const dim3 ElementDimension(m_Vx.cols(), m_Vx.rows(),1);
    const dim3 GridDim{CudaUtilities::getGridDimension(ElementDimension, BlockDim)};
    
    // Launch kernel
    updateVxKernel<<<GridDim, BlockDim>>>(
        m_Pres.data(),
        m_Vx.data(),
        m_Vx.rows(),
        m_Vx.cols(),
        m_Pres.cols(),
        courantNb
    );

    CHECK_CUDA_ERROR(cudaGetLastError());
    
    CHECK_CUDA_ERROR(cudaDeviceSynchronize());
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
    const dim3 ElementDimension(m_Vy.cols(), m_Vy.rows(),1);
    const dim3 GridDim{CudaUtilities::getGridDimension(ElementDimension, BlockDim)};
    
    // Launch kernel
    updateVyKernel<<<GridDim, BlockDim>>>(
        m_Pres.data(),
        m_Vy.data(),
        m_Vy.rows(),
        m_Vy.cols(),
        m_Pres.cols(),
        courantNb
    );
    
    // Check for errors
    CHECK_CUDA_ERROR(cudaGetLastError());
    
    CHECK_CUDA_ERROR(cudaDeviceSynchronize());
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
    const dim3 ElementDimension(m_Pres.cols(), m_Pres.rows(),1);
    const dim3 GridDim{CudaUtilities::getGridDimension(ElementDimension, BlockDim)};
    
    // Launch kernel
    updatePressureKernel<<<GridDim, BlockDim>>>(
        m_Vx.data(),
        m_Vy.data(),
        m_Pres.data(),
        m_Pres.rows(),
        m_Pres.cols(),
        m_Vx.cols(),
        m_Vy.cols(),
        crSquareTimesCourantNb
    );
    
    // Check for errors
    CHECK_CUDA_ERROR(cudaGetLastError());
    
    CHECK_CUDA_ERROR(cudaDeviceSynchronize());
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
    updateVx(courantNb);
    updateVy(courantNb);
    updatepressure(crSquareTimesCourantNb);
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

