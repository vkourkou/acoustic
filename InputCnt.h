#ifndef INPUT_CNT_H
#define INPUT_CNT_H

#include <InputCompiler.h>
#include <tuple>
#include <type_traits>
#include <ostream>
#include <utility>
#include <vector>

namespace input {

class InputCnt {
private:
    using StatementsTuple = std::tuple<Input::SourceStatement, Input::BBoxStatement, 
                                       Input::WallStatement, Input::VelocityStatement, 
                                       Input::MaxResolutionStatement, Input::SimulationParamStatement>;
    StatementsTuple m_statements;
    std::vector<Input::WallStatement> m_walls;

    template<typename T>
    constexpr T& getNonConst() {
        return const_cast<T&>(static_cast<const InputCnt*>(this)->get<T>());
    }

    template<std::size_t... I>
    void saveImpl(std::ostream& OS, std::index_sequence<I...>) const {
        ((std::get<I>(m_statements).save(OS), OS << "\n"), ...);
    }
    
public:
    InputCnt() = default;
    ~InputCnt() = default;
    
    // Template function to set statement based on statement class type
    template<typename T>
    bool set(const T& statement) {
        getNonConst<T>() = statement;
        return true;
    }
    
    template<typename T>
    constexpr const T& get() const {
        static_assert(
            std::is_same_v<T, Input::SourceStatement> ||
            std::is_same_v<T, Input::BBoxStatement> ||
            std::is_same_v<T, Input::VelocityStatement> ||
            std::is_same_v<T, Input::MaxResolutionStatement> ||
            std::is_same_v<T, Input::SimulationParamStatement>,
            "T must be one of the statement types in the tuple"
        );
        //We shhould not be using the wallstatement from here. We should have a vector of walls.
        
        constexpr Input::StatementType type = Input::getStatementType<T>();
        return std::get<static_cast<size_t>(type)>(m_statements);
    }

    void appendWall(const Input::WallStatement& wall);
    
    const std::vector<Input::WallStatement>& getWalls() const;

    //The timestep is purely dictated by the MaxResolution statement and the frequency of the source
    Time_t computeTimeStep() const;

    //The spatial step is purely dictated by the MaxResolution statement and wave length (velocity/frequency)
    Dimension_t computeSpatialStep() const;

    Dimension_t computeWavelength() const;
    
    bool isSane() const;
    
    void save(std::ostream& OS) const;

};

} // namespace input

#endif // INPUT_CNT_H

