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

// -----------------------------------------------------------------------------

// Test caseInsensitiveEquals with identical strings (same case)
TEST_F(UtilitiesTest, CaseInsensitiveEquals_IdenticalSameCase) {
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("Hello", "Hello"));
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("WORLD", "WORLD"));
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("test123", "test123"));
}

// Test caseInsensitiveEquals with identical strings (different case)
TEST_F(UtilitiesTest, CaseInsensitiveEquals_IdenticalDifferentCase) {
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("Hello", "HELLO"));
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("HELLO", "hello"));
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("World", "wORLD"));
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("BBox", "bbox"));
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("bbox", "BBOX"));
}

// Test caseInsensitiveEquals with mixed case strings
TEST_F(UtilitiesTest, CaseInsensitiveEquals_MixedCase) {
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("HeLlO", "hElLo"));
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("XMin", "xmin"));
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("XmIn", "xMiN"));
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("BbOx", "bBoX"));
}

// Test caseInsensitiveEquals with different strings (should return false)
TEST_F(UtilitiesTest, CaseInsensitiveEquals_DifferentStrings) {
    EXPECT_FALSE(Utilities::caseInsensitiveEquals("Hello", "World"));
    EXPECT_FALSE(Utilities::caseInsensitiveEquals("test", "testing"));
    EXPECT_FALSE(Utilities::caseInsensitiveEquals("BBox", "Source"));
    EXPECT_FALSE(Utilities::caseInsensitiveEquals("XMin", "XMax"));
}

// Test caseInsensitiveEquals with different length strings (should return false)
TEST_F(UtilitiesTest, CaseInsensitiveEquals_DifferentLength) {
    EXPECT_FALSE(Utilities::caseInsensitiveEquals("Hello", "Hel"));
    EXPECT_FALSE(Utilities::caseInsensitiveEquals("test", "test123"));
    EXPECT_FALSE(Utilities::caseInsensitiveEquals("", "Hello"));
    EXPECT_FALSE(Utilities::caseInsensitiveEquals("BBox", "BBoxX"));
}

// Test caseInsensitiveEquals with empty strings
TEST_F(UtilitiesTest, CaseInsensitiveEquals_EmptyStrings) {
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("", ""));
}

// Test caseInsensitiveEquals with single character strings
TEST_F(UtilitiesTest, CaseInsensitiveEquals_SingleCharacter) {
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("A", "a"));
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("Z", "z"));
    EXPECT_FALSE(Utilities::caseInsensitiveEquals("A", "B"));
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("a", "A"));
}

// Test caseInsensitiveEquals with strings containing numbers and special characters
TEST_F(UtilitiesTest, CaseInsensitiveEquals_WithNumbersAndSpecialChars) {
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("Test123", "test123"));
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("XMin_1", "xmin_1"));
    EXPECT_FALSE(Utilities::caseInsensitiveEquals("Test123", "test456"));
    EXPECT_TRUE(Utilities::caseInsensitiveEquals("Hello_World", "HELLO_WORLD"));
}

