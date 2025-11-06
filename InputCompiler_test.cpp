#include <InputCompiler.h>
#include <gtest/gtest.h>
#include <vector>
#include <string>

using Input::BBoxStatement;
using Input::SourceStatement;

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

