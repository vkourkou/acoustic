#include <InputCompiler.h>
#include <cctype>
#include <stdexcept>
#include <iostream>

namespace Input {

// -----------------------------------------------------------------------------

std::string
getString(StatementType type) {
    switch (type) {
        case StatementType::SOURCE: return "Source";
        case StatementType::BBOX: return "BBox";
        case StatementType::MAX: return "Max";
        default: return "INVALID";
    }
}

// -----------------------------------------------------------------------------

StatementType
getStatementType(const std::string& str) {
    if (Utilities::caseInsensitiveEquals(str, "Source")) {
        return StatementType::SOURCE;
    }
    else if (Utilities::caseInsensitiveEquals(str, "BBox")) {
        return StatementType::BBOX;
    }
    else {
        return StatementType::MAX;
    }
}
// -----------------------------------------------------------------------------

InputCompiler::InputCompiler() {
}

// -----------------------------------------------------------------------------

StatementType
InputCompiler::processLine(const std::vector<std::string>& tokens) {
    // If no tokens, return MAX
    if (tokens.empty()) {
        return StatementType::MAX;
    }
    
    // Check if first token is a comment (starts with //)
    const std::string& firstToken = tokens[0];
    if (firstToken.length() >= 2 && firstToken[0] == '/' && firstToken[1] == '/') {
        // It's a comment, return MAX
        return StatementType::MAX;
    }
    
    // Get StatementType from first token
    StatementType type = getStatementType(firstToken);
    
    // If invalid type, return MAX
    if (type == StatementType::MAX) {
        return StatementType::MAX;
    }
    
    // Process tokens using the appropriate statement type
    bool success = false;
    if (type == StatementType::SOURCE) {
        success = m_statementCnt.process<StatementType::SOURCE>(tokens);
    }
    else if (type == StatementType::BBOX) {
        success = m_statementCnt.process<StatementType::BBOX>(tokens);
    }
    
    // If processing failed, return MAX
    if (!success) {
        return StatementType::MAX;
    }
    
    // Return the successfully processed type
    return type;
}

// -----------------------------------------------------------------------------

FileParser::FileParser(std::istream& stream)
    :
    m_Tokeninzer(stream),
    m_Compiler()
{
}

// -----------------------------------------------------------------------------

FileParser::const_iterator::const_iterator(FileParser& parser)
    : m_Tokenizer(parser.m_Tokeninzer),
      m_CompilerRef(parser.m_Compiler),
      m_Type(StatementType::MAX) {
    compileCurrentLine();
}

// -----------------------------------------------------------------------------

bool
FileParser::const_iterator::isValid() const {
    return m_Tokenizer.isValid();
}

// -----------------------------------------------------------------------------

StatementType
FileParser::const_iterator::getType() const {
    return m_Type;
}

// -----------------------------------------------------------------------------

size_t
FileParser::const_iterator::getLineNumber() const {
    return m_Tokenizer.getLineNumber();
}

// -----------------------------------------------------------------------------

const std::string&
FileParser::const_iterator::getLine() const {
    return m_Tokenizer.getLine();
}

// -----------------------------------------------------------------------------

void
FileParser::const_iterator::operator++() {
    ++m_Tokenizer;
    compileCurrentLine();
}

// -----------------------------------------------------------------------------

void
FileParser::const_iterator::compileCurrentLine() {
    if (!m_Tokenizer.isValid()) {
        m_Type = StatementType::MAX;
        return;
    }

    const std::vector<std::string>& tokens = m_Tokenizer.getTokens();
    m_Type = m_CompilerRef.processLine(tokens);
}

// -----------------------------------------------------------------------------

bool
SourceStatement::process(const std::vector<std::string>& tokens) {
    // Must have exactly 5 tokens: 1 for "Source" and 2 pairs (4 tokens)
    if (tokens.size() != 5) {
        return false;
    }
    
    // Check if first token is "Source" (case insensitive)
    if (!Utilities::caseInsensitiveEquals(tokens[0], "Source")) {
        return false;
    }

    // Process tokens in pairs starting from index 1
    for (size_t i = 1; i < tokens.size(); i += 2) {
        // Need at least two tokens for a pair
        
        const std::string& key = tokens[i];
        const std::string& value = tokens[i + 1];
        
        // Check for Freq or Frequency
        if (Utilities::caseInsensitiveEquals(key, "Frequency")) {
            auto convertedValue = Utilities::convertTo<Frequency_t>(value);
            if (convertedValue.has_value()) {
                m_Freq = convertedValue.value();
            }
        }
        // Check for Amplitude
        else if (Utilities::caseInsensitiveEquals(key, "Amplitude")) {
            auto convertedValue = Utilities::convertTo<Amplitude_t>(value);
            if (convertedValue.has_value()) {
                m_Amplitude = convertedValue.value();
            }
        }
    }
    
    return isValid();
}

// -----------------------------------------------------------------------------

bool
SourceStatement::isValid() const
{
    // Check that frequency is positive
    if (m_Freq <= 0.0f) {
        std::cout << "Invalid Source: Frequency (" << m_Freq << ") must be positive" << std::endl;
        return false;
    }
    
    return true;
}

// -----------------------------------------------------------------------------

Frequency_t
SourceStatement::getFreq() const
{
    return m_Freq;
}

// -----------------------------------------------------------------------------

Amplitude_t
SourceStatement::getAmplitude() const
{
    return m_Amplitude;
}

// -----------------------------------------------------------------------------

bool
BBoxStatement::process(const std::vector<std::string>& tokens) {
    // Check if there's at least one token
    // Must have exactly 9 tokens: 1 for "BBox" and 4 pairs (8 tokens)
    if (tokens.size() != 9) {
        return false;
    }   
    
    // Check if first token is "BBox" (case insensitive)
    if (!Utilities::caseInsensitiveEquals(tokens[0], "BBox")) {
        return false;
    }

    // Process tokens in pairs starting from index 1
    for (size_t i = 1; i < tokens.size(); i += 2) {
        // Need at least two tokens for a pair
        if (i + 1 >= tokens.size()) {
            break;
        }
        
        const std::string& key = tokens[i];
        const std::string& value = tokens[i + 1];
        
        // Check for XMin
        if (Utilities::caseInsensitiveEquals(key, "XMin")) {
            auto convertedValue = Utilities::convertTo<Dimension_t>(value);
            if (convertedValue.has_value()) {
                m_XMin = convertedValue.value();
            }
        }
        // Check for XMax
        else if (Utilities::caseInsensitiveEquals(key, "XMax")) {
            auto convertedValue = Utilities::convertTo<Dimension_t>(value);
            if (convertedValue.has_value()) {
                m_XMax = convertedValue.value();
            }
        }
        // Check for YMin
        else if (Utilities::caseInsensitiveEquals(key, "YMin")) {
            auto convertedValue = Utilities::convertTo<Dimension_t>(value);
            if (convertedValue.has_value()) {
                m_YMin = convertedValue.value();
            }
        }
        // Check for YMax
        else if (Utilities::caseInsensitiveEquals(key, "YMax")) {
            auto convertedValue = Utilities::convertTo<Dimension_t>(value);
            if (convertedValue.has_value()) {
                m_YMax = convertedValue.value();
            }
        }
    }
    
    return isValid();
}

// -----------------------------------------------------------------------------

bool
BBoxStatement::isValid() const
{
    bool valid = true;
    
    // Check XMin < XMax
    if (m_XMin >= m_XMax) {
        std::cout << "Invalid BBox: XMin (" << m_XMin << ") >= XMax (" << m_XMax << ")" << std::endl;
        valid = false;
    }
    
    // Check YMin < YMax
    if (m_YMin >= m_YMax) {
        std::cout << "Invalid BBox: YMin (" << m_YMin << ") >= YMax (" << m_YMax << ")" << std::endl;
        valid = false;
    }
    
    return valid;
}

// -----------------------------------------------------------------------------

Dimension_t
BBoxStatement::getXMin() const
{
    return m_XMin;
}

// -----------------------------------------------------------------------------

Dimension_t
BBoxStatement::getXMax() const
{
    return m_XMax;
}

// -----------------------------------------------------------------------------

Dimension_t
BBoxStatement::getYMin() const
{
    return m_YMin;
}

// -----------------------------------------------------------------------------

Dimension_t
BBoxStatement::getYMax() const
{
    return m_YMax;
}

} // namespace Input

