#include <Grid.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <sstream>

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
    EXPECT_TRUE(grid.isSane());
    
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
    EXPECT_TRUE(grid.isSane());
}

// -----------------------------------------------------------------------------

// Test constructor with size 1
TEST_F(Grid1DTest, ConstructorSizeOne) {
    Grid1D grid(1, 42, 5);
    
    EXPECT_EQ(1, grid.size());
    EXPECT_FALSE(grid.empty());
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(42, grid[0]);
}

// -----------------------------------------------------------------------------

// Test constructor with negative Min
TEST_F(Grid1DTest, ConstructorNegativeMin) {
    Grid1D grid(4, -10, 3);
    
    EXPECT_EQ(4, grid.size());
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(-10, grid[0]);
    EXPECT_EQ(-7, grid[1]);
    EXPECT_EQ(-4, grid[2]);
    EXPECT_EQ(-1, grid[3]);
}

// -----------------------------------------------------------------------------

// Test constructor with large size
TEST_F(Grid1DTest, ConstructorLargeSize) {
    Grid1D grid(100, 0, 1);
    
    EXPECT_EQ(100, grid.size());
    EXPECT_FALSE(grid.empty());
    EXPECT_TRUE(grid.isSane());
    
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
    Grid1D grid1(0, 0, 1);
    EXPECT_EQ(0, grid1.size());
    EXPECT_TRUE(grid1.isSane());
    
    Grid1D grid2(1, 0, 1);
    EXPECT_EQ(1, grid2.size());
    EXPECT_TRUE(grid2.isSane());
    
    Grid1D grid3(100, 0, 1);
    EXPECT_EQ(100, grid3.size());
    EXPECT_TRUE(grid3.isSane());
}

// -----------------------------------------------------------------------------

// Test empty method
TEST_F(Grid1DTest, EmptyMethod) {
    Grid1D grid1(0, 0, 1);
    EXPECT_TRUE(grid1.empty());
    EXPECT_TRUE(grid1.isSane());
    
    Grid1D grid2(1, 0, 1);
    EXPECT_FALSE(grid2.empty());
    EXPECT_TRUE(grid2.isSane());
    
    Grid1D grid3(10, 0, 1);
    EXPECT_FALSE(grid3.empty());
    EXPECT_TRUE(grid3.isSane());
}

// -----------------------------------------------------------------------------

// Test operator[] access
TEST_F(Grid1DTest, OperatorIndexAccess) {
    Grid1D grid(5, 10, 2);
    
    EXPECT_TRUE(grid.isSane());
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
    EXPECT_TRUE(grid1.isSane());
    for (std::size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(static_cast<Grid_t>(i), grid1[i]);
    }
    
    // Test case 2: Min=5, Delta=2
    Grid1D grid2(4, 5, 2);
    EXPECT_TRUE(grid2.isSane());
    EXPECT_EQ(5, grid2[0]);
    EXPECT_EQ(7, grid2[1]);
    EXPECT_EQ(9, grid2[2]);
    EXPECT_EQ(11, grid2[3]);
    
    // Test case 3: Min=-5, Delta=3
    Grid1D grid3(4, -5, 3);
    EXPECT_TRUE(grid3.isSane());
    EXPECT_EQ(-5, grid3[0]);
    EXPECT_EQ(-2, grid3[1]);
    EXPECT_EQ(1, grid3[2]);
    EXPECT_EQ(4, grid3[3]);
}

// -----------------------------------------------------------------------------

// Test with large Delta
TEST_F(Grid1DTest, LargeDelta) {
    Grid1D grid(5, 0, 100);
    
    EXPECT_TRUE(grid.isSane());
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
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(1000, grid[0]);
    EXPECT_EQ(1010, grid[1]);
    EXPECT_EQ(1020, grid[2]);
}

// -----------------------------------------------------------------------------

// Test const correctness of operator[]
TEST_F(Grid1DTest, ConstOperatorIndex) {
    const Grid1D grid(3, -2, 2);
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(-2, grid[0]);
    EXPECT_EQ(0, grid[1]);
    EXPECT_EQ(2, grid[2]);
}

// -----------------------------------------------------------------------------

