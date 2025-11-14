#ifndef INPUT_COMPILER_H
#define INPUT_COMPILER_H

#include <string>
#include <istream>
#include <vector>
#include <tuple>
#include <InputFileParser.h>
#include <Utilities.h>

namespace Input {

// Enumerator for statement types
enum class StatementType {
    SOURCE = 0,
    BBOX = 1,
    VELOCITY = 2,
    MAX = 3
};

std::string getString(StatementType type);

StatementType getStatementType(const std::string& str);

// Source statement class
class SourceStatement {
public:
    SourceStatement() = default;
    ~SourceStatement() = default;
    
    bool process(const std::vector<std::string>& tokens);
    
    Frequency_t getFreq() const;
    Amplitude_t getAmplitude() const;
    
    bool isValid() const;

private:
    Frequency_t m_Freq{0};
    Amplitude_t m_Amplitude{0};
};

// BBox statement class
class BBoxStatement {
public:
    BBoxStatement() = default;
    ~BBoxStatement() = default;
    
    bool process(const std::vector<std::string>& tokens);
    
    Dimension_t getXMin() const;
    Dimension_t getXMax() const;
    Dimension_t getYMin() const;
    Dimension_t getYMax() const;
    
    bool isValid() const;

private:
    Dimension_t m_XMin{0};
    Dimension_t m_XMax{0};
    Dimension_t m_YMin{0};
    Dimension_t m_YMax{0};
};

// Velocity statement class
class VelocityStatement {
public:
    VelocityStatement() = default;
    ~VelocityStatement() = default;
    
    bool process(const std::vector<std::string>& tokens);
    
    Velocity_t getVelocity() const;
    
    bool isValid() const;

private:
    Velocity_t m_Velocity{0};
};

// Class to hold a tuple of SourceStatement, BBoxStatement, and VelocityStatement
class StatementCnt {
    std::tuple<SourceStatement, BBoxStatement, VelocityStatement> m_statements;
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
    // Returns the StatementType if successfully processed, StatementType::MAX otherwise
    StatementType processLine(const std::vector<std::string>& tokens);

    // Get a const reference to a statement based on StatementType
    template<StatementType type>
    constexpr const auto& getStatement() const {
        return m_statementCnt.get<type>();
    }
    
private:
    StatementCnt m_statementCnt;
};

class FileParser {
public:
    explicit FileParser(std::istream& stream);
    ~FileParser() = default;

    class const_iterator {
    public:
        explicit const_iterator(FileParser& parser);
        ~const_iterator() = default;

        bool isValid() const;
        void operator++();
        StatementType getType() const;
        size_t getLineNumber() const;
        const std::string& getLine() const;

        template<StatementType type>
        constexpr const auto& getStatement() const {
            return m_CompilerRef.getStatement<type>();
        }

    private:
        InputFileTokenizer& m_Tokenizer;
        InputCompiler& m_CompilerRef;
        StatementType m_Type;

        void compileCurrentLine();
    };

private:
    InputFileTokenizer m_Tokeninzer;
    InputCompiler m_Compiler;
};

// -----------------------------------------------------------------------------

} // namespace Input

#endif // INPUT_COMPILER_H

