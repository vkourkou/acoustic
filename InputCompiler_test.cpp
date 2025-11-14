#include <InputCompiler.h>
#include <gtest/gtest.h>
#include <vector>
#include <string>

using Input::BBoxStatement;
using Input::SourceStatement;
using Input::VelocityStatement;
using Input::MaxResolutionStatement;
using Input::FileParser;
using Input::StatementType;

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

// Test process with valid Source tokens (5 tokens total)
TEST_F(SourceStatementTest, Process_ValidSource) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "0.5"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.5f, source.getAmplitude());
}

// -----------------------------------------------------------------------------

// Test process with case-insensitive Source (lowercase)
TEST_F(SourceStatementTest, Process_CaseInsensitiveSource) {
    SourceStatement source;
    std::vector<std::string> tokens = {"source", "Frequency", "1000.0", "Amplitude", "0.5"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
}

// Test process with case-insensitive Source (mixed case)
TEST_F(SourceStatementTest, Process_CaseInsensitiveSourceMixed) {
    SourceStatement source;
    std::vector<std::string> tokens = {"SoUrCe", "Frequency", "1000.0", "Amplitude", "0.5"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
}

// -----------------------------------------------------------------------------

// Test process with case-insensitive parameter names
TEST_F(SourceStatementTest, Process_CaseInsensitiveParameters) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "frequency", "1000.0", "amplitude", "0.5"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.5f, source.getAmplitude());
}

// Test process with mixed case parameter names
TEST_F(SourceStatementTest, Process_MixedCaseParameters) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "FrEqUeNcY", "1000.0", "AmPlItUdE", "0.5"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.5f, source.getAmplitude());
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

// Test process with wrong number of tokens (less than 5)
TEST_F(SourceStatementTest, Process_TooFewTokens) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0"};
    
    bool result = source.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with wrong number of tokens (more than 5)
TEST_F(SourceStatementTest, Process_TooManyTokens) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "0.5", "Extra"};
    
    bool result = source.process(tokens);
    EXPECT_FALSE(result);
}

// Test process with exactly 4 tokens (missing one)
TEST_F(SourceStatementTest, Process_Exactly4Tokens) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude"};
    
    bool result = source.process(tokens);
    EXPECT_FALSE(result);
}

// -----------------------------------------------------------------------------

// Test process with negative frequency (should fail validation)
TEST_F(SourceStatementTest, Process_NegativeFrequency) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "-1000.0", "Amplitude", "0.5"};
    
    bool result = source.process(tokens);
    EXPECT_FALSE(result);  // Should fail because frequency is negative
    
    // Values may still be set, but process should return false
    EXPECT_FLOAT_EQ(-1000.0f, source.getFreq());
}

// Test process with zero frequency (should fail validation)
TEST_F(SourceStatementTest, Process_ZeroFrequency) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "0.0", "Amplitude", "0.5"};
    
    bool result = source.process(tokens);
    EXPECT_FALSE(result);  // Should fail because frequency is zero
    
    // Values may still be set, but process should return false
    EXPECT_FLOAT_EQ(0.0f, source.getFreq());
}

// Test process with large values
TEST_F(SourceStatementTest, Process_LargeValues) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000000.0", "Amplitude", "1000.0"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1000000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(1000.0f, source.getAmplitude());
}

// Test process with decimal values
TEST_F(SourceStatementTest, Process_DecimalValues) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1234.567", "Amplitude", "0.12345"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1234.567f, source.getFreq());
    EXPECT_FLOAT_EQ(0.12345f, source.getAmplitude());
}

// -----------------------------------------------------------------------------

// Test process with different parameter order
TEST_F(SourceStatementTest, Process_DifferentParameterOrder) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Amplitude", "0.5", "Frequency", "1000.0"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.5f, source.getAmplitude());
}

// -----------------------------------------------------------------------------

