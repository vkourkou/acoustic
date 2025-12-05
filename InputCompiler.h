#ifndef INPUT_COMPILER_H
#define INPUT_COMPILER_H

#include <string>
#include <istream>
#include <ostream>
#include <vector>
#include <tuple>
#include <limits>
#include <InputFileParser.h>
#include <Utilities.h>

namespace Input {

// Enumerator for statement types
enum class StatementType {
    SOURCE = 0,
    BBOX = 1,
    WALL = 2,
    VELOCITY = 3,
    MAXRESOLUTION = 4,
    SIMULATIONPARAM = 5,
    COMMENT = 6,
    MAX = 7
};

std::string getString(StatementType type);

StatementType getStatementType(const std::string& str);

// Template function to get StatementType from statement class type
template<typename StatementClass>
constexpr StatementType getStatementType();

// Source statement class
class SourceStatement {
public:
    SourceStatement() = default;
    ~SourceStatement() = default;
    
    bool process(const std::vector<std::string>& tokens);
    
    Frequency_t getFreq() const;
    Amplitude_t getAmplitude() const;
    
    Amplitude_t getValue(Time_t t) const;
    
    bool isValid() const;
    
    void save(std::ostream& OS) const;

    Dimension_t getX() const {
        return m_X;
    }
    Dimension_t getY() const {
        return m_Y;
    }
    Time_t getDuration() const {
        return m_Duration;
    }
private:
    Frequency_t m_Freq{0};
    Amplitude_t m_Amplitude{0};
    Dimension_t m_X{0};
    Dimension_t m_Y{0};
    Time_t m_Duration{std::numeric_limits<Time_t>::max()};
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
    
    bool isPointStrictlyInside(Dimension_t X, Dimension_t Y) const;
    
    void save(std::ostream& OS) const;

private:
    Dimension_t m_XMin{0};
    Dimension_t m_XMax{0};
    Dimension_t m_YMin{0};
    Dimension_t m_YMax{0};
};

// Wall statement class
class WallStatement {
public:
    WallStatement() = default;
    ~WallStatement() = default;
    
    bool process(const std::vector<std::string>& tokens);
    
    Dimension_t getXMin() const;
    Dimension_t getXMax() const;
    Dimension_t getYMin() const;
    Dimension_t getYMax() const;
    
    bool isValid() const;
    
    bool isPointStrictlyInside(Dimension_t X, Dimension_t Y) const;
    
    void save(std::ostream& OS) const;

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
    
    void save(std::ostream& OS) const;

private:
    Velocity_t m_Velocity{0};
};

// MaxResolution statement class
class MaxResolutionStatement {
public:
    MaxResolutionStatement() = default;
    ~MaxResolutionStatement() = default;
    
    bool process(const std::vector<std::string>& tokens);
    
    float getSpatial() const;
    float getTemporal() const;
    
    bool isValid() const;
    
    void save(std::ostream& OS) const;

private:
    //Courant number Sc = velocity * timestep / spatial_step
    //Sc = wavelength * frequency * timestep / spatial_step
    //Sc = (timestep / period) / (spatial_step / wavelenght)
    //Forstability Sc < 1
    //Hence (timestep / period) < (spatial_step / wavelenght)
    //That mean the time resolution should be smaller than the spatial resolution.
    float m_Spatial{0};
    float m_Temporal{0};
};

// -----------------------------------------------------------------------------

// SimulationParam statement class
class SimulationParamStatement {
public:
    SimulationParamStatement() = default;
    ~SimulationParamStatement() = default;
    
    bool process(const std::vector<std::string>& tokens);
    
    size_t getMaxIteration() const;
    size_t getBatchSize() const;
    ProcessingType getProcessingType() const;
    
    bool isValid() const;
    
    void save(std::ostream& OS) const;

private:
    size_t m_MaxIteration{0};
    size_t m_BatchSize{100};
    ProcessingType m_PT{CPU};
};

// Class to hold a tuple of SourceStatement, BBoxStatement, WallStatement, VelocityStatement, MaxResolutionStatement, and SimulationParamStatement
class StatementCnt {
    std::tuple<SourceStatement, BBoxStatement, WallStatement, VelocityStatement, MaxResolutionStatement, SimulationParamStatement> m_statements;
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


// Template specializations for getStatementType<StatementClass>()
template<>
inline constexpr StatementType
getStatementType<SourceStatement>() {
    return StatementType::SOURCE;
}

template<>
inline constexpr StatementType
getStatementType<BBoxStatement>() {
    return StatementType::BBOX;
}

template<>
inline constexpr StatementType
getStatementType<WallStatement>() {
    return StatementType::WALL;
}

template<>
inline constexpr StatementType
getStatementType<VelocityStatement>() {
    return StatementType::VELOCITY;
}

template<>
inline constexpr StatementType
getStatementType<MaxResolutionStatement>() {
    return StatementType::MAXRESOLUTION;
}

template<>
inline constexpr StatementType
getStatementType<SimulationParamStatement>() {
    return StatementType::SIMULATIONPARAM;
}

// -----------------------------------------------------------------------------

} // namespace Input

#endif // INPUT_COMPILER_H

