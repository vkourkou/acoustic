#ifndef CUDA_ARRAY_H
#define CUDA_ARRAY_H

#include <cstddef>
#include <stdexcept>
#include <iostream>
#include <cuda_runtime.h>
#include <vector>

template<typename T>
class CudaArray {
private:
    T* m_devicePtr;
    std::size_t m_size;

public:
    // Constructor - allocates CUDA memory
    CudaArray(std::size_t size);

    // Destructor - deallocates CUDA memory
    ~CudaArray();

    // Copy constructor - deleted
    CudaArray(const CudaArray& other) = delete;

    // Move constructor - deleted
    CudaArray(CudaArray&& other) noexcept = delete;

    // Copy assignment operator - deleted
    CudaArray& operator=(const CudaArray& other) = delete;

    // Move assignment operator - deleted
    CudaArray& operator=(CudaArray&& other) noexcept = delete;

    // -----------------------------------------------------------------------------

    // Get device pointer
    T*
    getDevicePtr() const;

    // -----------------------------------------------------------------------------

    // Get device pointer (alias for getDevicePtr for consistency with DenseMatrix)
    T*
    data() const;

    // -----------------------------------------------------------------------------

    // Get size
    std::size_t
    size() const;

    // -----------------------------------------------------------------------------

    // Resize array
    void
    resize(std::size_t newSize);

    // -----------------------------------------------------------------------------

    // Initialize all elements to the specified value
    void
    initialize(const T& t);

    // -----------------------------------------------------------------------------

};

// -----------------------------------------------------------------------------

template<typename T>
CudaArray<T>::CudaArray(std::size_t size)
    : m_devicePtr(nullptr), m_size(size)
{
    if (size == 0) {
        return;
    }

    cudaError_t err = cudaMalloc(&m_devicePtr, size * sizeof(T));
    if (err != cudaSuccess) {
        std::cerr << "CUDA memory allocation failed: " << cudaGetErrorString(err) << std::endl;
        throw std::runtime_error("Failed to allocate CUDA memory");
    }
}

// -----------------------------------------------------------------------------

template<typename T>
CudaArray<T>::~CudaArray()
{
    if (m_devicePtr != nullptr) {
        cudaFree(m_devicePtr);
        m_devicePtr = nullptr;
    }
}

// -----------------------------------------------------------------------------

template<typename T>
T*
CudaArray<T>::getDevicePtr() const
{
    return m_devicePtr;
}

// -----------------------------------------------------------------------------

template<typename T>
T*
CudaArray<T>::data() const
{
    return m_devicePtr;
}

// -----------------------------------------------------------------------------

template<typename T>
std::size_t
CudaArray<T>::size() const
{
    return m_size;
}

// -----------------------------------------------------------------------------

template<typename T>
void
CudaArray<T>::resize(std::size_t newSize)
{
    // Free old memory if it exists
    if (newSize == m_size) {
        return;
    }

    if (m_devicePtr != nullptr) {
        cudaFree(m_devicePtr);
        m_devicePtr = nullptr;
    }

    m_size = newSize;

    if (newSize == 0) {
        return;
    }

    // Allocate new memory
    cudaError_t err = cudaMalloc(&m_devicePtr, newSize * sizeof(T));
    if (err != cudaSuccess) {
        std::cerr << "CUDA memory allocation failed: " << cudaGetErrorString(err) << std::endl;
        throw std::runtime_error("Failed to allocate CUDA memory");
    }
}

// -----------------------------------------------------------------------------

template<typename T>
void
CudaArray<T>::initialize(const T& t)
{
    if (m_size == 0 || m_devicePtr == nullptr) {
        return;
    }

    // Allocate temporary host memory and initialize with value
    std::vector<T> hostData(m_size, t);
    cudaError_t err = cudaMemcpy(m_devicePtr, hostData.data(), m_size * sizeof(T), cudaMemcpyHostToDevice);
    if (err != cudaSuccess) {
        std::cerr << "CUDA memory copy failed: " << cudaGetErrorString(err) << std::endl;
        throw std::runtime_error("Failed to copy data to CUDA memory");
    }
}

// -----------------------------------------------------------------------------

#endif // CUDA_ARRAY_H

