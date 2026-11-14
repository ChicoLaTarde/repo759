#include "convolution.h"

void convolve(
    const float *image,
    float *output,
    std::size_t n,
    const float *mask,
    std::size_t m
)
{
    const std::size_t radius = m / 2;

    #pragma omp parallel for schedule(static)
    for (std::size_t i = 0; i < n; ++i)
    {
        for (std::size_t j = 0; j < n; ++j)
        {
            float sum = 0.0f;

            for (std::size_t ki = 0; ki < m; ++ki)
            {
                for (std::size_t kj = 0; kj < m; ++kj)
                {
                    const std::ptrdiff_t row =
                        static_cast<std::ptrdiff_t>(i) +
                        static_cast<std::ptrdiff_t>(ki) -
                        static_cast<std::ptrdiff_t>(radius);

                    const std::ptrdiff_t col =
                        static_cast<std::ptrdiff_t>(j) +
                        static_cast<std::ptrdiff_t>(kj) -
                        static_cast<std::ptrdiff_t>(radius);

                    if (row >= 0 &&
                        row < static_cast<std::ptrdiff_t>(n) &&
                        col >= 0 &&
                        col < static_cast<std::ptrdiff_t>(n))
                    {
                        sum += image[
                            static_cast<std::size_t>(row) * n +
                            static_cast<std::size_t>(col)
                        ] * mask[ki * m + kj];
                    }
                }
            }

            output[i * n + j] = sum;
        }
    }
}