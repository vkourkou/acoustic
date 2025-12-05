#include <InputCompiler.h>
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <cmath>

using Input::BBoxStatement;
using Input::WallStatement;
using Input::SourceStatement;
using Input::VelocityStatement;
using Input::MaxResolutionStatement;
using Input::SimulationParamStatement;
using Input::FileParser;
using Input::StatementType;
using ::ProcessingType;
using ::CPU;
using ::GPU;

// Test fixture for BBoxStatement tests
class BBoxStatementTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test process with valid BBox tokens (9 tokens total)
TEST_F(BBoxStatementTest, Process_ValidBBox) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    
    bool result = bbox.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1.0f, bbox.getXMin());
    EXPECT_FLOAT_EQ(2.0f, bbox.getXMax());
    EXPECT_FLOAT_EQ(3.0f, bbox.getYMin());
    EXPECT_FLOAT_EQ(4.0f, bbox.getYMax());
}

// -----------------------------------------------------------------------------

// Test process with case-insensitive BBox (lowercase)
TEST_F(BBoxStatementTest, Process_CaseInsensitiveBBox) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"bbox", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    
    bool result = bbox.process(tokens);
    EXPECT_TRUE(result);
}

// Test process with case-insensitive BBox (mixed case)
TEST_F(BBoxStatementTest, Process_CaseInsensitiveBBoxMixed) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BbOx", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    
    bool result = bbox.process(tokens);
    EXPECT_TRUE(result);
}

// -----------------------------------------------------------------------------

// Test process with case-insensitive parameter names
TEST_F(BBoxStatementTest, Process_CaseInsensitiveParameters) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "xmin", "1.0", "xmax", "2.0", "ymin", "3.0", "ymax", "4.0"};
    
    bool result = bbox.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1.0f, bbox.getXMin());
    EXPECT_FLOAT_EQ(2.0f, bbox.getXMax());
    EXPECT_FLOAT_EQ(3.0f, bbox.getYMin());
    EXPECT_FLOAT_EQ(4.0f, bbox.getYMax());
}

// Test process with mixed case parameter names
TEST_F(BBoxStatementTest, Process_MixedCaseParameters) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XmIn", "1.0", "XmAx", "2.0", "YmIn", "3.0", "YmAx", "4.0"};
    
    bool result = bbox.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1.0f, bbox.getXMin());
    EXPECT_FLOAT_EQ(2.0f, bbox.getXMax());
    EXPECT_FLOAT_EQ(3.0f, bbox.getYMin());
    EXPECT_FLOAT_EQ(4.0f, bbox.getYMax());
}

// -----------------------------------------------------------------------------

// Test process with invalid first token (not BBox)
TEST_F(BBoxStatementTest, Process_InvalidFirstToken) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"Source", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    
    bool result = bbox.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with empty tokens
TEST_F(BBoxStatementTest, Process_EmptyTokens) {
    BBoxStatement bbox;
    std::vector<std::string> tokens;
    
    bool result = bbox.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with wrong number of tokens (less than 9)
TEST_F(BBoxStatementTest, Process_TooFewTokens) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "2.0"};
    
    bool result = bbox.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with wrong number of tokens (more than 9)
TEST_F(BBoxStatementTest, Process_TooManyTokens) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0", "Extra"};
    
    bool result = bbox.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with exactly 8 tokens (missing one)
TEST_F(BBoxStatementTest, Process_Exactly8Tokens) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax"};
    
    bool result = bbox.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with negative values (valid when min < max)
TEST_F(BBoxStatementTest, Process_NegativeValues) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "-2.5", "XMax", "-1.5", "YMin", "-4.5", "YMax", "-3.5"};
    
    bool result = bbox.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(-2.5f, bbox.getXMin());
    EXPECT_FLOAT_EQ(-1.5f, bbox.getXMax());
    EXPECT_FLOAT_EQ(-4.5f, bbox.getYMin());
    EXPECT_FLOAT_EQ(-3.5f, bbox.getYMax());
}

// Test process with XMin >= XMax (should fail validation)
TEST_F(BBoxStatementTest, Process_XMinGreaterThanOrEqualXMax) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "2.0", "XMax", "1.0", "YMin", "3.0", "YMax", "4.0"};
    
    bool result = bbox.process(tokens);
    EXPECT_FALSE(result);  // Should fail because XMin >= XMax
    
    // Values may still be set, but process should return false
    EXPECT_FLOAT_EQ(2.0f, bbox.getXMin());
    EXPECT_FLOAT_EQ(1.0f, bbox.getXMax());
}

// Test process with XMin equal to XMax (should fail validation)
TEST_F(BBoxStatementTest, Process_XMinEqualXMax) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "1.0", "YMin", "3.0", "YMax", "4.0"};
    
    bool result = bbox.process(tokens);
    EXPECT_FALSE(result);  // Should fail because XMin == XMax
    
    EXPECT_FLOAT_EQ(1.0f, bbox.getXMin());
    EXPECT_FLOAT_EQ(1.0f, bbox.getXMax());
}

// Test process with YMin >= YMax (should fail validation)
TEST_F(BBoxStatementTest, Process_YMinGreaterThanOrEqualYMax) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "2.0", "YMin", "4.0", "YMax", "3.0"};
    
    bool result = bbox.process(tokens);
    EXPECT_FALSE(result);  // Should fail because YMin >= YMax
    
    // Values may still be set, but process should return false
    EXPECT_FLOAT_EQ(4.0f, bbox.getYMin());
    EXPECT_FLOAT_EQ(3.0f, bbox.getYMax());
}

// Test process with YMin equal to YMax (should fail validation)
TEST_F(BBoxStatementTest, Process_YMinEqualYMax) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "3.0"};
    
    bool result = bbox.process(tokens);
    EXPECT_FALSE(result);  // Should fail because YMin == YMax
    
    EXPECT_FLOAT_EQ(3.0f, bbox.getYMin());
    EXPECT_FLOAT_EQ(3.0f, bbox.getYMax());
}

// Test process with both XMin >= XMax and YMin >= YMax (should fail validation)
TEST_F(BBoxStatementTest, Process_BothDimensionsInvalid) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "2.0", "XMax", "1.0", "YMin", "4.0", "YMax", "3.0"};
    
    bool result = bbox.process(tokens);
    EXPECT_FALSE(result);  // Should fail because both dimensions are invalid
    
    EXPECT_FLOAT_EQ(2.0f, bbox.getXMin());
    EXPECT_FLOAT_EQ(1.0f, bbox.getXMax());
    EXPECT_FLOAT_EQ(4.0f, bbox.getYMin());
    EXPECT_FLOAT_EQ(3.0f, bbox.getYMax());
}

// Test process with large values
TEST_F(BBoxStatementTest, Process_LargeValues) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1000.0", "XMax", "2000.0", "YMin", "3000.0", "YMax", "4000.0"};
    
    bool result = bbox.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1000.0f, bbox.getXMin());
    EXPECT_FLOAT_EQ(2000.0f, bbox.getXMax());
    EXPECT_FLOAT_EQ(3000.0f, bbox.getYMin());
    EXPECT_FLOAT_EQ(4000.0f, bbox.getYMax());
}

// Test process with decimal values
TEST_F(BBoxStatementTest, Process_DecimalValues) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1.5", "XMax", "2.75", "YMin", "3.14159", "YMax", "4.99"};
    
    bool result = bbox.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1.5f, bbox.getXMin());
    EXPECT_FLOAT_EQ(2.75f, bbox.getXMax());
    EXPECT_FLOAT_EQ(3.14159f, bbox.getYMin());
    EXPECT_FLOAT_EQ(4.99f, bbox.getYMax());
}

// -----------------------------------------------------------------------------

// Test process with different parameter order
TEST_F(BBoxStatementTest, Process_DifferentParameterOrder) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "YMax", "4.0", "XMin", "1.0", "YMin", "3.0", "XMax", "2.0"};
    
    bool result = bbox.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1.0f, bbox.getXMin());
    EXPECT_FLOAT_EQ(2.0f, bbox.getXMax());
    EXPECT_FLOAT_EQ(3.0f, bbox.getYMin());
    EXPECT_FLOAT_EQ(4.0f, bbox.getYMax());
}

// -----------------------------------------------------------------------------

// Test process with invalid value (non-numeric string)
TEST_F(BBoxStatementTest, Process_InvalidValue) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "invalid", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    
    bool result = bbox.process(tokens);
    EXPECT_TRUE(result);  // process returns true, but invalid value is not assigned
    
    // XMin should remain uninitialized (default value 0.0 or whatever was there)
    // Other values should be set correctly
    EXPECT_FLOAT_EQ(2.0f, bbox.getXMax());
    EXPECT_FLOAT_EQ(3.0f, bbox.getYMin());
    EXPECT_FLOAT_EQ(4.0f, bbox.getYMax());
}

// Test process with unknown parameter name (should fail due to else clause)
TEST_F(BBoxStatementTest, Process_UnknownParameterName) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "Unknown", "4.0"};
    
    bool result = bbox.process(tokens);
    EXPECT_FALSE(result);  // Should fail because "Unknown" is not a recognized parameter
}

// -----------------------------------------------------------------------------

// Test accessors with default values (before processing)
TEST_F(BBoxStatementTest, Accessors_DefaultValues) {
    BBoxStatement bbox;
    
    // Before processing, values should be uninitialized (default float value is 0.0)
    EXPECT_FLOAT_EQ(0.0f, bbox.getXMin());
    EXPECT_FLOAT_EQ(0.0f, bbox.getXMax());
    EXPECT_FLOAT_EQ(0.0f, bbox.getYMin());
    EXPECT_FLOAT_EQ(0.0f, bbox.getYMax());
}

// Test accessors after processing
TEST_F(BBoxStatementTest, Accessors_AfterProcessing) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "10.5", "XMax", "20.5", "YMin", "30.5", "YMax", "40.5"};
    
    bbox.process(tokens);
    
    EXPECT_FLOAT_EQ(10.5f, bbox.getXMin());
    EXPECT_FLOAT_EQ(20.5f, bbox.getXMax());
    EXPECT_FLOAT_EQ(30.5f, bbox.getYMin());
    EXPECT_FLOAT_EQ(40.5f, bbox.getYMax());
}

// Test accessors are const
TEST_F(BBoxStatementTest, Accessors_Const) {
    const BBoxStatement bbox;
    
    // Should compile - accessors are const
    Dimension_t xmin = bbox.getXMin();
    Dimension_t xmax = bbox.getXMax();
    Dimension_t ymin = bbox.getYMin();
    Dimension_t ymax = bbox.getYMax();
    
    EXPECT_FLOAT_EQ(0.0f, xmin);
    EXPECT_FLOAT_EQ(0.0f, xmax);
    EXPECT_FLOAT_EQ(0.0f, ymin);
    EXPECT_FLOAT_EQ(0.0f, ymax);
}

// -----------------------------------------------------------------------------

// Test multiple process calls (should update values)
TEST_F(BBoxStatementTest, Process_MultipleCalls) {
    BBoxStatement bbox;
    
    std::vector<std::string> tokens1 = {"BBox", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    bool result1 = bbox.process(tokens1);
    EXPECT_TRUE(result1);
    
    EXPECT_FLOAT_EQ(1.0f, bbox.getXMin());
    EXPECT_FLOAT_EQ(2.0f, bbox.getXMax());
    EXPECT_FLOAT_EQ(3.0f, bbox.getYMin());
    EXPECT_FLOAT_EQ(4.0f, bbox.getYMax());
    
    std::vector<std::string> tokens2 = {"BBox", "XMin", "10.0", "XMax", "20.0", "YMin", "30.0", "YMax", "40.0"};
    bool result2 = bbox.process(tokens2);
    EXPECT_TRUE(result2);
    
    EXPECT_FLOAT_EQ(10.0f, bbox.getXMin());
    EXPECT_FLOAT_EQ(20.0f, bbox.getXMax());
    EXPECT_FLOAT_EQ(30.0f, bbox.getYMin());
    EXPECT_FLOAT_EQ(40.0f, bbox.getYMax());
}

// -----------------------------------------------------------------------------

// Test isPointStrictlyInside with point inside bounding box
TEST_F(BBoxStatementTest, IsPointStrictlyInside_Inside) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "5.0", "YMin", "2.0", "YMax", "6.0"};
    bbox.process(tokens);
    
    EXPECT_TRUE(bbox.isValid());
    EXPECT_TRUE(bbox.isPointStrictlyInside(3.0f, 4.0f));
    EXPECT_TRUE(bbox.isPointStrictlyInside(2.5f, 3.5f));
    EXPECT_TRUE(bbox.isPointStrictlyInside(4.5f, 5.5f));
}

// -----------------------------------------------------------------------------

// Test isPointStrictlyInside with point on X boundary (should return false)
TEST_F(BBoxStatementTest, IsPointStrictlyInside_OnXBoundary) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "5.0", "YMin", "2.0", "YMax", "6.0"};
    bbox.process(tokens);
    
    EXPECT_TRUE(bbox.isValid());
    EXPECT_FALSE(bbox.isPointStrictlyInside(1.0f, 4.0f));  // On XMin boundary
    EXPECT_FALSE(bbox.isPointStrictlyInside(5.0f, 4.0f));  // On XMax boundary
}

// -----------------------------------------------------------------------------

// Test isPointStrictlyInside with point on Y boundary (should return false)
TEST_F(BBoxStatementTest, IsPointStrictlyInside_OnYBoundary) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "5.0", "YMin", "2.0", "YMax", "6.0"};
    bbox.process(tokens);
    
    EXPECT_TRUE(bbox.isValid());
    EXPECT_FALSE(bbox.isPointStrictlyInside(3.0f, 2.0f));  // On YMin boundary
    EXPECT_FALSE(bbox.isPointStrictlyInside(3.0f, 6.0f));  // On YMax boundary
}

// -----------------------------------------------------------------------------

// Test isPointStrictlyInside with point on corner (should return false)
TEST_F(BBoxStatementTest, IsPointStrictlyInside_OnCorner) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "5.0", "YMin", "2.0", "YMax", "6.0"};
    bbox.process(tokens);
    
    EXPECT_TRUE(bbox.isValid());
    EXPECT_FALSE(bbox.isPointStrictlyInside(1.0f, 2.0f));  // Bottom-left corner
    EXPECT_FALSE(bbox.isPointStrictlyInside(5.0f, 2.0f));  // Bottom-right corner
    EXPECT_FALSE(bbox.isPointStrictlyInside(1.0f, 6.0f));  // Top-left corner
    EXPECT_FALSE(bbox.isPointStrictlyInside(5.0f, 6.0f));  // Top-right corner
}

// -----------------------------------------------------------------------------

// Test isPointStrictlyInside with point outside bounding box
TEST_F(BBoxStatementTest, IsPointStrictlyInside_Outside) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "5.0", "YMin", "2.0", "YMax", "6.0"};
    bbox.process(tokens);
    
    EXPECT_TRUE(bbox.isValid());
    EXPECT_FALSE(bbox.isPointStrictlyInside(0.0f, 4.0f));   // Left of XMin
    EXPECT_FALSE(bbox.isPointStrictlyInside(6.0f, 4.0f));  // Right of XMax
    EXPECT_FALSE(bbox.isPointStrictlyInside(3.0f, 1.0f));  // Below YMin
    EXPECT_FALSE(bbox.isPointStrictlyInside(3.0f, 7.0f));  // Above YMax
    EXPECT_FALSE(bbox.isPointStrictlyInside(0.0f, 0.0f));  // Outside both
}

// -----------------------------------------------------------------------------

// Test isPointStrictlyInside with negative coordinates
TEST_F(BBoxStatementTest, IsPointStrictlyInside_NegativeCoordinates) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "-5.0", "XMax", "-1.0", "YMin", "-6.0", "YMax", "-2.0"};
    bbox.process(tokens);
    
    EXPECT_TRUE(bbox.isValid());
    EXPECT_TRUE(bbox.isPointStrictlyInside(-3.0f, -4.0f));   // Inside
    EXPECT_FALSE(bbox.isPointStrictlyInside(-5.0f, -4.0f));  // On XMin boundary
    EXPECT_FALSE(bbox.isPointStrictlyInside(-1.0f, -4.0f));  // On XMax boundary
    EXPECT_FALSE(bbox.isPointStrictlyInside(-3.0f, -6.0f));  // On YMin boundary
    EXPECT_FALSE(bbox.isPointStrictlyInside(-3.0f, -2.0f));   // On YMax boundary
    EXPECT_FALSE(bbox.isPointStrictlyInside(-10.0f, -4.0f)); // Outside
}

