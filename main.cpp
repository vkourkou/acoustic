#include <iostream>
#include <fstream>
#include <InputFileParser.h>

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

    
    
    // Iterate through lines one at a time
    // First line is already read during construction
    size_t lineCount = 0;
    for (InputFileParser parser(file); parser.isValid(); ++parser) {
        const std::string& line = parser.getLine();
        std::cout << "Line " << parser.getLineNumber() << ": " << line << std::endl;
        lineCount++;
    }
    
    std::cout << "=== Total lines: " << lineCount << " ===" << std::endl;
    
    return 0;
}



