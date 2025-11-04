#ifndef INPUT_COMPILER_H
#define INPUT_COMPILER_H

#include <string>
#include <vector>
#include <Utilities.h>

class InputCompiler {
public:
    // Constructor
    InputCompiler();
    
    // Destructor
    ~InputCompiler() = default;
    
    // Process a line of tokens
    // Returns true if the line was successfully processed, false otherwise
    bool processLine(const std::vector<std::string>& tokens);
    
    // Get the value of m_nX
    size_t getNX() const;
    
    // Check if m_nX has been set
    bool hasNX() const;

private:
    size_t m_nX;
    bool m_nXSet;
    
    // Helper method to check if string is case-insensitive equal
    bool caseInsensitiveEquals(const std::string& a, const std::string& b) const;
};

#endif // INPUT_COMPILER_H

