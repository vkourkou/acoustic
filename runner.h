#ifndef RUNNER_H
#define RUNNER_H

#include <istream>
#include <input_compiler.h>
#include <input_cnt.h>

class Runner {
public:
    Runner() = default;
    ~Runner() = default;
    
    bool run(std::istream& IS);

private:
    bool parseInput(std::istream& IS);
    bool execute();
    bool createDBfolder();
    input::InputCnt m_InputCnt;
    static std::string getDbPathName() { return "acoustic.db"; }
};

#endif // RUNNER_H