// Test save with basic grid
TEST_F(Grid1DTest, SaveBasic) {
    Grid1D grid(5, 10, 3);
    
    EXPECT_TRUE(grid.isSane());
    std::ostringstream oss;
    grid.save(oss);
    
    EXPECT_EQ("5 10 13 16 19 22", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with zero size grid
TEST_F(Grid1DTest, SaveZeroSize) {
    Grid1D grid(0, 10, 3);
    
    EXPECT_TRUE(grid.isSane());
    std::ostringstream oss;
    grid.save(oss);
    
    EXPECT_EQ("0", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with size 1
TEST_F(Grid1DTest, SaveSizeOne) {
    Grid1D grid(1, 42, 5);
    
    EXPECT_TRUE(grid.isSane());
    std::ostringstream oss;
    grid.save(oss);
    
    EXPECT_EQ("1 42", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with negative values
TEST_F(Grid1DTest, SaveNegativeValues) {
    Grid1D grid(4, -10, 3);
    
    EXPECT_TRUE(grid.isSane());
    std::ostringstream oss;
    grid.save(oss);
    
    EXPECT_EQ("4 -10 -7 -4 -1", oss.str());
}

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

// Test save const correctness
TEST_F(Grid1DTest, SaveConstCorrectness) {
    const Grid1D grid(3, 5, 2);
    
    EXPECT_TRUE(grid.isSane());
    std::ostringstream oss;
    grid.save(oss);
    
    EXPECT_EQ("3 5 7 9", oss.str());
}

// -----------------------------------------------------------------------------

// Test computeDiffs with basic grid (constant Delta)
TEST_F(Grid1DTest, ComputeDiffsBasic) {
    Grid1D grid(5, 10, 3);
    
    EXPECT_TRUE(grid.isSane());
    std::vector<Grid_t> diffs = grid.computeDiffs();
    
    EXPECT_EQ(4, diffs.size());
    EXPECT_EQ(3, diffs[0]);
    EXPECT_EQ(3, diffs[1]);
    EXPECT_EQ(3, diffs[2]);
    EXPECT_EQ(3, diffs[3]);
}

// -----------------------------------------------------------------------------

// Test computeDiffs with zero size grid
TEST_F(Grid1DTest, ComputeDiffsZeroSize) {
    Grid1D grid(0, 10, 3);
    
    EXPECT_TRUE(grid.isSane());
    std::vector<Grid_t> diffs = grid.computeDiffs();
    
    EXPECT_EQ(0, diffs.size());
    EXPECT_TRUE(diffs.empty());
}

// -----------------------------------------------------------------------------

// Test computeDiffs with size 1 grid
TEST_F(Grid1DTest, ComputeDiffsSizeOne) {
    Grid1D grid(1, 42, 5);
    
    EXPECT_TRUE(grid.isSane());
    std::vector<Grid_t> diffs = grid.computeDiffs();
    
    EXPECT_EQ(0, diffs.size());
    EXPECT_TRUE(diffs.empty());
}

// -----------------------------------------------------------------------------

// Test computeDiffs with negative values
TEST_F(Grid1DTest, ComputeDiffsNegativeValues) {
    Grid1D grid(4, -10, 3);
    
    EXPECT_TRUE(grid.isSane());
    std::vector<Grid_t> diffs = grid.computeDiffs();
    
    EXPECT_EQ(3, diffs.size());
    EXPECT_EQ(3, diffs[0]);
    EXPECT_EQ(3, diffs[1]);
    EXPECT_EQ(3, diffs[2]);
}

// -----------------------------------------------------------------------------

// Test computeDiffs with large grid
TEST_F(Grid1DTest, ComputeDiffsLargeGrid) {
    Grid1D grid(100, 0, 1);
    
    EXPECT_TRUE(grid.isSane());
    std::vector<Grid_t> diffs = grid.computeDiffs();
    
    EXPECT_EQ(99, diffs.size());
    // All diffs should be 1
    for (std::size_t i = 0; i < diffs.size(); ++i) {
        EXPECT_EQ(1, diffs[i]);
    }
}

// -----------------------------------------------------------------------------

// Test computeDiffs const correctness
TEST_F(Grid1DTest, ComputeDiffsConstCorrectness) {
    const Grid1D grid(4, 5, 2);
    
    EXPECT_TRUE(grid.isSane());
    std::vector<Grid_t> diffs = grid.computeDiffs();
    
    EXPECT_EQ(3, diffs.size());
    EXPECT_EQ(2, diffs[0]);
    EXPECT_EQ(2, diffs[1]);
    EXPECT_EQ(2, diffs[2]);
}

// -----------------------------------------------------------------------------

// Test computeDiffs correctness with specific values
TEST_F(Grid1DTest, ComputeDiffsCorrectness) {
    Grid1D grid(5, 10, 3);
    // Grid values: {10, 13, 16, 19, 22}
    // Expected diffs: {3, 3, 3, 3}
    
    EXPECT_TRUE(grid.isSane());
    std::vector<Grid_t> diffs = grid.computeDiffs();
    
    ASSERT_EQ(4, diffs.size());
    for (std::size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(3, diffs[i]) << "Difference at index " << i << " should be 3";
    }
}

// -----------------------------------------------------------------------------

// Test isUniform with uniform grid (constant Delta)
TEST_F(Grid1DTest, IsUniformTrue) {
    Grid1D grid(5, 10, 3);
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.isUniform());
}

// -----------------------------------------------------------------------------

// Test isUniform with empty grid
TEST_F(Grid1DTest, IsUniformEmpty) {
    Grid1D grid(0, 10, 3);
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.isUniform());
}

// -----------------------------------------------------------------------------

// Test isUniform with size 1 grid
TEST_F(Grid1DTest, IsUniformSizeOne) {
    Grid1D grid(1, 42, 5);
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.isUniform());
}

// -----------------------------------------------------------------------------

// Test isUniform with size 2 grid
TEST_F(Grid1DTest, IsUniformSizeTwo) {
    Grid1D grid(2, 10, 3);
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.isUniform());
}

// -----------------------------------------------------------------------------

// Test isUniform with large uniform grid
TEST_F(Grid1DTest, IsUniformLarge) {
    Grid1D grid(100, 0, 1);
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.isUniform());
}

// -----------------------------------------------------------------------------

// Test isUniform with different Delta values
TEST_F(Grid1DTest, IsUniformDifferentDeltas) {
    Grid1D grid1(5, 0, 1);
    EXPECT_TRUE(grid1.isSane());
    EXPECT_TRUE(grid1.isUniform());
    
    Grid1D grid2(5, 0, 2);
    EXPECT_TRUE(grid2.isSane());
    EXPECT_TRUE(grid2.isUniform());
    
    Grid1D grid3(5, 0, 5);
    EXPECT_TRUE(grid3.isSane());
    EXPECT_TRUE(grid3.isUniform());
    
    Grid1D grid4(5, 0, 100);
    EXPECT_TRUE(grid4.isSane());
    EXPECT_TRUE(grid4.isUniform());
}

// -----------------------------------------------------------------------------

// Test isUniform with negative Min values
TEST_F(Grid1DTest, IsUniformNegativeMin) {
    Grid1D grid(4, -10, 3);
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.isUniform());
}

// -----------------------------------------------------------------------------

// Test isUniform const correctness
TEST_F(Grid1DTest, IsUniformConstCorrectness) {
    const Grid1D grid(5, 10, 3);
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.isUniform());
}

// -----------------------------------------------------------------------------

