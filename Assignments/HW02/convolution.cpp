


#include "scan.h"
#include "convolution.h"



void convolve(const float *image, float *output, std::size_t n, const float *mask, std::size_t m)
{
    
    //mask size must be odd.
    if((m & 1) == 0) return;

    int offset = static_cast<int>((m-1)/2);

    //loop through every output pixel
    for (std::size_t x = 0; x < n; ++x)
    {
        for (std::size_t y = 0; y < n; ++y)
        {
            float sum = 0.0;

            //apply mask
            for (std::size_t i = 0; i < m; ++i)
            {
                for (std::size_t j = 0; j < m; ++j)
                {
                    int row = static_cast<int>(x)
                            + static_cast<int>(i)
                            - offset;

                    int col = static_cast<int>(y)
                               + static_cast<int>(j)
                               - offset;

                    float value;

                    bool rowInBounds = 
                        row >= 0 && row < static_cast<int>(n);

                    bool columnInBOunds =
                        col >= 0 && col < static_cast<int>(n);

                    if(rowInBounds && columnInBOunds)
                        // within image
                        value = image[row * n + col];
                    else if (!rowInBounds && !columnInBOunds)
                        // corner padding
                        value = 0.0f; //Outside both dimension
                    else
                    {
                        value = 1.0f; //edge padding
                    
                        sum += mask[i * m + j] * value;
                    }
                }
            }
            
            output[x * n + y] = sum;
        }

    }
}
