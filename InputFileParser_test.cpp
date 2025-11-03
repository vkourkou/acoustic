#include <InputFileParser.h>
#include <gtest/gtest.h>
#include <sstream>
#include <string>

// Test fixture for InputFileParser tests
class InputFileParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// Test basic line reading
TEST_F(InputFileParserTest, BasicLineReading) {
    std::istringstream iss("Hello World");
    InputFileParser parser(iss);
    
    // First line is read during construction
    const std::string& line = parser.getLine();
    EXPECT_EQ("Hello World", line);
    EXPECT_EQ(1, parser.getLineNumber());
}

// Test multiple lines
TEST_F(InputFileParserTest, MultipleLines) {
    std::istringstream iss("Line 1\nLine 2\nLine 3");
    InputFileParser parser(iss);
    
    // First line is read during construction
    const std::string& line1 = parser.getLine();
    EXPECT_EQ("Line 1", line1);
    EXPECT_EQ(1, parser.getLineNumber());
    
    ++parser;
    const std::string& line2 = parser.getLine();
    EXPECT_EQ("Line 2", line2);
    EXPECT_EQ(2, parser.getLineNumber());
    
    ++parser;
    const std::string& line3 = parser.getLine();
    EXPECT_EQ("Line 3", line3);
    EXPECT_EQ(3, parser.getLineNumber());
}

// Test empty lines
TEST_F(InputFileParserTest, EmptyLines) {
    std::istringstream iss("\n\n");
    InputFileParser parser(iss);
    
    // First line is read during construction
    const std::string& line1 = parser.getLine();
    EXPECT_EQ("", line1);
    EXPECT_EQ(1, parser.getLineNumber());
    
    ++parser;
    const std::string& line2 = parser.getLine();
    EXPECT_EQ("", line2);
    EXPECT_EQ(2, parser.getLineNumber());
}

// Test whitespace trimming
TEST_F(InputFileParserTest, WhitespaceTrimming) {
    std::istringstream iss("  Hello World  \n\tTabbed Line\t");
    InputFileParser parser(iss);
    
    // First line is read during construction
    const std::string& line1 = parser.getLine();
    EXPECT_EQ("Hello World", line1);
    EXPECT_EQ(1, parser.getLineNumber());
    
    ++parser;
    const std::string& line2 = parser.getLine();
    EXPECT_EQ("Tabbed Line", line2);
    EXPECT_EQ(2, parser.getLineNumber());
}

// Test line with only whitespace (should return empty string after trim)
TEST_F(InputFileParserTest, WhitespaceOnlyLines) {
    std::istringstream iss("   \n\t\t\n  ");
    InputFileParser parser(iss);
    
    // First line is read during construction
    const std::string& line1 = parser.getLine();
    EXPECT_EQ("", line1);
    EXPECT_EQ(1, parser.getLineNumber());
    
    ++parser;
    const std::string& line2 = parser.getLine();
    EXPECT_EQ("", line2);
    EXPECT_EQ(2, parser.getLineNumber());
}

// Test line number tracking
TEST_F(InputFileParserTest, LineNumberTracking) {
    std::istringstream iss("Line 1\nLine 2\nLine 3\nLine 4");
    InputFileParser parser(iss);
    
    // First line is read during construction
    EXPECT_EQ(1, parser.getLineNumber());
    
    ++parser;
    EXPECT_EQ(2, parser.getLineNumber());
    
    ++parser;
    EXPECT_EQ(3, parser.getLineNumber());
    
    ++parser;
    EXPECT_EQ(4, parser.getLineNumber());
}

// Test end of stream
TEST_F(InputFileParserTest, EndOfStream) {
    std::istringstream iss("Single Line");
    InputFileParser parser(iss);
    
    // First line is read during construction
    const std::string& line1 = parser.getLine();
    EXPECT_EQ("Single Line", line1);
    EXPECT_EQ(1, parser.getLineNumber());
    
    // Try to read beyond end of stream
    ++parser;
    const std::string& line2 = parser.getLine();
    EXPECT_EQ("", line2);
    EXPECT_EQ(1, parser.getLineNumber()); // Line number doesn't increment on EOF
}

// Test empty stream
TEST_F(InputFileParserTest, EmptyStream) {
    std::istringstream iss("");
    InputFileParser parser(iss);
    
    // Empty stream - no line read during construction
    const std::string& line = parser.getLine();
    EXPECT_EQ("", line);
    EXPECT_EQ(0, parser.getLineNumber());
}

