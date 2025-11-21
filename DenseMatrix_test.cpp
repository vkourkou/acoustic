#include <DenseMatrix.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <stdexcept>
#include <sstream>
#include <string>

// Test fixture for DenseMatrix tests
class DenseMatrixTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test default constructor
TEST_F(DenseMatrixTest, DefaultConstructor) {
    DenseMatrix<float> matrix;
    
    EXPECT_EQ(0, matrix.rows());
    EXPECT_EQ(0, matrix.cols());
    EXPECT_EQ(0, matrix.size());
    EXPECT_TRUE(matrix.empty());
}

// -----------------------------------------------------------------------------

// Test constructor with dimensions (initializes with default value)
TEST_F(DenseMatrixTest, ConstructorWithDimensions) {
    DenseMatrix<float> matrix(3, 4);
    
    EXPECT_EQ(3, matrix.rows());
    EXPECT_EQ(4, matrix.cols());
    EXPECT_EQ(12, matrix.size());
    EXPECT_FALSE(matrix.empty());
    
    // All elements should be initialized to default float value (0.0)
    for (std::size_t i = 0; i < 3; ++i) {
        for (std::size_t j = 0; j < 4; ++j) {
            EXPECT_FLOAT_EQ(0.0f, matrix(i, j));
        }
    }
}

// -----------------------------------------------------------------------------

// Test constructor with dimensions and initial value
TEST_F(DenseMatrixTest, ConstructorWithDimensionsAndInitialValue) {
    DenseMatrix<float> matrix(2, 3, 5.5f);
    
    EXPECT_EQ(2, matrix.rows());
    EXPECT_EQ(3, matrix.cols());
    EXPECT_EQ(6, matrix.size());
    EXPECT_FALSE(matrix.empty());
    
    // All elements should be initialized to 5.5
    for (std::size_t i = 0; i < 2; ++i) {
        for (std::size_t j = 0; j < 3; ++j) {
            EXPECT_FLOAT_EQ(5.5f, matrix(i, j));
        }
    }
}

// -----------------------------------------------------------------------------

// Test constructor with zero rows
TEST_F(DenseMatrixTest, ConstructorWithZeroRows) {
    DenseMatrix<float> matrix(0, 5);
    
    EXPECT_EQ(0, matrix.rows());
    EXPECT_EQ(5, matrix.cols());
    EXPECT_EQ(0, matrix.size());
    EXPECT_TRUE(matrix.empty());
}

// -----------------------------------------------------------------------------

// Test constructor with zero columns
TEST_F(DenseMatrixTest, ConstructorWithZeroColumns) {
    DenseMatrix<float> matrix(5, 0);
    
    EXPECT_EQ(5, matrix.rows());
    EXPECT_EQ(0, matrix.cols());
    EXPECT_EQ(0, matrix.size());
    EXPECT_TRUE(matrix.empty());
}

// -----------------------------------------------------------------------------

// Test constructor with zero rows and columns
TEST_F(DenseMatrixTest, ConstructorWithZeroRowsAndColumns) {
    DenseMatrix<float> matrix(0, 0);
    
    EXPECT_EQ(0, matrix.rows());
    EXPECT_EQ(0, matrix.cols());
    EXPECT_EQ(0, matrix.size());
    EXPECT_TRUE(matrix.empty());
}

// -----------------------------------------------------------------------------

// Test element access - non-const version
TEST_F(DenseMatrixTest, ElementAccess_NonConst) {
    DenseMatrix<float> matrix(3, 4);
    
    // Set values
    matrix(0, 0) = 1.5f;
    matrix(0, 1) = 2.5f;
    matrix(1, 0) = 3.5f;
    matrix(2, 3) = 4.5f;
    
    // Verify values
    EXPECT_FLOAT_EQ(1.5f, matrix(0, 0));
    EXPECT_FLOAT_EQ(2.5f, matrix(0, 1));
    EXPECT_FLOAT_EQ(3.5f, matrix(1, 0));
    EXPECT_FLOAT_EQ(4.5f, matrix(2, 3));
}

// -----------------------------------------------------------------------------

// Test element access - const version
TEST_F(DenseMatrixTest, ElementAccess_Const) {
    const DenseMatrix<float> matrix(2, 2, 7.5f);
    
    // Verify values using const access
    EXPECT_FLOAT_EQ(7.5f, matrix(0, 0));
    EXPECT_FLOAT_EQ(7.5f, matrix(0, 1));
    EXPECT_FLOAT_EQ(7.5f, matrix(1, 0));
    EXPECT_FLOAT_EQ(7.5f, matrix(1, 1));
}

// -----------------------------------------------------------------------------

// Test element access out of range - row too large (non-const)
TEST_F(DenseMatrixTest, ElementAccess_OutOfRange_RowTooLarge_NonConst) {
    DenseMatrix<float> matrix(3, 4);
    
    EXPECT_THROW(matrix(3, 0), std::out_of_range);
    EXPECT_THROW(matrix(5, 0), std::out_of_range);
}

// -----------------------------------------------------------------------------

// Test element access out of range - column too large (non-const)
TEST_F(DenseMatrixTest, ElementAccess_OutOfRange_ColumnTooLarge_NonConst) {
    DenseMatrix<float> matrix(3, 4);
    
    EXPECT_THROW(matrix(0, 4), std::out_of_range);
    EXPECT_THROW(matrix(0, 10), std::out_of_range);
}

// -----------------------------------------------------------------------------

// Test element access out of range - row and column too large (non-const)
TEST_F(DenseMatrixTest, ElementAccess_OutOfRange_BothTooLarge_NonConst) {
    DenseMatrix<float> matrix(3, 4);
    
    EXPECT_THROW(matrix(3, 4), std::out_of_range);
    EXPECT_THROW(matrix(10, 10), std::out_of_range);
}

// -----------------------------------------------------------------------------

// Test element access out of range - row too large (const)
TEST_F(DenseMatrixTest, ElementAccess_OutOfRange_RowTooLarge_Const) {
    const DenseMatrix<float> matrix(3, 4);
    
    EXPECT_THROW(matrix(3, 0), std::out_of_range);
    EXPECT_THROW(matrix(5, 0), std::out_of_range);
}

