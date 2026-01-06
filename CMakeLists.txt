cmake_minimum_required(VERSION 3.20)

project(AcousticProject LANGUAGES C CXX CUDA)

find_package(CUDAToolkit REQUIRED)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CUDA_ARCHITECTURES 75 80 86 87 89 90 100 110 120)
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Wno-deprecated-gpu-targets")
if(ENABLE_CUDA_DEBUG)
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -G")  # enable cuda-gdb (may significantly affect performance on some targets)
else()
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -lineinfo") # add line information to all builds for debug tools (exclusive to -G option)
endif()
if ( NOT CMAKE_BUILD_TYPE )
    message("Build Type not set, default to Debug")
    set(CMAKE_BUILD_TYPE Debug)
else()
    message("Build Type is set to: " ${CMAKE_BUILD_TYPE})
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
    simulator_kernels.cu
    cuda_utilities.cu
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
    simulator_kernels.cuh
    cuda_utilities.cuh
    CudaArray.h
    CudaDenseMatrix.h
)

# Add target for acoustic
add_executable(acoustic ${SOURCES} ${CUDA_SOURCES} ${HEADERS})

target_include_directories(acoustic PRIVATE ${CUDAToolkit_INCLUDE_DIRS})

target_compile_options(acoustic PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--extended-lambda>)

target_compile_features(acoustic PRIVATE cxx_std_17 cuda_std_17)

set_target_properties(acoustic PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

# Installation (optional)
install(TARGETS acoustic RUNTIME DESTINATION bin)

# ------------------------------------------------------------------------------
# Google Test setup
# ------------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
    URL_HASH SHA256=8ad598c73ad796e0d8280b082cebd82a630d73e73cd3c70057938a6501bba5d7
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

# Find all test files (files ending with _test.cpp)
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_SOURCE_DIR}/*_test.cpp")

# Add target for unit tests
add_executable(ut
    ${TEST_SOURCES}
    InputFileParser.cpp
    InputCompiler.cpp
    Utilities.cpp
    Grid.cpp
)

target_link_libraries(ut PRIVATE GTest::gtest GTest::gtest_main)

target_compile_features(ut PRIVATE cxx_std_17)

add_test(NAME UnitTests COMMAND ut)
