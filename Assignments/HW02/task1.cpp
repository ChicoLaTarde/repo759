#include "scan.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <random>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " n\n";
        return 1;
    }

    std::size_t n = std::stoull(argv[1]);

    // Allocate arrays
    float* arr = new float[n];
    float* output = new float[n];

    // Generate random floats between -1.0 and 1.0
    std::mt19937 generator(0);
    std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

    for (std::size_t i = 0; i < n; ++i) {
        arr[i] = distribution(generator);
    }

    // Time only the scan function
    auto start = std::chrono::high_resolution_clock::now();

    scan(arr, output, n);

    auto end = std::chrono::high_resolution_clock::now();

    // Convert elapsed time to milliseconds
    std::chrono::duration<double, std::milli> elapsed = end - start;

    // Required output
    std::cout << elapsed.count() << '\n';
    std::cout << output[0] << '\n';
    std::cout << output[n - 1] << '\n';

    // Deallocate memory
    delete[] arr;
    delete[] output;

    return 0;
}