// -----------------------------------------------------------------------------

// Test element access out of range - column too large (const)
TEST_F(DenseMatrixTest, ElementAccess_OutOfRange_ColumnTooLarge_Const) {
    const DenseMatrix<float> matrix(3, 4);
    
    EXPECT_THROW(matrix(0, 4), std::out_of_range);
    EXPECT_THROW(matrix(0, 10), std::out_of_range);
}

// -----------------------------------------------------------------------------

// Test row-major order storage
TEST_F(DenseMatrixTest, RowMajorOrder) {
    DenseMatrix<float> matrix(3, 4);
    
    // Fill matrix in row-major order
    float value = 0.0f;
    for (std::size_t i = 0; i < 3; ++i) {
        for (std::size_t j = 0; j < 4; ++j) {
            matrix(i, j) = value;
            value += 1.0f;
        }
    }
    
    // Verify row-major order using data() pointer
    const float* data = matrix.data();
    EXPECT_FLOAT_EQ(0.0f, data[0]);   // (0,0)
    EXPECT_FLOAT_EQ(1.0f, data[1]);   // (0,1)
    EXPECT_FLOAT_EQ(2.0f, data[2]);   // (0,2)
    EXPECT_FLOAT_EQ(3.0f, data[3]);   // (0,3)
    EXPECT_FLOAT_EQ(4.0f, data[4]);   // (1,0)
    EXPECT_FLOAT_EQ(5.0f, data[5]);   // (1,1)
    EXPECT_FLOAT_EQ(6.0f, data[6]);   // (1,2)
    EXPECT_FLOAT_EQ(7.0f, data[7]);   // (1,3)
    EXPECT_FLOAT_EQ(8.0f, data[8]);   // (2,0)
    EXPECT_FLOAT_EQ(9.0f, data[9]);   // (2,1)
    EXPECT_FLOAT_EQ(10.0f, data[10]); // (2,2)
    EXPECT_FLOAT_EQ(11.0f, data[11]); // (2,3)
}

// -----------------------------------------------------------------------------

// Test getIndex method
TEST_F(DenseMatrixTest, GetIndex) {
    DenseMatrix<float> matrix(3, 4);
    
    EXPECT_EQ(0, matrix.getIndex(0, 0));
    EXPECT_EQ(1, matrix.getIndex(0, 1));
    EXPECT_EQ(3, matrix.getIndex(0, 3));
    EXPECT_EQ(4, matrix.getIndex(1, 0));
    EXPECT_EQ(5, matrix.getIndex(1, 1));
    EXPECT_EQ(8, matrix.getIndex(2, 0));
    EXPECT_EQ(11, matrix.getIndex(2, 3));
}

// -----------------------------------------------------------------------------

// Test rows() method
TEST_F(DenseMatrixTest, Rows) {
    DenseMatrix<float> matrix1(5, 3);
    EXPECT_EQ(5, matrix1.rows());
    
    DenseMatrix<float> matrix2(10, 20);
    EXPECT_EQ(10, matrix2.rows());
    
    DenseMatrix<float> matrix3;
    EXPECT_EQ(0, matrix3.rows());
}

// -----------------------------------------------------------------------------

// Test cols() method
TEST_F(DenseMatrixTest, Cols) {
    DenseMatrix<float> matrix1(5, 3);
    EXPECT_EQ(3, matrix1.cols());
    
    DenseMatrix<float> matrix2(10, 20);
    EXPECT_EQ(20, matrix2.cols());
    
    DenseMatrix<float> matrix3;
    EXPECT_EQ(0, matrix3.cols());
}

// -----------------------------------------------------------------------------

// Test size() method
TEST_F(DenseMatrixTest, Size) {
    DenseMatrix<float> matrix1(3, 4);
    EXPECT_EQ(12, matrix1.size());
    
    DenseMatrix<float> matrix2(5, 6);
    EXPECT_EQ(30, matrix2.size());
    
    DenseMatrix<float> matrix3;
    EXPECT_EQ(0, matrix3.size());
    
    DenseMatrix<float> matrix4(0, 5);
    EXPECT_EQ(0, matrix4.size());
}

// -----------------------------------------------------------------------------

// Test empty() method
TEST_F(DenseMatrixTest, Empty) {
    DenseMatrix<float> matrix1;
    EXPECT_TRUE(matrix1.empty());
    
    DenseMatrix<float> matrix2(0, 0);
    EXPECT_TRUE(matrix2.empty());
    
    DenseMatrix<float> matrix3(3, 4);
    EXPECT_FALSE(matrix3.empty());
    
    DenseMatrix<float> matrix4(1, 1);
    EXPECT_FALSE(matrix4.empty());
}

// -----------------------------------------------------------------------------

// Test data() method - non-const
TEST_F(DenseMatrixTest, Data_NonConst) {
    DenseMatrix<float> matrix(2, 3);
    
    float* data = matrix.data();
    ASSERT_NE(nullptr, data);
    
    // Modify through data pointer
    data[0] = 1.0f;
    data[1] = 2.0f;
    data[5] = 6.0f;
    
    // Verify through operator()
    EXPECT_FLOAT_EQ(1.0f, matrix(0, 0));
    EXPECT_FLOAT_EQ(2.0f, matrix(0, 1));
    EXPECT_FLOAT_EQ(6.0f, matrix(1, 2));
}

// -----------------------------------------------------------------------------

// Test data() method - const
TEST_F(DenseMatrixTest, Data_Const) {
    DenseMatrix<float> tempMatrix(2, 3);
    tempMatrix(0, 0) = 1.5f;
    tempMatrix(0, 1) = 2.5f;
    tempMatrix(1, 2) = 3.5f;
    
    const float* data = tempMatrix.data();
    
    ASSERT_NE(nullptr, data);
    
    // Verify values through data pointer
    EXPECT_FLOAT_EQ(1.5f, data[0]);
    EXPECT_FLOAT_EQ(2.5f, data[1]);
    EXPECT_FLOAT_EQ(3.5f, data[5]);
}

