#include "scan.h"
#include <iostream>



// Given an array [a0, a1,· · · , an−1], 
// the inclusive scan function produces the array [a0, a0 + a1, a0 + a1 + a2,· · · ,
// a0 +a1 +· · · + an−1].
// Performs an inclusive scan on input array arr and stores
void scan(const float *arr, float *output, std::size_t n) 
{
    if (n == 0) return; // Handle empty array case

    output[0] = arr[0]; // Initialize the first element of output

    for (std::size_t i = 1; i < n; ++i) 
    {
        output[i] = output[i - 1] + arr[i]; // Perform inclusive scan
    }
}
