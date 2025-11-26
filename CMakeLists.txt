cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(AcousticProject
    VERSION 1.0.0
    LANGUAGES CXX CUDA
    DESCRIPTION "C++ Project with Input File Parser"
)

# Ensure C++17 and CUDA runtime settings match
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CUDA_STANDARD 17)

# Build options
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Compiler-specific options
if(MSVC)
    # Set output directories
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
else()
    # GCC/Clang compiler flags
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Source files
set(SOURCES
    main.cpp
    Runner.cpp
    InputCnt.cpp
    InputFileParser.cpp
    InputCompiler.cpp
    Utilities.cpp
    Grid.cpp
    Simulator.cpp
)

# CUDA source files
set(CUDA_SOURCES
    add_arrays.cu
)

# Header files
set(HEADERS
    Runner.h
    InputCnt.h
    InputFileParser.h
    InputCompiler.h
    Utilities.h
    Grid.h
    Simulator.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${CUDA_SOURCES})

# Include headers from current directory
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Add compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)

# Compiler-specific options for C++ only (not CUDA)
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:/W4 /permissive- /wd4996>
    )
endif()

# Required for MSVC + CUDA to play nice together
set_target_properties(${PROJECT_NAME} PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    CUDA_ARCHITECTURES 90  # Adjust: 75=Turing, 80=Ampere, 86=Ampere (laptop), 89=Ada, 90=Hopper
    CUDA_RESOLVE_DEVICE_SYMBOLS ON
)

# Set CUDA host compiler to match C++ compiler
if(MSVC)
    set(CMAKE_CUDA_HOST_COMPILER ${CMAKE_CXX_COMPILER} CACHE FILEPATH "CUDA host compiler" FORCE)
endif()


# Set target properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "acoustic"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/RelWithDebInfo/bin"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/MinSizeRel/bin"
)

# Compiler options - simplified approach matching working project
if(MSVC)
    # C++ compiler options
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:/MD /EHsc>
    )
    
    # CUDA compiler options - match working project exactly
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<COMPILE_LANGUAGE:CUDA>:--compiler-options "/MD /EHsc">
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-g -O0 -DDEBUG>
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
        $<$<CONFIG:RelWithDebInfo>:-O2 -g>
        $<$<CONFIG:MinSizeRel>:-Os -DNDEBUG>
    )
endif()

# Installation (optional)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Google Test setup
include(FetchContent)

# Try to download from GitHub release archive (doesn't require Git)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
    URL_HASH SHA256=8ad598c73ad796e0d8280b082cebd82a630d73e73cd3c70057938a6501bba5d7
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Only enable testing if Google Test is available
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Find all test files (files ending with _test.cpp)
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_SOURCE_DIR}/*_test.cpp")

# Create a single unit test executable
add_executable(ut
    ${TEST_SOURCES}
    InputFileParser.cpp
    InputCompiler.cpp
    Utilities.cpp
    Grid.cpp
)

target_link_libraries(ut
    PRIVATE
    GTest::gtest
    GTest::gtest_main
)

# Set output name to ut.exe on all platforms
set_target_properties(ut PROPERTIES
    OUTPUT_NAME "ut"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/RelWithDebInfo/bin"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/MinSizeRel/bin"
)

# Add test to ctest
add_test(NAME UnitTests COMMAND ut)

# Print configuration info
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")

