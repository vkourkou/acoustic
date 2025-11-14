#include <InputFileParser.h>
#include <gtest/gtest.h>
#include <sstream>
#include <string>

using Input::InputFileTokenizer;

// Test fixture for InputFileTokenizer tests
class InputFileTokenizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// Test basic line reading
TEST_F(InputFileTokenizerTest, BasicLineReading) {
    std::istringstream iss("Hello World");
    InputFileTokenizer parser(iss);
    
    // First line is read during construction
    const std::string& line = parser.getLine();
    EXPECT_EQ("Hello World", line);
    EXPECT_EQ(1, parser.getLineNumber());
}

// Test multiple lines
TEST_F(InputFileTokenizerTest, MultipleLines) {
    std::istringstream iss("Line 1\nLine 2\nLine 3");
    InputFileTokenizer parser(iss);
    
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
TEST_F(InputFileTokenizerTest, EmptyLines) {
    std::istringstream iss("\n\n");
    InputFileTokenizer parser(iss);
    
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
TEST_F(InputFileTokenizerTest, WhitespaceTrimming) {
    std::istringstream iss("  Hello World  \n\tTabbed Line\t");
    InputFileTokenizer parser(iss);
    
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
TEST_F(InputFileTokenizerTest, WhitespaceOnlyLines) {
    std::istringstream iss("   \n\t\t\n  ");
    InputFileTokenizer parser(iss);
    
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
TEST_F(InputFileTokenizerTest, LineNumberTracking) {
    std::istringstream iss("Line 1\nLine 2\nLine 3\nLine 4");
    InputFileTokenizer parser(iss);
    
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
TEST_F(InputFileTokenizerTest, EndOfStream) {
    std::istringstream iss("Single Line");
    InputFileTokenizer parser(iss);
    
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
TEST_F(InputFileTokenizerTest, EmptyStream) {
    std::istringstream iss("");
    InputFileTokenizer parser(iss);
    
    // Empty stream - no line read during construction
    const std::string& line = parser.getLine();
    EXPECT_EQ("", line);
    EXPECT_EQ(0, parser.getLineNumber());
}

// Test isValid() with good stream
TEST_F(InputFileTokenizerTest, IsValidWithGoodStream) {
    std::istringstream iss("Test line");
    InputFileTokenizer parser(iss);
    
    EXPECT_TRUE(parser.isValid());
    // First line is read during construction
    EXPECT_EQ("Test line", parser.getLine());
    EXPECT_EQ(1, parser.getLineNumber());
    
    ++parser;
    EXPECT_FALSE(parser.isValid()); // EOF reached
}

// Test isValid() after reading all lines
TEST_F(InputFileTokenizerTest, IsValidAfterEOF) {
    std::istringstream iss("One line");
    InputFileTokenizer parser(iss);
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
TEST_F(InputFileTokenizerTest, MixedContent) {
    std::istringstream iss("Line 1\nLine 2\n  Padded Line  \nLine 4");
    InputFileTokenizer parser(iss);
    
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
TEST_F(InputFileTokenizerTest, ReadingAfterStreamInvalid) {
    std::istringstream iss("Valid line");
    InputFileTokenizer parser(iss);
    
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
TEST_F(InputFileTokenizerTest, SpecialCharacters) {
    std::istringstream iss("Line with !@#$%^&*()\nLine with tabs\tand spaces");
    InputFileTokenizer parser(iss);
    
    // First line is read during construction
    const std::string& line1 = parser.getLine();
    EXPECT_EQ("Line with !@#$%^&*()", line1);
    
    ++parser;
    const std::string& line2 = parser.getLine();
    EXPECT_EQ("Line with tabs\tand spaces", line2);
}

// Test line number accuracy with empty lines
TEST_F(InputFileTokenizerTest, LineNumberWithEmptyLines) {
    std::istringstream iss("First\n\nSecond\n\nThird");
    InputFileTokenizer parser(iss);
    
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
TEST_F(InputFileTokenizerTest, TokenizationBasicSingleWord) {
    std::istringstream iss("Hello");
    InputFileTokenizer parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(1, tokens.size());
    EXPECT_EQ("Hello", tokens[0]);
}

// Test tokenization - multiple words with spaces
TEST_F(InputFileTokenizerTest, TokenizationMultipleWordsSpaces) {
    std::istringstream iss("Hello World Test");
    InputFileTokenizer parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(3, tokens.size());
    EXPECT_EQ("Hello", tokens[0]);
    EXPECT_EQ("World", tokens[1]);
    EXPECT_EQ("Test", tokens[2]);
}

// Test tokenization - multiple words with tabs
TEST_F(InputFileTokenizerTest, TokenizationMultipleWordsTabs) {
    std::istringstream iss("Hello\tWorld\tTest");
    InputFileTokenizer parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(3, tokens.size());
    EXPECT_EQ("Hello", tokens[0]);
    EXPECT_EQ("World", tokens[1]);
    EXPECT_EQ("Test", tokens[2]);
}

// Test tokenization - mixed spaces and tabs
TEST_F(InputFileTokenizerTest, TokenizationMixedSpacesAndTabs) {
    std::istringstream iss("Hello\t World  Test");
    InputFileTokenizer parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(3, tokens.size());
    EXPECT_EQ("Hello", tokens[0]);
    EXPECT_EQ("World", tokens[1]);
    EXPECT_EQ("Test", tokens[2]);
}

// Test tokenization - multiple spaces/tabs between tokens
TEST_F(InputFileTokenizerTest, TokenizationMultipleDelimiters) {
    std::istringstream iss("Hello    World\t\tTest");
    InputFileTokenizer parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(3, tokens.size());
    EXPECT_EQ("Hello", tokens[0]);
    EXPECT_EQ("World", tokens[1]);
    EXPECT_EQ("Test", tokens[2]);
}

// Test tokenization - empty line (no tokens)
TEST_F(InputFileTokenizerTest, TokenizationEmptyLine) {
    std::istringstream iss("");
    InputFileTokenizer parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    EXPECT_EQ(0, tokens.size());
}

// Test tokenization - line with only spaces/tabs (no tokens)
TEST_F(InputFileTokenizerTest, TokenizationOnlyWhitespace) {
    std::istringstream iss("   \t\t  ");
    InputFileTokenizer parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    EXPECT_EQ(0, tokens.size());
}

// Test tokenization - tokens with special characters
TEST_F(InputFileTokenizerTest, TokenizationSpecialCharacters) {
    std::istringstream iss("Hello! World@ Test#");
    InputFileTokenizer parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(3, tokens.size());
    EXPECT_EQ("Hello!", tokens[0]);
    EXPECT_EQ("World@", tokens[1]);
    EXPECT_EQ("Test#", tokens[2]);
}

// Test tokenization - tokens across multiple lines
TEST_F(InputFileTokenizerTest, TokenizationMultipleLines) {
    std::istringstream iss("Line1 Token1 Token2\nLine2 Token3 Token4");
    InputFileTokenizer parser(iss);
    
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
TEST_F(InputFileTokenizerTest, TokenizationAfterEOF) {
    std::istringstream iss("Hello World");
    InputFileTokenizer parser(iss);
    
    // First line has tokens
    const std::vector<std::string>& tokens1 = parser.getTokens();
    ASSERT_EQ(2, tokens1.size());
    
    // Try to read past EOF
    ++parser;
    const std::vector<std::string>& tokens2 = parser.getTokens();
    EXPECT_EQ(0, tokens2.size());
}

// Test tokenization - single token with leading/trailing whitespace
TEST_F(InputFileTokenizerTest, TokenizationWhitespacePadding) {
    std::istringstream iss("  Hello  ");
    InputFileTokenizer parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(1, tokens.size());
    EXPECT_EQ("Hello", tokens[0]);
}

// Test tokenization - complex example with many tokens
TEST_F(InputFileTokenizerTest, TokenizationComplex) {
    std::istringstream iss("  Command   arg1\targ2  arg3\t arg4  ");
    InputFileTokenizer parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(5, tokens.size());
    EXPECT_EQ("Command", tokens[0]);
    EXPECT_EQ("arg1", tokens[1]);
    EXPECT_EQ("arg2", tokens[2]);
    EXPECT_EQ("arg3", tokens[3]);
    EXPECT_EQ("arg4", tokens[4]);
}

// -----------------------------------------------------------------------------

// Test tokenization - comment line with "//" only
TEST_F(InputFileTokenizerTest, TokenizationCommentOnly) {
    std::istringstream iss("//");
    InputFileTokenizer parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(1, tokens.size());
    EXPECT_EQ("//", tokens[0]);
}

// Test tokenization - comment line with "//" and text
TEST_F(InputFileTokenizerTest, TokenizationCommentWithText) {
    std::istringstream iss("// This is a comment");
    InputFileTokenizer parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(2, tokens.size());
    EXPECT_EQ("//", tokens[0]);
    EXPECT_EQ("This is a comment", tokens[1]);
}

// Test tokenization - comment line with "//" and text (no space after //)
TEST_F(InputFileTokenizerTest, TokenizationCommentWithTextNoSpace) {
    std::istringstream iss("//Comment");
    InputFileTokenizer parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(2, tokens.size());
    EXPECT_EQ("//", tokens[0]);
    EXPECT_EQ("Comment", tokens[1]);
}

// Test tokenization - comment line with "//" and leading spaces after //
TEST_F(InputFileTokenizerTest, TokenizationCommentWithLeadingSpaces) {
    std::istringstream iss("//   Comment with spaces");
    InputFileTokenizer parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(2, tokens.size());
    EXPECT_EQ("//", tokens[0]);
    EXPECT_EQ("Comment with spaces", tokens[1]);  // Leading spaces should be trimmed
}

// Test tokenization - comment line with "//" and tabs after //
TEST_F(InputFileTokenizerTest, TokenizationCommentWithTabs) {
    std::istringstream iss("//\t\tComment with tabs");
    InputFileTokenizer parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(2, tokens.size());
    EXPECT_EQ("//", tokens[0]);
    EXPECT_EQ("Comment with tabs", tokens[1]);  // Leading tabs should be trimmed
}

// Test tokenization - comment line with only whitespace after //
TEST_F(InputFileTokenizerTest, TokenizationCommentWithOnlyWhitespace) {
    std::istringstream iss("//   \t\t  ");
    InputFileTokenizer parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(1, tokens.size());  // Only "//" token, rest is empty after trimming
    EXPECT_EQ("//", tokens[0]);
}

// Test tokenization - comment lines mixed with regular lines
TEST_F(InputFileTokenizerTest, TokenizationCommentMixedWithRegular) {
    std::istringstream iss("// Comment line\nRegular line\n// Another comment");
    InputFileTokenizer parser(iss);
    
    // First line - comment
    const std::vector<std::string>& tokens1 = parser.getTokens();
    ASSERT_EQ(2, tokens1.size());
    EXPECT_EQ("//", tokens1[0]);
    EXPECT_EQ("Comment line", tokens1[1]);
    
    // Second line - regular
    ++parser;
    const std::vector<std::string>& tokens2 = parser.getTokens();
    ASSERT_EQ(2, tokens2.size());
    EXPECT_EQ("Regular", tokens2[0]);
    EXPECT_EQ("line", tokens2[1]);
    
    // Third line - comment
    ++parser;
    const std::vector<std::string>& tokens3 = parser.getTokens();
    ASSERT_EQ(2, tokens3.size());
    EXPECT_EQ("//", tokens3[0]);
    EXPECT_EQ("Another comment", tokens3[1]);
}

// Test tokenization - comment line with leading whitespace before //
TEST_F(InputFileTokenizerTest, TokenizationCommentWithLeadingWhitespace) {
    std::istringstream iss("  // Comment with leading spaces");
    InputFileTokenizer parser(iss);
    
    const std::vector<std::string>& tokens = parser.getTokens();
    ASSERT_EQ(2, tokens.size());
    EXPECT_EQ("//", tokens[0]);
    EXPECT_EQ("Comment with leading spaces", tokens[1]);  // Leading spaces before // are trimmed by trim()
}

// Test tokenization - multiple comment lines
TEST_F(InputFileTokenizerTest, TokenizationMultipleComments) {
    std::istringstream iss("// First comment\n// Second comment\n// Third comment");
    InputFileTokenizer parser(iss);
    
    // First comment
    const std::vector<std::string>& tokens1 = parser.getTokens();
    ASSERT_EQ(2, tokens1.size());
    EXPECT_EQ("//", tokens1[0]);
    EXPECT_EQ("First comment", tokens1[1]);
    
    // Second comment
    ++parser;
    const std::vector<std::string>& tokens2 = parser.getTokens();
    ASSERT_EQ(2, tokens2.size());
    EXPECT_EQ("//", tokens2[0]);
    EXPECT_EQ("Second comment", tokens2[1]);
    
    // Third comment
    ++parser;
    const std::vector<std::string>& tokens3 = parser.getTokens();
    ASSERT_EQ(2, tokens3.size());
    EXPECT_EQ("//", tokens3[0]);
    EXPECT_EQ("Third comment", tokens3[1]);
}

