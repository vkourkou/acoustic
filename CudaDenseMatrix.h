#ifndef CUDA_DENSE_MATRIX_H
#define CUDA_DENSE_MATRIX_H

#include <CudaArray.h>
#include <cstddef>
#include <stdexcept>
#include <ostream>
#include <string>
#include <new>

template<typename Elem_t>
class CudaDenseMatrix {
private:
    std::size_t m_rows;
    std::size_t m_cols;
    CudaArray<Elem_t> m_data;

public:
    // Default constructor
    CudaDenseMatrix()
        : m_rows(0), m_cols(0), m_data(0)
    {
    }

    // Constructor with dimensions
    CudaDenseMatrix(std::size_t rows, std::size_t cols)
        : m_rows(rows), m_cols(cols), m_data(rows * cols)
    {
    }

    // Destructor
    ~CudaDenseMatrix() = default;

    // Copy constructor - deleted
    CudaDenseMatrix(const CudaDenseMatrix& other) = delete;

    // Move constructor - deleted
    CudaDenseMatrix(CudaDenseMatrix&& other) noexcept = delete;

    // Copy assignment operator - deleted
    CudaDenseMatrix& operator=(const CudaDenseMatrix& other) = delete;

    // Move assignment operator - deleted
    CudaDenseMatrix& operator=(CudaDenseMatrix&& other) noexcept = delete;

    // -----------------------------------------------------------------------------

    // Get number of rows
    std::size_t
    rows() const;

    // -----------------------------------------------------------------------------

    // Get number of columns
    std::size_t
    cols() const;

    // -----------------------------------------------------------------------------

    // Get total number of elements
    std::size_t
    size() const;

    // -----------------------------------------------------------------------------

    // Check if matrix is empty
    bool
    empty() const;

    // -----------------------------------------------------------------------------

    // Resize matrix
    void
    resize(std::size_t newRows, std::size_t newCols);

    // -----------------------------------------------------------------------------

    // Get device pointer
    Elem_t*
    data() const;

    // -----------------------------------------------------------------------------

    // Initialize all elements to the specified value
    void
    initialize(const Elem_t& t);

    // -----------------------------------------------------------------------------

};

// -----------------------------------------------------------------------------

template<typename Elem_t>
std::size_t
CudaDenseMatrix<Elem_t>::rows() const
{
    return m_rows;
}

// -----------------------------------------------------------------------------

template<typename Elem_t>
std::size_t
CudaDenseMatrix<Elem_t>::cols() const
{
    return m_cols;
}

// -----------------------------------------------------------------------------

template<typename Elem_t>
std::size_t
CudaDenseMatrix<Elem_t>::size() const
{
    return m_data.size();
}

// -----------------------------------------------------------------------------

template<typename Elem_t>
bool
CudaDenseMatrix<Elem_t>::empty() const
{
    return m_data.size() == 0;
}

// -----------------------------------------------------------------------------

template<typename Elem_t>
void
CudaDenseMatrix<Elem_t>::resize(std::size_t newRows, std::size_t newCols)
{
    m_rows = newRows;
    m_cols = newCols;
    m_data.resize(newRows * newCols);
}

// -----------------------------------------------------------------------------

template<typename Elem_t>
Elem_t*
CudaDenseMatrix<Elem_t>::data() const
{
    return m_data.data();
}

// -----------------------------------------------------------------------------

template<typename Elem_t>
void
CudaDenseMatrix<Elem_t>::initialize(const Elem_t& t)
{
    m_data.initialize(t);
}

// -----------------------------------------------------------------------------

#endif // CUDA_DENSE_MATRIX_H