// Test isUniform with non-uniform grid (after adding point that breaks uniformity)
TEST_F(Grid1DTest, IsUniformFalse) {
    Grid1D grid(3, 10, 3);
    // Grid: {10, 13, 16} - uniform with spacing 3
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.isUniform());
    
    // Add point that breaks uniformity: 12 between 10 and 13
    // Grid becomes: {10, 12, 13, 16} with spacings {2, 1, 3} - not uniform!
    EXPECT_TRUE(grid.addGridPoint(12));
    EXPECT_TRUE(grid.isSane());
    EXPECT_FALSE(grid.isUniform());
    
    // Verify the grid values
    EXPECT_EQ(4, grid.size());
    EXPECT_EQ(10, grid[0]);
    EXPECT_EQ(12, grid[1]);
    EXPECT_EQ(13, grid[2]);
    EXPECT_EQ(16, grid[3]);
}

// -----------------------------------------------------------------------------

// Test addGridPoint to empty grid
TEST_F(Grid1DTest, AddGridPointEmpty) {
    Grid1D grid(0, 10, 3);
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint(5));
    EXPECT_EQ(1, grid.size());
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(5, grid[0]);
}

// -----------------------------------------------------------------------------

// Test addGridPoint to end (larger than all)
TEST_F(Grid1DTest, AddGridPointToEnd) {
    Grid1D grid(3, 10, 3);
    // Grid: {10, 13, 16}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint(20));
    EXPECT_EQ(4, grid.size());
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(10, grid[0]);
    EXPECT_EQ(13, grid[1]);
    EXPECT_EQ(16, grid[2]);
    EXPECT_EQ(20, grid[3]);
}

// -----------------------------------------------------------------------------

// Test addGridPoint to beginning (smaller than all)
TEST_F(Grid1DTest, AddGridPointToBeginning) {
    Grid1D grid(3, 10, 3);
    // Grid: {10, 13, 16}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint(5));
    EXPECT_EQ(4, grid.size());
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(5, grid[0]);
    EXPECT_EQ(10, grid[1]);
    EXPECT_EQ(13, grid[2]);
    EXPECT_EQ(16, grid[3]);
}

// -----------------------------------------------------------------------------

// Test addGridPoint to middle
TEST_F(Grid1DTest, AddGridPointToMiddle) {
    Grid1D grid(3, 10, 3);
    // Grid: {10, 13, 16}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint(12));
    EXPECT_EQ(4, grid.size());
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(10, grid[0]);
    EXPECT_EQ(12, grid[1]);
    EXPECT_EQ(13, grid[2]);
    EXPECT_EQ(16, grid[3]);
}

// -----------------------------------------------------------------------------

// Test addGridPoint duplicate (should return false)
TEST_F(Grid1DTest, AddGridPointDuplicate) {
    Grid1D grid(3, 10, 3);
    // Grid: {10, 13, 16}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_FALSE(grid.addGridPoint(10));
    EXPECT_EQ(3, grid.size());
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(10, grid[0]);
    EXPECT_EQ(13, grid[1]);
    EXPECT_EQ(16, grid[2]);
    
    EXPECT_FALSE(grid.addGridPoint(13));
    EXPECT_EQ(3, grid.size());
    EXPECT_TRUE(grid.isSane());
    
    EXPECT_FALSE(grid.addGridPoint(16));
    EXPECT_EQ(3, grid.size());
    EXPECT_TRUE(grid.isSane());
}

// -----------------------------------------------------------------------------

// Test addGridPoint multiple additions maintaining order
TEST_F(Grid1DTest, AddGridPointMultiple) {
    Grid1D grid(0, 0, 1);
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint(5));
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint(2));
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint(8));
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint(1));
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint(7));
    EXPECT_TRUE(grid.isSane());
    
    EXPECT_EQ(5, grid.size());
    EXPECT_EQ(1, grid[0]);
    EXPECT_EQ(2, grid[1]);
    EXPECT_EQ(5, grid[2]);
    EXPECT_EQ(7, grid[3]);
    EXPECT_EQ(8, grid[4]);
}

// -----------------------------------------------------------------------------

// Test addGridPoint with negative values
TEST_F(Grid1DTest, AddGridPointNegative) {
    Grid1D grid(2, 0, 1);
    // Grid: {0, 1}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint(-5));
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint(-2));
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint(3));
    EXPECT_TRUE(grid.isSane());
    
    EXPECT_EQ(5, grid.size());
    EXPECT_EQ(-5, grid[0]);
    EXPECT_EQ(-2, grid[1]);
    EXPECT_EQ(0, grid[2]);
    EXPECT_EQ(1, grid[3]);
    EXPECT_EQ(3, grid[4]);
}

// -----------------------------------------------------------------------------

// Test addGridPoint duplicate with negative values
TEST_F(Grid1DTest, AddGridPointDuplicateNegative) {
    Grid1D grid(2, -10, 3);
    // Grid: {-10, -7}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_FALSE(grid.addGridPoint(-10));
    EXPECT_TRUE(grid.isSane());
    EXPECT_FALSE(grid.addGridPoint(-7));
    EXPECT_EQ(2, grid.size());
    EXPECT_TRUE(grid.isSane());
}

// -----------------------------------------------------------------------------

// Test addGridPoint maintaining order with existing grid
TEST_F(Grid1DTest, AddGridPointMaintainOrder) {
    Grid1D grid(4, 10, 3);
    // Grid: {10, 13, 16, 19}
    
    EXPECT_TRUE(grid.isSane());
    // Add value between existing points
    EXPECT_TRUE(grid.addGridPoint(14));
    EXPECT_EQ(5, grid.size());
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(10, grid[0]);
    EXPECT_EQ(13, grid[1]);
    EXPECT_EQ(14, grid[2]);
    EXPECT_EQ(16, grid[3]);
    EXPECT_EQ(19, grid[4]);
    
    // Add value at the end
    EXPECT_TRUE(grid.addGridPoint(25));
    EXPECT_EQ(6, grid.size());
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(25, grid[5]);
    
    // Add value at the beginning
    EXPECT_TRUE(grid.addGridPoint(5));
    EXPECT_EQ(7, grid.size());
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(5, grid[0]);
}

