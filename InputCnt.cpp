#include <InputCnt.h>

namespace input {

// -----------------------------------------------------------------------------

bool
InputCnt::setSource(const Input::SourceStatement& Source) {
    m_Source = Source;
    return true;
}

// -----------------------------------------------------------------------------

bool
InputCnt::setBBox(const Input::BBoxStatement& BBox) {
    m_BBox = BBox;
    return true;
}

// -----------------------------------------------------------------------------

} // namespace input