// Test process with invalid value (non-numeric string for frequency)
TEST_F(SourceStatementTest, Process_InvalidFrequencyValue) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "invalid", "Amplitude", "0.5"};
    
    bool result = source.process(tokens);
    EXPECT_FALSE(result);  // Should fail because frequency is not set (remains 0.0) and isValid() fails
    
    // Frequency should remain uninitialized (default value 0.0)
    // Amplitude should be set correctly
    EXPECT_FLOAT_EQ(0.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.5f, source.getAmplitude());
}

// Test process with invalid value (non-numeric string for amplitude, but valid frequency)
TEST_F(SourceStatementTest, Process_InvalidAmplitudeValue) {
    SourceStatement source;
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "invalid"};
    
    bool result = source.process(tokens);
    EXPECT_TRUE(result);  // Should succeed because frequency is valid (amplitude is not validated)
    
    EXPECT_FLOAT_EQ(1000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.0f, source.getAmplitude());  // Amplitude remains default
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
    std::vector<std::string> tokens = {"Source", "Frequency", "2500.0", "Amplitude", "0.75"};
    
    source.process(tokens);
    
    EXPECT_FLOAT_EQ(2500.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.75f, source.getAmplitude());
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
    
    std::vector<std::string> tokens1 = {"Source", "Frequency", "1000.0", "Amplitude", "0.5"};
    bool result1 = source.process(tokens1);
    EXPECT_TRUE(result1);
    
    EXPECT_FLOAT_EQ(1000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.5f, source.getAmplitude());
    
    std::vector<std::string> tokens2 = {"Source", "Frequency", "2000.0", "Amplitude", "1.0"};
    bool result2 = source.process(tokens2);
    EXPECT_TRUE(result2);
    
    EXPECT_FLOAT_EQ(2000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(1.0f, source.getAmplitude());
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
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1.0", "Temporal", "2.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(2.0f, maxRes.getTemporal());
}

// -----------------------------------------------------------------------------

// Test process with case-insensitive MaxResolution (lowercase)
TEST_F(MaxResolutionStatementTest, Process_CaseInsensitiveMaxResolution) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"maxresolution", "Spatial", "1.0", "Temporal", "2.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(2.0f, maxRes.getTemporal());
}

// Test process with case-insensitive MaxResolution (mixed case)
TEST_F(MaxResolutionStatementTest, Process_CaseInsensitiveMaxResolutionMixed) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaXrEsOlUtIoN", "Spatial", "1.0", "Temporal", "2.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(2.0f, maxRes.getTemporal());
}

// -----------------------------------------------------------------------------

// Test process with case-insensitive parameter names
TEST_F(MaxResolutionStatementTest, Process_CaseInsensitiveParameters) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "spatial", "1.0", "temporal", "2.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(2.0f, maxRes.getTemporal());
}

// Test process with mixed case parameter names
TEST_F(MaxResolutionStatementTest, Process_MixedCaseParameters) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "SpAtIaL", "1.0", "TeMpOrAl", "2.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(2.0f, maxRes.getTemporal());
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

// Test process with large values
TEST_F(MaxResolutionStatementTest, Process_LargeValues) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1000.0", "Temporal", "2000.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1000.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(2000.0f, maxRes.getTemporal());
}

// Test process with decimal values
TEST_F(MaxResolutionStatementTest, Process_DecimalValues) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1.5", "Temporal", "2.75"};
    
    bool result = maxRes.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1.5f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(2.75f, maxRes.getTemporal());
}

// -----------------------------------------------------------------------------