// -----------------------------------------------------------------------------

// Test isPointStrictlyInside with decimal coordinates
TEST_F(BBoxStatementTest, IsPointStrictlyInside_DecimalCoordinates) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1.5", "XMax", "5.5", "YMin", "2.5", "YMax", "6.5"};
    bbox.process(tokens);
    
    EXPECT_TRUE(bbox.isValid());
    EXPECT_TRUE(bbox.isPointStrictlyInside(3.0f, 4.0f));
    EXPECT_TRUE(bbox.isPointStrictlyInside(2.0f, 3.0f));
    EXPECT_TRUE(bbox.isPointStrictlyInside(5.0f, 6.0f));
    EXPECT_FALSE(bbox.isPointStrictlyInside(1.5f, 4.0f));  // On XMin
    EXPECT_FALSE(bbox.isPointStrictlyInside(5.5f, 4.0f));  // On XMax
    EXPECT_FALSE(bbox.isPointStrictlyInside(3.0f, 2.5f));  // On YMin
    EXPECT_FALSE(bbox.isPointStrictlyInside(3.0f, 6.5f));  // On YMax
}

// -----------------------------------------------------------------------------

// Test isPointStrictlyInside const correctness
TEST_F(BBoxStatementTest, IsPointStrictlyInside_ConstCorrectness) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "5.0", "YMin", "2.0", "YMax", "6.0"};
    bbox.process(tokens);
    
    const BBoxStatement& constBbox = bbox;
    EXPECT_TRUE(constBbox.isValid());
    EXPECT_TRUE(constBbox.isPointStrictlyInside(3.0f, 4.0f));
    EXPECT_FALSE(constBbox.isPointStrictlyInside(1.0f, 4.0f));
}

// -----------------------------------------------------------------------------

// Test fixture for WallStatement tests
class WallStatementTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test process with valid Wall tokens (9 tokens total)
TEST_F(WallStatementTest, Process_ValidWall) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    
    bool result = wall.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1.0f, wall.getXMin());
    EXPECT_FLOAT_EQ(2.0f, wall.getXMax());
    EXPECT_FLOAT_EQ(3.0f, wall.getYMin());
    EXPECT_FLOAT_EQ(4.0f, wall.getYMax());
}

// -----------------------------------------------------------------------------

// Test process with case-insensitive Wall (lowercase)
TEST_F(WallStatementTest, Process_CaseInsensitiveWall) {
    WallStatement wall;
    std::vector<std::string> tokens = {"wall", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    
    bool result = wall.process(tokens);
    EXPECT_TRUE(result);
}

// Test process with case-insensitive Wall (mixed case)
TEST_F(WallStatementTest, Process_CaseInsensitiveWallMixed) {
    WallStatement wall;
    std::vector<std::string> tokens = {"WaLl", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    
    bool result = wall.process(tokens);
    EXPECT_TRUE(result);
}

// -----------------------------------------------------------------------------

// Test process with case-insensitive parameter names
TEST_F(WallStatementTest, Process_CaseInsensitiveParameters) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "xmin", "1.0", "xmax", "2.0", "ymin", "3.0", "ymax", "4.0"};
    
    bool result = wall.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1.0f, wall.getXMin());
    EXPECT_FLOAT_EQ(2.0f, wall.getXMax());
    EXPECT_FLOAT_EQ(3.0f, wall.getYMin());
    EXPECT_FLOAT_EQ(4.0f, wall.getYMax());
}

// Test process with mixed case parameter names
TEST_F(WallStatementTest, Process_MixedCaseParameters) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XmIn", "1.0", "XmAx", "2.0", "YmIn", "3.0", "YmAx", "4.0"};
    
    bool result = wall.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1.0f, wall.getXMin());
    EXPECT_FLOAT_EQ(2.0f, wall.getXMax());
    EXPECT_FLOAT_EQ(3.0f, wall.getYMin());
    EXPECT_FLOAT_EQ(4.0f, wall.getYMax());
}

// -----------------------------------------------------------------------------

// Test process with invalid first token (not Wall)
TEST_F(WallStatementTest, Process_InvalidFirstToken) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Source", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    
    bool result = wall.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with empty tokens
TEST_F(WallStatementTest, Process_EmptyTokens) {
    WallStatement wall;
    std::vector<std::string> tokens;
    
    bool result = wall.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with wrong number of tokens (less than 9)
TEST_F(WallStatementTest, Process_TooFewTokens) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "2.0"};
    
    bool result = wall.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with wrong number of tokens (more than 9)
TEST_F(WallStatementTest, Process_TooManyTokens) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0", "Extra"};
    
    bool result = wall.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with exactly 8 tokens (missing one)
TEST_F(WallStatementTest, Process_Exactly8Tokens) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax"};
    
    bool result = wall.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with negative values (valid when min < max)
TEST_F(WallStatementTest, Process_NegativeValues) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "-2.5", "XMax", "-1.5", "YMin", "-4.5", "YMax", "-3.5"};
    
    bool result = wall.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(-2.5f, wall.getXMin());
    EXPECT_FLOAT_EQ(-1.5f, wall.getXMax());
    EXPECT_FLOAT_EQ(-4.5f, wall.getYMin());
    EXPECT_FLOAT_EQ(-3.5f, wall.getYMax());
}

// Test process with XMin >= XMax (should fail validation)
TEST_F(WallStatementTest, Process_XMinGreaterThanOrEqualXMax) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "2.0", "XMax", "1.0", "YMin", "3.0", "YMax", "4.0"};
    
    bool result = wall.process(tokens);
    EXPECT_FALSE(result);  // Should fail because XMin >= XMax
    
    // Values may still be set, but process should return false
    EXPECT_FLOAT_EQ(2.0f, wall.getXMin());
    EXPECT_FLOAT_EQ(1.0f, wall.getXMax());
}

// Test process with XMin equal to XMax (should fail validation)
TEST_F(WallStatementTest, Process_XMinEqualXMax) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "1.0", "YMin", "3.0", "YMax", "4.0"};
    
    bool result = wall.process(tokens);
    EXPECT_FALSE(result);  // Should fail because XMin == XMax
    
    EXPECT_FLOAT_EQ(1.0f, wall.getXMin());
    EXPECT_FLOAT_EQ(1.0f, wall.getXMax());
}

// Test process with YMin >= YMax (should fail validation)
TEST_F(WallStatementTest, Process_YMinGreaterThanOrEqualYMax) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "2.0", "YMin", "4.0", "YMax", "3.0"};
    
    bool result = wall.process(tokens);
    EXPECT_FALSE(result);  // Should fail because YMin >= YMax
    
    // Values may still be set, but process should return false
    EXPECT_FLOAT_EQ(4.0f, wall.getYMin());
    EXPECT_FLOAT_EQ(3.0f, wall.getYMax());
}

// Test process with YMin equal to YMax (should fail validation)
TEST_F(WallStatementTest, Process_YMinEqualYMax) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "3.0"};
    
    bool result = wall.process(tokens);
    EXPECT_FALSE(result);  // Should fail because YMin == YMax
    
    EXPECT_FLOAT_EQ(3.0f, wall.getYMin());
    EXPECT_FLOAT_EQ(3.0f, wall.getYMax());
}

// Test process with both XMin >= XMax and YMin >= YMax (should fail validation)
TEST_F(WallStatementTest, Process_BothDimensionsInvalid) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "2.0", "XMax", "1.0", "YMin", "4.0", "YMax", "3.0"};
    
    bool result = wall.process(tokens);
    EXPECT_FALSE(result);  // Should fail because both dimensions are invalid
    
    EXPECT_FLOAT_EQ(2.0f, wall.getXMin());
    EXPECT_FLOAT_EQ(1.0f, wall.getXMax());
    EXPECT_FLOAT_EQ(4.0f, wall.getYMin());
    EXPECT_FLOAT_EQ(3.0f, wall.getYMax());
}

// Test process with large values
TEST_F(WallStatementTest, Process_LargeValues) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1000.0", "XMax", "2000.0", "YMin", "3000.0", "YMax", "4000.0"};
    
    bool result = wall.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1000.0f, wall.getXMin());
    EXPECT_FLOAT_EQ(2000.0f, wall.getXMax());
    EXPECT_FLOAT_EQ(3000.0f, wall.getYMin());
    EXPECT_FLOAT_EQ(4000.0f, wall.getYMax());
}

// Test process with decimal values
TEST_F(WallStatementTest, Process_DecimalValues) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.5", "XMax", "2.75", "YMin", "3.14159", "YMax", "4.99"};
    
    bool result = wall.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1.5f, wall.getXMin());
    EXPECT_FLOAT_EQ(2.75f, wall.getXMax());
    EXPECT_FLOAT_EQ(3.14159f, wall.getYMin());
    EXPECT_FLOAT_EQ(4.99f, wall.getYMax());
}

// -----------------------------------------------------------------------------

// Test process with different parameter order
TEST_F(WallStatementTest, Process_DifferentParameterOrder) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "YMax", "4.0", "XMin", "1.0", "YMin", "3.0", "XMax", "2.0"};
    
    bool result = wall.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1.0f, wall.getXMin());
    EXPECT_FLOAT_EQ(2.0f, wall.getXMax());
    EXPECT_FLOAT_EQ(3.0f, wall.getYMin());
    EXPECT_FLOAT_EQ(4.0f, wall.getYMax());
}

// -----------------------------------------------------------------------------

// Test process with invalid value (non-numeric string)
TEST_F(WallStatementTest, Process_InvalidValue) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "invalid", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    
    bool result = wall.process(tokens);
    EXPECT_TRUE(result);  // process returns true, but invalid value is not assigned
    
    // XMin should remain uninitialized (default value 0.0 or whatever was there)
    // Other values should be set correctly
    EXPECT_FLOAT_EQ(2.0f, wall.getXMax());
    EXPECT_FLOAT_EQ(3.0f, wall.getYMin());
    EXPECT_FLOAT_EQ(4.0f, wall.getYMax());
}

// Test process with unknown parameter name (should fail due to else clause)
TEST_F(WallStatementTest, Process_UnknownParameterName) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "Unknown", "4.0"};
    
    bool result = wall.process(tokens);
    EXPECT_FALSE(result);  // Should fail because "Unknown" is not a recognized parameter
}

// -----------------------------------------------------------------------------

// Test accessors with default values (before processing)
TEST_F(WallStatementTest, Accessors_DefaultValues) {
    WallStatement wall;
    
    // Before processing, values should be uninitialized (default float value is 0.0)
    EXPECT_FLOAT_EQ(0.0f, wall.getXMin());
    EXPECT_FLOAT_EQ(0.0f, wall.getXMax());
    EXPECT_FLOAT_EQ(0.0f, wall.getYMin());
    EXPECT_FLOAT_EQ(0.0f, wall.getYMax());
}

// Test accessors after processing
TEST_F(WallStatementTest, Accessors_AfterProcessing) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "10.5", "XMax", "20.5", "YMin", "30.5", "YMax", "40.5"};
    
    wall.process(tokens);
    
    EXPECT_FLOAT_EQ(10.5f, wall.getXMin());
    EXPECT_FLOAT_EQ(20.5f, wall.getXMax());
    EXPECT_FLOAT_EQ(30.5f, wall.getYMin());
    EXPECT_FLOAT_EQ(40.5f, wall.getYMax());
}

// Test accessors are const
TEST_F(WallStatementTest, Accessors_Const) {
    const WallStatement wall;
    
    // Should compile - accessors are const
    Dimension_t xmin = wall.getXMin();
    Dimension_t xmax = wall.getXMax();
    Dimension_t ymin = wall.getYMin();
    Dimension_t ymax = wall.getYMax();
    
    EXPECT_FLOAT_EQ(0.0f, xmin);
    EXPECT_FLOAT_EQ(0.0f, xmax);
    EXPECT_FLOAT_EQ(0.0f, ymin);
    EXPECT_FLOAT_EQ(0.0f, ymax);
}

// -----------------------------------------------------------------------------

// Test multiple process calls (should update values)
TEST_F(WallStatementTest, Process_MultipleCalls) {
    WallStatement wall;
    
    std::vector<std::string> tokens1 = {"Wall", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    bool result1 = wall.process(tokens1);
    EXPECT_TRUE(result1);
    
    EXPECT_FLOAT_EQ(1.0f, wall.getXMin());
    EXPECT_FLOAT_EQ(2.0f, wall.getXMax());
    EXPECT_FLOAT_EQ(3.0f, wall.getYMin());
    EXPECT_FLOAT_EQ(4.0f, wall.getYMax());
    
    std::vector<std::string> tokens2 = {"Wall", "XMin", "10.0", "XMax", "20.0", "YMin", "30.0", "YMax", "40.0"};
    bool result2 = wall.process(tokens2);
    EXPECT_TRUE(result2);
    
    EXPECT_FLOAT_EQ(10.0f, wall.getXMin());
    EXPECT_FLOAT_EQ(20.0f, wall.getXMax());
    EXPECT_FLOAT_EQ(30.0f, wall.getYMin());
    EXPECT_FLOAT_EQ(40.0f, wall.getYMax());
}

// -----------------------------------------------------------------------------

// Test isPointStrictlyInside with point inside bounding box
TEST_F(WallStatementTest, IsPointStrictlyInside_Inside) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "5.0", "YMin", "2.0", "YMax", "6.0"};
    wall.process(tokens);
    
    EXPECT_TRUE(wall.isValid());
    EXPECT_TRUE(wall.isPointStrictlyInside(3.0f, 4.0f));
    EXPECT_TRUE(wall.isPointStrictlyInside(2.5f, 3.5f));
    EXPECT_TRUE(wall.isPointStrictlyInside(4.5f, 5.5f));
}

// -----------------------------------------------------------------------------

// Test isPointStrictlyInside with point on X boundary (should return false)
TEST_F(WallStatementTest, IsPointStrictlyInside_OnXBoundary) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "5.0", "YMin", "2.0", "YMax", "6.0"};
    wall.process(tokens);
    
    EXPECT_TRUE(wall.isValid());
    EXPECT_FALSE(wall.isPointStrictlyInside(1.0f, 4.0f));  // On XMin boundary
    EXPECT_FALSE(wall.isPointStrictlyInside(5.0f, 4.0f));  // On XMax boundary
}

// -----------------------------------------------------------------------------

// Test isPointStrictlyInside with point on Y boundary (should return false)
TEST_F(WallStatementTest, IsPointStrictlyInside_OnYBoundary) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "5.0", "YMin", "2.0", "YMax", "6.0"};
    wall.process(tokens);
    
    EXPECT_TRUE(wall.isValid());
    EXPECT_FALSE(wall.isPointStrictlyInside(3.0f, 2.0f));  // On YMin boundary
    EXPECT_FALSE(wall.isPointStrictlyInside(3.0f, 6.0f));  // On YMax boundary
}

// -----------------------------------------------------------------------------

// Test isPointStrictlyInside with point on corner (should return false)
TEST_F(WallStatementTest, IsPointStrictlyInside_OnCorner) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "5.0", "YMin", "2.0", "YMax", "6.0"};
    wall.process(tokens);
    
    EXPECT_TRUE(wall.isValid());
    EXPECT_FALSE(wall.isPointStrictlyInside(1.0f, 2.0f));  // Bottom-left corner
    EXPECT_FALSE(wall.isPointStrictlyInside(5.0f, 2.0f));  // Bottom-right corner
    EXPECT_FALSE(wall.isPointStrictlyInside(1.0f, 6.0f));  // Top-left corner
    EXPECT_FALSE(wall.isPointStrictlyInside(5.0f, 6.0f));  // Top-right corner
}

// -----------------------------------------------------------------------------

// Test isPointStrictlyInside with point outside bounding box
TEST_F(WallStatementTest, IsPointStrictlyInside_Outside) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "5.0", "YMin", "2.0", "YMax", "6.0"};
    wall.process(tokens);
    
    EXPECT_TRUE(wall.isValid());
    EXPECT_FALSE(wall.isPointStrictlyInside(0.0f, 4.0f));   // Left of XMin
    EXPECT_FALSE(wall.isPointStrictlyInside(6.0f, 4.0f));  // Right of XMax
    EXPECT_FALSE(wall.isPointStrictlyInside(3.0f, 1.0f));  // Below YMin
    EXPECT_FALSE(wall.isPointStrictlyInside(3.0f, 7.0f));  // Above YMax
    EXPECT_FALSE(wall.isPointStrictlyInside(0.0f, 0.0f));  // Outside both
}

