#ifndef DENSE_MATRIX_H
#define DENSE_MATRIX_H

#include <vector>
#include <cstddef>
#include <stdexcept>
#include <ostream>
#include <string>

template<typename Elem_t>
class DenseMatrix {
private:
    std::size_t m_rows;
    std::size_t m_cols;
    std::vector<Elem_t> m_data;

public:
    // Default constructor
    DenseMatrix()
        : m_rows(0), m_cols(0)
    {
    }

    // Constructor with dimensions (initializes with default value)
    DenseMatrix(std::size_t rows, std::size_t cols)
        : m_rows(rows), m_cols(cols), m_data(rows * cols)
    {
    }

    // Constructor with dimensions and initial value
    DenseMatrix(std::size_t rows, std::size_t cols, const Elem_t& initValue)
        : m_rows(rows), m_cols(cols), m_data(rows * cols, initValue)
    {
    }

    // Destructor
    ~DenseMatrix() = default;

    // Copy constructor - deleted
    DenseMatrix(const DenseMatrix& other) = delete;

    // Move constructor - deleted
    DenseMatrix(DenseMatrix&& other) noexcept = delete;

    // Copy assignment operator - deleted
    DenseMatrix& operator=(const DenseMatrix& other) = delete;

    // Move assignment operator - deleted
    DenseMatrix& operator=(DenseMatrix&& other) noexcept = delete;

    // -----------------------------------------------------------------------------
    
    // Access element at (row, col) - non-const version
    Elem_t&
    operator()(std::size_t row, std::size_t col)
    {
        if (row >= m_rows || col >= m_cols) {
            throw std::out_of_range("DenseMatrix: index out of range (row: " + std::to_string(row) + ", col: " + std::to_string(col) + ")");
        }
        return m_data[row * m_cols + col];
    }

    // -----------------------------------------------------------------------------

    // Access element at (row, col) - const version
    const Elem_t&
    operator()(std::size_t row, std::size_t col) const
    {
        if (row >= m_rows || col >= m_cols) {
            throw std::out_of_range("DenseMatrix: index out of range (row: " + std::to_string(row) + ", col: " + std::to_string(col) + ")");
        }
        return m_data[row * m_cols + col];
    }

    // -----------------------------------------------------------------------------

    // Get number of rows
    std::size_t
    rows() const
    {
        return m_rows;
    }

    // -----------------------------------------------------------------------------

    // Get number of columns
    std::size_t
    cols() const
    {
        return m_cols;
    }

    // -----------------------------------------------------------------------------

    // Get total number of elements
    std::size_t
    size() const
    {
        return m_data.size();
    }

    // -----------------------------------------------------------------------------

    // Check if matrix is empty
    bool
    empty() const
    {
        return m_data.empty();
    }

    // -----------------------------------------------------------------------------

    // Resize matrix
    void
    resize(std::size_t newRows, std::size_t newCols)
    {
        m_rows = newRows;
        m_cols = newCols;
        m_data.resize(newRows * newCols);
    }

    // -----------------------------------------------------------------------------

    // Resize matrix with initial value
    void
    resize(std::size_t newRows, std::size_t newCols, const Elem_t& initValue)
    {
        m_rows = newRows;
        m_cols = newCols;
        m_data.resize(newRows * newCols, initValue);
    }

    // -----------------------------------------------------------------------------

    // Get raw data pointer (non-const)
    Elem_t*
    data()
    {
        return m_data.data();
    }

    // -----------------------------------------------------------------------------

    // Get raw data pointer (const)
    const Elem_t*
    data() const
    {
        return m_data.data();
    }

    // -----------------------------------------------------------------------------

    // Get row-major index for (row, col)
    std::size_t
    getIndex(std::size_t row, std::size_t col) const
    {
        return row * m_cols + col;
    }

    // -----------------------------------------------------------------------------

    // Save matrix to output stream
    void
    save(std::ostream& OS, bool bPrintTranspose, std::size_t pads = 0) const
    {
        auto printPadRows = [](std::ostream& os, std::size_t padCount, std::size_t cols) {
            for (std::size_t padRow = 0; padRow < padCount; ++padRow) {
                for (std::size_t j = 0; j < cols; ++j) {
                    os << "0";
                    if (j < cols - 1) {
                        os << " ";
                    }
                }
                os << "\n";
            }
        };
        
        auto printPadStartCol = [](std::ostream& os, std::size_t padCount) {
            for (std::size_t padCol = 0; padCol < padCount; ++padCol) {
                os << "0 ";
            }
        };

        auto printPadEndCol = [](std::ostream& os, std::size_t padCount) {
            for (std::size_t padCol = 0; padCol < padCount; ++padCol) {
                os << " 0";
            }
        };
        
        if (bPrintTranspose) {
            std::size_t outputRows = m_cols + 2 * pads;
            std::size_t outputCols = m_rows + 2 * pads;
            OS << outputRows << " " << outputCols << "\n";
            
            // Print pads rows of zeros at the start
            printPadRows(OS, pads, outputCols);
            // Print actual matrix data (transposed)
            for (std::size_t i = 0; i < m_cols; ++i) {
                // Print pads columns of zeros at the start
                printPadStartCol(OS, pads);
                // Print actual data
                for (std::size_t j = 0; j < m_rows; ++j) {
                    OS << m_data[j * m_cols + i];
                    if (j < m_rows - 1) {
                        OS << " ";
                    }
                }
                // Print pads columns of zeros at the end
                printPadEndCol(OS, pads);
                OS << "\n";
            }
            // Print pads rows of zeros at the end
            printPadRows(OS, pads, outputCols);
        } else {
            std::size_t outputRows = m_rows + 2 * pads;
            std::size_t outputCols = m_cols + 2 * pads;
            OS << outputRows << " " << outputCols << "\n";
            // Print pads rows of zeros at the start
            printPadRows(OS, pads, outputCols);
            // Print actual matrix data
            for (std::size_t i = 0; i < m_rows; ++i) {
                // Print pads columns of zeros at the start
                printPadStartCol(OS, pads);
                // Print actual data
                for (std::size_t j = 0; j < m_cols; ++j) {
                    OS << m_data[i * m_cols + j];
                    if (j < m_cols - 1) {
                        OS << " ";
                    }
                }
                // Print pads columns of zeros at the end
                printPadEndCol(OS, pads);
                OS << "\n";
            }
            // Print pads rows of zeros at the end
            printPadRows(OS, pads, outputCols);
        }
    }

    // -----------------------------------------------------------------------------

};

#endif // DENSE_MATRIX_H