// Test process with different parameter order
TEST_F(MaxResolutionStatementTest, Process_DifferentParameterOrder) {
    MaxResolutionStatement maxRes;
    std::vector<std::string> tokens = {"MaxResolution", "Temporal", "2.0", "Spatial", "1.0"};
    
    bool result = maxRes.process(tokens);
    EXPECT_TRUE(result);
    
    EXPECT_FLOAT_EQ(1.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(2.0f, maxRes.getTemporal());
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
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "10.5", "Temporal", "20.5"};
    
    maxRes.process(tokens);
    
    EXPECT_FLOAT_EQ(10.5f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(20.5f, maxRes.getTemporal());
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
    
    std::vector<std::string> tokens1 = {"MaxResolution", "Spatial", "1.0", "Temporal", "2.0"};
    bool result1 = maxRes.process(tokens1);
    EXPECT_TRUE(result1);
    
    EXPECT_FLOAT_EQ(1.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(2.0f, maxRes.getTemporal());
    
    std::vector<std::string> tokens2 = {"MaxResolution", "Spatial", "10.0", "Temporal", "20.0"};
    bool result2 = maxRes.process(tokens2);
    EXPECT_TRUE(result2);
    
    EXPECT_FLOAT_EQ(10.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(20.0f, maxRes.getTemporal());
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
    std::vector<std::string> tokens = {"Source", "Frequency", "1000.0", "Amplitude", "0.5"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::SOURCE, result);
    
    // Verify the statement values using getStatement
    const auto& source = compiler.getStatement<Input::StatementType::SOURCE>();
    EXPECT_FLOAT_EQ(1000.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.5f, source.getAmplitude());
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
    std::vector<std::string> tokens = {"source", "Frequency", "2000.0", "Amplitude", "0.75"};
    
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
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with comment (full comment syntax)
TEST_F(InputCompilerTest, ProcessLine_FullComment) {
    std::vector<std::string> tokens = {"//", "Comment", "line"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
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
    std::vector<std::string> tokens = {"Source", "Frequency", "0.0", "Amplitude", "0.5"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAX, result);
}

// Test processLine with Source but invalid frequency (negative)
TEST_F(InputCompilerTest, ProcessLine_Source_NegativeFrequency) {
    std::vector<std::string> tokens = {"Source", "Frequency", "-1000.0", "Amplitude", "0.5"};
    
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
    std::vector<std::string> tokens = {"MaxResolution", "Spatial", "1.0", "Temporal", "2.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAXRESOLUTION, result);
    
    // Verify the statement values using getStatement
    const auto& maxRes = compiler.getStatement<Input::StatementType::MAXRESOLUTION>();
    EXPECT_FLOAT_EQ(1.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(2.0f, maxRes.getTemporal());
}

// Test processLine with case-insensitive MaxResolution
TEST_F(InputCompilerTest, ProcessLine_CaseInsensitiveMaxResolution) {
    std::vector<std::string> tokens = {"maxresolution", "Spatial", "10.5", "Temporal", "20.5"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAXRESOLUTION, result);
    
    const auto& maxRes = compiler.getStatement<Input::StatementType::MAXRESOLUTION>();
    EXPECT_FLOAT_EQ(10.5f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(20.5f, maxRes.getTemporal());
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
    std::vector<std::string> tokens1 = {"Source", "Frequency", "1000.0", "Amplitude", "0.5"};
    Input::StatementType result1 = compiler.processLine(tokens1);
    EXPECT_EQ(Input::StatementType::SOURCE, result1);
    
    const auto& source1 = compiler.getStatement<Input::StatementType::SOURCE>();
    EXPECT_FLOAT_EQ(1000.0f, source1.getFreq());
    EXPECT_FLOAT_EQ(0.5f, source1.getAmplitude());
    
    std::vector<std::string> tokens2 = {"Source", "Frequency", "2000.0", "Amplitude", "1.0"};
    Input::StatementType result2 = compiler.processLine(tokens2);
    EXPECT_EQ(Input::StatementType::SOURCE, result2);
    
    const auto& source2 = compiler.getStatement<Input::StatementType::SOURCE>();
    EXPECT_FLOAT_EQ(2000.0f, source2.getFreq());
    EXPECT_FLOAT_EQ(1.0f, source2.getAmplitude());
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
    std::vector<std::string> tokens1 = {"MaxResolution", "Spatial", "1.0", "Temporal", "2.0"};
    Input::StatementType result1 = compiler.processLine(tokens1);
    EXPECT_EQ(Input::StatementType::MAXRESOLUTION, result1);
    
    const auto& maxRes1 = compiler.getStatement<Input::StatementType::MAXRESOLUTION>();
    EXPECT_FLOAT_EQ(1.0f, maxRes1.getSpatial());
    EXPECT_FLOAT_EQ(2.0f, maxRes1.getTemporal());
    
    std::vector<std::string> tokens2 = {"MaxResolution", "Spatial", "10.0", "Temporal", "20.0"};
    Input::StatementType result2 = compiler.processLine(tokens2);
    EXPECT_EQ(Input::StatementType::MAXRESOLUTION, result2);
    
    const auto& maxRes2 = compiler.getStatement<Input::StatementType::MAXRESOLUTION>();
    EXPECT_FLOAT_EQ(10.0f, maxRes2.getSpatial());
    EXPECT_FLOAT_EQ(20.0f, maxRes2.getTemporal());
}

// Test processLine with different parameter order for Source
TEST_F(InputCompilerTest, ProcessLine_Source_DifferentParameterOrder) {
    std::vector<std::string> tokens = {"Source", "Amplitude", "0.75", "Frequency", "1500.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::SOURCE, result);
    
    const auto& source = compiler.getStatement<Input::StatementType::SOURCE>();
    EXPECT_FLOAT_EQ(1500.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.75f, source.getAmplitude());
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

// Test processLine with different parameter order for MaxResolution
TEST_F(InputCompilerTest, ProcessLine_MaxResolution_DifferentParameterOrder) {
    std::vector<std::string> tokens = {"MaxResolution", "Temporal", "20.0", "Spatial", "10.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAXRESOLUTION, result);
    
    const auto& maxRes = compiler.getStatement<Input::StatementType::MAXRESOLUTION>();
    EXPECT_FLOAT_EQ(10.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(20.0f, maxRes.getTemporal());
}

// -----------------------------------------------------------------------------

// Test processLine with case-insensitive parameter names for Source
TEST_F(InputCompilerTest, ProcessLine_Source_CaseInsensitiveParameters) {
    std::vector<std::string> tokens = {"Source", "frequency", "2500.0", "amplitude", "0.9"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::SOURCE, result);
    
    const auto& source = compiler.getStatement<Input::StatementType::SOURCE>();
    EXPECT_FLOAT_EQ(2500.0f, source.getFreq());
    EXPECT_FLOAT_EQ(0.9f, source.getAmplitude());
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

// Test processLine with case-insensitive parameter names for MaxResolution
TEST_F(InputCompilerTest, ProcessLine_MaxResolution_CaseInsensitiveParameters) {
    std::vector<std::string> tokens = {"MaxResolution", "spatial", "5.0", "temporal", "15.0"};
    
    Input::StatementType result = compiler.processLine(tokens);
    EXPECT_EQ(Input::StatementType::MAXRESOLUTION, result);
    
    const auto& maxRes = compiler.getStatement<Input::StatementType::MAXRESOLUTION>();
    EXPECT_FLOAT_EQ(5.0f, maxRes.getSpatial());
    EXPECT_FLOAT_EQ(15.0f, maxRes.getTemporal());
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
        "Source Frequency 100 Amplitude 50\n"
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
        "Source Frequency 10 Amplitude 5\n"
    );

    FileParser parser(iss);
    std::vector<StatementType> types;
    std::vector<size_t> lineNumbers;

    for (FileParser::const_iterator iter(parser); iter.isValid(); ++iter) {
        types.push_back(iter.getType());
        lineNumbers.push_back(iter.getLineNumber());
    }

    ASSERT_EQ(4u, types.size());
    EXPECT_EQ(StatementType::MAX, types[0]);
    EXPECT_EQ(StatementType::MAX, types[1]);
    EXPECT_EQ(StatementType::MAX, types[2]);
    EXPECT_EQ(StatementType::SOURCE, types[3]);

    ASSERT_EQ(4u, lineNumbers.size());
    EXPECT_EQ(1u, lineNumbers[0]);
    EXPECT_EQ(2u, lineNumbers[1]);
    EXPECT_EQ(3u, lineNumbers[2]);
    EXPECT_EQ(4u, lineNumbers[3]);
}


