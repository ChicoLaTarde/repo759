#include "convolution.h"

#include <iostream>
#include <chrono>
#include <cstdlib>
#include <random>
#include <vector>
#include <omp.h>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: ./task2 n t\n";
        return 1;
    }

    // Read command-line arguments
    const int n_arg = std::atoi(argv[1]);
    const int t = std::atoi(argv[2]);

    if (n_arg <= 0 || t < 1 || t > 20)
    {
        std::cerr << "Invalid arguments\n";
        return 1;
    }

    const std::size_t n = static_cast<std::size_t>(n_arg);
    const std::size_t m = 3;
    const std::size_t size = n * n;

    // Set OpenMP thread count
    omp_set_dynamic(0);
    omp_set_num_threads(t);

    // Allocate image and output matrices
    std::vector<float> image(size);
    std::vector<float> output(size);

    // Initialize image with random float values
    std::mt19937 generator(0);
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

    for (std::size_t i = 0; i < size; ++i)
    {
        image[i] = distribution(generator);
    }

    // Initialize 3x3 convolution mask
    float mask[9] = {
        -1.0f, -1.0f, -1.0f,
        -1.0f,  8.0f, -1.0f,
        -1.0f, -1.0f, -1.0f
    };

    // Start timing
    auto start = std::chrono::high_resolution_clock::now();

    // Perform parallel convolution
    convolve(image.data(), output.data(), n, mask, m);

    // Stop timing
    auto end = std::chrono::high_resolution_clock::now();

    // Convert execution time to milliseconds
    std::chrono::duration<double, std::milli> elapsed = end - start;

    // Print required results
    std::cout << output[0] << '\n';
    std::cout << output[size - 1] << '\n';
    std::cout << elapsed.count() << '\n';

    return 0;
}