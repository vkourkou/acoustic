#include <iostream>
#include <fstream>
#include <Runner.h>

int
main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Failed to open input file: " << argv[1] << std::endl;
        return 1;
    }

    Runner runner;
    bool boolResult = runner.run(file);
   
    return boolResult ? 0 : 1   ;
}



