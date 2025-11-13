#ifndef INPUT_CNT_H
#define INPUT_CNT_H

#include <InputCompiler.h>

namespace input {

class InputCnt {
public:
    InputCnt() = default;
    ~InputCnt() = default;
    bool setSource(const Input::SourceStatement& Source);
    bool setBBox(const Input::BBoxStatement& BBox);
    
private:
    Input::SourceStatement m_Source;
    Input::BBoxStatement m_BBox;
};

} // namespace input

#endif // INPUT_CNT_H

