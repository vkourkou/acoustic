#include <InputCnt.h>

namespace input {

// -----------------------------------------------------------------------------

void
InputCnt::save(std::ostream& OS) const
{
    saveImpl(OS, std::make_index_sequence<std::tuple_size_v<StatementsTuple>>{});
}

// -----------------------------------------------------------------------------

} // namespace input