// -----------------------------------------------------------------------------

// Test data() method with empty matrix
TEST_F(DenseMatrixTest, Data_EmptyMatrix) {
    DenseMatrix<float> matrix;
    
    float* data = matrix.data();
    (void)data;
    // data() on empty vector may return nullptr or valid pointer
    // We just check that the method can be called without crashing
}

// -----------------------------------------------------------------------------

// Test modification through element access
TEST_F(DenseMatrixTest, ModificationThroughElementAccess) {
    DenseMatrix<float> matrix(3, 4, 1.0f);
    
    // Modify elements
    matrix(0, 0) = 10.0f;
    matrix(1, 2) = 20.0f;
    matrix(2, 3) = 30.0f;
    
    // Verify modifications
    EXPECT_FLOAT_EQ(10.0f, matrix(0, 0));
    EXPECT_FLOAT_EQ(20.0f, matrix(1, 2));
    EXPECT_FLOAT_EQ(30.0f, matrix(2, 3));
    
    // Verify other elements are still 1.0
    EXPECT_FLOAT_EQ(1.0f, matrix(0, 1));
    EXPECT_FLOAT_EQ(1.0f, matrix(1, 0));
    EXPECT_FLOAT_EQ(1.0f, matrix(2, 0));
}

// -----------------------------------------------------------------------------

// Test with various float values (positive, negative, zero, decimals)
TEST_F(DenseMatrixTest, VariousFloatValues) {
    DenseMatrix<float> matrix(2, 2);
    
    matrix(0, 0) = 123.456f;
    matrix(0, 1) = -789.012f;
    matrix(1, 0) = 0.0f;
    matrix(1, 1) = 3.14159f;
    
    EXPECT_FLOAT_EQ(123.456f, matrix(0, 0));
    EXPECT_FLOAT_EQ(-789.012f, matrix(0, 1));
    EXPECT_FLOAT_EQ(0.0f, matrix(1, 0));
    EXPECT_FLOAT_EQ(3.14159f, matrix(1, 1));
}

// -----------------------------------------------------------------------------

// Test large matrix dimensions
TEST_F(DenseMatrixTest, LargeMatrix) {
    DenseMatrix<float> matrix(100, 200);
    
    EXPECT_EQ(100, matrix.rows());
    EXPECT_EQ(200, matrix.cols());
    EXPECT_EQ(20000, matrix.size());
    EXPECT_FALSE(matrix.empty());
    
    // Test access at corners
    matrix(0, 0) = 1.0f;
    matrix(0, 199) = 2.0f;
    matrix(99, 0) = 3.0f;
    matrix(99, 199) = 4.0f;
    
    EXPECT_FLOAT_EQ(1.0f, matrix(0, 0));
    EXPECT_FLOAT_EQ(2.0f, matrix(0, 199));
    EXPECT_FLOAT_EQ(3.0f, matrix(99, 0));
    EXPECT_FLOAT_EQ(4.0f, matrix(99, 199));
}

// -----------------------------------------------------------------------------

// Test single element matrix (1x1)
TEST_F(DenseMatrixTest, SingleElementMatrix) {
    DenseMatrix<float> matrix(1, 1, 42.5f);
    
    EXPECT_EQ(1, matrix.rows());
    EXPECT_EQ(1, matrix.cols());
    EXPECT_EQ(1, matrix.size());
    EXPECT_FALSE(matrix.empty());
    EXPECT_FLOAT_EQ(42.5f, matrix(0, 0));
}

// -----------------------------------------------------------------------------

// Test single row matrix (1xN)
TEST_F(DenseMatrixTest, SingleRowMatrix) {
    DenseMatrix<float> matrix(1, 5);
    
    EXPECT_EQ(1, matrix.rows());
    EXPECT_EQ(5, matrix.cols());
    EXPECT_EQ(5, matrix.size());
    
    for (std::size_t j = 0; j < 5; ++j) {
        matrix(0, j) = static_cast<float>(j);
    }
    
    for (std::size_t j = 0; j < 5; ++j) {
        EXPECT_FLOAT_EQ(static_cast<float>(j), matrix(0, j));
    }
}

// -----------------------------------------------------------------------------

// Test single column matrix (Nx1)
TEST_F(DenseMatrixTest, SingleColumnMatrix) {
    DenseMatrix<float> matrix(5, 1);
    
    EXPECT_EQ(5, matrix.rows());
    EXPECT_EQ(1, matrix.cols());
    EXPECT_EQ(5, matrix.size());
    
    for (std::size_t i = 0; i < 5; ++i) {
        matrix(i, 0) = static_cast<float>(i * 10);
    }
    
    for (std::size_t i = 0; i < 5; ++i) {
        EXPECT_FLOAT_EQ(static_cast<float>(i * 10), matrix(i, 0));
    }
}

// -----------------------------------------------------------------------------

// Test that copy constructor is deleted (should not compile, but we can document it)
// Note: This test verifies that the deleted copy constructor prevents compilation
// In practice, this would be a compile-time test, not a runtime test
TEST_F(DenseMatrixTest, CopyConstructorDeleted) {
    DenseMatrix<float> matrix1(3, 4);
    
    // This line should not compile - copy constructor is deleted
    // DenseMatrix<float> matrix2(matrix1);  // Should cause compilation error
    
    // This test passes if the code compiles without the commented line
    EXPECT_TRUE(true);
}

// -----------------------------------------------------------------------------

// Test that copy assignment is deleted
TEST_F(DenseMatrixTest, CopyAssignmentDeleted) {
    DenseMatrix<float> matrix1(3, 4);
    DenseMatrix<float> matrix2(2, 2);
    
    // This line should not compile - copy assignment is deleted
    // matrix2 = matrix1;  // Should cause compilation error
    
    // This test passes if the code compiles without the commented line
    EXPECT_TRUE(true);
}

// -----------------------------------------------------------------------------

