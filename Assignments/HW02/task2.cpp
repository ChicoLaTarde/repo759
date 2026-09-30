#include "convolution.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <random>


int main(int argc, char* argv[])
{
    
    if( argc != 3)
    {
    std::cerr << "Usage" << argv[0] << " n  m\n";
    return 1;
    }

    std::size_t n = std::stoull(argv[1]);
    std::size_t m = std::stoull(argv[2]);

    //allocate image, mask, output arrays
     float* image = new float[n*n];
     float* mask = new float[m*m];
     float* output = new float[n*n];

     //random number generator
     std::mt19937 generator(0);

     // Image value; -10 to 10
     std::uniform_real_distribution<float> image_dist(-10.0f,10.0f);

     for (std::size_t i = 0; i < n * n; ++i)
     {
        image[i] = image_dist(generator);
     }

     // mask value; -1 to 1
     std::uniform_real_distribution<float> mask_dist(-1.0f,1.0f);

     for(std::size_t i = 0; i < m; ++i)
        mask[i] = mask_dist(generator);

    // time convulution
    auto start = std::chrono::high_resolution_clock::now();
        convolve(image,output,n,mask,m);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double,std::milli> elapsed = end - start;

    // Output
    std::cout << elapsed.count() << '\n';
    std::cout << output[0] << '\n';
    std::cout << output[n * n -1] << '\n';

    //delocate mem
    delete[] image;
    delete[] mask;
    delete[] output;
    

    return 0;
}