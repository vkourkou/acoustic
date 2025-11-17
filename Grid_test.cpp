#include <Grid.h>
#include <gtest/gtest.h>
#include <cstddef>

// Test fixture for Grid1D tests
class Grid1DTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test constructor with positive size, Min, and Delta
TEST_F(Grid1DTest, ConstructorBasic) {
    Grid1D grid(5, 10, 3);
    
    EXPECT_EQ(5, grid.size());
    EXPECT_FALSE(grid.empty());
    
    // Verify sequence: {10, 13, 16, 19, 22}
    EXPECT_EQ(10, grid[0]);
    EXPECT_EQ(13, grid[1]);
    EXPECT_EQ(16, grid[2]);
    EXPECT_EQ(19, grid[3]);
    EXPECT_EQ(22, grid[4]);
}

// -----------------------------------------------------------------------------

// Test constructor with zero size
TEST_F(Grid1DTest, ConstructorZeroSize) {
    Grid1D grid(0, 10, 3);
    
    EXPECT_EQ(0, grid.size());
    EXPECT_TRUE(grid.empty());
}

// -----------------------------------------------------------------------------

// Test constructor with size 1
TEST_F(Grid1DTest, ConstructorSizeOne) {
    Grid1D grid(1, 42, 5);
    
    EXPECT_EQ(1, grid.size());
    EXPECT_FALSE(grid.empty());
    EXPECT_EQ(42, grid[0]);
}

// -----------------------------------------------------------------------------

// Test constructor with negative Min
TEST_F(Grid1DTest, ConstructorNegativeMin) {
    Grid1D grid(4, -10, 3);
    
    EXPECT_EQ(4, grid.size());
    EXPECT_EQ(-10, grid[0]);
    EXPECT_EQ(-7, grid[1]);
    EXPECT_EQ(-4, grid[2]);
    EXPECT_EQ(-1, grid[3]);
}

// -----------------------------------------------------------------------------

// Test constructor with negative Delta
TEST_F(Grid1DTest, ConstructorNegativeDelta) {
    Grid1D grid(5, 20, -3);
    
    EXPECT_EQ(5, grid.size());
    EXPECT_EQ(20, grid[0]);
    EXPECT_EQ(17, grid[1]);
    EXPECT_EQ(14, grid[2]);
    EXPECT_EQ(11, grid[3]);
    EXPECT_EQ(8, grid[4]);
}

// -----------------------------------------------------------------------------

// Test constructor with zero Delta
TEST_F(Grid1DTest, ConstructorZeroDelta) {
    Grid1D grid(4, 5, 0);
    
    EXPECT_EQ(4, grid.size());
    EXPECT_EQ(5, grid[0]);
    EXPECT_EQ(5, grid[1]);
    EXPECT_EQ(5, grid[2]);
    EXPECT_EQ(5, grid[3]);
}

// -----------------------------------------------------------------------------

// Test constructor with large size
TEST_F(Grid1DTest, ConstructorLargeSize) {
    Grid1D grid(100, 0, 1);
    
    EXPECT_EQ(100, grid.size());
    EXPECT_FALSE(grid.empty());
    
    // Verify first few elements
    EXPECT_EQ(0, grid[0]);
    EXPECT_EQ(1, grid[1]);
    EXPECT_EQ(2, grid[2]);
    
    // Verify last few elements
    EXPECT_EQ(97, grid[97]);
    EXPECT_EQ(98, grid[98]);
    EXPECT_EQ(99, grid[99]);
}

// -----------------------------------------------------------------------------

// Test size method
TEST_F(Grid1DTest, SizeMethod) {
    Grid1D grid1(0, 0, 0);
    EXPECT_EQ(0, grid1.size());
    
    Grid1D grid2(1, 0, 0);
    EXPECT_EQ(1, grid2.size());
    
    Grid1D grid3(100, 0, 0);
    EXPECT_EQ(100, grid3.size());
}

// -----------------------------------------------------------------------------

// Test empty method
TEST_F(Grid1DTest, EmptyMethod) {
    Grid1D grid1(0, 0, 0);
    EXPECT_TRUE(grid1.empty());
    
    Grid1D grid2(1, 0, 0);
    EXPECT_FALSE(grid2.empty());
    
    Grid1D grid3(10, 0, 0);
    EXPECT_FALSE(grid3.empty());
}

// -----------------------------------------------------------------------------

// Test operator[] access
TEST_F(Grid1DTest, OperatorIndexAccess) {
    Grid1D grid(5, 10, 2);
    
    EXPECT_EQ(10, grid[0]);
    EXPECT_EQ(12, grid[1]);
    EXPECT_EQ(14, grid[2]);
    EXPECT_EQ(16, grid[3]);
    EXPECT_EQ(18, grid[4]);
}

// -----------------------------------------------------------------------------

// Test sequence correctness with various parameters
TEST_F(Grid1DTest, SequenceCorrectness) {
    // Test case 1: Min=0, Delta=1
    Grid1D grid1(5, 0, 1);
    for (std::size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(static_cast<Grid_t>(i), grid1[i]);
    }
    
    // Test case 2: Min=5, Delta=2
    Grid1D grid2(4, 5, 2);
    EXPECT_EQ(5, grid2[0]);
    EXPECT_EQ(7, grid2[1]);
    EXPECT_EQ(9, grid2[2]);
    EXPECT_EQ(11, grid2[3]);
    
    // Test case 3: Min=-5, Delta=3
    Grid1D grid3(4, -5, 3);
    EXPECT_EQ(-5, grid3[0]);
    EXPECT_EQ(-2, grid3[1]);
    EXPECT_EQ(1, grid3[2]);
    EXPECT_EQ(4, grid3[3]);
}

// -----------------------------------------------------------------------------

// Test with large Delta
TEST_F(Grid1DTest, LargeDelta) {
    Grid1D grid(5, 0, 100);
    
    EXPECT_EQ(0, grid[0]);
    EXPECT_EQ(100, grid[1]);
    EXPECT_EQ(200, grid[2]);
    EXPECT_EQ(300, grid[3]);
    EXPECT_EQ(400, grid[4]);
}

// -----------------------------------------------------------------------------

// Test with large Min value
TEST_F(Grid1DTest, LargeMin) {
    Grid1D grid(3, 1000, 10);
    
    EXPECT_EQ(1000, grid[0]);
    EXPECT_EQ(1010, grid[1]);
    EXPECT_EQ(1020, grid[2]);
}

// -----------------------------------------------------------------------------

// Test const correctness of operator[]
TEST_F(Grid1DTest, ConstOperatorIndex) {
    const Grid1D grid(3, -2, 2);
    
    EXPECT_EQ(-2, grid[0]);
    EXPECT_EQ(0, grid[1]);
    EXPECT_EQ(2, grid[2]);
}

// -----------------------------------------------------------------------------

