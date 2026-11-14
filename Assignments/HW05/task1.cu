#include <cstdio>
#include <cuda_runtime.h>

__global__ void print_factorials() {
    const int a = static_cast<int>(threadIdx.x) + 1;
    int factorial = 1;

    for (int factor = 2; factor <= a; ++factor) {
        factorial *= factor;
    }

    std::printf("%d!=%d\n", a, factorial);
}

int main() {
    print_factorials<<<1, 8>>>();
    const cudaError_t status = cudaDeviceSynchronize();
    if (status != cudaSuccess) {
        std::fprintf(stderr, "CUDA error: %s\n", cudaGetErrorString(status));
        return 1;
    }

    return 0;
}
