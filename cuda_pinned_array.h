#ifndef PINNED_ARRAY_H
#define PINNED_ARRAY_H

#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cuda_runtime.h>

// Page-locked host memory array for async host-to-device transfers.
// Grows on demand (never shrinks) to amortize cudaHostAlloc cost across batches.
template<typename T>
class PinnedArray {
    T* m_ptr{nullptr};
    std::size_t m_size{0};

public:
    PinnedArray() = default;
    ~PinnedArray();

    PinnedArray(const PinnedArray&) = delete;
    PinnedArray(PinnedArray&&) = delete;
    PinnedArray& operator=(const PinnedArray&) = delete;
    PinnedArray& operator=(PinnedArray&&) = delete;

    T* data() const { return m_ptr; }
    std::size_t size() const { return m_size; }

    // Ensures capacity for at least newSize elements.
    // Reallocates only when newSize exceeds current capacity.
    void resize(std::size_t newSize);

    // Copies values into pinned memory, growing the buffer if needed.
    void copyFrom(const std::vector<T>& values);
};

// -----------------------------------------------------------------------------

template<typename T>
PinnedArray<T>::~PinnedArray()
{
    if (m_ptr) {
        cudaError_t err = cudaFreeHost(m_ptr);
        if (err != cudaSuccess)
            std::cerr << "cudaFreeHost failed: " << cudaGetErrorString(err) << "\n";
    }
}

// -----------------------------------------------------------------------------

template<typename T>
void
PinnedArray<T>::resize(std::size_t newSize)
{
    if (newSize <= m_size) {
        return;
    }

    if (m_ptr) {
        cudaError_t err = cudaFreeHost(m_ptr);
        if (err != cudaSuccess)
            throw std::runtime_error(std::string("cudaFreeHost failed: ") + cudaGetErrorString(err));
        m_ptr = nullptr;
    }

    cudaError_t err = cudaHostAlloc(&m_ptr, newSize * sizeof(T), cudaHostAllocDefault);
    if (err != cudaSuccess)
        throw std::runtime_error(std::string("cudaHostAlloc failed: ") + cudaGetErrorString(err));

    m_size = newSize;
}

// -----------------------------------------------------------------------------

template<typename T>
void
PinnedArray<T>::copyFrom(const std::vector<T>& values)
{
    resize(values.size());
    std::memcpy(m_ptr, values.data(), values.size() * sizeof(T));
}

// -----------------------------------------------------------------------------

#endif // PINNED_ARRAY_H