// -----------------------------------------------------------------------------

// Test addGridPoint with single element grid
TEST_F(Grid1DTest, AddGridPointSizeOne) {
    Grid1D grid(1, 10, 3);
    // Grid: {10}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint(5));
    EXPECT_EQ(2, grid.size());
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(5, grid[0]);
    EXPECT_EQ(10, grid[1]);
    
    EXPECT_TRUE(grid.addGridPoint(15));
    EXPECT_EQ(3, grid.size());
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(5, grid[0]);
    EXPECT_EQ(10, grid[1]);
    EXPECT_EQ(15, grid[2]);
}

// -----------------------------------------------------------------------------

// Test front() method with basic grid
TEST_F(Grid1DTest, FrontBasic) {
    Grid1D grid(5, 10, 3);
    // Grid: {10, 13, 16, 19, 22}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(10, grid.front());
}

// -----------------------------------------------------------------------------

// Test front() method with negative Min
TEST_F(Grid1DTest, FrontNegativeMin) {
    Grid1D grid(4, -10, 3);
    // Grid: {-10, -7, -4, -1}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(-10, grid.front());
}

// -----------------------------------------------------------------------------

// Test front() method with size 1
TEST_F(Grid1DTest, FrontSizeOne) {
    Grid1D grid(1, 42, 5);
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(42, grid.front());
}

// -----------------------------------------------------------------------------

// Test back() method with basic grid
TEST_F(Grid1DTest, BackBasic) {
    Grid1D grid(5, 10, 3);
    // Grid: {10, 13, 16, 19, 22}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(22, grid.back());
}

// -----------------------------------------------------------------------------

// Test back() method with negative Min
TEST_F(Grid1DTest, BackNegativeMin) {
    Grid1D grid(4, -10, 3);
    // Grid: {-10, -7, -4, -1}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(-1, grid.back());
}

// -----------------------------------------------------------------------------

// Test back() method with size 1
TEST_F(Grid1DTest, BackSizeOne) {
    Grid1D grid(1, 42, 5);
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(42, grid.back());
}

// -----------------------------------------------------------------------------

// Test front() and back() are same for size 1
TEST_F(Grid1DTest, FrontBackSameSizeOne) {
    Grid1D grid(1, 100, 1);
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(grid.front(), grid.back());
    EXPECT_EQ(100, grid.front());
}

// -----------------------------------------------------------------------------

// Test front() and back() const correctness
TEST_F(Grid1DTest, FrontBackConstCorrectness) {
    const Grid1D grid(5, 10, 3);
    // Grid: {10, 13, 16, 19, 22}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(10, grid.front());
    EXPECT_EQ(22, grid.back());
}

// -----------------------------------------------------------------------------

// Test findIndexForClosestGridPoint with exact match
TEST_F(Grid1DTest, FindIndexForClosestGridPointExactMatch) {
    Grid1D grid(5, 10, 3);
    // Grid: {10, 13, 16, 19, 22}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(0, grid.findIndexForClosestGridPoint(10.0f));
    EXPECT_EQ(1, grid.findIndexForClosestGridPoint(13.0f));
    EXPECT_EQ(2, grid.findIndexForClosestGridPoint(16.0f));
    EXPECT_EQ(3, grid.findIndexForClosestGridPoint(19.0f));
    EXPECT_EQ(4, grid.findIndexForClosestGridPoint(22.0f));
}

// -----------------------------------------------------------------------------

// Test findIndexForClosestGridPoint with value between grid points
TEST_F(Grid1DTest, FindIndexForClosestGridPointBetween) {
    Grid1D grid(5, 10, 3);
    // Grid: {10, 13, 16, 19, 22}
    
    EXPECT_TRUE(grid.isSane());
    // 11.5 is closer to 13 (distance 1.5) than to 10 (distance 1.5), but 10 comes first
    // Actually, 11.5 - 10 = 1.5, 13 - 11.5 = 1.5, so it's a tie - should return first (index 0)
    // But let's test: 11.0 is closer to 10 (1.0) than to 13 (2.0)
    EXPECT_EQ(0, grid.findIndexForClosestGridPoint(11.0f));
    // 14.0 is closer to 13 (1.0) than to 16 (2.0)
    EXPECT_EQ(1, grid.findIndexForClosestGridPoint(14.0f));
    // 17.5 is exactly between 16 and 19, should prefer the first (index 2)
    EXPECT_EQ(2, grid.findIndexForClosestGridPoint(17.5f));
}

// -----------------------------------------------------------------------------

// Test findIndexForClosestGridPoint with value less than first
TEST_F(Grid1DTest, FindIndexForClosestGridPointBeforeFirst) {
    Grid1D grid(5, 10, 3);
    // Grid: {10, 13, 16, 19, 22}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(0, grid.findIndexForClosestGridPoint(5.0f));
    EXPECT_EQ(0, grid.findIndexForClosestGridPoint(0.0f));
    EXPECT_EQ(0, grid.findIndexForClosestGridPoint(-10.0f));
}

// -----------------------------------------------------------------------------

// Test findIndexForClosestGridPoint with value greater than last
TEST_F(Grid1DTest, FindIndexForClosestGridPointAfterLast) {
    Grid1D grid(5, 10, 3);
    // Grid: {10, 13, 16, 19, 22}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(4, grid.findIndexForClosestGridPoint(25.0f));
    EXPECT_EQ(4, grid.findIndexForClosestGridPoint(30.0f));
    EXPECT_EQ(4, grid.findIndexForClosestGridPoint(100.0f));
}

