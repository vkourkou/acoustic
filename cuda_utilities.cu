#include <cuda_utilities.cuh>
#include <cuda_runtime.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>

namespace CudaUtilities {

// -----------------------------------------------------------------------------

int _ConvertSMVer2Cores(int major, int minor) {

    // Defines for GPU Architecture types (using the SM version to determine
    // the # of cores per SM
    typedef struct {
        int SM;  // 0xMm (hexidecimal notation), M = SM Major version,
        // and m = SM minor version
        int Cores;
    } sSMtoCores;

    sSMtoCores nGpuArchCoresPerSM[] = {
        {0x30, 192},
        {0x32, 192},
        {0x35, 192},
        {0x37, 192},
        {0x50, 128},
        {0x52, 128},
        {0x53, 128},
        {0x60,  64},
        {0x61, 128},
        {0x62, 128},
        {0x70,  64},
        {0x72,  64},
        {0x75,  64},
        {0x80,  64},
        {0x86, 128},
        {0x87, 128},
        {0x89, 128},
        {0x90, 128},
        {0xa0, 128},
        {0xa1, 128},
        {0xa3, 128},
        {0xb0, 128},
        {0xc0, 128},
        {0xc1, 128},
        {-1, -1}};

    int index = 0;

    while (nGpuArchCoresPerSM[index].SM != -1) {
        if (nGpuArchCoresPerSM[index].SM == ((major << 4) + minor)) {
        return nGpuArchCoresPerSM[index].Cores;
        }

        index++;
    }

    // If we don't find the values, we default use the previous one
    // to run properly
    printf(
        "MapSMtoCores for SM %d.%d is undefined."
        "  Default to use %d Cores/SM\n",
        major, minor, nGpuArchCoresPerSM[index - 1].Cores);
    return nGpuArchCoresPerSM[index - 1].Cores;
}
  
inline const char* _ConvertSMVer2ArchName(int major, int minor) {
    // Defines for GPU Architecture types (using the SM version to determine
    // the GPU Arch name)
    typedef struct {
        int SM;  // 0xMm (hexidecimal notation), M = SM Major version,
        // and m = SM minor version
        const char* name;
    } sSMtoArchName;

    sSMtoArchName nGpuArchNameSM[] = {
        {0x30, "Kepler"},
        {0x32, "Kepler"},
        {0x35, "Kepler"},
        {0x37, "Kepler"},
        {0x50, "Maxwell"},
        {0x52, "Maxwell"},
        {0x53, "Maxwell"},
        {0x60, "Pascal"},
        {0x61, "Pascal"},
        {0x62, "Pascal"},
        {0x70, "Volta"},
        {0x72, "Xavier"},
        {0x75, "Turing"},
        {0x80, "Ampere"},
        {0x86, "Ampere"},
        {0x87, "Ampere"},
        {0x89, "Ada"},
        {0x90, "Hopper"},
        {0xa0, "Blackwell"},
        {0xa1, "Blackwell"},
        {0xa3, "Blackwell"},
        {0xb0, "Blackwell"},
        {0xc0, "Blackwell"},
        {0xc1, "Blackwell"},
        {-1, "Graphics Device"}};

    int index = 0;

    while (nGpuArchNameSM[index].SM != -1) {
        if (nGpuArchNameSM[index].SM == ((major << 4) + minor)) {
        return nGpuArchNameSM[index].name;
        }

        index++;
    }

    // If we don't find the values, we default use the previous one
    // to run properly
    printf(
        "MapSMtoArchName for SM %d.%d is undefined."
        "  Default to use %s\n",
        major, minor, nGpuArchNameSM[index - 1].name);
    return nGpuArchNameSM[index - 1].name;
}


int getDeviceCount() {
    int  deviceCount = 0;
    cudaError_t error_id = cudaGetDeviceCount(&deviceCount);
    if (error_id != cudaSuccess) {
        printf("cudaGetDeviceCount returned %d\n-> %s\n", static_cast<int>(error_id), cudaGetErrorString(error_id));
        return -1;
    }
    return deviceCount;
}

// -----------------------------------------------------------------------------

void queryDeviceCount() {
    int deviceCount = getDeviceCount();
    if (deviceCount >= 0) {
        std::cout << "Cuda devices found: " << deviceCount << std::endl;
    }
    else {
        std::cerr << "Failed to get device count" << std::endl;
        exit(EXIT_FAILURE);
    }
}

// -----------------------------------------------------------------------------

void querySpecificDevice(int deviceIndex) {
    int driverVersion = 0, runtimeVersion = 0;
    cudaSetDevice(deviceIndex);
    cudaDeviceProp deviceProp;
    cudaGetDeviceProperties(&deviceProp, deviceIndex);

    printf("Device %d: \"%s\"\n", deviceIndex, deviceProp.name);

    // Console log
    cudaDriverGetVersion(&driverVersion);
    cudaRuntimeGetVersion(&runtimeVersion);
    printf("  CUDA Driver Version / Runtime Version          %d.%d / %d.%d\n",
            driverVersion / 1000,
            (driverVersion % 100) / 10,
            runtimeVersion / 1000,
            (runtimeVersion % 100) / 10);
    printf("  CUDA Capability Major/Minor version number:    %d.%d\n", deviceProp.major, deviceProp.minor);

    char msg[256];
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
    sprintf_s(msg,
                sizeof(msg),
                "  Total amount of global memory:                 %.0f MBytes "
                "(%llu bytes)\n",
                static_cast<float>(deviceProp.totalGlobalMem / 1048576.0f),
                (unsigned long long)deviceProp.totalGlobalMem);
#else
    snprintf(msg,
                sizeof(msg),
                "  Total amount of global memory:                 %.0f MBytes "
                "(%llu bytes)\n",
                static_cast<float>(deviceProp.totalGlobalMem / 1048576.0f),
                (unsigned long long)deviceProp.totalGlobalMem);
#endif
    printf("%s", msg);

    printf("  (%03d) Multiprocessors, (%03d) CUDA Cores/MP:    %d CUDA Cores\n",
            deviceProp.multiProcessorCount,
            _ConvertSMVer2Cores(deviceProp.major, deviceProp.minor),
            _ConvertSMVer2Cores(deviceProp.major, deviceProp.minor) * deviceProp.multiProcessorCount);
    int clockRate;
    CHECK_CUDA_ERROR(cudaDeviceGetAttribute(&clockRate, cudaDevAttrClockRate, deviceIndex));
    printf("  GPU Max Clock rate:                            %.0f MHz (%0.2f "
            "GHz)\n",
            clockRate * 1e-3f,
            clockRate * 1e-6f);
#if CUDART_VERSION >= 5000
    int memoryClockRate;
#if CUDART_VERSION >= 13000
    CHECK_CUDA_ERROR(cudaDeviceGetAttribute(&memoryClockRate, cudaDevAttrMemoryClockRate, deviceIndex));
#else
    memoryClockRate = deviceProp.memoryClockRate;
#endif
    printf("  Memory Clock rate:                             %.0f Mhz\n", memoryClockRate * 1e-3f);
    printf("  Memory Bus Width:                              %d-bit\n", deviceProp.memoryBusWidth);

    if (deviceProp.l2CacheSize) {
        printf("  L2 Cache Size:                                 %d bytes\n", deviceProp.l2CacheSize);
    }

#else
    // This only available in CUDA 4.0-4.2 (but these were only exposed in the
    // CUDA Driver API)
    int memoryClock;
    getCudaAttribute<int>(&memoryClock, CU_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE, deviceIndex);
    printf("  Memory Clock rate:                             %.0f Mhz\n", memoryClock * 1e-3f);
    int memBusWidth;
    getCudaAttribute<int>(&memBusWidth, CU_DEVICE_ATTRIBUTE_GLOBAL_MEMORY_BUS_WIDTH, deviceIndex);
    printf("  Memory Bus Width:                              %d-bit\n", memBusWidth);
    int L2CacheSize;
    getCudaAttribute<int>(&L2CacheSize, CU_DEVICE_ATTRIBUTE_L2_CACHE_SIZE, deviceIndex);

    if (L2CacheSize) {
        printf("  L2 Cache Size:                                 %d bytes\n", L2CacheSize);
    }

#endif

    printf("  Maximum Texture Dimension Size (x,y,z)         1D=(%d), 2D=(%d, "
            "%d), 3D=(%d, %d, %d)\n",
            deviceProp.maxTexture1D,
            deviceProp.maxTexture2D[0],
            deviceProp.maxTexture2D[1],
            deviceProp.maxTexture3D[0],
            deviceProp.maxTexture3D[1],
            deviceProp.maxTexture3D[2]);
    printf("  Maximum Layered 1D Texture Size, (num) layers  1D=(%d), %d layers\n",
            deviceProp.maxTexture1DLayered[0],
            deviceProp.maxTexture1DLayered[1]);
    printf("  Maximum Layered 2D Texture Size, (num) layers  2D=(%d, %d), %d "
            "layers\n",
            deviceProp.maxTexture2DLayered[0],
            deviceProp.maxTexture2DLayered[1],
            deviceProp.maxTexture2DLayered[2]);

    printf("  Total amount of constant memory:               %zu bytes\n", deviceProp.totalConstMem);
    printf("  Total amount of shared memory per block:       %zu bytes\n", deviceProp.sharedMemPerBlock);
    printf("  Total shared memory per multiprocessor:        %zu bytes\n", deviceProp.sharedMemPerMultiprocessor);
    printf("  Total number of registers available per block: %d\n", deviceProp.regsPerBlock);
    printf("  Warp size:                                     %d\n", deviceProp.warpSize);
    printf("  Maximum number of threads per multiprocessor:  %d\n", deviceProp.maxThreadsPerMultiProcessor);
    printf("  Maximum number of threads per block:           %d\n", deviceProp.maxThreadsPerBlock);
    printf("  Max dimension size of a thread block (x,y,z): (%d, %d, %d)\n",
            deviceProp.maxThreadsDim[0],
            deviceProp.maxThreadsDim[1],
            deviceProp.maxThreadsDim[2]);
    printf("  Max dimension size of a grid size    (x,y,z): (%d, %d, %d)\n",
            deviceProp.maxGridSize[0],
            deviceProp.maxGridSize[1],
            deviceProp.maxGridSize[2]);
    printf("  Maximum memory pitch:                          %zu bytes\n", deviceProp.memPitch);
    printf("  Texture alignment:                             %zu bytes\n", deviceProp.textureAlignment);
    int gpuOverlap;
    CHECK_CUDA_ERROR(cudaDeviceGetAttribute(&gpuOverlap, cudaDevAttrGpuOverlap, deviceIndex));
    printf("  Concurrent copy and kernel execution:          %s with %d copy "
            "engine(s)\n",
            (gpuOverlap ? "Yes" : "No"),
            deviceProp.asyncEngineCount);
    int kernelExecTimeout;
    CHECK_CUDA_ERROR(cudaDeviceGetAttribute(&kernelExecTimeout, cudaDevAttrKernelExecTimeout, deviceIndex));
    printf("  Run time limit on kernels:                     %s\n", kernelExecTimeout ? "Yes" : "No");
    printf("  Integrated GPU sharing Host Memory:            %s\n", deviceProp.integrated ? "Yes" : "No");
    printf("  Support host page-locked memory mapping:       %s\n", deviceProp.canMapHostMemory ? "Yes" : "No");
    printf("  Alignment requirement for Surfaces:            %s\n", deviceProp.surfaceAlignment ? "Yes" : "No");
    printf("  Device has ECC support:                        %s\n", deviceProp.ECCEnabled ? "Enabled" : "Disabled");
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
    printf("  CUDA Device Driver Mode (TCC or WDDM):         %s\n",
            deviceProp.tccDriver ? "TCC (Tesla Compute Cluster Driver)" : "WDDM (Windows Display Driver Model)");
#endif
    printf("  Device supports Unified Addressing (UVA):      %s\n", deviceProp.unifiedAddressing ? "Yes" : "No");
    printf("  Device supports Managed Memory:                %s\n", deviceProp.managedMemory ? "Yes" : "No");
    printf("  Device supports Compute Preemption:            %s\n",
            deviceProp.computePreemptionSupported ? "Yes" : "No");
    printf("  Supports Cooperative Kernel Launch:            %s\n", deviceProp.cooperativeLaunch ? "Yes" : "No");
    // The property cooperativeMultiDeviceLaunch is deprecated in CUDA 13.0
#if CUDART_VERSION < 13000
    printf("  Supports MultiDevice Co-op Kernel Launch:      %s\n",
            deviceProp.cooperativeMultiDeviceLaunch ? "Yes" : "No");
#endif
    printf("  Device PCI Domain ID / Bus ID / location ID:   %d / %d / %d\n",
            deviceProp.pciDomainID,
            deviceProp.pciBusID,
            deviceProp.pciDeviceID);

    const char *sComputeMode[] = {"Default (multiple host threads can use ::cudaSetDevice() with device "
                                    "simultaneously)",
                                    "Exclusive (only one host thread in one process is able to use "
                                    "::cudaSetDevice() with this device)",
                                    "Prohibited (no host thread can use ::cudaSetDevice() with this "
                                    "device)",
                                    "Exclusive Process (many threads in one process is able to use "
                                    "::cudaSetDevice() with this device)",
                                    "Unknown",
                                    NULL};
    int         computeMode;
    CHECK_CUDA_ERROR(cudaDeviceGetAttribute(&computeMode, cudaDevAttrComputeMode, deviceIndex));
    printf("  Compute Mode:\n");
    printf("     < %s >\n", sComputeMode[computeMode]);
}

// -----------------------------------------------------------------------------

void queryGPU() {
    std::cout << "************************************************" << std::endl;
    std::cout << "Querying GPU..." << std::endl;
    queryDeviceCount();
    int iDeviceCnt = getDeviceCount();
    for (int iDevice = 0; iDevice < iDeviceCnt; iDevice++) {
        querySpecificDevice(iDevice);
    }
    std::cout << "GPU queried successfully" << std::endl;
    std::cout << "************************************************" << std::endl;
}

// -----------------------------------------------------------------------------

size_t 
getSize(const dim3& d)
{
    return d.x * d.y * d.z;
}

// -----------------------------------------------------------------------------

dim3 
getGridDimension(const dim3& ElementDimension, const dim3& BlockDimension)
{
    return dim3((ElementDimension.x + BlockDimension.x - 1) / BlockDimension.x,
            (ElementDimension.y + BlockDimension.y - 1) / BlockDimension.y,
            (ElementDimension.z + BlockDimension.z - 1) / BlockDimension.z);
}

// -----------------------------------------------------------------------------

int
getNbOfBlocksMaxOccupancy(const void* func, int  blockSize, size_t dynamicSMemSize )
{
    int numBlocks = 0;
    CHECK_CUDA_ERROR(cudaOccupancyMaxActiveBlocksPerMultiprocessor(&numBlocks, func, blockSize, dynamicSMemSize));
    return numBlocks;
}

// -----------------------------------------------------------------------------

float
computeMaxOccupancy(int numBlocks, int numBlockSize)
{
    int device;
    cudaDeviceProp prop;
    CHECK_CUDA_ERROR(cudaGetDevice(&device) );
    CHECK_CUDA_ERROR(cudaGetDeviceProperties(&prop, device));
    int ActiveWarps = numBlocks * numBlockSize / prop.warpSize;
    int MaxWarps = prop.maxThreadsPerMultiProcessor / prop.warpSize;
    return float(ActiveWarps) / float(MaxWarps);
}

}
