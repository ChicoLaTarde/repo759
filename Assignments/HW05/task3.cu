#include <charconv>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <string_view>
#include <system_error>
#include <vector>
#include <cuda_runtime.h>

#include "vscale.cuh"

#ifndef VSCALE_THREADS_PER_BLOCK
#define VSCALE_THREADS_PER_BLOCK 512
#endif

void check_cuda(cudaError_t status, const char *operation) {
    if (status != cudaSuccess) {
        std::fprintf(stderr, "%s: %s\n", operation, cudaGetErrorString(status));
        std::exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::fprintf(stderr, "Usage: %s n (positive integer)\n", argv[0]);
        return EXIT_FAILURE;
    }

    const std::string_view argument(argv[1]);
    const char *end = argument.data() + argument.size();
    unsigned int n = 0;
    const auto parsed = std::from_chars(argument.data(), end, n);
    if (parsed.ec != std::errc{} || parsed.ptr != end || n == 0) {
        std::fprintf(stderr, "n must be a positive unsigned integer\n");
        return EXIT_FAILURE;
    }

    std::vector<float> a(n);
    std::vector<float> b(n);
    std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<float> distribution_a(-10.0f, 10.0f);
    std::uniform_real_distribution<float> distribution_b(0.0f, 1.0f);
    for (unsigned int i = 0; i < n; ++i) {
        a[i] = distribution_a(generator);
        b[i] = distribution_b(generator);
    }

    const std::size_t bytes = static_cast<std::size_t>(n) * sizeof(float);
    float *dA = nullptr;
    float *dB = nullptr;
    check_cuda(cudaMalloc(&dA, bytes), "cudaMalloc dA");
    check_cuda(cudaMalloc(&dB, bytes), "cudaMalloc dB");
    check_cuda(cudaMemcpy(dA, a.data(), bytes, cudaMemcpyHostToDevice), "copy a to device");
    check_cuda(cudaMemcpy(dB, b.data(), bytes, cudaMemcpyHostToDevice), "copy b to device");

    constexpr unsigned int threads_per_block = VSCALE_THREADS_PER_BLOCK;
    const unsigned int blocks = (n - 1) / threads_per_block + 1;
    cudaEvent_t start;
    cudaEvent_t stop;
    check_cuda(cudaEventCreate(&start), "create start event");
    check_cuda(cudaEventCreate(&stop), "create stop event");
    check_cuda(cudaEventRecord(start), "record start event");
    vscale<<<blocks, threads_per_block>>>(dA, dB, n);
    check_cuda(cudaGetLastError(), "launch vscale");
    check_cuda(cudaEventRecord(stop), "record stop event");
    check_cuda(cudaEventSynchronize(stop), "wait for stop event");

    float milliseconds = 0.0f;
    check_cuda(cudaEventElapsedTime(&milliseconds, start, stop), "measure kernel time");
    check_cuda(cudaMemcpy(b.data(), dB, bytes, cudaMemcpyDeviceToHost), "copy b to host");

    check_cuda(cudaEventDestroy(start), "destroy start event");
    check_cuda(cudaEventDestroy(stop), "destroy stop event");
    check_cuda(cudaFree(dA), "cudaFree dA");
    check_cuda(cudaFree(dB), "cudaFree dB");

    std::printf("%.6f\n%g\n%g\n", milliseconds, b.front(), b.back());
    return EXIT_SUCCESS;
}