// -----------------------------------------------------------------------------

// Test findIndexForClosestGridPoint with empty grid
TEST_F(Grid1DTest, FindIndexForClosestGridPointEmpty) {
    Grid1D grid(0, 10, 3);
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(0, grid.findIndexForClosestGridPoint(5.0f));
    EXPECT_EQ(0, grid.findIndexForClosestGridPoint(100.0f));
}

// -----------------------------------------------------------------------------

// Test findIndexForClosestGridPoint with size 1
TEST_F(Grid1DTest, FindIndexForClosestGridPointSizeOne) {
    Grid1D grid(1, 42, 5);
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(0, grid.findIndexForClosestGridPoint(42.0f));
    EXPECT_EQ(0, grid.findIndexForClosestGridPoint(0.0f));
    EXPECT_EQ(0, grid.findIndexForClosestGridPoint(100.0f));
}

// -----------------------------------------------------------------------------

// Test findIndexForClosestGridPoint with negative values
TEST_F(Grid1DTest, FindIndexForClosestGridPointNegative) {
    Grid1D grid(4, -10, 3);
    // Grid: {-10, -7, -4, -1}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(0, grid.findIndexForClosestGridPoint(-11.0f));
    EXPECT_EQ(0, grid.findIndexForClosestGridPoint(-10.0f));
    EXPECT_EQ(1, grid.findIndexForClosestGridPoint(-7.0f));
    EXPECT_EQ(2, grid.findIndexForClosestGridPoint(-4.0f));
    EXPECT_EQ(3, grid.findIndexForClosestGridPoint(-1.0f));
    // -8.0 is closer to -7 (1.0) than to -10 (2.0)
    EXPECT_EQ(1, grid.findIndexForClosestGridPoint(-8.0f));
    // -5.0 is closer to -4 (1.0) than to -7 (2.0)
    EXPECT_EQ(2, grid.findIndexForClosestGridPoint(-5.0f));

    EXPECT_EQ(3, grid.findIndexForClosestGridPoint(10.0f));
}

// -----------------------------------------------------------------------------

// Test findIndexForClosestGridPoint const correctness
TEST_F(Grid1DTest, FindIndexForClosestGridPointConstCorrectness) {
    const Grid1D grid(5, 10, 3);
    // Grid: {10, 13, 16, 19, 22}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(1, grid.findIndexForClosestGridPoint(13.0f));
    EXPECT_EQ(2, grid.findIndexForClosestGridPoint(16.0f));
}

// -----------------------------------------------------------------------------

// Test findIndexForClosestGridPoint with tie-breaking (exactly between two points)
TEST_F(Grid1DTest, FindIndexForClosestGridPointTieBreak) {
    Grid1D grid(5, 10, 3);
    // Grid: {10, 13, 16, 19, 22}
    // 11.5 is exactly between 10 and 13 (both distances are 1.5)
    // The implementation should prefer the first one (index 0)
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_EQ(0, grid.findIndexForClosestGridPoint(11.5f));
    // 14.5 is exactly between 13 and 16
    EXPECT_EQ(1, grid.findIndexForClosestGridPoint(14.5f));
}

// -----------------------------------------------------------------------------

// Test fixture for Grid2D tests
class Grid2DTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test constructor with basic parameters
TEST_F(Grid2DTest, ConstructorBasic) {
    Grid2D grid(5, 10, 3, 4, 20, 5);
    
    EXPECT_EQ(5, grid.get<X>().size());
    EXPECT_EQ(4, grid.get<Y>().size());
    
    // Verify X grid: {10, 13, 16, 19, 22}
    const Grid1D& gridX = grid.get<X>();
    EXPECT_EQ(10, gridX[0]);
    EXPECT_EQ(13, gridX[1]);
    EXPECT_EQ(16, gridX[2]);
    EXPECT_EQ(19, gridX[3]);
    EXPECT_EQ(22, gridX[4]);
    
    // Verify Y grid: {20, 25, 30, 35}
    const Grid1D& gridY = grid.get<Y>();
    EXPECT_EQ(20, gridY[0]);
    EXPECT_EQ(25, gridY[1]);
    EXPECT_EQ(30, gridY[2]);
    EXPECT_EQ(35, gridY[3]);
}

// -----------------------------------------------------------------------------

// Test constructor with zero sizes
TEST_F(Grid2DTest, ConstructorZeroSizes) {
    Grid2D grid1(0, 10, 3, 4, 20, 5);
    EXPECT_EQ(0, grid1.get<X>().size());
    EXPECT_EQ(4, grid1.get<Y>().size());
    
    Grid2D grid2(5, 10, 3, 0, 20, 5);
    EXPECT_EQ(5, grid2.get<X>().size());
    EXPECT_EQ(0, grid2.get<Y>().size());
    
    Grid2D grid3(0, 10, 3, 0, 20, 5);
    EXPECT_EQ(0, grid3.get<X>().size());
    EXPECT_EQ(0, grid3.get<Y>().size());
}

// -----------------------------------------------------------------------------

// Test constructor with size 1 for both dimensions
TEST_F(Grid2DTest, ConstructorSizeOne) {
    Grid2D grid(1, 42, 5, 1, 100, 10);
    
    EXPECT_EQ(1, grid.get<X>().size());
    EXPECT_EQ(1, grid.get<Y>().size());
    
    EXPECT_EQ(42, grid.get<X>()[0]);
    EXPECT_EQ(100, grid.get<Y>()[0]);
}

// -----------------------------------------------------------------------------

