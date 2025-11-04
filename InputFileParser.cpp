#include <InputFileParser.h>

namespace Input {

InputFileParser::InputFileParser(std::istream& stream)
    : stream_(stream), lineNumber_(0), m_isValid(false) {
    // Read the first line during construction
    operator++();
}

// -----------------------------------------------------------------------------

void
InputFileParser::operator++() {
    // Check both isValid() and stream_.good() before doing anything
    // Allow first read if lineNumber_ is 0 (initial state, haven't read yet)
    if (!stream_.good() || (!isValid() && lineNumber_ > 0)) {
        bufferedLine_.clear();
        m_vTokens.clear();
        m_isValid = false;
        return;
    }
    
    if (std::getline(stream_, bufferedLine_)) {
        lineNumber_++;
        // Trim the line in place
        trim(bufferedLine_);
        // Tokenize the line
        tokenize(bufferedLine_);
        m_isValid = true;  // Previous getline was successful
    } else {
        // No more lines, clear the buffer
        bufferedLine_.clear();
        m_vTokens.clear();
        m_isValid = false;  // Previous getline failed
    }
}

// -----------------------------------------------------------------------------

bool
InputFileParser::isValid() const {
    return m_isValid;
}

// -----------------------------------------------------------------------------

size_t
InputFileParser::getLineNumber() const {
    return lineNumber_;
}

// -----------------------------------------------------------------------------

const std::string&
InputFileParser::getLine() const {
    return bufferedLine_;
}

// -----------------------------------------------------------------------------

const std::vector<std::string>&
InputFileParser::getTokens() const {
    return m_vTokens;
}

// -----------------------------------------------------------------------------

void
InputFileParser::trim(std::string& str) const {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        str.clear();
        return;
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    str = str.substr(first, (last - first + 1));
}

// -----------------------------------------------------------------------------

void
InputFileParser::tokenize(const std::string& str) {
    m_vTokens.clear();
    
    if (str.empty()) {
        return;
    }
    
    size_t start = 0;
    size_t pos = 0;
    
    while (pos < str.length()) {
        // Skip spaces and tabs
        if (str[pos] == ' ' || str[pos] == '\t') {
            pos++;
            continue;
        }
        
        // Found start of a token
        start = pos;
        // Find end of token (next space/tab or end of string)
        while (pos < str.length() && str[pos] != ' ' && str[pos] != '\t') {
            pos++;
        }
        
        // Extract token
        if (start < pos) {
            m_vTokens.push_back(str.substr(start, pos - start));
        }
    }
}

} // namespace Input

