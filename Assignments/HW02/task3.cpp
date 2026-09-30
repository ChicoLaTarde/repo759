#include "matmul.h"

#include <chrono>
#include <iostream>
#include <random>
#include <vector>

int main()
{
    const unsigned int n = 1024;
    const std::size_t size = static_cast<std::size_t>(n) * n;

    // A and B for mmul1, mmul2, and mmul3
    double* A = new double[size];
    double* B = new double[size];

    // C output matrix
    double* C = new double[size];

    // Generate random values
    std::mt19937 generator(0);
    std::uniform_real_distribution<double> distribution(-1.0, 1.0);

    for (std::size_t i = 0; i < size; ++i) {
        A[i] = distribution(generator);
        B[i] = distribution(generator);
    }

    // Prepare vector versions for mmul4
    std::vector<double> A_vec(A, A + size);
    std::vector<double> B_vec(B, B + size);

    // Print number of rows
    std::cout << n << '\n';

    // ---------------- mmul1 ----------------
    auto start = std::chrono::high_resolution_clock::now();

    mmul1(A, B, C, n);

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << elapsed.count() << '\n';
    std::cout << C[size - 1] << '\n';

    // ---------------- mmul2 ----------------
    start = std::chrono::high_resolution_clock::now();

    mmul2(A, B, C, n);

    end = std::chrono::high_resolution_clock::now();

    elapsed = end - start;

    std::cout << elapsed.count() << '\n';
    std::cout << C[size - 1] << '\n';

    // ---------------- mmul3 ----------------
    start = std::chrono::high_resolution_clock::now();

    mmul3(A, B, C, n);

    end = std::chrono::high_resolution_clock::now();

    elapsed = end - start;

    std::cout << elapsed.count() << '\n';
    std::cout << C[size - 1] << '\n';

    // ---------------- mmul4 ----------------
    start = std::chrono::high_resolution_clock::now();

    mmul4(A_vec, B_vec, C, n);

    end = std::chrono::high_resolution_clock::now();

    elapsed = end - start;

    std::cout << elapsed.count() << '\n';
    std::cout << C[size - 1] << '\n';

    delete[] A;
    delete[] B;
    delete[] C;

    return 0;
}