// Test constructor with negative Min values
TEST_F(Grid2DTest, ConstructorNegativeMin) {
    Grid2D grid(3, -10, 3, 4, -20, 5);
    
    EXPECT_EQ(3, grid.get<X>().size());
    EXPECT_EQ(4, grid.get<Y>().size());
    
    // Verify X grid: {-10, -7, -4}
    const Grid1D& gridX = grid.get<X>();
    EXPECT_EQ(-10, gridX[0]);
    EXPECT_EQ(-7, gridX[1]);
    EXPECT_EQ(-4, gridX[2]);
    
    // Verify Y grid: {-20, -15, -10, -5}
    const Grid1D& gridY = grid.get<Y>();
    EXPECT_EQ(-20, gridY[0]);
    EXPECT_EQ(-15, gridY[1]);
    EXPECT_EQ(-10, gridY[2]);
    EXPECT_EQ(-5, gridY[3]);
}

// -----------------------------------------------------------------------------

// Test get<X>() method
TEST_F(Grid2DTest, GetX) {
    Grid2D grid(4, 10, 2, 3, 20, 5);
    
    const Grid1D& gridX = grid.get<X>();
    EXPECT_EQ(4, gridX.size());
    EXPECT_EQ(10, gridX[0]);
    EXPECT_EQ(12, gridX[1]);
    EXPECT_EQ(14, gridX[2]);
    EXPECT_EQ(16, gridX[3]);
}

// -----------------------------------------------------------------------------

// Test get<Y>() method
TEST_F(Grid2DTest, GetY) {
    Grid2D grid(3, 10, 2, 5, 20, 5);
    
    const Grid1D& gridY = grid.get<Y>();
    EXPECT_EQ(5, gridY.size());
    EXPECT_EQ(20, gridY[0]);
    EXPECT_EQ(25, gridY[1]);
    EXPECT_EQ(30, gridY[2]);
    EXPECT_EQ(35, gridY[3]);
    EXPECT_EQ(40, gridY[4]);
}

// -----------------------------------------------------------------------------

// Test get<X>().size() method
TEST_F(Grid2DTest, SizeX) {
    Grid2D grid1(0, 0, 1, 5, 0, 1);
    EXPECT_EQ(0, grid1.get<X>().size());
    
    Grid2D grid2(1, 0, 1, 5, 0, 1);
    EXPECT_EQ(1, grid2.get<X>().size());
    
    Grid2D grid3(100, 0, 1, 5, 0, 1);
    EXPECT_EQ(100, grid3.get<X>().size());
}

// -----------------------------------------------------------------------------

// Test get<Y>().size() method
TEST_F(Grid2DTest, SizeY) {
    Grid2D grid1(5, 0, 1, 0, 0, 1);
    EXPECT_EQ(0, grid1.get<Y>().size());
    
    Grid2D grid2(5, 0, 1, 1, 0, 1);
    EXPECT_EQ(1, grid2.get<Y>().size());
    
    Grid2D grid3(5, 0, 1, 100, 0, 1);
    EXPECT_EQ(100, grid3.get<Y>().size());
}

// -----------------------------------------------------------------------------

// Test that get<X>() and get<Y>() return independent grids
TEST_F(Grid2DTest, IndependentGrids) {
    Grid2D grid(3, 10, 2, 4, 20, 5);
    
    const Grid1D& gridX = grid.get<X>();
    const Grid1D& gridY = grid.get<Y>();
    
    // Verify they have different sizes
    EXPECT_EQ(3, gridX.size());
    EXPECT_EQ(4, gridY.size());
    
    // Verify they have different values
    EXPECT_EQ(10, gridX[0]);
    EXPECT_EQ(20, gridY[0]);
}

// -----------------------------------------------------------------------------

// Test with large sizes
TEST_F(Grid2DTest, LargeSizes) {
    Grid2D grid(100, 0, 1, 50, 0, 2);
    
    EXPECT_EQ(100, grid.get<X>().size());
    EXPECT_EQ(50, grid.get<Y>().size());
    
    // Verify X grid first and last elements
    const Grid1D& gridX = grid.get<X>();
    EXPECT_EQ(0, gridX[0]);
    EXPECT_EQ(99, gridX[99]);
    
    // Verify Y grid first and last elements
    const Grid1D& gridY = grid.get<Y>();
    EXPECT_EQ(0, gridY[0]);
    EXPECT_EQ(98, gridY[49]);
}

// -----------------------------------------------------------------------------

// Test const correctness
TEST_F(Grid2DTest, ConstCorrectness) {
    const Grid2D grid(3, 5, 2, 4, 10, 3);
    
    // Test const get<X>()
    const Grid1D& gridX = grid.get<X>();
    EXPECT_EQ(3, gridX.size());
    EXPECT_EQ(5, gridX[0]);
    
    // Test const get<Y>()
    const Grid1D& gridY = grid.get<Y>();
    EXPECT_EQ(4, gridY.size());
    EXPECT_EQ(10, gridY[0]);
    
    // Test const get<X>().size() and get<Y>().size()
    EXPECT_EQ(3, grid.get<X>().size());
    EXPECT_EQ(4, grid.get<Y>().size());
}

// -----------------------------------------------------------------------------

// Test with different combinations of parameters
TEST_F(Grid2DTest, VariousParameterCombinations) {
    // Test case 1: Different sizes, same Min and Delta
    Grid2D grid1(5, 0, 1, 3, 0, 1);
    EXPECT_EQ(5, grid1.get<X>().size());
    EXPECT_EQ(3, grid1.get<Y>().size());
    
    // Test case 2: Same sizes, different Min and Delta
    Grid2D grid2(4, 0, 1, 4, 10, 2);
    EXPECT_EQ(4, grid2.get<X>().size());
    EXPECT_EQ(4, grid2.get<Y>().size());
    EXPECT_EQ(0, grid2.get<X>()[0]);
    EXPECT_EQ(10, grid2.get<Y>()[0]);
    
    // Test case 3: All different
    Grid2D grid3(6, -5, 2, 8, 100, 3);
    EXPECT_EQ(6, grid3.get<X>().size());
    EXPECT_EQ(8, grid3.get<Y>().size());
    EXPECT_EQ(-5, grid3.get<X>()[0]);
    EXPECT_EQ(100, grid3.get<Y>()[0]);
}

