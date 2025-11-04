#ifndef INPUT_COMPILER_H
#define INPUT_COMPILER_H

#include <string>
#include <vector>
#include <tuple>
#include <Utilities.h>

namespace Input {

// Enumerator for statement types
enum class StatementType {
    GRID = 0,
    BBOX = 1,
    MAX = 2
};

// Grid statement class
class GridStatement {
public:
    GridStatement() = default;
    ~GridStatement() = default;
    
    bool process(const std::vector<std::string>& tokens);
};

// BBox statement class
class BBoxStatement {
public:
    BBoxStatement() = default;
    ~BBoxStatement() = default;
    
    bool process(const std::vector<std::string>& tokens);
};

// Class to hold a tuple of GridStatement and BBoxStatement
class StatementCnt {
    std::tuple<GridStatement, BBoxStatement> m_statements;
public:
    StatementCnt() = default;
    ~StatementCnt() = default;
    
    // Get a const reference to a tuple element based on StatementType
    template<StatementType type>
    constexpr const auto& get() const {
        return std::get<static_cast<size_t>(type)>(m_statements);
    }
    
    // Process tokens by delegating to the appropriate statement type
    template<StatementType type>
    bool process(const std::vector<std::string>& tokens) {
        return std::get<static_cast<size_t>(type)>(m_statements).process(tokens);
    }
};

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

} // namespace Input

#endif // INPUT_COMPILER_H

