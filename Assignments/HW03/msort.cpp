#include "msort.h"

#include <algorithm>
#include <cstddef>
#include <vector>

// Merge two sorted subarrays:
// Left:  [left, mid)
// Right: [mid, right)
static void merge(
    int* arr,
    int* temp,
    std::size_t left,
    std::size_t mid,
    std::size_t right
)
{
    std::size_t i = left;
    std::size_t j = mid;
    std::size_t k = left;

    while (i < mid && j < right)
    {
        if (arr[i] <= arr[j])
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
    }

    while (i < mid)
        temp[k++] = arr[i++];

    while (j < right)
        temp[k++] = arr[j++];

    for (std::size_t p = left; p < right; ++p)
        arr[p] = temp[p];
}

// Recursive parallel merge sort
static void merge_sort_recursive(
    int* arr,
    int* temp,
    std::size_t left,
    std::size_t right,
    std::size_t threshold
)
{
    const std::size_t length = right - left;

    // Serial sorting for small subarrays
    if (length <= threshold)
    {
        std::sort(arr + left, arr + right);
        return;
    }

    const std::size_t mid = left + length / 2;

    // Create task for left half
    #pragma omp task shared(arr, temp) firstprivate(left, mid, threshold)
    {
        merge_sort_recursive(
            arr, temp, left, mid, threshold
        );
    }

    // Create task for right half
    #pragma omp task shared(arr, temp) firstprivate(mid, right, threshold)
    {
        merge_sort_recursive(
            arr, temp, mid, right, threshold
        );
    }

    // Wait for both tasks
    #pragma omp taskwait

    // Merge sorted halves
    merge(arr, temp, left, mid, right);
}

// Required function from msort.h
void msort(
    int* arr,
    const std::size_t n,
    const std::size_t threshold
)
{
    if (n <= 1)
        return;

    std::vector<int> temp(n);

    #pragma omp parallel
    {
        #pragma omp single
        {
            merge_sort_recursive(
                arr,
                temp.data(),
                0,
                n,
                std::max<std::size_t>(threshold, 1)
            );
        }
    }
}