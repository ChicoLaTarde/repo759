#include <cstdio>
#include <random>
#include <cuda_runtime.h>

__global__ void fill_array(int *dA, int a) {
    const int x = static_cast<int>(threadIdx.x);
    const int y = static_cast<int>(blockIdx.x);
    const int index = y * static_cast<int>(blockDim.x) + x;
    dA[index] = a * x + y;
}

int main() {
    constexpr int blocks = 2;
    constexpr int threads_per_block = 8;
    constexpr int count = blocks * threads_per_block;
    int hA[count];
    int *dA = nullptr;

    std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution(1, 10);
    const int a = distribution(generator);

    const cudaError_t alloc_status = cudaMalloc(&dA, sizeof(hA));
    if (alloc_status != cudaSuccess) {
        std::fprintf(stderr, "cudaMalloc: %s\n", cudaGetErrorString(alloc_status));
        return 1;
    }

    fill_array<<<blocks, threads_per_block>>>(dA, a);
    cudaError_t status = cudaGetLastError();
    if (status == cudaSuccess) {
        status = cudaMemcpy(hA, dA, sizeof(hA), cudaMemcpyDeviceToHost);
    }
    const cudaError_t free_status = cudaFree(dA);
    if (status != cudaSuccess || free_status != cudaSuccess) {
        const cudaError_t error = status != cudaSuccess ? status : free_status;
        std::fprintf(stderr, "CUDA error: %s\n", cudaGetErrorString(error));
        return 1;
    }

    for (int i = 0; i < count; ++i) {
        std::printf("%d%c", hA[i], i == count - 1 ? '\n' : ' ');
    }
    return 0;
}