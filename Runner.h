#ifndef RUNNER_H
#define RUNNER_H

#include <istream>
#include <InputCompiler.h>
#include <InputCnt.h>

class Runner {
public:
    Runner() = default;
    ~Runner() = default;
    
    bool run(std::istream& IS);

private:
    bool parseInput(std::istream& IS);
    input::InputCnt m_InputCnt;

};

#endif // RUNNER_H