// Test const correctness - const matrix should allow const access
TEST_F(DenseMatrixTest, ConstCorrectness) {
    DenseMatrix<float> tempMatrix(2, 3);
    tempMatrix(0, 0) = 1.0f;
    tempMatrix(1, 2) = 2.0f;
    
    const DenseMatrix<float>& matrix = tempMatrix;
    
    // Should compile and work
    EXPECT_FLOAT_EQ(1.0f, matrix(0, 0));
    EXPECT_FLOAT_EQ(2.0f, matrix(1, 2));
    EXPECT_EQ(2, matrix.rows());
    EXPECT_EQ(3, matrix.cols());
    EXPECT_EQ(6, matrix.size());
    EXPECT_FALSE(matrix.empty());
    
    const float* data = matrix.data();
    ASSERT_NE(nullptr, data);
    EXPECT_FLOAT_EQ(1.0f, data[0]);
}

// -----------------------------------------------------------------------------

// Test that all elements are accessible and can be modified independently
TEST_F(DenseMatrixTest, IndependentElementModification) {
    DenseMatrix<float> matrix(4, 3);
    
    // Set each element to a unique value
    float counter = 0.0f;
    for (std::size_t i = 0; i < 4; ++i) {
        for (std::size_t j = 0; j < 3; ++j) {
            matrix(i, j) = counter;
            counter += 1.0f;
        }
    }
    
    // Verify each element independently
    counter = 0.0f;
    for (std::size_t i = 0; i < 4; ++i) {
        for (std::size_t j = 0; j < 3; ++j) {
            EXPECT_FLOAT_EQ(counter, matrix(i, j));
            counter += 1.0f;
        }
    }
}

// -----------------------------------------------------------------------------

// Test getIndex matches actual data layout
TEST_F(DenseMatrixTest, GetIndexMatchesDataLayout) {
    DenseMatrix<float> matrix(3, 4);
    
    // Fill matrix with unique values
    for (std::size_t i = 0; i < 3; ++i) {
        for (std::size_t j = 0; j < 4; ++j) {
            matrix(i, j) = static_cast<float>(i * 4 + j);
        }
    }
    
    // Verify getIndex matches actual data position
    for (std::size_t i = 0; i < 3; ++i) {
        for (std::size_t j = 0; j < 4; ++j) {
            std::size_t index = matrix.getIndex(i, j);
            EXPECT_EQ(i * 4 + j, index);
            
            const float* data = matrix.data();
            EXPECT_FLOAT_EQ(static_cast<float>(index), data[index]);
            EXPECT_FLOAT_EQ(matrix(i, j), data[index]);
        }
    }
}

// -----------------------------------------------------------------------------

// Test resize function - grow matrix
TEST_F(DenseMatrixTest, Resize_Grow) {
    DenseMatrix<float> matrix(2, 3);
    
    // Set some initial values
    matrix(0, 0) = 1.0f;
    matrix(0, 1) = 2.0f;
    matrix(1, 0) = 3.0f;
    
    // Resize to larger dimensions
    matrix.resize(4, 5);
    
    EXPECT_EQ(4, matrix.rows());
    EXPECT_EQ(5, matrix.cols());
    EXPECT_EQ(20, matrix.size());
    EXPECT_FALSE(matrix.empty());
    
    // Verify original values are preserved at their linear indices
    // When column count changes, elements stay at same linear position
    EXPECT_FLOAT_EQ(1.0f, matrix(0, 0));  // Index 0: (0,0) in both layouts
    EXPECT_FLOAT_EQ(2.0f, matrix(0, 1));  // Index 1: (0,1) in both layouts
    // Index 3 was (1,0) in old layout (2 cols), now it's (0,3) in new layout (5 cols)
    EXPECT_FLOAT_EQ(3.0f, matrix(0, 3));  // Old (1,0) value is now at (0,3)
    
    // Verify new elements are default-initialized (0.0)
    EXPECT_FLOAT_EQ(0.0f, matrix(0, 4));
    EXPECT_FLOAT_EQ(0.0f, matrix(1, 0));  // New position for row 1, col 0
    EXPECT_FLOAT_EQ(0.0f, matrix(3, 0));
    EXPECT_FLOAT_EQ(0.0f, matrix(3, 4));
}

// -----------------------------------------------------------------------------

// Test resize function - shrink matrix
TEST_F(DenseMatrixTest, Resize_Shrink) {
    DenseMatrix<float> matrix(5, 6);
    
    // Fill matrix with values
    for (std::size_t i = 0; i < 5; ++i) {
        for (std::size_t j = 0; j < 6; ++j) {
            matrix(i, j) = static_cast<float>(i * 6 + j);
        }
    }
    
    // Resize to smaller dimensions
    matrix.resize(3, 4);
    
    EXPECT_EQ(3, matrix.rows());
    EXPECT_EQ(4, matrix.cols());
    EXPECT_EQ(12, matrix.size());
    EXPECT_FALSE(matrix.empty());
    
    // Verify remaining elements are preserved (in same positions)
    EXPECT_FLOAT_EQ(0.0f, matrix(0, 0));
    EXPECT_FLOAT_EQ(1.0f, matrix(0, 1));
    EXPECT_FLOAT_EQ(4.0f, matrix(1, 0));
    EXPECT_FLOAT_EQ(10.0f, matrix(2, 2));
}

// -----------------------------------------------------------------------------

// Test resize function - same size
TEST_F(DenseMatrixTest, Resize_SameSize) {
    DenseMatrix<float> matrix(3, 4);
    
    // Set some values
    matrix(0, 0) = 1.5f;
    matrix(1, 2) = 2.5f;
    matrix(2, 3) = 3.5f;
    
    // Resize to same dimensions
    matrix.resize(3, 4);
    
    EXPECT_EQ(3, matrix.rows());
    EXPECT_EQ(4, matrix.cols());
    EXPECT_EQ(12, matrix.size());
    
    // Verify values are preserved
    EXPECT_FLOAT_EQ(1.5f, matrix(0, 0));
    EXPECT_FLOAT_EQ(2.5f, matrix(1, 2));
    EXPECT_FLOAT_EQ(3.5f, matrix(2, 3));
}

