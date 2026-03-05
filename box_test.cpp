#include <Box.h>
#include <gtest/gtest.h>
#include <limits>

// Test fixture for Box<float> tests
class BoxFloatTest : public ::testing::Test {
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
TEST_F(BoxFloatTest, DefaultConstructor) {
    Box<float> box;
    
    EXPECT_FLOAT_EQ(0.0f, box.getXMin());
    EXPECT_FLOAT_EQ(0.0f, box.getXMax());
    EXPECT_FLOAT_EQ(0.0f, box.getYMin());
    EXPECT_FLOAT_EQ(0.0f, box.getYMax());
}

// -----------------------------------------------------------------------------

// Test parameterized constructor
TEST_F(BoxFloatTest, ParameterizedConstructor) {
    Box<float> box(1.0f, 2.0f, 3.0f, 4.0f);
    
    EXPECT_FLOAT_EQ(1.0f, box.getXMin());
    EXPECT_FLOAT_EQ(2.0f, box.getXMax());
    EXPECT_FLOAT_EQ(3.0f, box.getYMin());
    EXPECT_FLOAT_EQ(4.0f, box.getYMax());
}

// -----------------------------------------------------------------------------

// Test getters
TEST_F(BoxFloatTest, Getters) {
    Box<float> box(10.5f, 20.5f, 30.5f, 40.5f);
    
    EXPECT_FLOAT_EQ(10.5f, box.getXMin());
    EXPECT_FLOAT_EQ(20.5f, box.getXMax());
    EXPECT_FLOAT_EQ(30.5f, box.getYMin());
    EXPECT_FLOAT_EQ(40.5f, box.getYMax());
}

// -----------------------------------------------------------------------------

// Test setters
TEST_F(BoxFloatTest, Setters) {
    Box<float> box;
    
    box.setXMin(1.0f);
    box.setXMax(2.0f);
    box.setYMin(3.0f);
    box.setYMax(4.0f);
    
    EXPECT_FLOAT_EQ(1.0f, box.getXMin());
    EXPECT_FLOAT_EQ(2.0f, box.getXMax());
    EXPECT_FLOAT_EQ(3.0f, box.getYMin());
    EXPECT_FLOAT_EQ(4.0f, box.getYMax());
}

// -----------------------------------------------------------------------------

// Test setters with negative values
TEST_F(BoxFloatTest, SettersNegativeValues) {
    Box<float> box;
    
    box.setXMin(-5.0f);
    box.setXMax(-1.0f);
    box.setYMin(-10.0f);
    box.setYMax(-2.0f);
    
    EXPECT_FLOAT_EQ(-5.0f, box.getXMin());
    EXPECT_FLOAT_EQ(-1.0f, box.getXMax());
    EXPECT_FLOAT_EQ(-10.0f, box.getYMin());
    EXPECT_FLOAT_EQ(-2.0f, box.getYMax());
}

// -----------------------------------------------------------------------------

// Test setters with decimal values
TEST_F(BoxFloatTest, SettersDecimalValues) {
    Box<float> box;
    
    box.setXMin(1.5f);
    box.setXMax(2.75f);
    box.setYMin(3.14159f);
    box.setYMax(4.99f);
    
    EXPECT_FLOAT_EQ(1.5f, box.getXMin());
    EXPECT_FLOAT_EQ(2.75f, box.getXMax());
    EXPECT_FLOAT_EQ(3.14159f, box.getYMin());
    EXPECT_FLOAT_EQ(4.99f, box.getYMax());
}

// -----------------------------------------------------------------------------

// Test setters with large values
TEST_F(BoxFloatTest, SettersLargeValues) {
    Box<float> box;
    
    box.setXMin(1000.0f);
    box.setXMax(2000.0f);
    box.setYMin(3000.0f);
    box.setYMax(4000.0f);
    
    EXPECT_FLOAT_EQ(1000.0f, box.getXMin());
    EXPECT_FLOAT_EQ(2000.0f, box.getXMax());
    EXPECT_FLOAT_EQ(3000.0f, box.getYMin());
    EXPECT_FLOAT_EQ(4000.0f, box.getYMax());
}

// -----------------------------------------------------------------------------

// Test setters with zero values
TEST_F(BoxFloatTest, SettersZeroValues) {
    Box<float> box(1.0f, 2.0f, 3.0f, 4.0f);
    
    box.setXMin(0.0f);
    box.setXMax(0.0f);
    box.setYMin(0.0f);
    box.setYMax(0.0f);
    
    EXPECT_FLOAT_EQ(0.0f, box.getXMin());
    EXPECT_FLOAT_EQ(0.0f, box.getXMax());
    EXPECT_FLOAT_EQ(0.0f, box.getYMin());
    EXPECT_FLOAT_EQ(0.0f, box.getYMax());
}

// -----------------------------------------------------------------------------

// Test multiple set operations
TEST_F(BoxFloatTest, MultipleSetOperations) {
    Box<float> box;
    
    box.setXMin(1.0f);
    box.setXMax(2.0f);
    box.setYMin(3.0f);
    box.setYMax(4.0f);
    
    EXPECT_FLOAT_EQ(1.0f, box.getXMin());
    EXPECT_FLOAT_EQ(2.0f, box.getXMax());
    EXPECT_FLOAT_EQ(3.0f, box.getYMin());
    EXPECT_FLOAT_EQ(4.0f, box.getYMax());
    
    // Update values
    box.setXMin(10.0f);
    box.setXMax(20.0f);
    box.setYMin(30.0f);
    box.setYMax(40.0f);
    
    EXPECT_FLOAT_EQ(10.0f, box.getXMin());
    EXPECT_FLOAT_EQ(20.0f, box.getXMax());
    EXPECT_FLOAT_EQ(30.0f, box.getYMin());
    EXPECT_FLOAT_EQ(40.0f, box.getYMax());
}

// -----------------------------------------------------------------------------

// Test const correctness of getters
TEST_F(BoxFloatTest, ConstCorrectness) {
    const Box<float> box(1.0f, 2.0f, 3.0f, 4.0f);
    
    // Should compile - getters are const
    float xMin = box.getXMin();
    float xMax = box.getXMax();
    float yMin = box.getYMin();
    float yMax = box.getYMax();
    
    EXPECT_FLOAT_EQ(1.0f, xMin);
    EXPECT_FLOAT_EQ(2.0f, xMax);
    EXPECT_FLOAT_EQ(3.0f, yMin);
    EXPECT_FLOAT_EQ(4.0f, yMax);
}

// -----------------------------------------------------------------------------

// Test fixture for Box<int> tests
class BoxIntTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test Box with int type
TEST_F(BoxIntTest, IntType) {
    Box<int> box(1, 2, 3, 4);
    
    EXPECT_EQ(1, box.getXMin());
    EXPECT_EQ(2, box.getXMax());
    EXPECT_EQ(3, box.getYMin());
    EXPECT_EQ(4, box.getYMax());
}

// -----------------------------------------------------------------------------

// Test Box with int type - setters
TEST_F(BoxIntTest, IntTypeSetters) {
    Box<int> box;
    
    box.setXMin(10);
    box.setXMax(20);
    box.setYMin(30);
    box.setYMax(40);
    
    EXPECT_EQ(10, box.getXMin());
    EXPECT_EQ(20, box.getXMax());
    EXPECT_EQ(30, box.getYMin());
    EXPECT_EQ(40, box.getYMax());
}

// -----------------------------------------------------------------------------

// Test Box with int type - negative values
TEST_F(BoxIntTest, IntTypeNegativeValues) {
    Box<int> box(-5, -1, -10, -2);
    
    EXPECT_EQ(-5, box.getXMin());
    EXPECT_EQ(-1, box.getXMax());
    EXPECT_EQ(-10, box.getYMin());
    EXPECT_EQ(-2, box.getYMax());
}

// -----------------------------------------------------------------------------

// Test fixture for Box<double> tests
class BoxDoubleTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test Box with double type
TEST_F(BoxDoubleTest, DoubleType) {
    Box<double> box(1.5, 2.5, 3.5, 4.5);
    
    EXPECT_DOUBLE_EQ(1.5, box.getXMin());
    EXPECT_DOUBLE_EQ(2.5, box.getXMax());
    EXPECT_DOUBLE_EQ(3.5, box.getYMin());
    EXPECT_DOUBLE_EQ(4.5, box.getYMax());
}

// -----------------------------------------------------------------------------

// Test Box with double type - high precision
TEST_F(BoxDoubleTest, DoubleTypeHighPrecision) {
    Box<double> box(1.123456789, 2.987654321, 3.141592653589793, 4.718281828459045);
    
    EXPECT_DOUBLE_EQ(1.123456789, box.getXMin());
    EXPECT_DOUBLE_EQ(2.987654321, box.getXMax());
    EXPECT_DOUBLE_EQ(3.141592653589793, box.getYMin());
    EXPECT_DOUBLE_EQ(4.718281828459045, box.getYMax());
}

// -----------------------------------------------------------------------------

// Test Box with double type - setters
TEST_F(BoxDoubleTest, DoubleTypeSetters) {
    Box<double> box;
    
    box.setXMin(10.123);
    box.setXMax(20.456);
    box.setYMin(30.789);
    box.setYMax(40.012);
    
    EXPECT_DOUBLE_EQ(10.123, box.getXMin());
    EXPECT_DOUBLE_EQ(20.456, box.getXMax());
    EXPECT_DOUBLE_EQ(30.789, box.getYMin());
    EXPECT_DOUBLE_EQ(40.012, box.getYMax());
}

// -----------------------------------------------------------------------------

// Test Box with edge case values (min/max float)
TEST_F(BoxFloatTest, EdgeCaseValues) {
    Box<float> box;
    
    float minVal = std::numeric_limits<float>::lowest();
    float maxVal = std::numeric_limits<float>::max();
    
    box.setXMin(minVal);
    box.setXMax(maxVal);
    box.setYMin(minVal);
    box.setYMax(maxVal);
    
    EXPECT_FLOAT_EQ(minVal, box.getXMin());
    EXPECT_FLOAT_EQ(maxVal, box.getXMax());
    EXPECT_FLOAT_EQ(minVal, box.getYMin());
    EXPECT_FLOAT_EQ(maxVal, box.getYMax());
}

// -----------------------------------------------------------------------------

// Test Box with same values for min and max
TEST_F(BoxFloatTest, SameMinMaxValues) {
    Box<float> box(1.0f, 1.0f, 2.0f, 2.0f);
    
    EXPECT_FLOAT_EQ(1.0f, box.getXMin());
    EXPECT_FLOAT_EQ(1.0f, box.getXMax());
    EXPECT_FLOAT_EQ(2.0f, box.getYMin());
    EXPECT_FLOAT_EQ(2.0f, box.getYMax());
}

// -----------------------------------------------------------------------------

// Test Box with swapped min/max (min > max) - should still work
TEST_F(BoxFloatTest, SwappedMinMax) {
    Box<float> box(5.0f, 1.0f, 10.0f, 2.0f);
    
    // Box class doesn't validate, so it should accept these values
    EXPECT_FLOAT_EQ(5.0f, box.getXMin());
    EXPECT_FLOAT_EQ(1.0f, box.getXMax());
    EXPECT_FLOAT_EQ(10.0f, box.getYMin());
    EXPECT_FLOAT_EQ(2.0f, box.getYMax());
}

// -----------------------------------------------------------------------------

// Test Box copy construction (implicit)
TEST_F(BoxFloatTest, CopyConstruction) {
    Box<float> box1(1.0f, 2.0f, 3.0f, 4.0f);
    Box<float> box2 = box1;
    
    EXPECT_FLOAT_EQ(1.0f, box2.getXMin());
    EXPECT_FLOAT_EQ(2.0f, box2.getXMax());
    EXPECT_FLOAT_EQ(3.0f, box2.getYMin());
    EXPECT_FLOAT_EQ(4.0f, box2.getYMax());
}

// -----------------------------------------------------------------------------

// Test Box assignment
TEST_F(BoxFloatTest, Assignment) {
    Box<float> box1(1.0f, 2.0f, 3.0f, 4.0f);
    Box<float> box2;
    
    box2 = box1;
    
    EXPECT_FLOAT_EQ(1.0f, box2.getXMin());
    EXPECT_FLOAT_EQ(2.0f, box2.getXMax());
    EXPECT_FLOAT_EQ(3.0f, box2.getYMin());
    EXPECT_FLOAT_EQ(4.0f, box2.getYMax());
}

// -----------------------------------------------------------------------------

