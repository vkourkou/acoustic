# Configure the build (creates the build directory)
#cmake -B build
# Build the project
#cmake --build build


#For release
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

#For debug
#cmake -B build -DCMAKE_BUILD_TYPE=Debug
#cmake --build build

#For cuda debug
#cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_CUDA_DEBUG=ON
#cmake --build build