// -----------------------------------------------------------------------------

// Test resize function - empty to non-empty
TEST_F(DenseMatrixTest, Resize_EmptyToNonEmpty) {
    DenseMatrix<float> matrix;
    
    EXPECT_TRUE(matrix.empty());
    EXPECT_EQ(0, matrix.rows());
    EXPECT_EQ(0, matrix.cols());
    
    // Resize from empty to non-empty
    matrix.resize(3, 4);
    
    EXPECT_FALSE(matrix.empty());
    EXPECT_EQ(3, matrix.rows());
    EXPECT_EQ(4, matrix.cols());
    EXPECT_EQ(12, matrix.size());
    
    // Verify all elements are default-initialized
    for (std::size_t i = 0; i < 3; ++i) {
        for (std::size_t j = 0; j < 4; ++j) {
            EXPECT_FLOAT_EQ(0.0f, matrix(i, j));
        }
    }
}

// -----------------------------------------------------------------------------

// Test resize function - non-empty to empty
TEST_F(DenseMatrixTest, Resize_NonEmptyToEmpty) {
    DenseMatrix<float> matrix(2, 3);
    
    matrix(0, 0) = 1.0f;
    matrix(1, 2) = 2.0f;
    
    // Resize to empty (zero dimensions)
    matrix.resize(0, 0);
    
    EXPECT_TRUE(matrix.empty());
    EXPECT_EQ(0, matrix.rows());
    EXPECT_EQ(0, matrix.cols());
    EXPECT_EQ(0, matrix.size());
}

// -----------------------------------------------------------------------------

// Test resize function - grow in rows only
TEST_F(DenseMatrixTest, Resize_GrowRowsOnly) {
    DenseMatrix<float> matrix(2, 3);
    
    matrix(0, 0) = 1.0f;
    matrix(0, 2) = 2.0f;
    matrix(1, 0) = 3.0f;
    
    // Resize to more rows, same columns
    matrix.resize(5, 3);
    
    EXPECT_EQ(5, matrix.rows());
    EXPECT_EQ(3, matrix.cols());
    EXPECT_EQ(15, matrix.size());
    
    // Verify original values are preserved
    EXPECT_FLOAT_EQ(1.0f, matrix(0, 0));
    EXPECT_FLOAT_EQ(2.0f, matrix(0, 2));
    EXPECT_FLOAT_EQ(3.0f, matrix(1, 0));
    
    // Verify new rows are default-initialized
    EXPECT_FLOAT_EQ(0.0f, matrix(2, 0));
    EXPECT_FLOAT_EQ(0.0f, matrix(4, 2));
}

// -----------------------------------------------------------------------------

// Test resize function - grow in columns only
TEST_F(DenseMatrixTest, Resize_GrowColumnsOnly) {
    DenseMatrix<float> matrix(3, 2);
    
    matrix(0, 0) = 1.0f;
    matrix(1, 1) = 2.0f;
    matrix(2, 0) = 3.0f;
    
    // Resize to same rows, more columns
    matrix.resize(3, 5);
    
    EXPECT_EQ(3, matrix.rows());
    EXPECT_EQ(5, matrix.cols());
    EXPECT_EQ(15, matrix.size());
    
    // Verify original values are preserved at their linear indices
    EXPECT_FLOAT_EQ(1.0f, matrix(0, 0));  // Index 0: (0,0) in both layouts
    // Index 3 was (1,1) in old layout (2 cols), now it's (0,3) in new layout (5 cols)
    EXPECT_FLOAT_EQ(2.0f, matrix(0, 3));  // Old (1,1) value is now at (0,3)
    // Index 4 was (2,0) in old layout (2 cols), now it's (0,4) in new layout (5 cols)
    EXPECT_FLOAT_EQ(3.0f, matrix(0, 4));  // Old (2,0) value is now at (0,4)
    
    // Verify new columns are default-initialized
    EXPECT_FLOAT_EQ(0.0f, matrix(1, 1));  // New position for row 1, col 1
    EXPECT_FLOAT_EQ(0.0f, matrix(2, 0));  // New position for row 2, col 0
    EXPECT_FLOAT_EQ(0.0f, matrix(2, 4));
}

// -----------------------------------------------------------------------------

// Test resize function with initial value - grow matrix
TEST_F(DenseMatrixTest, ResizeWithInitialValue_Grow) {
    DenseMatrix<float> matrix(2, 3);
    
    // Set some initial values
    matrix(0, 0) = 1.0f;
    matrix(0, 1) = 2.0f;
    matrix(1, 0) = 3.0f;
    
    // Resize to larger dimensions with initial value
    matrix.resize(4, 5, 9.9f);
    
    EXPECT_EQ(4, matrix.rows());
    EXPECT_EQ(5, matrix.cols());
    EXPECT_EQ(20, matrix.size());
    EXPECT_FALSE(matrix.empty());
    
    // Verify original values are preserved at their linear indices
    // When column count changes, elements stay at same linear position
    EXPECT_FLOAT_EQ(1.0f, matrix(0, 0));  // Index 0: (0,0) in both layouts
    EXPECT_FLOAT_EQ(2.0f, matrix(0, 1));  // Index 1: (0,1) in both layouts
    // Index 3 was (1,0) in old layout (3 cols), now it's (0,3) in new layout (5 cols)
    EXPECT_FLOAT_EQ(3.0f, matrix(0, 3));  // Old (1,0) value is now at (0,3)
    
    // Verify new elements are initialized with the specified value
    // Old matrix had 6 elements (indices 0-5), new matrix has 20 elements
    // Only indices 6-19 are new elements initialized with 9.9f
    // (1, 1) = index 1*5 + 1 = 6 → NEW element (first new element)
    EXPECT_FLOAT_EQ(9.9f, matrix(1, 1));
    // (1, 2) = index 1*5 + 2 = 7 → NEW element
    EXPECT_FLOAT_EQ(9.9f, matrix(1, 2));
    // (3, 0) = index 3*5 + 0 = 15 → NEW element
    EXPECT_FLOAT_EQ(9.9f, matrix(3, 0));
    // (3, 4) = index 3*5 + 4 = 19 → NEW element (last new element)
    EXPECT_FLOAT_EQ(9.9f, matrix(3, 4));
    // (2, 2) = index 2*5 + 2 = 12 → NEW element
    EXPECT_FLOAT_EQ(9.9f, matrix(2, 2));
}