// Test isValid() with good stream
TEST_F(InputFileParserTest, IsValidWithGoodStream) {
    std::istringstream iss("Test line");
    InputFileParser parser(iss);
    
    EXPECT_TRUE(parser.isValid());
    // First line is read during construction
    EXPECT_EQ("Test line", parser.getLine());
    EXPECT_EQ(1, parser.getLineNumber());
    
    ++parser;
    EXPECT_FALSE(parser.isValid()); // EOF reached
}

// Test isValid() after reading all lines
TEST_F(InputFileParserTest, IsValidAfterEOF) {
    std::istringstream iss("One line");
    InputFileParser parser(iss);
    EXPECT_TRUE(parser.isValid()); // First line was successfully read during construction
    EXPECT_EQ(1, parser.getLineNumber());
    ++parser; // Try to read past EOF
    EXPECT_FALSE(parser.isValid()); // Now invalid after failed read
    EXPECT_EQ(1, parser.getLineNumber()); // Line number doesn't increment on failed read
    ++parser; // Try to read past EOF
    // Stream may still be in good state after EOF, depends on implementation
    // This test verifies current behavior
    EXPECT_FALSE(parser.isValid()); // Stream should not be good after EOF
    EXPECT_EQ(1, parser.getLineNumber()); // Line number still doesn't increment
}

// Test mixed content with various line endings
TEST_F(InputFileParserTest, MixedContent) {
    std::istringstream iss("Line 1\nLine 2\n  Padded Line  \nLine 4");
    InputFileParser parser(iss);
    
    // First line is read during construction
    EXPECT_EQ("Line 1", parser.getLine());
    EXPECT_EQ(1, parser.getLineNumber());
    
    ++parser;
    EXPECT_EQ("Line 2", parser.getLine());
    EXPECT_EQ(2, parser.getLineNumber());
    
    ++parser;
    EXPECT_EQ("Padded Line", parser.getLine());
    EXPECT_EQ(3, parser.getLineNumber());
    
    ++parser;
    EXPECT_EQ("Line 4", parser.getLine());
    EXPECT_EQ(4, parser.getLineNumber());
}

// Test reading after stream becomes invalid
TEST_F(InputFileParserTest, ReadingAfterStreamInvalid) {
    std::istringstream iss("Valid line");
    InputFileParser parser(iss);
    
    // First line is read during construction
    const std::string& line = parser.getLine();
    EXPECT_EQ("Valid line", line);
    EXPECT_EQ(1, parser.getLineNumber());
    
    // Try to read past EOF
    // Should return empty string when stream is at EOF
    ++parser;
    const std::string& empty = parser.getLine();
    EXPECT_EQ("", empty);
}

// Test with special characters
TEST_F(InputFileParserTest, SpecialCharacters) {
    std::istringstream iss("Line with !@#$%^&*()\nLine with tabs\tand spaces");
    InputFileParser parser(iss);
    
    // First line is read during construction
    const std::string& line1 = parser.getLine();
    EXPECT_EQ("Line with !@#$%^&*()", line1);
    
    ++parser;
    const std::string& line2 = parser.getLine();
    EXPECT_EQ("Line with tabs\tand spaces", line2);
}

// Test line number accuracy with empty lines
TEST_F(InputFileParserTest, LineNumberWithEmptyLines) {
    std::istringstream iss("First\n\nSecond\n\nThird");
    InputFileParser parser(iss);
    
    // First line is read during construction
    EXPECT_EQ(1, parser.getLineNumber()); // "First"
    
    ++parser; // Empty
    EXPECT_EQ(2, parser.getLineNumber());
    
    ++parser; // "Second"
    EXPECT_EQ(3, parser.getLineNumber());
    
    ++parser; // Empty
    EXPECT_EQ(4, parser.getLineNumber());
    
    ++parser; // "Third"
    EXPECT_EQ(5, parser.getLineNumber());
}

// Test tokenization - basic single word
TEST_F(InputFileParserTest, TokenizationBasicSingleWord) {
    std::istringstream iss("Hello");
    InputFileParser parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(1, tokens.size());
    EXPECT_EQ("Hello", tokens[0]);
}

// Test tokenization - multiple words with spaces
TEST_F(InputFileParserTest, TokenizationMultipleWordsSpaces) {
    std::istringstream iss("Hello World Test");
    InputFileParser parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(3, tokens.size());
    EXPECT_EQ("Hello", tokens[0]);
    EXPECT_EQ("World", tokens[1]);
    EXPECT_EQ("Test", tokens[2]);
}

