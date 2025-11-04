#include <Utilities.h>
#include <gtest/gtest.h>
#include <string>
#include <limits>

// Test fixture for Utilities tests
class UtilitiesTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// -----------------------------------------------------------------------------

// Test convertTo<size_t> with valid positive integers
TEST_F(UtilitiesTest, ConvertToSizeT_ValidPositive) {
    auto result = Utilities::convertTo<size_t>("0");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(0, result.value());
    
    result = Utilities::convertTo<size_t>("42");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(42, result.value());
    
    result = Utilities::convertTo<size_t>("1000");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(1000, result.value());
}

// Test convertTo<size_t> with negative numbers (should fail)
TEST_F(UtilitiesTest, ConvertToSizeT_Negative) {
    auto result = Utilities::convertTo<size_t>("-1");
    EXPECT_FALSE(result.has_value());
    
    result = Utilities::convertTo<size_t>("-42");
    EXPECT_FALSE(result.has_value());
}

// Test convertTo<size_t> with empty string (should fail)
TEST_F(UtilitiesTest, ConvertToSizeT_EmptyString) {
    auto result = Utilities::convertTo<size_t>("");
    EXPECT_FALSE(result.has_value());
}

// Test convertTo<size_t> with invalid format (should fail)
TEST_F(UtilitiesTest, ConvertToSizeT_InvalidFormat) {
    auto result = Utilities::convertTo<size_t>("abc");
    EXPECT_FALSE(result.has_value());
    
    result = Utilities::convertTo<size_t>("12abc");
    EXPECT_FALSE(result.has_value());
    
    result = Utilities::convertTo<size_t>("12.5");
    EXPECT_FALSE(result.has_value());
}

// Test convertTo<size_t> with whitespace (should fail)
TEST_F(UtilitiesTest, ConvertToSizeT_WithWhitespace) {
    auto result = Utilities::convertTo<size_t>(" 42");
    EXPECT_FALSE(result.has_value());
    
    result = Utilities::convertTo<size_t>("42 ");
    EXPECT_FALSE(result.has_value());
    
    result = Utilities::convertTo<size_t>(" 42 ");
    EXPECT_FALSE(result.has_value());
}

// -----------------------------------------------------------------------------

// Test convertTo<long> with valid positive integers
TEST_F(UtilitiesTest, ConvertToLong_ValidPositive) {
    auto result = Utilities::convertTo<long>("0");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(0, result.value());
    
    result = Utilities::convertTo<long>("42");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(42, result.value());
    
    result = Utilities::convertTo<long>("1000");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(1000, result.value());
}

// Test convertTo<long> with valid negative integers
TEST_F(UtilitiesTest, ConvertToLong_ValidNegative) {
    auto result = Utilities::convertTo<long>("-1");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(-1, result.value());
    
    result = Utilities::convertTo<long>("-42");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(-42, result.value());
    
    result = Utilities::convertTo<long>("-1000");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(-1000, result.value());
}

// Test convertTo<long> with empty string (should fail)
TEST_F(UtilitiesTest, ConvertToLong_EmptyString) {
    auto result = Utilities::convertTo<long>("");
    EXPECT_FALSE(result.has_value());
}

// Test convertTo<long> with invalid format (should fail)
TEST_F(UtilitiesTest, ConvertToLong_InvalidFormat) {
    auto result = Utilities::convertTo<long>("abc");
    EXPECT_FALSE(result.has_value());
    
    result = Utilities::convertTo<long>("12abc");
    EXPECT_FALSE(result.has_value());
    
    result = Utilities::convertTo<long>("12.5");
    EXPECT_FALSE(result.has_value());
}

// Test convertTo<long> with whitespace (should fail)
TEST_F(UtilitiesTest, ConvertToLong_WithWhitespace) {
    auto result = Utilities::convertTo<long>(" 42");
    EXPECT_FALSE(result.has_value());
    
    result = Utilities::convertTo<long>("42 ");
    EXPECT_FALSE(result.has_value());
    
    result = Utilities::convertTo<long>(" 42 ");
    EXPECT_FALSE(result.has_value());
}

// Test convertTo<long> with large numbers
TEST_F(UtilitiesTest, ConvertToLong_LargeNumbers) {
    auto result = Utilities::convertTo<long>("2147483647");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(2147483647L, result.value());
    
    result = Utilities::convertTo<long>("-2147483648");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(-2147483648L, result.value());
}

// -----------------------------------------------------------------------------

// Test convertTo<float> with valid positive floats
TEST_F(UtilitiesTest, ConvertToFloat_ValidPositive) {
    auto result = Utilities::convertTo<float>("0.0");
    EXPECT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(0.0f, result.value());
    
    result = Utilities::convertTo<float>("42.5");
    EXPECT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(42.5f, result.value());
    
    result = Utilities::convertTo<float>("1000.0");
    EXPECT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(1000.0f, result.value());
    
    result = Utilities::convertTo<float>("3.14159");
    EXPECT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(3.14159f, result.value());
}

// Test convertTo<float> with valid negative floats
TEST_F(UtilitiesTest, ConvertToFloat_ValidNegative) {
    auto result = Utilities::convertTo<float>("-1.5");
    EXPECT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(-1.5f, result.value());
    
    result = Utilities::convertTo<float>("-42.5");
    EXPECT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(-42.5f, result.value());
    
    result = Utilities::convertTo<float>("-1000.0");
    EXPECT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(-1000.0f, result.value());
}

// Test convertTo<float> with integers (should work)
TEST_F(UtilitiesTest, ConvertToFloat_WithIntegers) {
    auto result = Utilities::convertTo<float>("42");
    EXPECT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(42.0f, result.value());
    
    result = Utilities::convertTo<float>("-100");
    EXPECT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(-100.0f, result.value());
}

// Test convertTo<float> with scientific notation
TEST_F(UtilitiesTest, ConvertToFloat_ScientificNotation) {
    auto result = Utilities::convertTo<float>("1.5e2");
    EXPECT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(150.0f, result.value());
    
    result = Utilities::convertTo<float>("1.5e-2");
    EXPECT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(0.015f, result.value());
}

// Test convertTo<float> with empty string (should fail)
TEST_F(UtilitiesTest, ConvertToFloat_EmptyString) {
    auto result = Utilities::convertTo<float>("");
    EXPECT_FALSE(result.has_value());
}

// Test convertTo<float> with invalid format (should fail)
TEST_F(UtilitiesTest, ConvertToFloat_InvalidFormat) {
    auto result = Utilities::convertTo<float>("abc");
    EXPECT_FALSE(result.has_value());
    
    result = Utilities::convertTo<float>("12.5abc");
    EXPECT_FALSE(result.has_value());
}

// Test convertTo<float> with whitespace (should fail)
TEST_F(UtilitiesTest, ConvertToFloat_WithWhitespace) {
    auto result = Utilities::convertTo<float>(" 42.5");
    EXPECT_FALSE(result.has_value());
    
    result = Utilities::convertTo<float>("42.5 ");
    EXPECT_FALSE(result.has_value());
    
    result = Utilities::convertTo<float>(" 42.5 ");
    EXPECT_FALSE(result.has_value());
}

// Test convertTo<float> with very small numbers
TEST_F(UtilitiesTest, ConvertToFloat_VerySmallNumbers) {
    auto result = Utilities::convertTo<float>("0.000001");
    EXPECT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(0.000001f, result.value());
    
    result = Utilities::convertTo<float>("-0.000001");
    EXPECT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(-0.000001f, result.value());
}