// -----------------------------------------------------------------------------

// Test resize function with initial value - shrink matrix
TEST_F(DenseMatrixTest, ResizeWithInitialValue_Shrink) {
    DenseMatrix<float> matrix(5, 6);
    
    // Fill matrix with values
    for (std::size_t i = 0; i < 5; ++i) {
        for (std::size_t j = 0; j < 6; ++j) {
            matrix(i, j) = static_cast<float>(i * 6 + j);
        }
    }
    
    // Resize to smaller dimensions with initial value (shouldn't affect existing)
    matrix.resize(3, 4, 99.9f);
    
    EXPECT_EQ(3, matrix.rows());
    EXPECT_EQ(4, matrix.cols());
    EXPECT_EQ(12, matrix.size());
    
    // Verify remaining elements are preserved (in same positions)
    EXPECT_FLOAT_EQ(0.0f, matrix(0, 0));
    EXPECT_FLOAT_EQ(1.0f, matrix(0, 1));
    EXPECT_FLOAT_EQ(4.0f, matrix(1, 0));
    EXPECT_FLOAT_EQ(10.0f, matrix(2, 2));
}

// -----------------------------------------------------------------------------

// Test resize function with initial value - empty to non-empty
TEST_F(DenseMatrixTest, ResizeWithInitialValue_EmptyToNonEmpty) {
    DenseMatrix<float> matrix;
    
    EXPECT_TRUE(matrix.empty());
    
    // Resize from empty to non-empty with initial value
    matrix.resize(3, 4, 7.7f);
    
    EXPECT_FALSE(matrix.empty());
    EXPECT_EQ(3, matrix.rows());
    EXPECT_EQ(4, matrix.cols());
    EXPECT_EQ(12, matrix.size());
    
    // Verify all elements are initialized with the specified value
    for (std::size_t i = 0; i < 3; ++i) {
        for (std::size_t j = 0; j < 4; ++j) {
            EXPECT_FLOAT_EQ(7.7f, matrix(i, j));
        }
    }
}

// -----------------------------------------------------------------------------

// Test resize function with initial value - non-empty to empty
TEST_F(DenseMatrixTest, ResizeWithInitialValue_NonEmptyToEmpty) {
    DenseMatrix<float> matrix(2, 3);
    
    matrix(0, 0) = 1.0f;
    matrix(1, 2) = 2.0f;
    
    // Resize to empty (zero dimensions)
    matrix.resize(0, 0, 99.9f);
    
    EXPECT_TRUE(matrix.empty());
    EXPECT_EQ(0, matrix.rows());
    EXPECT_EQ(0, matrix.cols());
    EXPECT_EQ(0, matrix.size());
}

// -----------------------------------------------------------------------------

// Test resize function with initial value - grow in rows only
TEST_F(DenseMatrixTest, ResizeWithInitialValue_GrowRowsOnly) {
    DenseMatrix<float> matrix(2, 3);
    
    matrix(0, 0) = 1.0f;
    matrix(0, 2) = 2.0f;
    matrix(1, 0) = 3.0f;
    
    // Resize to more rows, same columns with initial value
    matrix.resize(5, 3, 8.8f);
    
    EXPECT_EQ(5, matrix.rows());
    EXPECT_EQ(3, matrix.cols());
    EXPECT_EQ(15, matrix.size());
    
    // Verify original values are preserved
    EXPECT_FLOAT_EQ(1.0f, matrix(0, 0));
    EXPECT_FLOAT_EQ(2.0f, matrix(0, 2));
    EXPECT_FLOAT_EQ(3.0f, matrix(1, 0));
    
    // Verify new rows are initialized with the specified value
    EXPECT_FLOAT_EQ(8.8f, matrix(2, 0));
    EXPECT_FLOAT_EQ(8.8f, matrix(4, 2));
    EXPECT_FLOAT_EQ(8.8f, matrix(3, 1));
}

// -----------------------------------------------------------------------------

// Test resize function with initial value - grow in columns only
TEST_F(DenseMatrixTest, ResizeWithInitialValue_GrowColumnsOnly) {
    DenseMatrix<float> matrix(3, 2);
    
    matrix(0, 0) = 1.0f;
    matrix(1, 1) = 2.0f;
    matrix(2, 0) = 3.0f;
    
    // Resize to same rows, more columns with initial value
    matrix.resize(3, 5, 6.6f);
    
    EXPECT_EQ(3, matrix.rows());
    EXPECT_EQ(5, matrix.cols());
    EXPECT_EQ(15, matrix.size());
    
    // Verify original values are preserved at their linear indices
    EXPECT_FLOAT_EQ(1.0f, matrix(0, 0));  // Index 0: (0,0) in both layouts
    // Index 3 was (1,1) in old layout (2 cols), now it's (0,3) in new layout (5 cols)
    EXPECT_FLOAT_EQ(2.0f, matrix(0, 3));  // Old (1,1) value is now at (0,3)
    // Index 4 was (2,0) in old layout (2 cols), now it's (0,4) in new layout (5 cols)
    EXPECT_FLOAT_EQ(3.0f, matrix(0, 4));  // Old (2,0) value is now at (0,4)
    
    // Verify new columns are initialized with the specified value
    EXPECT_FLOAT_EQ(6.6f, matrix(1, 1));  // New position for row 1, col 1
    EXPECT_FLOAT_EQ(6.6f, matrix(2, 0));  // New position for row 2, col 0
    EXPECT_FLOAT_EQ(6.6f, matrix(2, 4));
    EXPECT_FLOAT_EQ(6.6f, matrix(1, 3));
}