// -----------------------------------------------------------------------------

// Test save with basic grid
TEST_F(Grid2DTest, SaveBasic) {
    Grid2D grid(5, 10, 3, 4, 20, 5);
    
    std::ostringstream oss;
    grid.save(oss);
    
    std::string result = oss.str();
    EXPECT_EQ("2\n5 10 13 16 19 22\n4 20 25 30 35\n", result);
}

// -----------------------------------------------------------------------------

// Test save with zero sizes
TEST_F(Grid2DTest, SaveZeroSizes) {
    Grid2D grid(0, 10, 3, 4, 20, 5);
    
    std::ostringstream oss;
    grid.save(oss);
    
    std::string result = oss.str();
    EXPECT_EQ("2\n0\n4 20 25 30 35\n", result);
}

// -----------------------------------------------------------------------------

// Test save with both zero sizes
TEST_F(Grid2DTest, SaveBothZeroSizes) {
    Grid2D grid(0, 10, 3, 0, 20, 5);
    
    std::ostringstream oss;
    grid.save(oss);
    
    std::string result = oss.str();
    EXPECT_EQ("2\n0\n0\n", result);
}

// -----------------------------------------------------------------------------

// Test save with size 1 for both dimensions
TEST_F(Grid2DTest, SaveSizeOne) {
    Grid2D grid(1, 42, 5, 1, 100, 10);
    
    std::ostringstream oss;
    grid.save(oss);
    
    std::string result = oss.str();
    EXPECT_EQ("2\n1 42\n1 100\n", result);
}

// -----------------------------------------------------------------------------

// Test save with negative values
TEST_F(Grid2DTest, SaveNegativeValues) {
    Grid2D grid(3, -10, 3, 4, -20, 5);
    
    std::ostringstream oss;
    grid.save(oss);
    
    std::string result = oss.str();
    EXPECT_EQ("2\n3 -10 -7 -4\n4 -20 -15 -10 -5\n", result);
}

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

// Test save const correctness
TEST_F(Grid2DTest, SaveConstCorrectness) {
    const Grid2D grid(3, 5, 2, 4, 10, 3);
    
    std::ostringstream oss;
    grid.save(oss);
    
    std::string result = oss.str();
    EXPECT_EQ("2\n3 5 7 9\n4 10 13 16 19\n", result);
}

// -----------------------------------------------------------------------------

// Test save format consistency
TEST_F(Grid2DTest, SaveFormatConsistency) {
    Grid2D grid(2, 0, 1, 3, 0, 2);
    
    std::ostringstream oss;
    grid.save(oss);
    
    std::string result = oss.str();
    // Verify it starts with "2\n"
    EXPECT_EQ(0, result.find("2\n"));
    // Verify it contains the X grid: "2 0 1"
    EXPECT_NE(std::string::npos, result.find("2 0 1"));
    // Verify it contains the Y grid: "3 0 2 4"
    EXPECT_NE(std::string::npos, result.find("3 0 2 4"));
}

// -----------------------------------------------------------------------------

// Test addGridPoint to X direction
TEST_F(Grid2DTest, AddGridPointX) {
    Grid2D grid(3, 10, 3, 4, 20, 5);
    // X grid: {10, 13, 16}, Y grid: {20, 25, 30, 35}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint<X>(12));
    EXPECT_EQ(4, grid.get<X>().size());
    EXPECT_EQ(4, grid.get<Y>().size());
    EXPECT_TRUE(grid.isSane());
    
    const Grid1D& gridX = grid.get<X>();
    EXPECT_EQ(10, gridX[0]);
    EXPECT_EQ(12, gridX[1]);
    EXPECT_EQ(13, gridX[2]);
    EXPECT_EQ(16, gridX[3]);
}

// -----------------------------------------------------------------------------

// Test addGridPoint to Y direction
TEST_F(Grid2DTest, AddGridPointY) {
    Grid2D grid(3, 10, 3, 4, 20, 5);
    // X grid: {10, 13, 16}, Y grid: {20, 25, 30, 35}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint<Y>(22));
    EXPECT_EQ(3, grid.get<X>().size());
    EXPECT_EQ(5, grid.get<Y>().size());
    EXPECT_TRUE(grid.isSane());
    
    const Grid1D& gridY = grid.get<Y>();
    EXPECT_EQ(20, gridY[0]);
    EXPECT_EQ(22, gridY[1]);
    EXPECT_EQ(25, gridY[2]);
    EXPECT_EQ(30, gridY[3]);
    EXPECT_EQ(35, gridY[4]);
}

// -----------------------------------------------------------------------------

// Test addGridPoint duplicate to X direction
TEST_F(Grid2DTest, AddGridPointXDuplicate) {
    Grid2D grid(3, 10, 3, 4, 20, 5);
    // X grid: {10, 13, 16}, Y grid: {20, 25, 30, 35}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_FALSE(grid.addGridPoint<X>(10));
    EXPECT_FALSE(grid.addGridPoint<X>(13));
    EXPECT_FALSE(grid.addGridPoint<X>(16));
    EXPECT_EQ(3, grid.get<X>().size());
    EXPECT_EQ(4, grid.get<Y>().size());
    EXPECT_TRUE(grid.isSane());
}

// -----------------------------------------------------------------------------

