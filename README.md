# C++ Project

A simple C++ project with an input file parser.

## Building

### Windows Native (Recommended for Windows)

#### Using CMake with Batch Script

The easiest way on Windows:

```cmd
build.bat
```

Build in Debug mode:

```cmd
build.bat debug
```

Build in Release mode:

```cmd
build.bat release
```

Clean build artifacts:

```cmd
build.bat clean
```

Rebuild:

```cmd
build.bat rebuild
```

For Visual Studio projects:

```cmd
build.bat vs2022
build.bat vs2019
```

#### Using CMake Directly

Configure and build:

```cmd
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

Or generate Visual Studio solution:

```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

Then open `build/AcousticProject.sln` in Visual Studio.

#### Quick Build (Direct MSVC)

For quick compilation without CMake:

```cmd
build_msvc.bat
```

**Note:** Requires Visual Studio Developer Command Prompt.

### Linux/macOS/WSL (Using Makefile)

Build the project:

```bash
make
```

Or build in Debug mode:

```bash
make debug
```

Or build in Release mode:

```bash
make release
```

Run the program:

```bash
make run ARGS="input_file.txt"
```

Other useful commands:

```bash
make clean      # Remove build artifacts
make rebuild    # Clean and rebuild
make help       # Show all available targets
```

### Manual Compilation

Alternatively, compile manually:

**Linux/macOS:**
```bash
g++ -std=c++11 main.cpp InputFileParser.cpp -o program
```

**Windows (MSVC):**
```cmd
cl /std:c++11 /EHsc main.cpp InputFileParser.cpp /Fe:program.exe
```

**Windows (MinGW):**
```cmd
g++ -std=c++11 main.cpp InputFileParser.cpp -o program.exe
```

## Usage

```bash
./program <input_file>
```

The program will read the input file, parse its contents, and display them.

## Testing

The project includes unit tests using Google Test framework.

### Running Tests

After building the project with CMake, you can run the tests:

**Windows:**
```cmd
cd build
cmake --build . --config Release
ctest --build-config Release
```

Or run the test executable directly:
```cmd
build\Release\bin\ut.exe
```

**Linux/macOS:**
```bash
cd build
cmake --build .
ctest
```

Or run the test executable directly:
```bash
./build/bin/ut
```

### Test Coverage

The unit tests cover:
- Basic line reading functionality
- Multiple lines parsing
- Empty lines handling
- Whitespace trimming
- Line number tracking
- End of stream behavior
- Stream validity checking
- Special characters handling
- Edge cases (empty streams, whitespace-only lines)

The tests are located in `InputFileParser_test.cpp`.

## Project Structure

- `main.cpp` - Main entry point
- `InputFileParser.h` - Header file for the InputFileParser class
- `InputFileParser.cpp` - Implementation of the InputFileParser class
- `InputFileParser_test.cpp` - Unit tests for InputFileParser (Google Test)
- `CMakeLists.txt` - CMake build configuration (cross-platform, Windows native)
- `Makefile` - Makefile for Unix-like systems (Linux, macOS, WSL)
- `build.bat` - Windows batch script for building with CMake
- `build_msvc.bat` - Simple Windows batch script for direct MSVC compilation
- `build/` - Build directory (created automatically, contains object files and binaries)

## Build System Features

### CMake (Windows/Cross-platform)
- **Visual Studio Integration** - Generates `.sln` and `.vcxproj` files
- **Automatic dependency tracking** - Recompiles only what's needed
- **Debug and Release builds** - Separate build configurations
- **Organized build structure** - Object files and binaries in separate directories
- **Cross-platform** - Works on Windows, Linux, and macOS

### Makefile (Unix-like systems)
- **Automatic dependency tracking** - Recompiles only what's needed
- **Debug and Release builds** - Separate build configurations
- **Organized build structure** - Object files and binaries in separate directories
- **Works on Unix-like systems** (Linux, macOS, WSL, MinGW/MSYS2)