// -----------------------------------------------------------------------------

// Test isPointStrictlyInside with negative coordinates
TEST_F(WallStatementTest, IsPointStrictlyInside_NegativeCoordinates) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "-5.0", "XMax", "-1.0", "YMin", "-6.0", "YMax", "-2.0"};
    wall.process(tokens);
    
    EXPECT_TRUE(wall.isValid());
    EXPECT_TRUE(wall.isPointStrictlyInside(-3.0f, -4.0f));   // Inside
    EXPECT_FALSE(wall.isPointStrictlyInside(-5.0f, -4.0f));  // On XMin boundary
    EXPECT_FALSE(wall.isPointStrictlyInside(-1.0f, -4.0f));  // On XMax boundary
    EXPECT_FALSE(wall.isPointStrictlyInside(-3.0f, -6.0f));  // On YMin boundary
    EXPECT_FALSE(wall.isPointStrictlyInside(-3.0f, -2.0f));   // On YMax boundary
    EXPECT_FALSE(wall.isPointStrictlyInside(-10.0f, -4.0f)); // Outside
}

// -----------------------------------------------------------------------------

// Test isPointStrictlyInside with decimal coordinates
TEST_F(WallStatementTest, IsPointStrictlyInside_DecimalCoordinates) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.5", "XMax", "5.5", "YMin", "2.5", "YMax", "6.5"};
    wall.process(tokens);
    
    EXPECT_TRUE(wall.isValid());
    EXPECT_TRUE(wall.isPointStrictlyInside(3.0f, 4.0f));
    EXPECT_TRUE(wall.isPointStrictlyInside(2.0f, 3.0f));
    EXPECT_TRUE(wall.isPointStrictlyInside(5.0f, 6.0f));
    EXPECT_FALSE(wall.isPointStrictlyInside(1.5f, 4.0f));  // On XMin
    EXPECT_FALSE(wall.isPointStrictlyInside(5.5f, 4.0f));  // On XMax
    EXPECT_FALSE(wall.isPointStrictlyInside(3.0f, 2.5f));  // On YMin
    EXPECT_FALSE(wall.isPointStrictlyInside(3.0f, 6.5f));  // On YMax
}

// -----------------------------------------------------------------------------

// Test isPointStrictlyInside const correctness
TEST_F(WallStatementTest, IsPointStrictlyInside_ConstCorrectness) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "5.0", "YMin", "2.0", "YMax", "6.0"};
    wall.process(tokens);
    
    const WallStatement& constWall = wall;
    EXPECT_TRUE(constWall.isValid());
    EXPECT_TRUE(constWall.isPointStrictlyInside(3.0f, 4.0f));
    EXPECT_FALSE(constWall.isPointStrictlyInside(1.0f, 4.0f));
}

// -----------------------------------------------------------------------------

// Test isValid with valid wall
TEST_F(WallStatementTest, IsValid_ValidWall) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    wall.process(tokens);
    
    EXPECT_TRUE(wall.isValid());
}

// Test isValid with invalid wall (XMin >= XMax)
TEST_F(WallStatementTest, IsValid_InvalidXDimension) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "2.0", "XMax", "1.0", "YMin", "3.0", "YMax", "4.0"};
    wall.process(tokens);
    
    EXPECT_FALSE(wall.isValid());
}

// Test isValid with invalid wall (YMin >= YMax)
TEST_F(WallStatementTest, IsValid_InvalidYDimension) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "2.0", "YMin", "4.0", "YMax", "3.0"};
    wall.process(tokens);
    
    EXPECT_FALSE(wall.isValid());
}

// -----------------------------------------------------------------------------

// Test fixture for SourceStatement tests
class SourceStatementTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test process with valid Source tokens (9 tokens total)
TEST_F(SourceStatementTest, Process_ValidSource) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "0.5", "X", "1.3", "Y", "-2.5"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.5f, source.getAmplitude());
    EXPECT_FLOAT_EQ(1.3f, source.getX());
    EXPECT_FLOAT_EQ(-2.5f, source.getY());
}

// -----------------------------------------------------------------------------

// Test process with case-insensitive Source (lowercase)
TEST_F(SourceStatementTest, Process_CaseInsensitiveSource) {
    SourceStatement source;
    std::vector<std::string> tokens = {"source", "Frequency", "1000.0", "Amplitude", "0.5", "X", "1.0", "Y", "2.0"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
}

// Test process with case-insensitive Source (mixed case)
TEST_F(SourceStatementTest, Process_CaseInsensitiveSourceMixed) {
    SourceStatement source;
    std::vector<std::string> tokens = {"SoUrCe", "Frequency", "1000.0", "Amplitude", "0.5", "X", "1.0", "Y", "2.0"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
}

// -----------------------------------------------------------------------------

// Test process with case-insensitive parameter names
TEST_F(SourceStatementTest, Process_CaseInsensitiveParameters) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "frequency", "1000.0", "amplitude", "0.5", "x", "1.5", "y", "2.5"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.5f, source.getAmplitude());
    EXPECT_FLOAT_EQ(1.5f, source.getX());
    EXPECT_FLOAT_EQ(2.5f, source.getY());
}

// Test process with mixed case parameter names
TEST_F(SourceStatementTest, Process_MixedCaseParameters) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "FrEqUeNcY", "1000.0", "AmPlItUdE", "0.5", "X", "1.5", "Y", "2.5"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.5f, source.getAmplitude());
    EXPECT_FLOAT_EQ(1.5f, source.getX());
    EXPECT_FLOAT_EQ(2.5f, source.getY());
}

// -----------------------------------------------------------------------------

// Test process with invalid first token (not Source)
TEST_F(SourceStatementTest, Process_InvalidFirstToken) {
    SourceStatement source;
    std::vector<std::string> tokens = {"BBox", "Frequency", "1000.0", "Amplitude", "0.5"};
    
    bool result = source.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with empty tokens
TEST_F(SourceStatementTest, Process_EmptyTokens) {
    SourceStatement source;
    std::vector<std::string> tokens;
    
    bool result = source.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with wrong number of tokens (less than 9)
TEST_F(SourceStatementTest, Process_TooFewTokens) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "0.5"};
    
    bool result = source.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with wrong number of tokens (more than 9)
TEST_F(SourceStatementTest, Process_TooManyTokens) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "0.5", "X", "1.5", "Y", "2.5", "Extra"};
    
    bool result = source.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with exactly 8 tokens (missing one pair)
TEST_F(SourceStatementTest, Process_Exactly4Tokens) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "0.5", "X", "1.5", "Y"};
    
    bool result = source.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with negative frequency (should fail validation)
TEST_F(SourceStatementTest, Process_NegativeFrequency) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "-1000.0", "Amplitude", "0.5", "X", "1.5", "Y", "2.5"};
    
    bool result = source.process(tokens);
    EXPECT_FALSE(result);  // Should fail because frequency is negative
    
    // Values may still be set, but process should return false
    EXPECT_FLOAT_EQ(-1000.0f, source.getFreq());
}

// Test process with zero frequency (should fail validation)
TEST_F(SourceStatementTest, Process_ZeroFrequency) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "0.0", "Amplitude", "0.5", "X", "1.5", "Y", "2.5"};
    
    bool result = source.process(tokens);
    EXPECT_FALSE(result);  // Should fail because frequency is zero
    
    // Values may still be set, but process should return false
    EXPECT_FLOAT_EQ(0.0f, source.getFreq());
}

// Test process with large values
TEST_F(SourceStatementTest, Process_LargeValues) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000000.0", "Amplitude", "1000.0", "X", "5000.0", "Y", "6000.0"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1000000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(1000.0f, source.getAmplitude());
    EXPECT_FLOAT_EQ(5000.0f, source.getX());
    EXPECT_FLOAT_EQ(6000.0f, source.getY());
}

// Test process with decimal values
TEST_F(SourceStatementTest, Process_DecimalValues) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1234.567", "Amplitude", "0.12345", "X", "12.345", "Y", "23.456"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1234.567f, source.getFreq());
    EXPECT_FLOAT_EQ(0.12345f, source.getAmplitude());
    EXPECT_FLOAT_EQ(12.345f, source.getX());
    EXPECT_FLOAT_EQ(23.456f, source.getY());
}

// -----------------------------------------------------------------------------

// Test process with different parameter order
TEST_F(SourceStatementTest, Process_DifferentParameterOrder) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Amplitude", "0.5", "Frequency", "1000.0", "Y", "2.5", "X", "1.5"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.5f, source.getAmplitude());
    EXPECT_FLOAT_EQ(1.5f, source.getX());
    EXPECT_FLOAT_EQ(2.5f, source.getY());
}

// -----------------------------------------------------------------------------

// Test process with invalid value (non-numeric string for frequency)
TEST_F(SourceStatementTest, Process_InvalidFrequencyValue) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "invalid", "Amplitude", "0.5", "X", "1.5", "Y", "2.5"};
    
    bool result = source.process(tokens);
    EXPECT_FALSE(result);  // Should fail because frequency conversion fails
    
    // Frequency should remain uninitialized (default value 0.0)
    // Amplitude should not be set because we return false immediately
    EXPECT_FLOAT_EQ(0.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.0f, source.getAmplitude());
}

// Test process with invalid value (non-numeric string for amplitude, but valid frequency)
TEST_F(SourceStatementTest, Process_InvalidAmplitudeValue) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "invalid", "X", "1.5", "Y", "2.5"};
    
    bool result = source.process(tokens);
    EXPECT_FALSE(result);  // Should fail because amplitude conversion fails
    
    EXPECT_FLOAT_EQ(1000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.0f, source.getAmplitude());  // Amplitude remains default
}

// Test process with unknown parameter name (should fail due to else clause)
TEST_F(SourceStatementTest, Process_UnknownParameterName) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Unknown", "0.5", "X", "1.5", "Y", "2.5"};
    
    bool result = source.process(tokens);
    EXPECT_FALSE(result);  // Should fail because "Unknown" is not a recognized parameter
}

// -----------------------------------------------------------------------------

// Test accessors with default values (before processing)
TEST_F(SourceStatementTest, Accessors_DefaultValues) {
    SourceStatement source;
    
    // Before processing, values should be uninitialized (default float value is 0.0)
    EXPECT_FLOAT_EQ(0.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.0f, source.getAmplitude());
}

// Test accessors after processing
TEST_F(SourceStatementTest, Accessors_AfterProcessing) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "2500.0", "Amplitude", "0.75", "X", "3.5", "Y", "4.5"};
    
    source.process(tokens);
    
    EXPECT_FLOAT_EQ(2500.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.75f, source.getAmplitude());
    EXPECT_FLOAT_EQ(3.5f, source.getX());
    EXPECT_FLOAT_EQ(4.5f, source.getY());
}

// Test accessors are const
TEST_F(SourceStatementTest, Accessors_Const) {
    const SourceStatement source;
    
    // Should compile - accessors are const
    Frequency_t freq = source.getFreq();
    Amplitude_t amplitude = source.getAmplitude();
    
    EXPECT_FLOAT_EQ(0.0f, freq);
    EXPECT_FLOAT_EQ(0.0f, amplitude);
}

// -----------------------------------------------------------------------------

// Test multiple process calls (should update values)
TEST_F(SourceStatementTest, Process_MultipleCalls) {
    SourceStatement source;
    
    std::vector<std::string> tokens1 = {"Source", "Frequency", "1000.0", "Amplitude", "0.5", "X", "1.0", "Y", "2.0"};
    bool result1 = source.process(tokens1);
    EXPECT_TRUE(result1);
    
    EXPECT_FLOAT_EQ(1000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.5f, source.getAmplitude());
    EXPECT_FLOAT_EQ(1.0f, source.getX());
    EXPECT_FLOAT_EQ(2.0f, source.getY());
    
    std::vector<std::string> tokens2 = {"Source", "Frequency", "2000.0", "Amplitude", "1.0", "X", "3.0", "Y", "4.0"};
    bool result2 = source.process(tokens2);
    EXPECT_TRUE(result2);
    
    EXPECT_FLOAT_EQ(2000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(1.0f, source.getAmplitude());
}

// -----------------------------------------------------------------------------

// Test default duration value
TEST_F(SourceStatementTest, Duration_DefaultValue) {
    SourceStatement source;
    
    // Default duration should be max value
    EXPECT_FLOAT_EQ(std::numeric_limits<float>::max(), source.getDuration());
}

// -----------------------------------------------------------------------------

// Test process with Duration parameter (11 tokens)
TEST_F(SourceStatementTest, Process_WithDuration) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "0.5", "X", "1.3", "Y", "-2.5", "Duration", "5.0"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.5f, source.getAmplitude());
    EXPECT_FLOAT_EQ(1.3f, source.getX());
    EXPECT_FLOAT_EQ(-2.5f, source.getY());
    EXPECT_FLOAT_EQ(5.0f, source.getDuration());
}

// -----------------------------------------------------------------------------

// Test process without Duration parameter (9 tokens, should use default)
TEST_F(SourceStatementTest, Process_WithoutDuration) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "0.5", "X", "1.3", "Y", "-2.5"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.5f, source.getAmplitude());
    EXPECT_FLOAT_EQ(1.3f, source.getX());
    EXPECT_FLOAT_EQ(-2.5f, source.getY());
    // Duration should remain at default (max value)
    EXPECT_FLOAT_EQ(std::numeric_limits<float>::max(), source.getDuration());
}

// -----------------------------------------------------------------------------

// Test process with case-insensitive Duration parameter
TEST_F(SourceStatementTest, Process_CaseInsensitiveDuration) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "0.5", "X", "1.3", "Y", "-2.5", "duration", "3.0"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(3.0f, source.getDuration());
}

// -----------------------------------------------------------------------------

// Test process with mixed case Duration parameter
TEST_F(SourceStatementTest, Process_MixedCaseDuration) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "0.5", "X", "1.3", "Y", "-2.5", "DuRaTiOn", "7.5"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(7.5f, source.getDuration());
}

// -----------------------------------------------------------------------------

// Test process with invalid Duration value (negative)
TEST_F(SourceStatementTest, Process_NegativeDuration) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "0.5", "X", "1.3", "Y", "-2.5", "Duration", "-1.0"};
    
    bool result = source.process(tokens);
    EXPECT_FALSE(result);  // Should fail because duration is negative
    
    // Duration may still be set, but process should return false
    EXPECT_FLOAT_EQ(-1.0f, source.getDuration());
}

// -----------------------------------------------------------------------------

// Test process with invalid Duration value (zero)
TEST_F(SourceStatementTest, Process_ZeroDuration) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "0.5", "X", "1.3", "Y", "-2.5", "Duration", "0.0"};
    
    bool result = source.process(tokens);
    EXPECT_FALSE(result);  // Should fail because duration is zero
    
    // Duration may still be set, but process should return false
    EXPECT_FLOAT_EQ(0.0f, source.getDuration());
}

// -----------------------------------------------------------------------------

// Test getValue with time less than duration
TEST_F(SourceStatementTest, GetValue_TimeLessThanDuration) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "1.0", "X", "0.0", "Y", "0.0", "Duration", "5.0"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    // Test at time = 0.0
    float value0 = source.getValue(0.0f);
    float expected0 = 1.0f * std::sin(2.0f * 3.14159265358979323846f * 1000.0f * 0.0f);
    EXPECT_NEAR(expected0, value0, 1e-5f);
    
    // Test at time = 2.0 (less than duration)
    float value2 = source.getValue(2.0f);
    float expected2 = 1.0f * std::sin(2.0f * 3.14159265358979323846f * 1000.0f * 2.0f);
    EXPECT_NEAR(expected2, value2, 1e-5f);
    
    // Test at time = 4.99 (less than duration)
    float value499 = source.getValue(4.99f);
    float expected499 = 1.0f * std::sin(2.0f * 3.14159265358979323846f * 1000.0f * 4.99f);
    EXPECT_NEAR(expected499, value499, 1e-5f);
    
}

// -----------------------------------------------------------------------------