// Test addGridPoint duplicate to Y direction
TEST_F(Grid2DTest, AddGridPointYDuplicate) {
    Grid2D grid(3, 10, 3, 4, 20, 5);
    // X grid: {10, 13, 16}, Y grid: {20, 25, 30, 35}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_FALSE(grid.addGridPoint<Y>(20));
    EXPECT_FALSE(grid.addGridPoint<Y>(25));
    EXPECT_FALSE(grid.addGridPoint<Y>(30));
    EXPECT_FALSE(grid.addGridPoint<Y>(35));
    EXPECT_EQ(3, grid.get<X>().size());
    EXPECT_EQ(4, grid.get<Y>().size());
    EXPECT_TRUE(grid.isSane());
}

// -----------------------------------------------------------------------------

// Test addGridPoint to X direction at beginning
TEST_F(Grid2DTest, AddGridPointXBeginning) {
    Grid2D grid(3, 10, 3, 4, 20, 5);
    // X grid: {10, 13, 16}, Y grid: {20, 25, 30, 35}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint<X>(5));
    EXPECT_EQ(4, grid.get<X>().size());
    EXPECT_TRUE(grid.isSane());
    
    const Grid1D& gridX = grid.get<X>();
    EXPECT_EQ(5, gridX[0]);
    EXPECT_EQ(10, gridX[1]);
    EXPECT_EQ(13, gridX[2]);
    EXPECT_EQ(16, gridX[3]);
}

// -----------------------------------------------------------------------------

// Test addGridPoint to Y direction at end
TEST_F(Grid2DTest, AddGridPointYEnd) {
    Grid2D grid(3, 10, 3, 4, 20, 5);
    // X grid: {10, 13, 16}, Y grid: {20, 25, 30, 35}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint<Y>(40));
    EXPECT_EQ(5, grid.get<Y>().size());
    EXPECT_TRUE(grid.isSane());
    
    const Grid1D& gridY = grid.get<Y>();
    EXPECT_EQ(20, gridY[0]);
    EXPECT_EQ(25, gridY[1]);
    EXPECT_EQ(30, gridY[2]);
    EXPECT_EQ(35, gridY[3]);
    EXPECT_EQ(40, gridY[4]);
}

// -----------------------------------------------------------------------------

// Test addGridPoint to X direction with empty grid
TEST_F(Grid2DTest, AddGridPointXEmpty) {
    Grid2D grid(0, 10, 3, 4, 20, 5);
    // X grid: {}, Y grid: {20, 25, 30, 35}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint<X>(5));
    EXPECT_EQ(1, grid.get<X>().size());
    EXPECT_EQ(4, grid.get<Y>().size());
    EXPECT_TRUE(grid.isSane());
    
    const Grid1D& gridX = grid.get<X>();
    EXPECT_EQ(5, gridX[0]);
}

// -----------------------------------------------------------------------------

// Test addGridPoint to Y direction with empty grid
TEST_F(Grid2DTest, AddGridPointYEmpty) {
    Grid2D grid(3, 10, 3, 0, 20, 5);
    // X grid: {10, 13, 16}, Y grid: {}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint<Y>(25));
    EXPECT_EQ(3, grid.get<X>().size());
    EXPECT_EQ(1, grid.get<Y>().size());
    EXPECT_TRUE(grid.isSane());
    
    const Grid1D& gridY = grid.get<Y>();
    EXPECT_EQ(25, gridY[0]);
}

// -----------------------------------------------------------------------------

// Test addGridPoint maintains independence between X and Y
TEST_F(Grid2DTest, AddGridPointIndependence) {
    Grid2D grid(3, 10, 3, 4, 20, 5);
    // X grid: {10, 13, 16}, Y grid: {20, 25, 30, 35}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint<X>(12));
    EXPECT_TRUE(grid.addGridPoint<Y>(22));
    EXPECT_EQ(4, grid.get<X>().size());
    EXPECT_EQ(5, grid.get<Y>().size());
    EXPECT_TRUE(grid.isSane());
    
    // Verify X grid
    const Grid1D& gridX = grid.get<X>();
    EXPECT_EQ(10, gridX[0]);
    EXPECT_EQ(12, gridX[1]);
    EXPECT_EQ(13, gridX[2]);
    EXPECT_EQ(16, gridX[3]);
    
    // Verify Y grid
    const Grid1D& gridY = grid.get<Y>();
    EXPECT_EQ(20, gridY[0]);
    EXPECT_EQ(22, gridY[1]);
    EXPECT_EQ(25, gridY[2]);
    EXPECT_EQ(30, gridY[3]);
    EXPECT_EQ(35, gridY[4]);
}

// -----------------------------------------------------------------------------

// Test addGridPoint multiple additions to both directions
TEST_F(Grid2DTest, AddGridPointMultiple) {
    Grid2D grid(2, 10, 3, 2, 20, 5);
    // X grid: {10, 13}, Y grid: {20, 25}
    
    EXPECT_TRUE(grid.isSane());
    EXPECT_TRUE(grid.addGridPoint<X>(12));
    EXPECT_TRUE(grid.addGridPoint<X>(15));
    EXPECT_TRUE(grid.addGridPoint<Y>(22));
    EXPECT_TRUE(grid.addGridPoint<Y>(23));
    EXPECT_EQ(4, grid.get<X>().size());
    EXPECT_EQ(4, grid.get<Y>().size());
    EXPECT_TRUE(grid.isSane());
    
    // Verify X grid: {10, 12, 13, 15}
    const Grid1D& gridX = grid.get<X>();
    EXPECT_EQ(10, gridX[0]);
    EXPECT_EQ(12, gridX[1]);
    EXPECT_EQ(13, gridX[2]);
    EXPECT_EQ(15, gridX[3]);
    
    // Verify Y grid: {20, 22, 23, 25}
    const Grid1D& gridY = grid.get<Y>();
    EXPECT_EQ(20, gridY[0]);
    EXPECT_EQ(22, gridY[1]);
    EXPECT_EQ(23, gridY[2]);
    EXPECT_EQ(25, gridY[3]);
}

// -----------------------------------------------------------------------------

