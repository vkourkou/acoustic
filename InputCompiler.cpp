#include <InputCompiler.h>
#include <cctype>
#include <stdexcept>

namespace Input {

InputCompiler::InputCompiler() : m_nX(0), m_nXSet(false) {
}

// -----------------------------------------------------------------------------

bool
InputCompiler::processLine(const std::vector<std::string>& tokens) {
    // If no tokens, do nothing
    if (tokens.empty()) {
        return false;
    }
    
    // Check if first token is a comment (starts with //)
    const std::string& firstToken = tokens[0];
    if (firstToken.length() >= 2 && firstToken[0] == '/' && firstToken[1] == '/') {
        // It's a comment, do nothing
        return false;
    }
    
    // Check if first token is "nx" (case insensitive)
    if (caseInsensitiveEquals(firstToken, "nx")) {
        // Make sure the line has only two tokens
        if (tokens.size() != 2) {
            return false;
        }
        
        // Convert the second token to size_t
        auto value = Utilities::convertTo<size_t>(tokens[1]);
        if (value.has_value()) {
            m_nX = value.value();
            m_nXSet = true;
            return true;
        }
        return false;
    }
    
    // First token is neither a comment nor "nx", do nothing
    return false;
}

// -----------------------------------------------------------------------------

size_t
InputCompiler::getNX() const {
    return m_nX;
}

// -----------------------------------------------------------------------------

bool
InputCompiler::hasNX() const {
    return m_nXSet;
}

// -----------------------------------------------------------------------------

bool
InputCompiler::caseInsensitiveEquals(const std::string& a, const std::string& b) const {
    if (a.length() != b.length()) {
        return false;
    }
    
    for (size_t i = 0; i < a.length(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) != 
            std::tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    
    return true;
}

// -----------------------------------------------------------------------------

bool
GridStatement::process(const std::vector<std::string>& tokens) {
    // Empty implementation
    return false;
}

// -----------------------------------------------------------------------------

bool
BBoxStatement::process(const std::vector<std::string>& tokens) {
    // Empty implementation

    return tokens.empty();
}

} // namespace Input