// Test getValue with time equal to duration
TEST_F(SourceStatementTest, GetValue_TimeEqualToDuration) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "1.0", "X", "0.0", "Y", "0.0", "Duration", "5.0"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    // Test at time = 5.0 (equal to duration)
    float value5 = source.getValue(5.0f);
    float expected5 = 1.0f * std::sin(2.0f * 3.14159265358979323846f * 1000.0f * 5.0f);
    EXPECT_NEAR(expected5, value5, 1e-5f);
}

// -----------------------------------------------------------------------------

// Test getValue with time greater than duration
TEST_F(SourceStatementTest, GetValue_TimeGreaterThanDuration) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "1.0", "X", "0.0", "Y", "0.0", "Duration", "5.0"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    float time = 1 / (1000.0f) * 0.25f;
    float value401 = source.getValue(time);
    EXPECT_FLOAT_EQ(1.0f, value401);
    // Test at time = 5.01 (greater than duration)
    float value501 = source.getValue(5.01f);
    EXPECT_FLOAT_EQ(0.0f, value501);
    
    // Test at time = 10.0 (greater than duration)
    float value10 = source.getValue(10.0f);
    EXPECT_FLOAT_EQ(0.0f, value10);
    
    // Test at time = 100.0 (much greater than duration)
    float value100 = source.getValue(100.0f);
    EXPECT_FLOAT_EQ(0.0f, value100);
}

// -----------------------------------------------------------------------------

// Test getValue with default duration (max value)
TEST_F(SourceStatementTest, GetValue_DefaultDuration) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "1.0", "X", "0.0", "Y", "0.0"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    // With default duration (max), getValue should always return sine wave
    // Test at various times
    float value1 = source.getValue(1.0f);
    float expected1 = 1.0f * std::sin(2.0f * 3.14159265358979323846f * 1000.0f * 1.0f);
    EXPECT_NEAR(expected1, value1, 1e-5f);
    
    float value100 = source.getValue(100.0f);
    float expected100 = 1.0f * std::sin(2.0f * 3.14159265358979323846f * 1000.0f * 100.0f);
    EXPECT_NEAR(expected100, value100, 1e-5f);
}

// -----------------------------------------------------------------------------

// Test getDuration accessor
TEST_F(SourceStatementTest, GetDuration_Accessor) {
    SourceStatement source;
    
    // Default duration
    EXPECT_FLOAT_EQ(std::numeric_limits<float>::max(), source.getDuration());
    
    // After processing with Duration
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "0.5", "X", "1.3", "Y", "-2.5", "Duration", "3.5"};
    source.process(tokens);
    
    EXPECT_FLOAT_EQ(3.5f, source.getDuration());
}

// -----------------------------------------------------------------------------

// Test getDuration const correctness
TEST_F(SourceStatementTest, GetDuration_ConstCorrectness) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "0.5", "X", "1.3", "Y", "-2.5", "Duration", "4.5"};
    source.process(tokens);
    
    const SourceStatement& constSource = source;
    Time_t duration = constSource.getDuration();
    EXPECT_FLOAT_EQ(4.5f, duration);
}

// -----------------------------------------------------------------------------

// Test getValue const correctness
TEST_F(SourceStatementTest, GetValue_ConstCorrectness) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "1.0", "X", "0.0", "Y", "0.0", "Duration", "5.0"};
    source.process(tokens);
    
    const SourceStatement& constSource = source;
    float value = constSource.getValue(2.0f);
    float expected = 1.0f * std::sin(2.0f * 3.14159265358979323846f * 1000.0f * 2.0f);
    EXPECT_NEAR(expected, value, 1e-5f);
}

// -----------------------------------------------------------------------------

// Test fixture for VelocityStatement tests
class VelocityStatementTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test process with valid Velocity tokens (2 tokens total)
TEST_F(VelocityStatementTest, Process_ValidVelocity) {
    VelocityStatement velocity;
    std::vector<std::string> tokens = {"Velocity", "130.0"};
    
    bool result = velocity.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(130.0f, velocity.getVelocity());
}

// -----------------------------------------------------------------------------

// Test process with case-insensitive Velocity (lowercase)
TEST_F(VelocityStatementTest, Process_CaseInsensitiveVelocity) {
    VelocityStatement velocity;
    std::vector<std::string> tokens = {"velocity", "130.0"};
    
    bool result = velocity.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(130.0f, velocity.getVelocity());
}

// Test process with case-insensitive Velocity (mixed case)
TEST_F(VelocityStatementTest, Process_CaseInsensitiveVelocityMixed) {
    VelocityStatement velocity;
    std::vector<std::string> tokens = {"VeLoCiTy", "130.0"};
    
    bool result = velocity.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(130.0f, velocity.getVelocity());
}

// -----------------------------------------------------------------------------

// Test process with invalid first token (not Velocity)
TEST_F(VelocityStatementTest, Process_InvalidFirstToken) {
    VelocityStatement velocity;
    std::vector<std::string> tokens = {"Source", "130.0"};
    
    bool result = velocity.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with empty tokens
TEST_F(VelocityStatementTest, Process_EmptyTokens) {
    VelocityStatement velocity;
    std::vector<std::string> tokens;
    
    bool result = velocity.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with wrong number of tokens (less than 2)
TEST_F(VelocityStatementTest, Process_TooFewTokens) {
    VelocityStatement velocity;
    std::vector<std::string> tokens = {"Velocity"};
    
    bool result = velocity.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with wrong number of tokens (more than 2)
TEST_F(VelocityStatementTest, Process_TooManyTokens) {
    VelocityStatement velocity;
    std::vector<std::string> tokens = {"Velocity", "130.0", "Extra"};
    
    bool result = velocity.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with negative values (should fail validation)
TEST_F(VelocityStatementTest, Process_NegativeValues) {
    VelocityStatement velocity;
    std::vector<std::string> tokens = {"Velocity", "-130.0"};
    
    bool result = velocity.process(tokens);
    EXPECT_FALSE(result);  // Should fail because velocity is negative
    
    // Values may still be set, but process should return false
    EXPECT_FLOAT_EQ(-130.0f, velocity.getVelocity());
}

// Test process with zero value (should fail validation)
TEST_F(VelocityStatementTest, Process_ZeroValue) {
    VelocityStatement velocity;
    std::vector<std::string> tokens = {"Velocity", "0.0"};
    
    bool result = velocity.process(tokens);
    EXPECT_FALSE(result);  // Should fail because velocity is zero
    
    // Values may still be set, but process should return false
    EXPECT_FLOAT_EQ(0.0f, velocity.getVelocity());
}

// Test process with large values
TEST_F(VelocityStatementTest, Process_LargeValues) {
    VelocityStatement velocity;
    std::vector<std::string> tokens = {"Velocity", "1000000.0"};
    
    bool result = velocity.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1000000.0f, velocity.getVelocity());
}

// Test process with decimal values
TEST_F(VelocityStatementTest, Process_DecimalValues) {
    VelocityStatement velocity;
    std::vector<std::string> tokens = {"Velocity", "123.456"};
    
    bool result = velocity.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(123.456f, velocity.getVelocity());
}

// -----------------------------------------------------------------------------

// Test process with invalid value (non-numeric string)
TEST_F(VelocityStatementTest, Process_InvalidValue) {
    VelocityStatement velocity;
    std::vector<std::string> tokens = {"Velocity", "invalid"};
    
    bool result = velocity.process(tokens);
    EXPECT_FALSE(result);
    
    // Velocity should remain uninitialized (default value 0.0)
    EXPECT_FLOAT_EQ(0.0f, velocity.getVelocity());
}

// -----------------------------------------------------------------------------

// Test accessors with default values (before processing)
TEST_F(VelocityStatementTest, Accessors_DefaultValues) {
    VelocityStatement velocity;
    
    // Before processing, value should be uninitialized (default float value is 0.0)
    EXPECT_FLOAT_EQ(0.0f, velocity.getVelocity());
}

// Test accessors after processing
TEST_F(VelocityStatementTest, Accessors_AfterProcessing) {
    VelocityStatement velocity;
    std::vector<std::string> tokens = {"Velocity", "250.5"};
    
    velocity.process(tokens);
    
    EXPECT_FLOAT_EQ(250.5f, velocity.getVelocity());
}

// Test accessors are const
TEST_F(VelocityStatementTest, Accessors_Const) {
    const VelocityStatement velocity;
    
    // Should compile - accessors are const
    Velocity_t vel = velocity.getVelocity();
    
    EXPECT_FLOAT_EQ(0.0f, vel);
}

// -----------------------------------------------------------------------------

// Test multiple process calls (should update values)
TEST_F(VelocityStatementTest, Process_MultipleCalls) {
    VelocityStatement velocity;
    
    std::vector<std::string> tokens1 = {"Velocity", "130.0"};
    bool result1 = velocity.process(tokens1);
    EXPECT_TRUE(result1);
    
    EXPECT_FLOAT_EQ(130.0f, velocity.getVelocity());
    
    std::vector<std::string> tokens2 = {"Velocity", "200.0"};
    bool result2 = velocity.process(tokens2);
    EXPECT_TRUE(result2);
    
    EXPECT_FLOAT_EQ(200.0f, velocity.getVelocity());
}

// -----------------------------------------------------------------------------

// Test fixture for MaxResolutionStatement tests
class MaxResolutionStatementTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test process with valid MaxResolution tokens (5 tokens total)
TEST_F(MaxResolutionStatementTest, Process_ValidMaxResolution) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "2.0", "Temporal", "1.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(2.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(1.0f, maxRes.getTemporal());
}

// -----------------------------------------------------------------------------

// Test process with case-insensitive MaxResolution (lowercase)
TEST_F(MaxResolutionStatementTest, Process_CaseInsensitiveMaxResolution) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"maxresolution", "Spatial", "2.0", "Temporal", "1.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(2.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(1.0f, maxRes.getTemporal());
}

// Test process with case-insensitive MaxResolution (mixed case)
TEST_F(MaxResolutionStatementTest, Process_CaseInsensitiveMaxResolutionMixed) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaXrEsOlUtIoN", "Spatial", "2.0", "Temporal", "1.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(2.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(1.0f, maxRes.getTemporal());
}

// -----------------------------------------------------------------------------

// Test process with case-insensitive parameter names
TEST_F(MaxResolutionStatementTest, Process_CaseInsensitiveParameters) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "spatial", "2.0", "temporal", "1.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(2.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(1.0f, maxRes.getTemporal());
}

// Test process with mixed case parameter names
TEST_F(MaxResolutionStatementTest, Process_MixedCaseParameters) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "SpAtIaL", "2.0", "TeMpOrAl", "1.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(2.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(1.0f, maxRes.getTemporal());
}

// -----------------------------------------------------------------------------

// Test process with invalid first token (not MaxResolution)
TEST_F(MaxResolutionStatementTest, Process_InvalidFirstToken) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"Source", "Spatial", "1.0", "Temporal", "2.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with empty tokens
TEST_F(MaxResolutionStatementTest, Process_EmptyTokens) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens;
    
    bool result = maxRes.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with wrong number of tokens (less than 5)
TEST_F(MaxResolutionStatementTest, Process_TooFewTokens) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with wrong number of tokens (more than 5)
TEST_F(MaxResolutionStatementTest, Process_TooManyTokens) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1.0", "Temporal", "2.0", "Extra"};
    
    bool result = maxRes.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with exactly 4 tokens (missing one)
TEST_F(MaxResolutionStatementTest, Process_Exactly4Tokens) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1.0", "Temporal"};
    
    bool result = maxRes.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with negative Spatial value (should fail validation)
TEST_F(MaxResolutionStatementTest, Process_NegativeSpatial) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "-1.0", "Temporal", "2.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_FALSE(result);  // Should fail because Spatial is negative
    
    // Values may still be set, but process should return false
    EXPECT_FLOAT_EQ(-1.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(2.0f, maxRes.getTemporal());
}

// Test process with zero Spatial value (should fail validation)
TEST_F(MaxResolutionStatementTest, Process_ZeroSpatial) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "0.0", "Temporal", "2.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_FALSE(result);  // Should fail because Spatial is zero
    
    EXPECT_FLOAT_EQ(0.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(2.0f, maxRes.getTemporal());
}

// Test process with negative Temporal value (should fail validation)
TEST_F(MaxResolutionStatementTest, Process_NegativeTemporal) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1.0", "Temporal", "-2.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_FALSE(result);  // Should fail because Temporal is negative
    
    EXPECT_FLOAT_EQ(1.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(-2.0f, maxRes.getTemporal());
}

// Test process with zero Temporal value (should fail validation)
TEST_F(MaxResolutionStatementTest, Process_ZeroTemporal) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1.0", "Temporal", "0.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_FALSE(result);  // Should fail because Temporal is zero
    
    EXPECT_FLOAT_EQ(1.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(0.0f, maxRes.getTemporal());
}

// Test process with both negative values (should fail validation)
TEST_F(MaxResolutionStatementTest, Process_BothNegative) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "-1.0", "Temporal", "-2.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_FALSE(result);  // Should fail because both are negative
    
    EXPECT_FLOAT_EQ(-1.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(-2.0f, maxRes.getTemporal());
}

// Test process with Spatial equal to Temporal (should fail validation)
TEST_F(MaxResolutionStatementTest, Process_SpatialEqualTemporal) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "2.0", "Temporal", "2.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_FALSE(result);  // Should fail because Spatial must be greater than Temporal
    
    EXPECT_FLOAT_EQ(2.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(2.0f, maxRes.getTemporal());
}

// Test process with Spatial less than Temporal (should fail validation)
TEST_F(MaxResolutionStatementTest, Process_SpatialLessThanTemporal) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1.0", "Temporal", "2.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_FALSE(result);  // Should fail because Spatial must be greater than Temporal
    
    EXPECT_FLOAT_EQ(1.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(2.0f, maxRes.getTemporal());
}

// Test process with Spatial greater than Temporal (should pass validation)
TEST_F(MaxResolutionStatementTest, Process_SpatialGreaterThanTemporal) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "2.0", "Temporal", "1.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_TRUE(result);  // Should pass because Spatial is greater than Temporal
    
    EXPECT_FLOAT_EQ(2.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(1.0f, maxRes.getTemporal());
}

// Test process with large values
TEST_F(MaxResolutionStatementTest, Process_LargeValues) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "2000.0", "Temporal", "1000.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(2000.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(1000.0f, maxRes.getTemporal());
}

// Test process with decimal values
TEST_F(MaxResolutionStatementTest, Process_DecimalValues) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "2.75", "Temporal", "1.5"};
    
    bool result = maxRes.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(2.75f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(1.5f, maxRes.getTemporal());
}

// -----------------------------------------------------------------------------

// Test process with different parameter order
TEST_F(MaxResolutionStatementTest, Process_DifferentParameterOrder) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Temporal", "1.0", "Spatial", "2.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(2.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(1.0f, maxRes.getTemporal());
}

// -----------------------------------------------------------------------------

// Test process with invalid value (non-numeric string for Spatial)
TEST_F(MaxResolutionStatementTest, Process_InvalidSpatialValue) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "invalid", "Temporal", "2.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_FALSE(result);  // Should fail because Spatial is not set (remains 0.0) and isValid() fails
    
    // Spatial should remain uninitialized (default value 0.0)
    // Temporal should be set correctly
    EXPECT_FLOAT_EQ(0.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(2.0f, maxRes.getTemporal());
}

// Test process with invalid value (non-numeric string for Temporal, but valid Spatial)
TEST_F(MaxResolutionStatementTest, Process_InvalidTemporalValue) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1.0", "Temporal", "invalid"};
    
    bool result = maxRes.process(tokens);
    EXPECT_FALSE(result);  // Should fail because Temporal is not set (remains 0.0) and isValid() fails
    
    EXPECT_FLOAT_EQ(1.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(0.0f, maxRes.getTemporal());  // Temporal remains default
}

// Test process with unknown parameter name (should fail due to else clause)
TEST_F(MaxResolutionStatementTest, Process_UnknownParameterName) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1.0", "Unknown", "2.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_FALSE(result);  // Should fail because "Unknown" is not a recognized parameter
}

// -----------------------------------------------------------------------------

