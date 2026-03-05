#include <input_file_parser.h>
#include <cstdlib>

namespace Input {

InputFileTokenizer::InputFileTokenizer(std::istream& stream)
    : stream_(stream), lineNumber_(0), m_isValid(false) {
    // Read the first line during construction
    operator++();
}

// -----------------------------------------------------------------------------

void
InputFileTokenizer::operator++() {
    // Check both isValid() and stream_.good() before doing anything
    // Allow first read if lineNumber_ is 0 (initial state, haven't read yet)
    if (!stream_.good() || (!isValid() && lineNumber_ > 0)) {
        bufferedLine_.clear();
        m_vTokens.clear();
        m_isValid = false;
        return;
    }
    
    // Keep reading lines until we find a non-empty line or reach EOF
    while (std::getline(stream_, bufferedLine_)) {
        lineNumber_++;  // Increment line number for every line read (including empty lines)
        // Trim the line in place
        trim(bufferedLine_);
        
        // If line is not empty, process it and return
        if (!bufferedLine_.empty()) {
            // Tokenize the line
            tokenize(bufferedLine_);
            m_isValid = true;  // Previous getline was successful
            return;
        }
        // If line is empty, continue to next line
    }
    
    // No more lines, clear the buffer
    bufferedLine_.clear();
    m_vTokens.clear();
    m_isValid = false;  // Previous getline failed
}

// -----------------------------------------------------------------------------

bool
InputFileTokenizer::isValid() const {
    return m_isValid;
}

// -----------------------------------------------------------------------------

size_t
InputFileTokenizer::getLineNumber() const {
    return lineNumber_;
}

// -----------------------------------------------------------------------------

const std::string&
InputFileTokenizer::getLine() const {
    return bufferedLine_;
}

// -----------------------------------------------------------------------------

const std::vector<std::string>&
InputFileTokenizer::getTokens() const {
    return m_vTokens;
}

// -----------------------------------------------------------------------------

void
InputFileTokenizer::trim(std::string& str) const {
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
InputFileTokenizer::tokenize(const std::string& str) {
    m_vTokens.clear();
    
    if (str.empty()) {
        return;
    }
    
    // Check if line starts with "//" - if so, make "//" by itself a token
    // and add the rest of the line as another token
    if (str.length() >= 2 && str[0] == '/' && str[1] == '/') {
        m_vTokens.push_back("//");
        // Add the rest of the line (after "//") as a token
        if (str.length() > 2) {
            std::string restOfLine = str.substr(2);
            // Trim leading whitespace from the rest of the line
            size_t first = restOfLine.find_first_not_of(" \t");
            if (first != std::string::npos) {
                restOfLine = restOfLine.substr(first);
            } else {
                restOfLine.clear();
            }
            if (!restOfLine.empty()) {
                m_vTokens.push_back(restOfLine);
            }
        }
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
    unwrapEnvironmentVariablesForTokens();
}

// -----------------------------------------------------------------------------

void
InputFileTokenizer::unwrapEnvironmentVariablesForTokens() {
    for (auto& token : m_vTokens) {
        if (!token.empty() && token[0] == '$') {
            const char* envValue = std::getenv(token.substr(1).c_str());
            if (envValue != nullptr) {
                token = std::string(envValue);
            } else {
                // Environment variable not found, leave token as is or set to empty?
                // For now, setting to empty string
                token = std::string();
            }
        }
    }
}

// -----------------------------------------------------------------------------

} // namespace Input

