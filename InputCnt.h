#ifndef INPUT_CNT_H
#define INPUT_CNT_H

#include <InputCompiler.h>
#include <tuple>
#include <type_traits>
#include <ostream>
#include <utility>

namespace input {

class InputCnt {
private:
    using StatementsTuple = std::tuple<Input::SourceStatement, Input::BBoxStatement, 
                                       Input::VelocityStatement, Input::MaxResolutionStatement>;
    StatementsTuple m_statements;

    template<typename T>
    constexpr T& get() {
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
        get<T>() = statement;
        return true;
    }
    
    template<typename T>
    constexpr const T& get() const {
        static_assert(
            std::is_same_v<T, Input::SourceStatement> ||
            std::is_same_v<T, Input::BBoxStatement> ||
            std::is_same_v<T, Input::VelocityStatement> ||
            std::is_same_v<T, Input::MaxResolutionStatement>,
            "T must be one of the statement types in the tuple"
        );
        constexpr Input::StatementType type = Input::getStatementType<T>();
        return std::get<static_cast<size_t>(type)>(m_statements);
    }

    //The timestep is purely dictated by the MaxResolution statement and the frequency of the source
    Time_t computeTimeStep() const;

    //The spatial step is purely dictated by the MaxResolution statement and wave length (velocity/frequency)
    Dimension_t computeSpatialStep() const;
    
    void save(std::ostream& OS) const;

};

} // namespace input

#endif // INPUT_CNT_H