// Test accessors with default values (before processing)
TEST_F(MaxResolutionStatementTest, Accessors_DefaultValues) {
    MaxResolutionStatement maxRes;
    
    // Before processing, values should be uninitialized (default float value is 0.0)
    EXPECT_FLOAT_EQ(0.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(0.0f, maxRes.getTemporal());
}

// Test accessors after processing
TEST_F(MaxResolutionStatementTest, Accessors_AfterProcessing) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "20.5", "Temporal", "10.5"};
    
    maxRes.process(tokens);
    
    EXPECT_FLOAT_EQ(20.5f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(10.5f, maxRes.getTemporal());
}

// Test accessors are const
TEST_F(MaxResolutionStatementTest, Accessors_Const) {
    const MaxResolutionStatement maxRes;
    
    // Should compile - accessors are const
    float spatial = maxRes.getSpatial();
    float temporal = maxRes.getTemporal();
    
    EXPECT_FLOAT_EQ(0.0f, spatial);
    EXPECT_FLOAT_EQ(0.0f, temporal);
}

// -----------------------------------------------------------------------------

// Test multiple process calls (should update values)
TEST_F(MaxResolutionStatementTest, Process_MultipleCalls) {
    MaxResolutionStatement maxRes;
    
    std::vector<std::string> tokens1 = {"MaxResolution", "Spatial", "2.0", "Temporal", "1.0"};
    bool result1 = maxRes.process(tokens1);
    EXPECT_TRUE(result1);
    
    EXPECT_FLOAT_EQ(2.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(1.0f, maxRes.getTemporal());
    
    std::vector<std::string> tokens2 = {"MaxResolution", "Spatial", "20.0", "Temporal", "10.0"};
    bool result2 = maxRes.process(tokens2);
    EXPECT_TRUE(result2);
    
    EXPECT_FLOAT_EQ(20.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(10.0f, maxRes.getTemporal());
}

// -----------------------------------------------------------------------------

// Test fixture for StatementType utility function tests
class StatementTypeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test getString with SOURCE
TEST_F(StatementTypeTest, GetString_Source) {
    std::string result = Input::getString(Input::StatementType::SOURCE);
    EXPECT_EQ("Source", result);
}

// Test getString with BBOX
TEST_F(StatementTypeTest, GetString_BBox) {
    std::string result = Input::getString(Input::StatementType::BBOX);
    EXPECT_EQ("BBox", result);
}

// Test getString with VELOCITY
TEST_F(StatementTypeTest, GetString_Velocity) {
    std::string result = Input::getString(Input::StatementType::VELOCITY);
    EXPECT_EQ("Velocity", result);
}

// Test getString with MAXRESOLUTION
TEST_F(StatementTypeTest, GetString_MaxResolution) {
    std::string result = Input::getString(Input::StatementType::MAXRESOLUTION);
    EXPECT_EQ("MaxResolution", result);
}

// Test getString with MAX
TEST_F(StatementTypeTest, GetString_Max) {
    std::string result = Input::getString(Input::StatementType::MAX);
    EXPECT_EQ("Max", result);
}

// -----------------------------------------------------------------------------

// Test getStatementType with "Source" (exact case)
TEST_F(StatementTypeTest, GetStatementType_Source_ExactCase) {
    Input::StatementType result = Input::getStatementType("Source");
    EXPECT_EQ(Input::StatementType::SOURCE, result);
}

// Test getStatementType with "source" (lowercase)
TEST_F(StatementTypeTest, GetStatementType_Source_Lowercase) {
    Input::StatementType result = Input::getStatementType("source");
    EXPECT_EQ(Input::StatementType::SOURCE, result);
}

// Test getStatementType with "SOURCE" (uppercase)
TEST_F(StatementTypeTest, GetStatementType_Source_Uppercase) {
    Input::StatementType result = Input::getStatementType("SOURCE");
    EXPECT_EQ(Input::StatementType::SOURCE, result);
}

// Test getStatementType with "SoUrCe" (mixed case)
TEST_F(StatementTypeTest, GetStatementType_Source_MixedCase) {
    Input::StatementType result = Input::getStatementType("SoUrCe");
    EXPECT_EQ(Input::StatementType::SOURCE, result);
}

// -----------------------------------------------------------------------------

// Test getStatementType with "BBox" (exact case)
TEST_F(StatementTypeTest, GetStatementType_BBox_ExactCase) {
    Input::StatementType result = Input::getStatementType("BBox");
    EXPECT_EQ(Input::StatementType::BBOX, result);
}

// Test getStatementType with "bbox" (lowercase)
TEST_F(StatementTypeTest, GetStatementType_BBox_Lowercase) {
    Input::StatementType result = Input::getStatementType("bbox");
    EXPECT_EQ(Input::StatementType::BBOX, result);
}

// Test getStatementType with "BBOX" (uppercase)
TEST_F(StatementTypeTest, GetStatementType_BBox_Uppercase) {
    Input::StatementType result = Input::getStatementType("BBOX");
    EXPECT_EQ(Input::StatementType::BBOX, result);
}

// Test getStatementType with "BbOx" (mixed case)
TEST_F(StatementTypeTest, GetStatementType_BBox_MixedCase) {
    Input::StatementType result = Input::getStatementType("BbOx");
    EXPECT_EQ(Input::StatementType::BBOX, result);
}

// -----------------------------------------------------------------------------

// Test getStatementType with "Velocity" (exact case)
TEST_F(StatementTypeTest, GetStatementType_Velocity_ExactCase) {
    Input::StatementType result = Input::getStatementType("Velocity");
    EXPECT_EQ(Input::StatementType::VELOCITY, result);
}

// Test getStatementType with "velocity" (lowercase)
TEST_F(StatementTypeTest, GetStatementType_Velocity_Lowercase) {
    Input::StatementType result = Input::getStatementType("velocity");
    EXPECT_EQ(Input::StatementType::VELOCITY, result);
}

// Test getStatementType with "VELOCITY" (uppercase)
TEST_F(StatementTypeTest, GetStatementType_Velocity_Uppercase) {
    Input::StatementType result = Input::getStatementType("VELOCITY");
    EXPECT_EQ(Input::StatementType::VELOCITY, result);
}

// Test getStatementType with "VeLoCiTy" (mixed case)
TEST_F(StatementTypeTest, GetStatementType_Velocity_MixedCase) {
    Input::StatementType result = Input::getStatementType("VeLoCiTy");
    EXPECT_EQ(Input::StatementType::VELOCITY, result);
}

// -----------------------------------------------------------------------------

// Test getStatementType with "MaxResolution" (exact case)
TEST_F(StatementTypeTest, GetStatementType_MaxResolution_ExactCase) {
    Input::StatementType result = Input::getStatementType("MaxResolution");
    EXPECT_EQ(Input::StatementType::MAXRESOLUTION, result);
}

// Test getStatementType with "maxresolution" (lowercase)
TEST_F(StatementTypeTest, GetStatementType_MaxResolution_Lowercase) {
    Input::StatementType result = Input::getStatementType("maxresolution");
    EXPECT_EQ(Input::StatementType::MAXRESOLUTION, result);
}

// Test getStatementType with "MAXRESOLUTION" (uppercase)
TEST_F(StatementTypeTest, GetStatementType_MaxResolution_Uppercase) {
    Input::StatementType result = Input::getStatementType("MAXRESOLUTION");
    EXPECT_EQ(Input::StatementType::MAXRESOLUTION, result);
}

// Test getStatementType with "MaXrEsOlUtIoN" (mixed case)
TEST_F(StatementTypeTest, GetStatementType_MaxResolution_MixedCase) {
    Input::StatementType result = Input::getStatementType("MaXrEsOlUtIoN");
    EXPECT_EQ(Input::StatementType::MAXRESOLUTION, result);
}

// -----------------------------------------------------------------------------

// Test getStatementType with invalid string
TEST_F(StatementTypeTest, GetStatementType_InvalidString) {
    Input::StatementType result = Input::getStatementType("Invalid");
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test getStatementType with empty string
TEST_F(StatementTypeTest, GetStatementType_EmptyString) {
    Input::StatementType result = Input::getStatementType("");
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test getStatementType with "Unknown"
TEST_F(StatementTypeTest, GetStatementType_Unknown) {
    Input::StatementType result = Input::getStatementType("Unknown");
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test getStatementType with "SourceX" (similar but not exact)
TEST_F(StatementTypeTest, GetStatementType_SimilarButNotExact) {
    Input::StatementType result = Input::getStatementType("SourceX");
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// -----------------------------------------------------------------------------

// Test round-trip: getString then getStatementType
TEST_F(StatementTypeTest, RoundTrip_Source) {
    std::string str = Input::getString(Input::StatementType::SOURCE);
    Input::StatementType result = Input::getStatementType(str);
    EXPECT_EQ(Input::StatementType::SOURCE, result);
}

// Test round-trip: getString then getStatementType for BBox
TEST_F(StatementTypeTest, RoundTrip_BBox) {
    std::string str = Input::getString(Input::StatementType::BBOX);
    Input::StatementType result = Input::getStatementType(str);
    EXPECT_EQ(Input::StatementType::BBOX, result);
}

// Test round-trip: getString then getStatementType for Velocity
TEST_F(StatementTypeTest, RoundTrip_Velocity) {
    std::string str = Input::getString(Input::StatementType::VELOCITY);
    Input::StatementType result = Input::getStatementType(str);
    EXPECT_EQ(Input::StatementType::VELOCITY, result);
}

// Test round-trip: getString then getStatementType for MaxResolution
TEST_F(StatementTypeTest, RoundTrip_MaxResolution) {
    std::string str = Input::getString(Input::StatementType::MAXRESOLUTION);
    Input::StatementType result = Input::getStatementType(str);
    EXPECT_EQ(Input::StatementType::MAXRESOLUTION, result);
}

// -----------------------------------------------------------------------------

// Test fixture for InputCompiler tests
class InputCompilerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
    
    Input::InputCompiler compiler;
};

// -----------------------------------------------------------------------------

// Test processLine with valid Source tokens
TEST_F(InputCompilerTest, ProcessLine_ValidSource) {
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "0.5", "X", "1.5", "Y", "2.5"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::SOURCE, result);
    
    // Verify the statement values using getStatement
    const auto& source = compiler.getStatement<Input::StatementType::SOURCE>();
    EXPECT_FLOAT_EQ(1000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.5f, source.getAmplitude());
    EXPECT_FLOAT_EQ(1.5f, source.getX());
    EXPECT_FLOAT_EQ(2.5f, source.getY());
}

// Test processLine with valid BBox tokens
TEST_F(InputCompilerTest, ProcessLine_ValidBBox) {
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::BBOX, result);
    
    // Verify the statement values using getStatement
    const auto& bbox = compiler.getStatement<Input::StatementType::BBOX>();
    EXPECT_FLOAT_EQ(1.0f, bbox.getXMin());
    EXPECT_FLOAT_EQ(2.0f, bbox.getXMax());
    EXPECT_FLOAT_EQ(3.0f, bbox.getYMin());
    EXPECT_FLOAT_EQ(4.0f, bbox.getYMax());
}

// Test processLine with case-insensitive Source
TEST_F(InputCompilerTest, ProcessLine_CaseInsensitiveSource) {
    std::vector<std::string> tokens = {"source", "Frequency", "2000.0", "Amplitude", "0.75", "X", "2.0", "Y", "3.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::SOURCE, result);
    
    const auto& source = compiler.getStatement<Input::StatementType::SOURCE>();
    EXPECT_FLOAT_EQ(2000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.75f, source.getAmplitude());
}

// Test processLine with case-insensitive BBox
TEST_F(InputCompilerTest, ProcessLine_CaseInsensitiveBBox) {
    std::vector<std::string> tokens = {"bbox", "XMin", "10.5", "XMax", "20.5", "YMin", "30.5", "YMax", "40.5"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::BBOX, result);
    
    const auto& bbox = compiler.getStatement<Input::StatementType::BBOX>();
    EXPECT_FLOAT_EQ(10.5f, bbox.getXMin());
    EXPECT_FLOAT_EQ(20.5f, bbox.getXMax());
    EXPECT_FLOAT_EQ(30.5f, bbox.getYMin());
    EXPECT_FLOAT_EQ(40.5f, bbox.getYMax());
}

// -----------------------------------------------------------------------------

// Test processLine with empty tokens
TEST_F(InputCompilerTest, ProcessLine_EmptyTokens) {
    std::vector<std::string> tokens;
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with comment
TEST_F(InputCompilerTest, ProcessLine_Comment) {
    std::vector<std::string> tokens = {"//", "This", "is", "a", "comment"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::COMMENT, result);
}

// Test processLine with comment (full comment syntax)
TEST_F(InputCompilerTest, ProcessLine_FullComment) {
    std::vector<std::string> tokens = {"//", "Comment", "line"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::COMMENT, result);
}

// Test processLine with invalid statement type
TEST_F(InputCompilerTest, ProcessLine_InvalidStatementType) {
    std::vector<std::string> tokens = {"Invalid", "param1", "value1"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with Source but wrong number of tokens
TEST_F(InputCompilerTest, ProcessLine_Source_WrongTokenCount) {
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with BBox but wrong number of tokens
TEST_F(InputCompilerTest, ProcessLine_BBox_WrongTokenCount) {
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "2.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with Source but invalid frequency (zero)
TEST_F(InputCompilerTest, ProcessLine_Source_InvalidFrequency) {
    std::vector<std::string> tokens = {"Source", "Frequency", "0.0", "Amplitude", "0.5", "X", "1.5", "Y", "2.5"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with Source but invalid frequency (negative)
TEST_F(InputCompilerTest, ProcessLine_Source_NegativeFrequency) {
    std::vector<std::string> tokens = {"Source", "Frequency", "-1000.0", "Amplitude", "0.5", "X", "1.5", "Y", "2.5"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with BBox but invalid dimensions (XMin >= XMax)
TEST_F(InputCompilerTest, ProcessLine_BBox_InvalidDimensions) {
    std::vector<std::string> tokens = {"BBox", "XMin", "2.0", "XMax", "1.0", "YMin", "3.0", "YMax", "4.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with BBox but invalid dimensions (YMin >= YMax)
TEST_F(InputCompilerTest, ProcessLine_BBox_InvalidYDimensions) {
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "2.0", "YMin", "4.0", "YMax", "3.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with valid Wall tokens
TEST_F(InputCompilerTest, ProcessLine_ValidWall) {
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::WALL, result);
    
    // Verify the statement values using getStatement
    const auto& wall = compiler.getStatement<Input::StatementType::WALL>();
    EXPECT_FLOAT_EQ(1.0f, wall.getXMin());
    EXPECT_FLOAT_EQ(2.0f, wall.getXMax());
    EXPECT_FLOAT_EQ(3.0f, wall.getYMin());
    EXPECT_FLOAT_EQ(4.0f, wall.getYMax());
}

// -----------------------------------------------------------------------------

// Test processLine with case-insensitive Wall
TEST_F(InputCompilerTest, ProcessLine_CaseInsensitiveWall) {
    std::vector<std::string> tokens = {"wall", "XMin", "10.5", "XMax", "20.5", "YMin", "30.5", "YMax", "40.5"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::WALL, result);
    
    const auto& wall = compiler.getStatement<Input::StatementType::WALL>();
    EXPECT_FLOAT_EQ(10.5f, wall.getXMin());
    EXPECT_FLOAT_EQ(20.5f, wall.getXMax());
    EXPECT_FLOAT_EQ(30.5f, wall.getYMin());
    EXPECT_FLOAT_EQ(40.5f, wall.getYMax());
}

// -----------------------------------------------------------------------------

// Test processLine with Wall but wrong number of tokens
TEST_F(InputCompilerTest, ProcessLine_Wall_WrongTokenCount) {
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "2.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with Wall but invalid dimensions (XMin >= XMax)
TEST_F(InputCompilerTest, ProcessLine_Wall_InvalidDimensions) {
    std::vector<std::string> tokens = {"Wall", "XMin", "2.0", "XMax", "1.0", "YMin", "3.0", "YMax", "4.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with Wall but invalid dimensions (YMin >= YMax)
TEST_F(InputCompilerTest, ProcessLine_Wall_InvalidYDimensions) {
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "2.0", "YMin", "4.0", "YMax", "3.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with valid Velocity tokens
TEST_F(InputCompilerTest, ProcessLine_ValidVelocity) {
    std::vector<std::string> tokens = {"Velocity", "130.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::VELOCITY, result);
    
    // Verify the statement values using getStatement
    const auto& velocity = compiler.getStatement<Input::StatementType::VELOCITY>();
    EXPECT_FLOAT_EQ(130.0f, velocity.getVelocity());
}

// Test processLine with case-insensitive Velocity
TEST_F(InputCompilerTest, ProcessLine_CaseInsensitiveVelocity) {
    std::vector<std::string> tokens = {"velocity", "250.5"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::VELOCITY, result);
    
    const auto& velocity = compiler.getStatement<Input::StatementType::VELOCITY>();
    EXPECT_FLOAT_EQ(250.5f, velocity.getVelocity());
}

// Test processLine with Velocity but wrong number of tokens
TEST_F(InputCompilerTest, ProcessLine_Velocity_WrongTokenCount) {
    std::vector<std::string> tokens = {"Velocity", "130.0", "Extra"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with Velocity but invalid value
TEST_F(InputCompilerTest, ProcessLine_Velocity_InvalidValue) {
    std::vector<std::string> tokens = {"Velocity", "invalid"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with valid MaxResolution tokens
TEST_F(InputCompilerTest, ProcessLine_ValidMaxResolution) {
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "2.0", "Temporal", "1.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAXRESOLUTION, result);
    
    // Verify the statement values using getStatement
    const auto& maxRes = compiler.getStatement<Input::StatementType::MAXRESOLUTION>();
    EXPECT_FLOAT_EQ(2.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(1.0f, maxRes.getTemporal());
}

// Test processLine with case-insensitive MaxResolution
TEST_F(InputCompilerTest, ProcessLine_CaseInsensitiveMaxResolution) {
    std::vector<std::string> tokens = {"maxresolution", "Spatial", "20.5", "Temporal", "10.5"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAXRESOLUTION, result);
    
    const auto& maxRes = compiler.getStatement<Input::StatementType::MAXRESOLUTION>();
    EXPECT_FLOAT_EQ(20.5f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(10.5f, maxRes.getTemporal());
}

// Test processLine with MaxResolution but wrong number of tokens
TEST_F(InputCompilerTest, ProcessLine_MaxResolution_WrongTokenCount) {
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1.0", "Temporal", "2.0", "Extra"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with MaxResolution but invalid value
TEST_F(InputCompilerTest, ProcessLine_MaxResolution_InvalidValue) {
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "invalid", "Temporal", "2.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with MaxResolution but negative Spatial (should fail validation)
TEST_F(InputCompilerTest, ProcessLine_MaxResolution_NegativeSpatial) {
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "-1.0", "Temporal", "2.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with MaxResolution but zero Temporal (should fail validation)
TEST_F(InputCompilerTest, ProcessLine_MaxResolution_ZeroTemporal) {
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1.0", "Temporal", "0.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with MaxResolution but Spatial equal to Temporal (should fail validation)
TEST_F(InputCompilerTest, ProcessLine_MaxResolution_SpatialEqualTemporal) {
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "2.0", "Temporal", "2.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);  // Should fail because Spatial must be greater than Temporal
}

// Test processLine with MaxResolution but Spatial less than Temporal (should fail validation)
TEST_F(InputCompilerTest, ProcessLine_MaxResolution_SpatialLessThanTemporal) {
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1.0", "Temporal", "2.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);  // Should fail because Spatial must be greater than Temporal
}

// Test processLine with MaxResolution but Spatial greater than Temporal (should pass validation)
TEST_F(InputCompilerTest, ProcessLine_MaxResolution_SpatialGreaterThanTemporal) {
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "2.0", "Temporal", "1.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAXRESOLUTION, result);  // Should pass because Spatial is greater than Temporal
    
    const auto& maxRes = compiler.getStatement<Input::StatementType::MAXRESOLUTION>();
    EXPECT_FLOAT_EQ(2.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(1.0f, maxRes.getTemporal());
}

// Test processLine with Source but unknown parameter name (should fail due to else clause)
TEST_F(InputCompilerTest, ProcessLine_Source_UnknownParameterName) {
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Unknown", "0.5", "X", "1.5", "Y", "2.5"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with BBox but unknown parameter name (should fail due to else clause)
TEST_F(InputCompilerTest, ProcessLine_BBox_UnknownParameterName) {
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "Unknown", "4.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with Wall but unknown parameter name (should fail due to else clause)
TEST_F(InputCompilerTest, ProcessLine_Wall_UnknownParameterName) {
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "Unknown", "4.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with MaxResolution but unknown parameter name (should fail due to else clause)
TEST_F(InputCompilerTest, ProcessLine_MaxResolution_UnknownParameterName) {
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1.0", "Unknown", "2.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with Velocity but negative value (should fail validation)
TEST_F(InputCompilerTest, ProcessLine_Velocity_NegativeValue) {
    std::vector<std::string> tokens = {"Velocity", "-130.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with Velocity but zero value (should fail validation)
TEST_F(InputCompilerTest, ProcessLine_Velocity_ZeroValue) {
    std::vector<std::string> tokens = {"Velocity", "0.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// -----------------------------------------------------------------------------

// Test processLine with multiple Source calls (should update values)
TEST_F(InputCompilerTest, ProcessLine_MultipleSourceCalls) {
    std::vector<std::string> tokens1 = {"Source", "Frequency", "1000.0", "Amplitude", "0.5", "X", "1.0", "Y", "2.0"};
    Input::StatementType result1 = compiler.processLine(tokens1);
    EXPECT_EQ(Input::StatementType::SOURCE, result1);
    
    const auto& source1 = compiler.getStatement<Input::StatementType::SOURCE>();
    EXPECT_FLOAT_EQ(1000.0f, source1.getFreq());
    EXPECT_FLOAT_EQ(0.5f, source1.getAmplitude());
    EXPECT_FLOAT_EQ(1.0f, source1.getX());
    EXPECT_FLOAT_EQ(2.0f, source1.getY());
    
    std::vector<std::string> tokens2 = {"Source", "Frequency", "2000.0", "Amplitude", "1.0", "X", "3.0", "Y", "4.0"};
    Input::StatementType result2 = compiler.processLine(tokens2);
    EXPECT_EQ(Input::StatementType::SOURCE, result2);
    
    const auto& source2 = compiler.getStatement<Input::StatementType::SOURCE>();
    EXPECT_FLOAT_EQ(2000.0f, source2.getFreq());
    EXPECT_FLOAT_EQ(1.0f, source2.getAmplitude());
    EXPECT_FLOAT_EQ(3.0f, source2.getX());
    EXPECT_FLOAT_EQ(4.0f, source2.getY());
}

// Test processLine with multiple BBox calls (should update values)
TEST_F(InputCompilerTest, ProcessLine_MultipleBBoxCalls) {
    std::vector<std::string> tokens1 = {"BBox", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    Input::StatementType result1 = compiler.processLine(tokens1);
    EXPECT_EQ(Input::StatementType::BBOX, result1);
    
    const auto& bbox1 = compiler.getStatement<Input::StatementType::BBOX>();
    EXPECT_FLOAT_EQ(1.0f, bbox1.getXMin());
    EXPECT_FLOAT_EQ(2.0f, bbox1.getXMax());
    
    std::vector<std::string> tokens2 = {"BBox", "XMin", "10.0", "XMax", "20.0", "YMin", "30.0", "YMax", "40.0"};
    Input::StatementType result2 = compiler.processLine(tokens2);
    EXPECT_EQ(Input::StatementType::BBOX, result2);
    
    const auto& bbox2 = compiler.getStatement<Input::StatementType::BBOX>();
    EXPECT_FLOAT_EQ(10.0f, bbox2.getXMin());
    EXPECT_FLOAT_EQ(20.0f, bbox2.getXMax());
    EXPECT_FLOAT_EQ(30.0f, bbox2.getYMin());
    EXPECT_FLOAT_EQ(40.0f, bbox2.getYMax());
}

// Test processLine with multiple Wall calls (should update values)
TEST_F(InputCompilerTest, ProcessLine_MultipleWallCalls) {
    std::vector<std::string> tokens1 = {"Wall", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    Input::StatementType result1 = compiler.processLine(tokens1);
    EXPECT_EQ(Input::StatementType::WALL, result1);
    
    const auto& wall1 = compiler.getStatement<Input::StatementType::WALL>();
    EXPECT_FLOAT_EQ(1.0f, wall1.getXMin());
    EXPECT_FLOAT_EQ(2.0f, wall1.getXMax());
    
    std::vector<std::string> tokens2 = {"Wall", "XMin", "10.0", "XMax", "20.0", "YMin", "30.0", "YMax", "40.0"};
    Input::StatementType result2 = compiler.processLine(tokens2);
    EXPECT_EQ(Input::StatementType::WALL, result2);
    
    const auto& wall2 = compiler.getStatement<Input::StatementType::WALL>();
    EXPECT_FLOAT_EQ(10.0f, wall2.getXMin());
    EXPECT_FLOAT_EQ(20.0f, wall2.getXMax());
    EXPECT_FLOAT_EQ(30.0f, wall2.getYMin());
    EXPECT_FLOAT_EQ(40.0f, wall2.getYMax());
}

// Test processLine with multiple Velocity calls (should update values)
TEST_F(InputCompilerTest, ProcessLine_MultipleVelocityCalls) {
    std::vector<std::string> tokens1 = {"Velocity", "130.0"};
    Input::StatementType result1 = compiler.processLine(tokens1);
    EXPECT_EQ(Input::StatementType::VELOCITY, result1);
    
    const auto& velocity1 = compiler.getStatement<Input::StatementType::VELOCITY>();
    EXPECT_FLOAT_EQ(130.0f, velocity1.getVelocity());
    
    std::vector<std::string> tokens2 = {"Velocity", "200.0"};
    Input::StatementType result2 = compiler.processLine(tokens2);
    EXPECT_EQ(Input::StatementType::VELOCITY, result2);
    
    const auto& velocity2 = compiler.getStatement<Input::StatementType::VELOCITY>();
    EXPECT_FLOAT_EQ(200.0f, velocity2.getVelocity());
}

// Test processLine with multiple MaxResolution calls (should update values)
TEST_F(InputCompilerTest, ProcessLine_MultipleMaxResolutionCalls) {
    std::vector<std::string> tokens1 = {"MaxResolution", "Spatial", "2.0", "Temporal", "1.0"};
    Input::StatementType result1 = compiler.processLine(tokens1);
    EXPECT_EQ(Input::StatementType::MAXRESOLUTION, result1);
    
    const auto& maxRes1 = compiler.getStatement<Input::StatementType::MAXRESOLUTION>();
    EXPECT_FLOAT_EQ(2.0f, maxRes1.getSpatial());
    EXPECT_FLOAT_EQ(1.0f, maxRes1.getTemporal());
    
    std::vector<std::string> tokens2 = {"MaxResolution", "Spatial", "20.0", "Temporal", "10.0"};
    Input::StatementType result2 = compiler.processLine(tokens2);
    EXPECT_EQ(Input::StatementType::MAXRESOLUTION, result2);
    
    const auto& maxRes2 = compiler.getStatement<Input::StatementType::MAXRESOLUTION>();
    EXPECT_FLOAT_EQ(20.0f, maxRes2.getSpatial());
    EXPECT_FLOAT_EQ(10.0f, maxRes2.getTemporal());
}

// Test processLine with different parameter order for Source
TEST_F(InputCompilerTest, ProcessLine_Source_DifferentParameterOrder) {
    std::vector<std::string> tokens = {"Source", "Amplitude", "0.75", "Frequency", "1500.0", "Y", "4.5", "X", "3.5"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::SOURCE, result);
    
    const auto& source = compiler.getStatement<Input::StatementType::SOURCE>();
    EXPECT_FLOAT_EQ(1500.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.75f, source.getAmplitude());
    EXPECT_FLOAT_EQ(3.5f, source.getX());
    EXPECT_FLOAT_EQ(4.5f, source.getY());
}

// Test processLine with different parameter order for BBox
TEST_F(InputCompilerTest, ProcessLine_BBox_DifferentParameterOrder) {
    std::vector<std::string> tokens = {"BBox", "YMax", "40.0", "XMin", "10.0", "YMin", "30.0", "XMax", "20.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::BBOX, result);
    
    const auto& bbox = compiler.getStatement<Input::StatementType::BBOX>();
    EXPECT_FLOAT_EQ(10.0f, bbox.getXMin());
    EXPECT_FLOAT_EQ(20.0f, bbox.getXMax());
    EXPECT_FLOAT_EQ(30.0f, bbox.getYMin());
    EXPECT_FLOAT_EQ(40.0f, bbox.getYMax());
}

// Test processLine with different parameter order for Wall
TEST_F(InputCompilerTest, ProcessLine_Wall_DifferentParameterOrder) {
    std::vector<std::string> tokens = {"Wall", "YMax", "40.0", "XMin", "10.0", "YMin", "30.0", "XMax", "20.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::WALL, result);
    
    const auto& wall = compiler.getStatement<Input::StatementType::WALL>();
    EXPECT_FLOAT_EQ(10.0f, wall.getXMin());
    EXPECT_FLOAT_EQ(20.0f, wall.getXMax());
    EXPECT_FLOAT_EQ(30.0f, wall.getYMin());
    EXPECT_FLOAT_EQ(40.0f, wall.getYMax());
}

// Test processLine with different parameter order for MaxResolution
TEST_F(InputCompilerTest, ProcessLine_MaxResolution_DifferentParameterOrder) {
    std::vector<std::string> tokens = {"MaxResolution", "Temporal", "10.0", "Spatial", "20.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAXRESOLUTION, result);
    
    const auto& maxRes = compiler.getStatement<Input::StatementType::MAXRESOLUTION>();
    EXPECT_FLOAT_EQ(20.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(10.0f, maxRes.getTemporal());
}

// -----------------------------------------------------------------------------

// Test processLine with case-insensitive parameter names for Source
TEST_F(InputCompilerTest, ProcessLine_Source_CaseInsensitiveParameters) {
    std::vector<std::string> tokens = {"Source", "frequency", "2500.0", "amplitude", "0.9", "x", "5.5", "y", "6.5"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::SOURCE, result);
    
    const auto& source = compiler.getStatement<Input::StatementType::SOURCE>();
    EXPECT_FLOAT_EQ(2500.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.9f, source.getAmplitude());
    EXPECT_FLOAT_EQ(5.5f, source.getX());
    EXPECT_FLOAT_EQ(6.5f, source.getY());
}

// Test processLine with case-insensitive parameter names for BBox
TEST_F(InputCompilerTest, ProcessLine_BBox_CaseInsensitiveParameters) {
    std::vector<std::string> tokens = {"BBox", "xmin", "5.0", "xmax", "15.0", "ymin", "25.0", "ymax", "35.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::BBOX, result);
    
    const auto& bbox = compiler.getStatement<Input::StatementType::BBOX>();
    EXPECT_FLOAT_EQ(5.0f, bbox.getXMin());
    EXPECT_FLOAT_EQ(15.0f, bbox.getXMax());
    EXPECT_FLOAT_EQ(25.0f, bbox.getYMin());
    EXPECT_FLOAT_EQ(35.0f, bbox.getYMax());
}

// Test processLine with Wall but case-insensitive parameters
TEST_F(InputCompilerTest, ProcessLine_Wall_CaseInsensitiveParameters) {
    std::vector<std::string> tokens = {"Wall", "xmin", "5.0", "xmax", "15.0", "ymin", "25.0", "ymax", "35.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::WALL, result);
    
    const auto& wall = compiler.getStatement<Input::StatementType::WALL>();
    EXPECT_FLOAT_EQ(5.0f, wall.getXMin());
    EXPECT_FLOAT_EQ(15.0f, wall.getXMax());
    EXPECT_FLOAT_EQ(25.0f, wall.getYMin());
    EXPECT_FLOAT_EQ(35.0f, wall.getYMax());
}

// Test processLine with case-insensitive parameter names for MaxResolution
TEST_F(InputCompilerTest, ProcessLine_MaxResolution_CaseInsensitiveParameters) {
    std::vector<std::string> tokens = {"MaxResolution", "spatial", "15.0", "temporal", "5.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAXRESOLUTION, result);
    
    const auto& maxRes = compiler.getStatement<Input::StatementType::MAXRESOLUTION>();
    EXPECT_FLOAT_EQ(15.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(5.0f, maxRes.getTemporal());
}


class FileParserTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};



// -----------------------------------------------------------------------------

TEST_F(FileParserTest, ValidLinesProduceStatements) {
    std::istringstream iss(
        "Source Frequency 100 Amplitude 50 X 1.5 Y 2.5\n"
        "BBox XMin 0 XMax 10 YMin 0 YMax 20\n"
    );

    FileParser parser(iss);
    std::vector<StatementType> types;

    for (FileParser::const_iterator iter(parser); iter.isValid(); ++iter) {
        types.push_back(iter.getType());
        EXPECT_NE(StatementType::MAX, iter.getType());
        if (iter.getType() == StatementType::SOURCE) {
            const auto& source = iter.getStatement<StatementType::SOURCE>();
            EXPECT_TRUE(source.isValid());
            EXPECT_FLOAT_EQ(100.0f, source.getFreq());
            EXPECT_FLOAT_EQ(50.0f, source.getAmplitude());
            EXPECT_FLOAT_EQ(1.5f, source.getX());
            EXPECT_FLOAT_EQ(2.5f, source.getY());
        } else if (iter.getType() == StatementType::BBOX) {
            const auto& bbox = iter.getStatement<StatementType::BBOX>();
            EXPECT_TRUE(bbox.isValid());
            EXPECT_FLOAT_EQ(0.0f, bbox.getXMin());
            EXPECT_FLOAT_EQ(10.0f, bbox.getXMax());
            EXPECT_FLOAT_EQ(0.0f, bbox.getYMin());
            EXPECT_FLOAT_EQ(20.0f, bbox.getYMax());
        }
    }

    ASSERT_EQ(2u, types.size());
    EXPECT_EQ(StatementType::SOURCE, types[0]);
    EXPECT_EQ(StatementType::BBOX, types[1]);
}

// -----------------------------------------------------------------------------

TEST_F(FileParserTest, MixedLinesYieldMaxAndValid) {
    std::istringstream iss(
        "Invalid line\n"
        "// Comment line\n"
        "\n"
        "Source Frequency 10 Amplitude 5 X 1.0 Y 2.0\n"
    );

    FileParser parser(iss);
    std::vector<StatementType> types;
    std::vector<size_t> lineNumbers;

    for (FileParser::const_iterator iter(parser); iter.isValid(); ++iter) {
        types.push_back(iter.getType());
        lineNumbers.push_back(iter.getLineNumber());
    }

    // Empty line (line 3) is skipped, so we only get 3 items
    ASSERT_EQ(3u, types.size());
    EXPECT_EQ(StatementType::MAX, types[0]);      // Line 1: Invalid line
    EXPECT_EQ(StatementType::COMMENT, types[1]);  // Line 2: Comment
    EXPECT_EQ(StatementType::SOURCE, types[2]);   // Line 4: Source (line 3 empty, skipped)

    ASSERT_EQ(3u, lineNumbers.size());
    EXPECT_EQ(1u, lineNumbers[0]);  // Line 1
    EXPECT_EQ(2u, lineNumbers[1]);  // Line 2
    EXPECT_EQ(4u, lineNumbers[2]);  // Line 4 (line 3 was skipped but counted in line number)
}

// -----------------------------------------------------------------------------

// Test fixture for SourceStatement save tests
class SourceStatementSaveTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test save with valid Source values
TEST_F(SourceStatementSaveTest, Save_ValidValues) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "0.5", "X", "1.5", "Y", "2.5"};
    source.process(tokens);
    
    std::ostringstream oss;
    source.save(oss);
    
    EXPECT_EQ("Source Frequency 1000 Amplitude 0.5 X 1.5 Y 2.5", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with decimal values
TEST_F(SourceStatementSaveTest, Save_DecimalValues) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1234.567", "Amplitude", "0.12345", "X", "12.345", "Y", "23.456"};
    source.process(tokens);
    
    std::ostringstream oss;
    source.save(oss);
    
    EXPECT_EQ("Source Frequency 1234.57 Amplitude 0.12345 X 12.345 Y 23.456", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with large values
TEST_F(SourceStatementSaveTest, Save_LargeValues) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000000.0", "Amplitude", "1000.0", "X", "5000.0", "Y", "6000.0"};
    source.process(tokens);
    
    std::ostringstream oss;
    source.save(oss);
    
    EXPECT_EQ("Source Frequency 1e+06 Amplitude 1000 X 5000 Y 6000", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with default values (before processing)
TEST_F(SourceStatementSaveTest, Save_DefaultValues) {
    SourceStatement source;
    
    std::ostringstream oss;
    source.save(oss);
    
    EXPECT_EQ("Source Frequency 0 Amplitude 0 X 0 Y 0", oss.str());
}

// -----------------------------------------------------------------------------

// Test fixture for BBoxStatement save tests
class BBoxStatementSaveTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test save with valid BBox values
TEST_F(BBoxStatementSaveTest, Save_ValidValues) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    bbox.process(tokens);
    
    std::ostringstream oss;
    bbox.save(oss);
    
    EXPECT_EQ("BBox XMin 1 XMax 2 YMin 3 YMax 4", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with decimal values
TEST_F(BBoxStatementSaveTest, Save_DecimalValues) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1.5", "XMax", "2.75", "YMin", "3.14159", "YMax", "4.99"};
    bbox.process(tokens);
    
    std::ostringstream oss;
    bbox.save(oss);
    
    EXPECT_EQ("BBox XMin 1.5 XMax 2.75 YMin 3.14159 YMax 4.99", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with negative values
TEST_F(BBoxStatementSaveTest, Save_NegativeValues) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "-2.5", "XMax", "-1.5", "YMin", "-4.5", "YMax", "-3.5"};
    bbox.process(tokens);
    
    std::ostringstream oss;
    bbox.save(oss);
    
    EXPECT_EQ("BBox XMin -2.5 XMax -1.5 YMin -4.5 YMax -3.5", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with large values
TEST_F(BBoxStatementSaveTest, Save_LargeValues) {
    BBoxStatement bbox;
    std::vector<std::string> tokens = {"BBox", "XMin", "1000.0", "XMax", "2000.0", "YMin", "3000.0", "YMax", "4000.0"};
    bbox.process(tokens);
    
    std::ostringstream oss;
    bbox.save(oss);
    
    EXPECT_EQ("BBox XMin 1000 XMax 2000 YMin 3000 YMax 4000", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with default values (before processing)
TEST_F(BBoxStatementSaveTest, Save_DefaultValues) {
    BBoxStatement bbox;
    
    std::ostringstream oss;
    bbox.save(oss);
    
    EXPECT_EQ("BBox XMin 0 XMax 0 YMin 0 YMax 0", oss.str());
}

// -----------------------------------------------------------------------------

// Test fixture for WallStatement save tests
class WallStatementSaveTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test save with valid Wall values
TEST_F(WallStatementSaveTest, Save_ValidValues) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    wall.process(tokens);
    
    std::ostringstream oss;
    wall.save(oss);
    
    EXPECT_EQ("Wall XMin 1 XMax 2 YMin 3 YMax 4", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with decimal values
TEST_F(WallStatementSaveTest, Save_DecimalValues) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1.5", "XMax", "2.75", "YMin", "3.14159", "YMax", "4.99"};
    wall.process(tokens);
    
    std::ostringstream oss;
    wall.save(oss);
    
    EXPECT_EQ("Wall XMin 1.5 XMax 2.75 YMin 3.14159 YMax 4.99", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with negative values
TEST_F(WallStatementSaveTest, Save_NegativeValues) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "-2.5", "XMax", "-1.5", "YMin", "-4.5", "YMax", "-3.5"};
    wall.process(tokens);
    
    std::ostringstream oss;
    wall.save(oss);
    
    EXPECT_EQ("Wall XMin -2.5 XMax -1.5 YMin -4.5 YMax -3.5", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with large values
TEST_F(WallStatementSaveTest, Save_LargeValues) {
    WallStatement wall;
    std::vector<std::string> tokens = {"Wall", "XMin", "1000.0", "XMax", "2000.0", "YMin", "3000.0", "YMax", "4000.0"};
    wall.process(tokens);
    
    std::ostringstream oss;
    wall.save(oss);
    
    EXPECT_EQ("Wall XMin 1000 XMax 2000 YMin 3000 YMax 4000", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with default values (before processing)
TEST_F(WallStatementSaveTest, Save_DefaultValues) {
    WallStatement wall;
    
    std::ostringstream oss;
    wall.save(oss);
    
    EXPECT_EQ("Wall XMin 0 XMax 0 YMin 0 YMax 0", oss.str());
}

// -----------------------------------------------------------------------------

// Test fixture for VelocityStatement save tests
class VelocityStatementSaveTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test save with valid Velocity value
TEST_F(VelocityStatementSaveTest, Save_ValidValue) {
    VelocityStatement velocity;
    std::vector<std::string> tokens = {"Velocity", "130.0"};
    velocity.process(tokens);
    
    std::ostringstream oss;
    velocity.save(oss);
    
    EXPECT_EQ("Velocity 130", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with decimal value
TEST_F(VelocityStatementSaveTest, Save_DecimalValue) {
    VelocityStatement velocity;
    std::vector<std::string> tokens = {"Velocity", "123.456"};
    velocity.process(tokens);
    
    std::ostringstream oss;
    velocity.save(oss);
    
    EXPECT_EQ("Velocity 123.456", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with large value
TEST_F(VelocityStatementSaveTest, Save_LargeValue) {
    VelocityStatement velocity;
    std::vector<std::string> tokens = {"Velocity", "1000000.0"};
    velocity.process(tokens);
    
    std::ostringstream oss;
    velocity.save(oss);
    
    EXPECT_EQ("Velocity 1e+06", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with default value (before processing)
TEST_F(VelocityStatementSaveTest, Save_DefaultValue) {
    VelocityStatement velocity;
    
    std::ostringstream oss;
    velocity.save(oss);
    
    EXPECT_EQ("Velocity 0", oss.str());
}

// -----------------------------------------------------------------------------

// Test fixture for MaxResolutionStatement save tests
class MaxResolutionStatementSaveTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test save with valid MaxResolution values
TEST_F(MaxResolutionStatementSaveTest, Save_ValidValues) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1.0", "Temporal", "2.0"};
    maxRes.process(tokens);
    
    std::ostringstream oss;
    maxRes.save(oss);
    
    EXPECT_EQ("MaxResolution Spatial 1 Temporal 2", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with decimal values
TEST_F(MaxResolutionStatementSaveTest, Save_DecimalValues) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1.5", "Temporal", "2.75"};
    maxRes.process(tokens);
    
    std::ostringstream oss;
    maxRes.save(oss);
    
    EXPECT_EQ("MaxResolution Spatial 1.5 Temporal 2.75", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with small decimal values
TEST_F(MaxResolutionStatementSaveTest, Save_SmallDecimalValues) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "0.1", "Temporal", "0.001"};
    maxRes.process(tokens);
    
    std::ostringstream oss;
    maxRes.save(oss);
    
    EXPECT_EQ("MaxResolution Spatial 0.1 Temporal 0.001", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with large values
TEST_F(MaxResolutionStatementSaveTest, Save_LargeValues) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1000.0", "Temporal", "2000.0"};
    maxRes.process(tokens);
    
    std::ostringstream oss;
    maxRes.save(oss);
    
    EXPECT_EQ("MaxResolution Spatial 1000 Temporal 2000", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with default values (before processing)
TEST_F(MaxResolutionStatementSaveTest, Save_DefaultValues) {
    MaxResolutionStatement maxRes;
    
    std::ostringstream oss;
    maxRes.save(oss);
    
    EXPECT_EQ("MaxResolution Spatial 0 Temporal 0", oss.str());
}

// -----------------------------------------------------------------------------

// Test round-trip: process then save, then parse again
TEST_F(SourceStatementSaveTest, RoundTrip_ProcessSaveParse) {
    SourceStatement source1;
    std::vector<std::string> tokens1 = {"Source", "Frequency", "1000.0", "Amplitude", "0.5"};
    source1.process(tokens1);
    
    std::ostringstream oss;
    source1.save(oss);
    std::string saved = oss.str();
    
    // Parse the saved string (simplified - in real scenario would use tokenizer)
    // This test verifies the format is correct
    EXPECT_TRUE(saved.find("Source") != std::string::npos);
    EXPECT_TRUE(saved.find("Frequency") != std::string::npos);
    EXPECT_TRUE(saved.find("Amplitude") != std::string::npos);
}

// -----------------------------------------------------------------------------

// Test round-trip: process then save, then parse again for BBox
TEST_F(BBoxStatementSaveTest, RoundTrip_ProcessSaveParse) {
    BBoxStatement bbox1;
    std::vector<std::string> tokens1 = {"BBox", "XMin", "1.0", "XMax", "2.0", "YMin", "3.0", "YMax", "4.0"};
    bbox1.process(tokens1);
    
    std::ostringstream oss;
    bbox1.save(oss);
    std::string saved = oss.str();
    
    // Parse the saved string (simplified - in real scenario would use tokenizer)
    // This test verifies the format is correct
    EXPECT_TRUE(saved.find("BBox") != std::string::npos);
    EXPECT_TRUE(saved.find("XMin") != std::string::npos);
    EXPECT_TRUE(saved.find("XMax") != std::string::npos);
    EXPECT_TRUE(saved.find("YMin") != std::string::npos);
    EXPECT_TRUE(saved.find("YMax") != std::string::npos);
}

// -----------------------------------------------------------------------------

// Test round-trip: process then save, then parse again for Velocity
TEST_F(VelocityStatementSaveTest, RoundTrip_ProcessSaveParse) {
    VelocityStatement velocity1;
    std::vector<std::string> tokens1 = {"Velocity", "130.0"};
    velocity1.process(tokens1);
    
    std::ostringstream oss;
    velocity1.save(oss);
    std::string saved = oss.str();
    
    // Parse the saved string (simplified - in real scenario would use tokenizer)
    // This test verifies the format is correct
    EXPECT_TRUE(saved.find("Velocity") != std::string::npos);
}

// -----------------------------------------------------------------------------

// Test round-trip: process then save, then parse again for MaxResolution
TEST_F(MaxResolutionStatementSaveTest, RoundTrip_ProcessSaveParse) {
    MaxResolutionStatement maxRes1;
    std::vector<std::string> tokens1 = {"MaxResolution", "Spatial", "1.0", "Temporal", "2.0"};
    maxRes1.process(tokens1);
    
    std::ostringstream oss;
    maxRes1.save(oss);
    std::string saved = oss.str();
    
    // Parse the saved string (simplified - in real scenario would use tokenizer)
    // This test verifies the format is correct
    EXPECT_TRUE(saved.find("MaxResolution") != std::string::npos);
    EXPECT_TRUE(saved.find("Spatial") != std::string::npos);
    EXPECT_TRUE(saved.find("Temporal") != std::string::npos);
}

// -----------------------------------------------------------------------------

// Test fixture for SimulationParamStatement tests
class SimulationParamStatementTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test process with valid SimulationParam tokens (3 tokens total)
TEST_F(SimulationParamStatementTest, Process_ValidSimulationParam) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(1000u, simParam.getMaxIteration());
}

// -----------------------------------------------------------------------------

// Test process with case-insensitive SimulationParam (lowercase)
TEST_F(SimulationParamStatementTest, Process_CaseInsensitiveSimulationParam) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"simulationparam", "MaxIteration", "1000"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(1000u, simParam.getMaxIteration());
}

// Test process with case-insensitive SimulationParam (mixed case)
TEST_F(SimulationParamStatementTest, Process_CaseInsensitiveSimulationParamMixed) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SiMuLaTiOnPaRaM", "MaxIteration", "1000"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(1000u, simParam.getMaxIteration());
}

// -----------------------------------------------------------------------------

// Test process with case-insensitive parameter name
TEST_F(SimulationParamStatementTest, Process_CaseInsensitiveParameter) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "maxiteration", "1000"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(1000u, simParam.getMaxIteration());
}

// Test process with mixed case parameter name
TEST_F(SimulationParamStatementTest, Process_MixedCaseParameter) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaXiTeRaTiOn", "1000"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(1000u, simParam.getMaxIteration());
}

// -----------------------------------------------------------------------------

// Test process with invalid first token (not SimulationParam)
TEST_F(SimulationParamStatementTest, Process_InvalidFirstToken) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"Source", "MaxIteration", "1000"};
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with empty tokens
TEST_F(SimulationParamStatementTest, Process_EmptyTokens) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens;
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with wrong number of tokens (less than 3)
TEST_F(SimulationParamStatementTest, Process_TooFewTokens) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration"};
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with wrong number of tokens (more than 3)
TEST_F(SimulationParamStatementTest, Process_TooManyTokens) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "Extra"};
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with exactly 2 tokens
TEST_F(SimulationParamStatementTest, Process_Exactly2Tokens) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration"};
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with zero MaxIteration (should fail validation)
TEST_F(SimulationParamStatementTest, Process_ZeroMaxIteration) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "0"};
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);  // Should fail because MaxIteration must be positive
    
    // Value may still be set, but process should return false
    EXPECT_EQ(0u, simParam.getMaxIteration());
}

// Test process with positive MaxIteration
TEST_F(SimulationParamStatementTest, Process_PositiveMaxIteration) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(1u, simParam.getMaxIteration());
}

// -----------------------------------------------------------------------------

// Test process with large MaxIteration value
TEST_F(SimulationParamStatementTest, Process_LargeMaxIteration) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000000"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(1000000u, simParam.getMaxIteration());
}

// Test process with very large MaxIteration value
TEST_F(SimulationParamStatementTest, Process_VeryLargeMaxIteration) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "999999999"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(999999999u, simParam.getMaxIteration());
}

// -----------------------------------------------------------------------------

// Test process with invalid MaxIteration value (non-numeric)
TEST_F(SimulationParamStatementTest, Process_InvalidMaxIterationValue) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "invalid"};
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with invalid MaxIteration value (negative as string)
TEST_F(SimulationParamStatementTest, Process_NegativeMaxIterationValue) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "-100"};
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with invalid MaxIteration value (decimal)
TEST_F(SimulationParamStatementTest, Process_DecimalMaxIterationValue) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "100.5"};
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with wrong parameter name
TEST_F(SimulationParamStatementTest, Process_WrongParameterName) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "WrongParam", "1000"};
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test accessor methods - default values
TEST_F(SimulationParamStatementTest, Accessors_DefaultValues) {
    SimulationParamStatement simParam;
    
    EXPECT_EQ(0u, simParam.getMaxIteration());
}

// Test accessor methods - after processing
TEST_F(SimulationParamStatementTest, Accessors_AfterProcessing) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "5000"};
    simParam.process(tokens);
    
    EXPECT_EQ(5000u, simParam.getMaxIteration());
}

// Test accessor methods - const
TEST_F(SimulationParamStatementTest, Accessors_Const) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "2000"};
    simParam.process(tokens);
    
    const SimulationParamStatement& constSimParam = simParam;
    EXPECT_EQ(2000u, constSimParam.getMaxIteration());
}

// -----------------------------------------------------------------------------

// Test process with multiple calls (should overwrite previous value)
TEST_F(SimulationParamStatementTest, Process_MultipleCalls) {
    SimulationParamStatement simParam;
    
    std::vector<std::string> tokens1 = {"SimulationParam", "MaxIteration", "1000"};
    bool result1 = simParam.process(tokens1);
    EXPECT_TRUE(result1);
    EXPECT_EQ(1000u, simParam.getMaxIteration());
    
    std::vector<std::string> tokens2 = {"SimulationParam", "MaxIteration", "2000"};
    bool result2 = simParam.process(tokens2);
    EXPECT_TRUE(result2);
    EXPECT_EQ(2000u, simParam.getMaxIteration());
}

// -----------------------------------------------------------------------------

// Test isValid with zero value
TEST_F(SimulationParamStatementTest, IsValid_ZeroValue) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "0"};
    simParam.process(tokens);
    
    bool result = simParam.isValid();
    EXPECT_FALSE(result);
}

// Test isValid with positive value
TEST_F(SimulationParamStatementTest, IsValid_PositiveValue) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000"};
    simParam.process(tokens);
    
    bool result = simParam.isValid();
    EXPECT_TRUE(result);
}

// Test isValid with default value
TEST_F(SimulationParamStatementTest, IsValid_DefaultValue) {
    SimulationParamStatement simParam;
    
    bool result = simParam.isValid();
    EXPECT_FALSE(result);  // Default value 0 is invalid
}

// -----------------------------------------------------------------------------

// Test fixture for SimulationParamStatement save tests
class SimulationParamStatementSaveTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test save with valid MaxIteration value
TEST_F(SimulationParamStatementSaveTest, Save_ValidValue) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000"};
    simParam.process(tokens);
    
    std::ostringstream oss;
    simParam.save(oss);
    
    EXPECT_EQ("SimulationParam MaxIteration 1000 BatchSize 100 ProcessingType CPU", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with large value
TEST_F(SimulationParamStatementSaveTest, Save_LargeValue) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000000"};
    simParam.process(tokens);
    
    std::ostringstream oss;
    simParam.save(oss);
    
    EXPECT_EQ("SimulationParam MaxIteration 1000000 BatchSize 100 ProcessingType CPU", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with default value (before processing)
TEST_F(SimulationParamStatementSaveTest, Save_DefaultValue) {
    SimulationParamStatement simParam;
    
    std::ostringstream oss;
    simParam.save(oss);
    
    EXPECT_EQ("SimulationParam MaxIteration 0 BatchSize 100 ProcessingType CPU", oss.str());
}

// -----------------------------------------------------------------------------

// Test save and reload pattern
TEST_F(SimulationParamStatementSaveTest, Save_ReloadPattern) {
    SimulationParamStatement simParam1;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "5000"};
    simParam1.process(tokens);
    
    std::ostringstream oss;
    simParam1.save(oss);
    std::string saved = oss.str();
    
    // Parse the saved string (simplified - in real scenario would use tokenizer)
    // This test verifies the format is correct
    EXPECT_TRUE(saved.find("SimulationParam") != std::string::npos);
    EXPECT_TRUE(saved.find("MaxIteration") != std::string::npos);
    EXPECT_TRUE(saved.find("5000") != std::string::npos);
}

// -----------------------------------------------------------------------------

// Test process with BatchSize parameter (5 tokens total)
TEST_F(SimulationParamStatementTest, Process_WithBatchSize) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "BatchSize", "200"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(1000u, simParam.getMaxIteration());
    EXPECT_EQ(200u, simParam.getBatchSize());
}

// -----------------------------------------------------------------------------

// Test process with case-insensitive BatchSize token
TEST_F(SimulationParamStatementTest, Process_CaseInsensitiveBatchSize) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "batchsize", "150"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(1000u, simParam.getMaxIteration());
    EXPECT_EQ(150u, simParam.getBatchSize());
}

// -----------------------------------------------------------------------------

// Test process with mixed case BatchSize token
TEST_F(SimulationParamStatementTest, Process_MixedCaseBatchSize) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "BaTcHsIzE", "250"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(1000u, simParam.getMaxIteration());
    EXPECT_EQ(250u, simParam.getBatchSize());
}

// -----------------------------------------------------------------------------

// Test accessor for BatchSize default value
TEST_F(SimulationParamStatementTest, Accessors_BatchSizeDefaultValue) {
    SimulationParamStatement simParam;
    
    EXPECT_EQ(100u, simParam.getBatchSize());
}

// -----------------------------------------------------------------------------

// Test accessor for BatchSize after processing
TEST_F(SimulationParamStatementTest, Accessors_BatchSizeAfterProcessing) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "5000", "BatchSize", "300"};
    simParam.process(tokens);
    
    EXPECT_EQ(5000u, simParam.getMaxIteration());
    EXPECT_EQ(300u, simParam.getBatchSize());
}

// -----------------------------------------------------------------------------

// Test accessor for BatchSize - const
TEST_F(SimulationParamStatementTest, Accessors_BatchSizeConst) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "2000", "BatchSize", "400"};
    simParam.process(tokens);
    
    const SimulationParamStatement& constSimParam = simParam;
    EXPECT_EQ(2000u, constSimParam.getMaxIteration());
    EXPECT_EQ(400u, constSimParam.getBatchSize());
}

// -----------------------------------------------------------------------------

// Test isValid with BatchSize = 0
TEST_F(SimulationParamStatementTest, IsValid_BatchSizeZero) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "BatchSize", "0"};
    simParam.process(tokens);
    
    bool result = simParam.isValid();
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test isValid with BatchSize positive
TEST_F(SimulationParamStatementTest, IsValid_BatchSizePositive) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "BatchSize", "200"};
    simParam.process(tokens);
    
    bool result = simParam.isValid();
    EXPECT_TRUE(result);
}

// -----------------------------------------------------------------------------

// Test isValid with default BatchSize (should be valid since default is 100)
TEST_F(SimulationParamStatementTest, IsValid_DefaultBatchSize) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000"};
    simParam.process(tokens);
    
    bool result = simParam.isValid();
    EXPECT_TRUE(result);
    EXPECT_EQ(100u, simParam.getBatchSize());
}

// -----------------------------------------------------------------------------

// Test process with invalid BatchSize value
TEST_F(SimulationParamStatementTest, Process_InvalidBatchSizeValue) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "BatchSize", "invalid"};
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with negative BatchSize value
TEST_F(SimulationParamStatementTest, Process_NegativeBatchSizeValue) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "BatchSize", "-50"};
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with decimal BatchSize value
TEST_F(SimulationParamStatementTest, Process_DecimalBatchSizeValue) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "BatchSize", "100.5"};
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with wrong parameter name for BatchSize
TEST_F(SimulationParamStatementTest, Process_WrongBatchSizeParameterName) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "WrongParam", "200"};
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test save with BatchSize included
TEST_F(SimulationParamStatementSaveTest, Save_WithBatchSize) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "BatchSize", "200"};
    simParam.process(tokens);
    
    std::ostringstream oss;
    simParam.save(oss);
    
    EXPECT_EQ("SimulationParam MaxIteration 1000 BatchSize 200 ProcessingType CPU", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with default BatchSize
TEST_F(SimulationParamStatementSaveTest, Save_WithDefaultBatchSize) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "5000"};
    simParam.process(tokens);
    
    std::ostringstream oss;
    simParam.save(oss);
    
    EXPECT_EQ("SimulationParam MaxIteration 5000 BatchSize 100 ProcessingType CPU", oss.str());
}

// -----------------------------------------------------------------------------

// Test process with BatchSize only (should fail - MaxIteration is required)
TEST_F(SimulationParamStatementTest, Process_BatchSizeOnly) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "BatchSize", "200"};
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with BatchSize before MaxIteration (should work - order doesn't matter)
TEST_F(SimulationParamStatementTest, Process_BatchSizeBeforeMaxIteration) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "BatchSize", "200", "MaxIteration", "1000"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(1000u, simParam.getMaxIteration());
    EXPECT_EQ(200u, simParam.getBatchSize());
}

// -----------------------------------------------------------------------------

// Test process with ProcessingType CPU (7 tokens total)
TEST_F(SimulationParamStatementTest, Process_WithProcessingTypeCPU) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "BatchSize", "200", "ProcessingType", "CPU"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(1000u, simParam.getMaxIteration());
    EXPECT_EQ(200u, simParam.getBatchSize());
    EXPECT_EQ(CPU, simParam.getProcessingType());
}

// -----------------------------------------------------------------------------

// Test process with ProcessingType GPU (7 tokens total)
TEST_F(SimulationParamStatementTest, Process_WithProcessingTypeGPU) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "BatchSize", "200", "ProcessingType", "GPU"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(1000u, simParam.getMaxIteration());
    EXPECT_EQ(200u, simParam.getBatchSize());
    EXPECT_EQ(GPU, simParam.getProcessingType());
}

// -----------------------------------------------------------------------------

// Test process with ProcessingType CPU (case insensitive - lowercase)
TEST_F(SimulationParamStatementTest, Process_ProcessingTypeCPU_Lowercase) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "ProcessingType", "cpu"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(CPU, simParam.getProcessingType());
}

// -----------------------------------------------------------------------------

// Test process with ProcessingType GPU (case insensitive - lowercase)
TEST_F(SimulationParamStatementTest, Process_ProcessingTypeGPU_Lowercase) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "ProcessingType", "gpu"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(GPU, simParam.getProcessingType());
}

// -----------------------------------------------------------------------------

// Test process with ProcessingType CPU (case insensitive - mixed case)
TEST_F(SimulationParamStatementTest, Process_ProcessingTypeCPU_MixedCase) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "ProcessingType", "CpU"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(CPU, simParam.getProcessingType());
}

// -----------------------------------------------------------------------------

// Test process with ProcessingType GPU (case insensitive - mixed case)
TEST_F(SimulationParamStatementTest, Process_ProcessingTypeGPU_MixedCase) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "ProcessingType", "GpU"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(GPU, simParam.getProcessingType());
}

// -----------------------------------------------------------------------------

// Test process with ProcessingType parameter name (case insensitive)
TEST_F(SimulationParamStatementTest, Process_ProcessingTypeParameterName_CaseInsensitive) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "processingtype", "GPU"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(GPU, simParam.getProcessingType());
}

// -----------------------------------------------------------------------------

// Test process with invalid ProcessingType value
TEST_F(SimulationParamStatementTest, Process_InvalidProcessingTypeValue) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "ProcessingType", "INVALID"};
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with ProcessingType only (should fail - MaxIteration is required)
TEST_F(SimulationParamStatementTest, Process_ProcessingTypeOnly) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "ProcessingType", "GPU"};
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with ProcessingType in different positions
TEST_F(SimulationParamStatementTest, Process_ProcessingTypeFirst) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "ProcessingType", "GPU", "MaxIteration", "1000", "BatchSize", "200"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(1000u, simParam.getMaxIteration());
    EXPECT_EQ(200u, simParam.getBatchSize());
    EXPECT_EQ(GPU, simParam.getProcessingType());
}

// -----------------------------------------------------------------------------

// Test process with ProcessingType in middle position
TEST_F(SimulationParamStatementTest, Process_ProcessingTypeMiddle) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "ProcessingType", "GPU", "BatchSize", "200"};
    
    bool result = simParam.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_EQ(1000u, simParam.getMaxIteration());
    EXPECT_EQ(200u, simParam.getBatchSize());
    EXPECT_EQ(GPU, simParam.getProcessingType());
}

// -----------------------------------------------------------------------------

// Test default ProcessingType value (should be CPU)
TEST_F(SimulationParamStatementTest, DefaultProcessingType) {
    SimulationParamStatement simParam;
    
    EXPECT_EQ(CPU, simParam.getProcessingType());
}

// -----------------------------------------------------------------------------

// Test save with ProcessingType CPU
TEST_F(SimulationParamStatementSaveTest, Save_WithProcessingTypeCPU) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "BatchSize", "200", "ProcessingType", "CPU"};
    simParam.process(tokens);
    
    std::ostringstream oss;
    simParam.save(oss);
    
    EXPECT_EQ("SimulationParam MaxIteration 1000 BatchSize 200 ProcessingType CPU", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with ProcessingType GPU
TEST_F(SimulationParamStatementSaveTest, Save_WithProcessingTypeGPU) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "BatchSize", "200", "ProcessingType", "GPU"};
    simParam.process(tokens);
    
    std::ostringstream oss;
    simParam.save(oss);
    
    EXPECT_EQ("SimulationParam MaxIteration 1000 BatchSize 200 ProcessingType GPU", oss.str());
}

// -----------------------------------------------------------------------------

// Test save with default ProcessingType (CPU)
TEST_F(SimulationParamStatementSaveTest, Save_WithDefaultProcessingType) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "BatchSize", "200"};
    simParam.process(tokens);
    
    std::ostringstream oss;
    simParam.save(oss);
    
    EXPECT_EQ("SimulationParam MaxIteration 1000 BatchSize 200 ProcessingType CPU", oss.str());
}

// -----------------------------------------------------------------------------

// Test process with wrong number of tokens (6 tokens - missing ProcessingType value)
TEST_F(SimulationParamStatementTest, Process_ProcessingTypeMissingValue) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "BatchSize", "200", "ProcessingType"};
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with 7 tokens but invalid ProcessingType value
TEST_F(SimulationParamStatementTest, Process_SevenTokensInvalidProcessingType) {
    SimulationParamStatement simParam;
    std::vector<std::string> tokens = {"SimulationParam", "MaxIteration", "1000", "BatchSize", "200", "ProcessingType", "INVALID"};
    
    bool result = simParam.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------


