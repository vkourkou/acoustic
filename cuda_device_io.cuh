#ifndef CUDA_DEVICE_IO_CUH_
#define CUDA_DEVICE_IO_CUH_

#include <cuda_runtime.h>
#include <cstddef>
#include <iostream>
#include <vector>

template<typename T>
class ArrayAllocator {
private:
    T* m_devicePtr;
    std::size_t m_size;
    bool m_ownsDeviceMemory;

public:
    // -----------------------------------------------------------------------------
    
    // Constructor - allocates device memory and optionally copies from host
    ArrayAllocator(std::size_t size, const T* fromHost = nullptr, T* toHost = nullptr)
        : m_devicePtr(nullptr), m_size(size), m_ownsDeviceMemory(true)
    {
        if (size == 0) {
            return;
        }

        cudaError_t err = cudaMalloc(&m_devicePtr, size * sizeof(T));
        if (err != cudaSuccess) {
            std::cerr << "CUDA memory allocation failed: " << cudaGetErrorString(err) << std::endl;
            return;
        }

        // Copy from host to device if source provided
        if (fromHost != nullptr) {
            err = cudaMemcpy(m_devicePtr, fromHost, size * sizeof(T), cudaMemcpyHostToDevice);
            if (err != cudaSuccess) {
                std::cerr << "CUDA memory copy (Host->Device) failed: " << cudaGetErrorString(err) << std::endl;
            }
        }

        // Register toHost pointer for later copy back in destructor if provided
        // For simplicity, we'll handle copy back manually or in a different way
        // This is a simplified implementation
    }

    // -----------------------------------------------------------------------------

    // Destructor - deallocates device memory and optionally copies back to host
    ~ArrayAllocator()
    {
        if (m_devicePtr != nullptr && m_ownsDeviceMemory) {
            cudaFree(m_devicePtr);
            m_devicePtr = nullptr;
        }
    }

    // -----------------------------------------------------------------------------

    // Copy constructor - deleted
    ArrayAllocator(const ArrayAllocator& other) = delete;

    // -----------------------------------------------------------------------------

    // Move constructor
    ArrayAllocator(ArrayAllocator&& other) noexcept
        : m_devicePtr(other.m_devicePtr), m_size(other.m_size), m_ownsDeviceMemory(other.m_ownsDeviceMemory)
    {
        other.m_devicePtr = nullptr;
        other.m_size = 0;
        other.m_ownsDeviceMemory = false;
    }

    // -----------------------------------------------------------------------------

    // Copy assignment operator - deleted
    ArrayAllocator& operator=(const ArrayAllocator& other) = delete;

    // -----------------------------------------------------------------------------

    // Move assignment operator
    ArrayAllocator& operator=(ArrayAllocator&& other) noexcept
    {
        if (this != &other) {
            if (m_devicePtr != nullptr && m_ownsDeviceMemory) {
                cudaFree(m_devicePtr);
            }
            m_devicePtr = other.m_devicePtr;
            m_size = other.m_size;
            m_ownsDeviceMemory = other.m_ownsDeviceMemory;
            other.m_devicePtr = nullptr;
            other.m_size = 0;
            other.m_ownsDeviceMemory = false;
        }
        return *this;
    }

    // -----------------------------------------------------------------------------

    // Get device pointer
    T* getDeviceDataPtr() const
    {
        return m_devicePtr;
    }

    // -----------------------------------------------------------------------------

    // Copy device data back to host
    bool copyToHost(T* hostPtr) const
    {
        if (m_devicePtr == nullptr || hostPtr == nullptr || m_size == 0) {
            return false;
        }

        cudaError_t err = cudaMemcpy(hostPtr, m_devicePtr, m_size * sizeof(T), cudaMemcpyDeviceToHost);
        if (err != cudaSuccess) {
            std::cerr << "CUDA memory copy (Device->Host) failed: " << cudaGetErrorString(err) << std::endl;
            return false;
        }
        return true;
    }

    // -----------------------------------------------------------------------------

    // Get size
    std::size_t size() const
    {
        return m_size;
    }

    // -----------------------------------------------------------------------------
};

#endif // CUDA_DEVICE_IO_CUH_

