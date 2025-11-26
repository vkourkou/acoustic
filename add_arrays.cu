#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cuda_runtime.h>
#include <add_arrays.cuh>
#include <cuda_device_io.cuh>


namespace CudaBasic {

// CUDA kernel function to add two arrays
__global__ void addArrays(const float *a, const float *b, float *c, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        c[idx] = a[idx] + b[idx];
    }
}

int getDeviceCount() {
    int count = 0;
    cudaGetDeviceCount(&count);
    return count;
}

bool 
addVectors(bool bUseGPU, const std::vector<float>& A, const std::vector<float>& B, std::vector<float>& C)
{
    if (A.size() != B.size()) {
        std::cerr << "Vector sizes do not match!" << std::endl;
        return false;
    }

    if (A.empty()) {
        C.clear();
        return true;
    }
    else {
        C.resize(A.size());
    }
    size_t n = A.size();
    
    if (bUseGPU) {
        // GPU path
        float *d_a, *d_b, *d_c;
        
        // Allocate device memory
        cudaError_t err = cudaMalloc(&d_a, n * sizeof(float));
        if (err != cudaSuccess) {
            std::cerr << "CUDA malloc failed for d_a: " << cudaGetErrorString(err) << std::endl;
            return false;
        }
        
        err = cudaMalloc(&d_b, n * sizeof(float));
        if (err != cudaSuccess) {
            std::cerr << "CUDA malloc failed for d_b: " << cudaGetErrorString(err) << std::endl;
            cudaFree(d_a);
            return false;
        }
        
        err = cudaMalloc(&d_c, n * sizeof(float));
        if (err != cudaSuccess) {
            std::cerr << "CUDA malloc failed for d_c: " << cudaGetErrorString(err) << std::endl;
            cudaFree(d_a);
            cudaFree(d_b);
            return false;
        }
        
        // Copy host data to device
        err = cudaMemcpy(d_a, A.data(), n * sizeof(float), cudaMemcpyHostToDevice);
        if (err != cudaSuccess) {
            std::cerr << "CUDA memcpy failed (H->D) for d_a: " << cudaGetErrorString(err) << std::endl;
            cudaFree(d_a);
            cudaFree(d_b);
            cudaFree(d_c);
            return false;
        }
        
        err = cudaMemcpy(d_b, B.data(), n * sizeof(float), cudaMemcpyHostToDevice);
        if (err != cudaSuccess) {
            std::cerr << "CUDA memcpy failed (H->D) for d_b: " << cudaGetErrorString(err) << std::endl;
            cudaFree(d_a);
            cudaFree(d_b);
            cudaFree(d_c);
            return false;
        }
        
        // Launch kernel
        int threadsPerBlock = 256;
        int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;
        addArrays<<<blocksPerGrid, threadsPerBlock>>>(d_a, d_b, d_c, static_cast<int>(n));
        
        // Check for kernel launch errors
        err = cudaGetLastError();
        if (err != cudaSuccess) {
            std::cerr << "CUDA kernel launch failed: " << cudaGetErrorString(err) << std::endl;
            cudaFree(d_a);
            cudaFree(d_b);
            cudaFree(d_c);
            return false;
        }
        
        // Wait for kernel to complete
        err = cudaDeviceSynchronize();
        if (err != cudaSuccess) {
            std::cerr << "CUDA device synchronize failed: " << cudaGetErrorString(err) << std::endl;
            cudaFree(d_a);
            cudaFree(d_b);
            cudaFree(d_c);
            return false;
        }
        
        // Copy result back to host
        err = cudaMemcpy(C.data(), d_c, n * sizeof(float), cudaMemcpyDeviceToHost);
        if (err != cudaSuccess) {
            std::cerr << "CUDA memcpy failed (D->H) for d_c: " << cudaGetErrorString(err) << std::endl;
            cudaFree(d_a);
            cudaFree(d_b);
            cudaFree(d_c);
            return false;
        }
        
        // Free device memory
        cudaFree(d_a);
        cudaFree(d_b);
        cudaFree(d_c);
    } else {
        // CPU path
        for (size_t i = 0; i < n; ++i) {
            C[i] = A[i] + B[i];
        }
    }

    return true;
}

// -----------------------------------------------------------------------------

int launchKernel()
{
    // Simple test kernel launch function
    std::vector<float> A(1000, 1.0f);
    std::vector<float> B(1000, 2.0f);
    std::vector<float> C;
    
    int deviceCount = getDeviceCount();
    if (deviceCount == 0) {
        std::cerr << "No CUDA devices found!" << std::endl;
        return -1;
    }
    
    std::cout << "Found " << deviceCount << " CUDA device(s)" << std::endl;
    
    bool success = addVectors(true, A, B, C);
    if (!success) {
        std::cerr << "Kernel launch failed!" << std::endl;
        return -1;
    }
    
    // Verify results
    if (C.size() != A.size()) {
        std::cerr << "Result size mismatch!" << std::endl;
        return -1;
    }
    
    bool allCorrect = true;
    for (size_t i = 0; i < C.size(); ++i) {
        if (std::abs(C[i] - 3.0f) > 1e-5f) {
            allCorrect = false;
            break;
        }
    }
    
    if (allCorrect) {
        std::cout << "Kernel launch successful! Results verified." << std::endl;
        return 0;
    } else {
        std::cerr << "Kernel results incorrect!" << std::endl;
        return -1;
    }
}

// -----------------------------------------------------------------------------

} 