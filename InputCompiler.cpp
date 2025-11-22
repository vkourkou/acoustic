#include <InputCompiler.h>
#include <cctype>
#include <stdexcept>
#include <iostream>
#include <cmath>

namespace Input {

// -----------------------------------------------------------------------------

std::string
getString(StatementType type) {
    switch (type) {
        case StatementType::SOURCE: return "Source";
        case StatementType::BBOX: return "BBox";
        case StatementType::VELOCITY: return "Velocity";
        case StatementType::MAXRESOLUTION: return "MaxResolution";
        case StatementType::COMMENT: return "//";
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
    else if (Utilities::caseInsensitiveEquals(str, "Velocity")) {
        return StatementType::VELOCITY;
    }
    else if (Utilities::caseInsensitiveEquals(str, "MaxResolution")) {
        return StatementType::MAXRESOLUTION;
    }
    else if (str == "//") {
        return StatementType::COMMENT;
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
    
    // Get StatementType from first token
    const std::string& firstToken = tokens[0];
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
    else if (type == StatementType::VELOCITY) {
        success = m_statementCnt.process<StatementType::VELOCITY>(tokens);
    }
    else if (type == StatementType::MAXRESOLUTION) {
        success = m_statementCnt.process<StatementType::MAXRESOLUTION>(tokens);
    }
    else if (type == StatementType::COMMENT) {
        // Comments don't need processing, they're always valid
        success = true;
    }
    else {
        success = false;
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
    // Must have 9 tokens (1 for "Source" and 4 pairs) or 11 tokens (with optional Duration)
    if (tokens.size() != 9 && tokens.size() != 11) {
        return false;
    }
    
    // Check if first token is "Source" (case insensitive)
    if (!Utilities::caseInsensitiveEquals(tokens[0], "Source")) {
        return false;
    }

    // Process tokens in pairs starting from index 1
    for (size_t i = 1; i < tokens.size(); i += 2) {
        // Need at least two tokens for a pair
        if (i + 1 >= tokens.size()) {
            return false;
        }
        
        const std::string& key = tokens[i];
        const std::string& value = tokens[i + 1];
        
        // Check for Freq or Frequency
        if (Utilities::caseInsensitiveEquals(key, "Frequency")) {
            auto convertedValue = Utilities::convertTo<Frequency_t>(value);
            if (convertedValue.has_value()) {
                m_Freq = convertedValue.value();
            }
            else {
                return false;
            }
        }
        // Check for Amplitude
        else if (Utilities::caseInsensitiveEquals(key, "Amplitude")) {
            auto convertedValue = Utilities::convertTo<Amplitude_t>(value);
            if (convertedValue.has_value()) {
                m_Amplitude = convertedValue.value();
            }
            else {
                return false;
            }
        }
        // Check for X
        else if (Utilities::caseInsensitiveEquals(key, "X")) {
            auto convertedValue = Utilities::convertTo<Dimension_t>(value);
            if (convertedValue.has_value()) {
                m_X = convertedValue.value();
            }
            else {
                return false;
            }
        }
        // Check for Y
        else if (Utilities::caseInsensitiveEquals(key, "Y")) {
            auto convertedValue = Utilities::convertTo<Dimension_t>(value);
            if (convertedValue.has_value()) {
                m_Y = convertedValue.value();
            }
            else {
                return false;
            }
        }
        // Check for Duration
        else if (Utilities::caseInsensitiveEquals(key, "Duration")) {
            auto convertedValue = Utilities::convertTo<Time_t>(value);
            if (convertedValue.has_value()) {
                m_Duration = convertedValue.value();
            }
            else {
                return false;
            }
        }
        else {
            return false;
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
    if (m_Duration <= 0.0f) {
        std::cout << "Invalid Source: Duration (" << m_Duration << ") must be positive" << std::endl;
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

static const float pi_float = 3.14159265358979323846f;

Amplitude_t
SourceStatement::getValue(Time_t t) const
{
    if (t > m_Duration) {
        return 0.0f;
    }
    return m_Amplitude * std::sin(2.0e0f * pi_float * m_Freq * t);
}

// -----------------------------------------------------------------------------

void
SourceStatement::save(std::ostream& OS) const
{
    OS << "Source Frequency " << m_Freq << " Amplitude " << m_Amplitude << " X " << m_X << " Y " << m_Y;
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
        else {
            return false;
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

// -----------------------------------------------------------------------------

bool
BBoxStatement::isPointStrictlyInside(Dimension_t X, Dimension_t Y) const
{
    return X > m_XMin && X < m_XMax && Y > m_YMin && Y < m_YMax;
}

// -----------------------------------------------------------------------------

void
BBoxStatement::save(std::ostream& OS) const
{
    OS << "BBox XMin " << m_XMin << " XMax " << m_XMax << " YMin " << m_YMin << " YMax " << m_YMax;
}

// -----------------------------------------------------------------------------

bool
VelocityStatement::process(const std::vector<std::string>& tokens) {
    // Must have exactly 2 tokens: "Velocity" and the value
    if (tokens.size() != 2) {
        return false;
    }
    
    // Check if first token is "Velocity" (case insensitive)
    if (!Utilities::caseInsensitiveEquals(tokens[0], "Velocity")) {
        return false;
    }
    
    // Convert the second token to double
    auto convertedValue = Utilities::convertTo<Velocity_t>(tokens[1]);
    if (convertedValue.has_value()) {
        m_Velocity = convertedValue.value();
    }
    else {
        return false;
    }
    
    return isValid();
}

// -----------------------------------------------------------------------------

bool
VelocityStatement::isValid() const
{
    // Velocity must be positive
    if (m_Velocity <= 0.0f) {
        std::cout << "Invalid Velocity: Velocity (" << m_Velocity << ") must be positive" << std::endl;
        return false;
    }
    
    return true;
}

// -----------------------------------------------------------------------------

Velocity_t
VelocityStatement::getVelocity() const
{
    return m_Velocity;
}

// -----------------------------------------------------------------------------

void
VelocityStatement::save(std::ostream& OS) const
{
    OS << "Velocity " << m_Velocity;
}

// -----------------------------------------------------------------------------

bool
MaxResolutionStatement::process(const std::vector<std::string>& tokens) {
    // Must have exactly 5 tokens: "MaxResolution", "Spatial", value, "Temporal", value
    if (tokens.size() != 5) {
        return false;
    }
    
    // Check if first token is "MaxResolution" (case insensitive)
    if (!Utilities::caseInsensitiveEquals(tokens[0], "MaxResolution")) {
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
        
        // Check for Spatial
        if (Utilities::caseInsensitiveEquals(key, "Spatial")) {
            auto convertedValue = Utilities::convertTo<float>(value);
            if (convertedValue.has_value()) {
                m_Spatial = convertedValue.value();
            }
        }
        // Check for Temporal
        else if (Utilities::caseInsensitiveEquals(key, "Temporal")) {
            auto convertedValue = Utilities::convertTo<float>(value);
            if (convertedValue.has_value()) {
                m_Temporal = convertedValue.value();
            }
        }
        else {
            return false;
        }
    }
    
    return isValid();
}

// -----------------------------------------------------------------------------

bool
MaxResolutionStatement::isValid() const
{
    // Both Spatial and Temporal must be positive
    if (m_Spatial <= 0.0f) {
        std::cout << "Invalid MaxResolution: Spatial (" << m_Spatial << ") must be positive" << std::endl;
        return false;
    }
    
    if (m_Temporal <= 0.0f) {
        std::cout << "Invalid MaxResolution: Temporal (" << m_Temporal << ") must be positive" << std::endl;
        return false;
    }

    if (m_Spatial <= m_Temporal) {
        std::cout << "Invalid MaxResolution: Spatial (" << m_Spatial << ") must be greater than Temporal (" << m_Temporal << ")" << std::endl;
        return false;
    }
    
    return true;
}

// -----------------------------------------------------------------------------

float
MaxResolutionStatement::getSpatial() const
{
    return m_Spatial;
}

// -----------------------------------------------------------------------------

float
MaxResolutionStatement::getTemporal() const
{
    return m_Temporal;
}

// -----------------------------------------------------------------------------

void
MaxResolutionStatement::save(std::ostream& OS) const
{
    OS << "MaxResolution Spatial " << m_Spatial << " Temporal " << m_Temporal;
}

// -----------------------------------------------------------------------------

} // namespace Input