// Test tokenization - multiple words with tabs
TEST_F(InputFileParserTest, TokenizationMultipleWordsTabs) {
    std::istringstream iss("Hello\tWorld\tTest");
    InputFileParser parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(3, tokens.size());
    EXPECT_EQ("Hello", tokens[0]);
    EXPECT_EQ("World", tokens[1]);
    EXPECT_EQ("Test", tokens[2]);
}

// Test tokenization - mixed spaces and tabs
TEST_F(InputFileParserTest, TokenizationMixedSpacesAndTabs) {
    std::istringstream iss("Hello\t World  Test");
    InputFileParser parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(3, tokens.size());
    EXPECT_EQ("Hello", tokens[0]);
    EXPECT_EQ("World", tokens[1]);
    EXPECT_EQ("Test", tokens[2]);
}

// Test tokenization - multiple spaces/tabs between tokens
TEST_F(InputFileParserTest, TokenizationMultipleDelimiters) {
    std::istringstream iss("Hello    World\t\tTest");
    InputFileParser parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(3, tokens.size());
    EXPECT_EQ("Hello", tokens[0]);
    EXPECT_EQ("World", tokens[1]);
    EXPECT_EQ("Test", tokens[2]);
}

// Test tokenization - empty line (no tokens)
TEST_F(InputFileParserTest, TokenizationEmptyLine) {
    std::istringstream iss("");
    InputFileParser parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    EXPECT_EQ(0, tokens.size());
}

// Test tokenization - line with only spaces/tabs (no tokens)
TEST_F(InputFileParserTest, TokenizationOnlyWhitespace) {
    std::istringstream iss("   \t\t  ");
    InputFileParser parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    EXPECT_EQ(0, tokens.size());
}

// Test tokenization - tokens with special characters
TEST_F(InputFileParserTest, TokenizationSpecialCharacters) {
    std::istringstream iss("Hello! World@ Test#");
    InputFileParser parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(3, tokens.size());
    EXPECT_EQ("Hello!", tokens[0]);
    EXPECT_EQ("World@", tokens[1]);
    EXPECT_EQ("Test#", tokens[2]);
}

// Test tokenization - tokens across multiple lines
TEST_F(InputFileParserTest, TokenizationMultipleLines) {
    std::istringstream iss("Line1 Token1 Token2\nLine2 Token3 Token4");
    InputFileParser parser(iss);
    
    // First line
    const std::vector<std::string>& tokens1 = parser.getTokens();
    ASSERT_EQ(3, tokens1.size());
    EXPECT_EQ("Line1", tokens1[0]);
    EXPECT_EQ("Token1", tokens1[1]);
    EXPECT_EQ("Token2", tokens1[2]);
    
    // Second line
    ++parser;
    const std::vector<std::string>& tokens2 = parser.getTokens();
    ASSERT_EQ(3, tokens2.size());
    EXPECT_EQ("Line2", tokens2[0]);
    EXPECT_EQ("Token3", tokens2[1]);
    EXPECT_EQ("Token4", tokens2[2]);
}

// Test tokenization - tokens after EOF (should be empty)
TEST_F(InputFileParserTest, TokenizationAfterEOF) {
    std::istringstream iss("Hello World");
    InputFileParser parser(iss);
    
    // First line has tokens
    const std::vector<std::string>& tokens1 = parser.getTokens();
    ASSERT_EQ(2, tokens1.size());
    
    // Try to read past EOF
    ++parser;
    const std::vector<std::string>& tokens2 = parser.getTokens();
    EXPECT_EQ(0, tokens2.size());
}

// Test tokenization - single token with leading/trailing whitespace
TEST_F(InputFileParserTest, TokenizationWhitespacePadding) {
    std::istringstream iss("  Hello  ");
    InputFileParser parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(1, tokens.size());
    EXPECT_EQ("Hello", tokens[0]);
}

// Test tokenization - complex example with many tokens
TEST_F(InputFileParserTest, TokenizationComplex) {
    std::istringstream iss("  Command   arg1\targ2  arg3\t arg4  ");
    InputFileParser parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(5, tokens.size());
    EXPECT_EQ("Command", tokens[0]);
    EXPECT_EQ("arg1", tokens[1]);
    EXPECT_EQ("arg2", tokens[2]);
    EXPECT_EQ("arg3", tokens[3]);
    EXPECT_EQ("arg4", tokens[4]);
}

