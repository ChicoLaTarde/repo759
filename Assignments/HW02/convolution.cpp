
#include "convolution.h"

void convolve(const float *image, float *output, std::size_t n,
              const float *mask, std::size_t m)
{
    // Mask size must be odd
    if ((m & 1) == 0)
        return;

    int offset = static_cast<int>((m - 1) / 2);

    // Loop through every output pixel
    for (std::size_t x = 0; x < n; ++x)
    {
        for (std::size_t y = 0; y < n; ++y)
        {
            float sum = 0.0f;

            // Apply mask
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

                    bool rowInBounds =
                        row >= 0 && row < static_cast<int>(n);

                    bool columnInBounds =
                        col >= 0 && col < static_cast<int>(n);

                    float value;

                    if (rowInBounds && columnInBounds)
                    {
                        // Inside image
                        value = image[row * n + col];
                    }
                    else if (!rowInBounds && !columnInBounds)
                    {
                        // Outside both dimensions: corner padding
                        value = 0.0f;
                    }
                    else
                    {
                        // Outside exactly one dimension: edge padding
                        value = 1.0f;
                    }

                    // Every mask position contributes
                    sum += mask[i * m + j] * value;
                }
            }

            output[x * n + y] = sum;
        }
    }
}