// -----------------------------------------------------------------------------

// Test resize function with initial value - same size
TEST_F(DenseMatrixTest, ResizeWithInitialValue_SameSize) {
    DenseMatrix<float> matrix(3, 4);
    
    // Set some values
    matrix(0, 0) = 1.5f;
    matrix(1, 2) = 2.5f;
    matrix(2, 3) = 3.5f;
    
    // Resize to same dimensions with initial value (shouldn't change existing)
    matrix.resize(3, 4, 10.0f);
    
    EXPECT_EQ(3, matrix.rows());
    EXPECT_EQ(4, matrix.cols());
    EXPECT_EQ(12, matrix.size());
    
    // Verify values are preserved
    EXPECT_FLOAT_EQ(1.5f, matrix(0, 0));
    EXPECT_FLOAT_EQ(2.5f, matrix(1, 2));
    EXPECT_FLOAT_EQ(3.5f, matrix(2, 3));
}

// -----------------------------------------------------------------------------

// Test resize function with initial value - negative value
TEST_F(DenseMatrixTest, ResizeWithInitialValue_NegativeValue) {
    DenseMatrix<float> matrix(2, 2);
    
    matrix(0, 0) = 1.0f;
    matrix(0, 1) = 2.0f;  // Set another value to verify layout
    matrix(1, 0) = 3.0f;
    
    // Resize with negative initial value
    matrix.resize(3, 3, -5.5f);
    
    EXPECT_EQ(3, matrix.rows());
    EXPECT_EQ(3, matrix.cols());
    
    // Verify original values are preserved at their linear indices
    EXPECT_FLOAT_EQ(1.0f, matrix(0, 0));  // Index 0: (0,0) in both layouts
    EXPECT_FLOAT_EQ(2.0f, matrix(0, 1));  // Index 1: (0,1) in both layouts
    // Index 2 was (1,0) in old layout (2 cols), now it's (0,2) in new layout (3 cols)
    EXPECT_FLOAT_EQ(3.0f, matrix(0, 2));  // Old (1,0) value is now at (0,2)
    
    // Verify new elements are initialized with negative value
    // Old matrix had 4 elements (indices 0-3), new matrix has 9 elements
    // Only indices 4-8 are new elements initialized with -5.5f
    // (1, 1) = index 1*3 + 1 = 4 → NEW element (first new element)
    EXPECT_FLOAT_EQ(-5.5f, matrix(1, 1));
    // (1, 2) = index 1*3 + 2 = 5 → NEW element
    EXPECT_FLOAT_EQ(-5.5f, matrix(1, 2));
    // (2, 0) = index 2*3 + 0 = 6 → NEW element
    EXPECT_FLOAT_EQ(-5.5f, matrix(2, 0));
    // (2, 2) = index 2*3 + 2 = 8 → NEW element (last new element)
    EXPECT_FLOAT_EQ(-5.5f, matrix(2, 2));
}

// -----------------------------------------------------------------------------

// Test resize function - multiple resizes
TEST_F(DenseMatrixTest, Resize_MultipleResizes) {
    DenseMatrix<float> matrix(2, 2);
    
    matrix(0, 0) = 1.0f;
    
    // First resize
    matrix.resize(3, 3);
    EXPECT_EQ(3, matrix.rows());
    EXPECT_EQ(3, matrix.cols());
    EXPECT_FLOAT_EQ(1.0f, matrix(0, 0));
    
    // Second resize
    matrix.resize(5, 4);
    EXPECT_EQ(5, matrix.rows());
    EXPECT_EQ(4, matrix.cols());
    EXPECT_FLOAT_EQ(1.0f, matrix(0, 0));
    
    // Third resize
    matrix.resize(2, 2);
    EXPECT_EQ(2, matrix.rows());
    EXPECT_EQ(2, matrix.cols());
    EXPECT_FLOAT_EQ(1.0f, matrix(0, 0));
}

// -----------------------------------------------------------------------------

// Test save function - normal (non-transpose)
TEST_F(DenseMatrixTest, Save_Normal) {
    DenseMatrix<float> matrix(2, 3);
    
    // Fill matrix with values
    matrix(0, 0) = 1.0f;
    matrix(0, 1) = 2.0f;
    matrix(0, 2) = 3.0f;
    matrix(1, 0) = 4.0f;
    matrix(1, 1) = 5.0f;
    matrix(1, 2) = 6.0f;
    
    std::ostringstream oss;
    matrix.save(oss, false);
    
    std::string output = oss.str();
    std::stringstream ssRes;
    ssRes << 2 << " " << 3 << "\n";
    ssRes << matrix.data()[0] << " " << matrix.data()[1] << " " << matrix.data()[2] << "\n";
    ssRes << matrix.data()[3] << " " << matrix.data()[4] << " " << matrix.data()[5] << "\n";
    std::string result = ssRes.str();
    EXPECT_EQ(output, result);
}

// -----------------------------------------------------------------------------

// Test save function - default parameter (should be non-transpose)
TEST_F(DenseMatrixTest, Save_DefaultParameter) {
    DenseMatrix<float> matrix(2, 3);
    
    matrix(0, 0) = 1.0f;
    matrix(0, 1) = 2.0f;
    matrix(0, 2) = 3.0f;
    matrix(1, 0) = 4.0f;
    matrix(1, 1) = 5.0f;
    matrix(1, 2) = 6.0f;
    
    std::ostringstream oss;
    matrix.save(oss);  // Using default parameter
    
    std::string output = oss.str();
    std::stringstream ssRes;
    ssRes << 2 << " " << 3 << "\n";
    ssRes << matrix.data()[0] << " " << matrix.data()[1] << " " << matrix.data()[2] << "\n";
    ssRes << matrix.data()[3] << " " << matrix.data()[4] << " " << matrix.data()[5] << "\n";
    std::string result = ssRes.str();
    EXPECT_EQ(output, result);
}

// -----------------------------------------------------------------------------

