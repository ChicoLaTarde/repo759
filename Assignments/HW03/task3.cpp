#include "msort.h"

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cstdlib>
#include <omp.h>

int main(int argc, char* argv[])
{
    // Check command-line arguments
    if (argc != 4)
    {
        std::cerr << "Usage: ./task3 n t ts\n";
        return 1;
    }

    // Read arguments
    const long long n_arg = std::atoll(argv[1]);
    const int t = std::atoi(argv[2]);
    const long long ts_arg = std::atoll(argv[3]);

    // Validate arguments
    if (n_arg <= 0 || t < 1 || t > 20 || ts_arg <= 0)
    {
        std::cerr << "Invalid arguments\n";
        return 1;
    }

    const std::size_t n = static_cast<std::size_t>(n_arg);
    const std::size_t threshold = static_cast<std::size_t>(ts_arg);

    // Configure OpenMP
    omp_set_dynamic(0);
    omp_set_num_threads(t);

    // Allocate array
    std::vector<int> arr(n);

    // Generate random integers in [-1000, 1000]
    std::mt19937 generator(0);
    std::uniform_int_distribution<int> distribution(-1000, 1000);

    for (std::size_t i = 0; i < n; ++i)
    {
        arr[i] = distribution(generator);
    }

    // Start timing
    auto start = std::chrono::high_resolution_clock::now();

    // Perform parallel merge sort
    msort(arr.data(), n, threshold);

    // Stop timing
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate elapsed time in milliseconds
    std::chrono::duration<double, std::milli> elapsed = end - start;

    // Print required output
    std::cout << arr[0] << '\n';
    std::cout << arr[n - 1] << '\n';
    std::cout << elapsed.count() << '\n';

    return 0;
}