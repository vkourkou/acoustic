#ifndef INPUT_FILE_PARSER_H
#define INPUT_FILE_PARSER_H

#include <string>
#include <istream>

class InputFileParser {
public:
    // Constructor that takes an input stream reference
    // The caller is responsible for managing the stream's lifetime
    explicit InputFileParser(std::istream& stream);
    
    // Destructor
    ~InputFileParser() = default;
    
    // Prefix increment operator - advances to next line (only if stream is valid)
    void operator++();
    
    // Check if stream is in a valid state
    bool isValid() const;
    
    // Get current line number (1-based)
    size_t getLineNumber() const;
    
    // Get accessor for the buffered line
    const std::string& getLine() const;

private:
    std::istream& stream_;
    size_t lineNumber_;
    std::string bufferedLine_;
    bool m_isValid;
    
    // Helper method to trim whitespace from a string in place
    void trim(std::string& str) const;
};

#endif // INPUT_FILE_PARSER_H