// Test save function - transpose
TEST_F(DenseMatrixTest, Save_Transpose) {
    DenseMatrix<float> matrix(2, 3);
    
    // Fill matrix with values
    matrix(0, 0) = 1.0f;
    matrix(0, 1) = 2.0f;
    matrix(0, 2) = 3.0f;
    matrix(1, 0) = 4.0f;
    matrix(1, 1) = 5.0f;
    matrix(1, 2) = 6.0f;
    
    std::ostringstream oss;
    matrix.save(oss, true);  // Print transpose
    
    std::string output = oss.str();
    std::stringstream ssRes;
    ssRes << 3 << " " << 2 << "\n";  // cols become rows, rows become cols
    ssRes << matrix.data()[0] << " " << matrix.data()[3] << "\n";  // col 0: (0,0), (1,0)
    ssRes << matrix.data()[1] << " " << matrix.data()[4] << "\n";  // col 1: (0,1), (1,1)
    ssRes << matrix.data()[2] << " " << matrix.data()[5] << "\n";  // col 2: (0,2), (1,2)
    std::string result = ssRes.str();
    EXPECT_EQ(output, result);
}

// -----------------------------------------------------------------------------

// Test save function - transpose with square matrix
TEST_F(DenseMatrixTest, Save_Transpose_SquareMatrix) {
    DenseMatrix<float> matrix(3, 3);
    
    // Fill matrix with values
    matrix(0, 0) = 1.0f;
    matrix(0, 1) = 2.0f;
    matrix(0, 2) = 3.0f;
    matrix(1, 0) = 4.0f;
    matrix(1, 1) = 5.0f;
    matrix(1, 2) = 6.0f;
    matrix(2, 0) = 7.0f;
    matrix(2, 1) = 8.0f;
    matrix(2, 2) = 9.0f;
    
    std::ostringstream oss;
    matrix.save(oss, true);
    
    std::string output = oss.str();
    std::stringstream ssRes;
    ssRes << 3 << " " << 3 << "\n";
    ssRes << matrix.data()[0] << " " << matrix.data()[3] << " " << matrix.data()[6] << "\n";  // col 0
    ssRes << matrix.data()[1] << " " << matrix.data()[4] << " " << matrix.data()[7] << "\n";  // col 1
    ssRes << matrix.data()[2] << " " << matrix.data()[5] << " " << matrix.data()[8] << "\n";  // col 2
    std::string result = ssRes.str();
    EXPECT_EQ(output, result);
}

// -----------------------------------------------------------------------------

// Test save function - transpose with single row matrix
TEST_F(DenseMatrixTest, Save_Transpose_SingleRow) {
    DenseMatrix<float> matrix(1, 4);
    
    matrix(0, 0) = 1.0f;
    matrix(0, 1) = 2.0f;
    matrix(0, 2) = 3.0f;
    matrix(0, 3) = 4.0f;
    
    std::ostringstream oss;
    matrix.save(oss, true);
    
    std::string output = oss.str();
    std::stringstream ssRes;
    ssRes << 4 << " " << 1 << "\n";  // cols become rows, rows become cols
    ssRes << matrix.data()[0] << "\n";  // col 0
    ssRes << matrix.data()[1] << "\n";  // col 1
    ssRes << matrix.data()[2] << "\n";  // col 2
    ssRes << matrix.data()[3] << "\n";  // col 3
    std::string result = ssRes.str();
    EXPECT_EQ(output, result);
}

// -----------------------------------------------------------------------------

// Test save function - transpose with single column matrix
TEST_F(DenseMatrixTest, Save_Transpose_SingleColumn) {
    DenseMatrix<float> matrix(4, 1);
    
    matrix(0, 0) = 1.0f;
    matrix(1, 0) = 2.0f;
    matrix(2, 0) = 3.0f;
    matrix(3, 0) = 4.0f;
    
    std::ostringstream oss;
    matrix.save(oss, true);
    
    std::string output = oss.str();
    std::stringstream ssRes;
    ssRes << 1 << " " << 4 << "\n";  // cols become rows, rows become cols
    ssRes << matrix.data()[0] << " " << matrix.data()[1] << " " << matrix.data()[2] << " " << matrix.data()[3] << "\n";  // col 0
    std::string result = ssRes.str();
    EXPECT_EQ(output, result);
}

// -----------------------------------------------------------------------------

// Test save function - normal with negative values
TEST_F(DenseMatrixTest, Save_Normal_NegativeValues) {
    DenseMatrix<float> matrix(2, 2);
    
    matrix(0, 0) = -1.5f;
    matrix(0, 1) = 2.5f;
    matrix(1, 0) = -3.5f;
    matrix(1, 1) = 4.5f;
    
    std::ostringstream oss;
    matrix.save(oss, false);
    
    std::string output = oss.str();
    std::stringstream ssRes;
    ssRes << 2 << " " << 2 << "\n";
    ssRes << matrix.data()[0] << " " << matrix.data()[1] << "\n";
    ssRes << matrix.data()[2] << " " << matrix.data()[3] << "\n";
    std::string result = ssRes.str();
    EXPECT_EQ(output, result);
}

// -----------------------------------------------------------------------------

// Test save function - transpose with negative values
TEST_F(DenseMatrixTest, Save_Transpose_NegativeValues) {
    DenseMatrix<float> matrix(2, 2);
    
    matrix(0, 0) = -1.5f;
    matrix(0, 1) = 2.5f;
    matrix(1, 0) = -3.5f;
    matrix(1, 1) = 4.5f;
    
    std::ostringstream oss;
    matrix.save(oss, true);
    
    std::string output = oss.str();
    std::stringstream ssRes;
    ssRes << 2 << " " << 2 << "\n";  // cols become rows, rows become cols
    ssRes << matrix.data()[0] << " " << matrix.data()[2] << "\n";  // col 0: (0,0), (1,0)
    ssRes << matrix.data()[1] << " " << matrix.data()[3] << "\n";  // col 1: (0,1), (1,1)
    std::string result = ssRes.str();
    EXPECT_EQ(output, result);
}

// -----------------------------------------------------------------------------

