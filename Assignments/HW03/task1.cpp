#include "matmul.h"

#include <omp.h>
#include <chrono>
#include <iostream>
#include <random>
#include <cstdlib>
#include <vector>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: ./task1 n t\n";
        return 1;
    }

    const int n_arg = std::atoi(argv[1]);
    const int t = std::atoi(argv[2]);

    if (n_arg <= 0 || t < 1 || t > 20)
    {
        std::cerr << "Invalid arguments\n";
        return 1;
    }

    const std::size_t n = static_cast<std::size_t>(n_arg);
    const std::size_t size = n * n;

    // Set number of OpenMP threads
    omp_set_dynamic(0);
    omp_set_num_threads(t);

    // Allocate matrices
    std::vector<float> A(size);
    std::vector<float> B(size);
    std::vector<float> C(size);

    // Initialize matrices
    std::mt19937 generator(0);
    std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

    for (std::size_t i = 0; i < size; ++i)
    {
        A[i] = distribution(generator);
        B[i] = distribution(generator);
    }

    // Start timing
    auto start = std::chrono::high_resolution_clock::now();

    // Matrix multiplication
    mmul(A.data(), B.data(), C.data(), n);

    // End timing
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed =
        end - start;

    // Required output
    std::cout << C[0] << '\n';
    std::cout << C[size - 1] << '\n';
    std::cout << elapsed.count() << '\n';

    return 0;
}