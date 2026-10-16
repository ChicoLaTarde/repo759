#include <iostream>
#include <omp.h>

int main() {

    #pragma omp parallel num_threads(8)
    {
        int id = omp_get_thread_num();

        double start = omp_get_wtime();

        // Simulate computational work
        volatile double result = 0;

        for (int i = 0; i < 1000000; i++) {
            result += i * 0.000001;
        }

        #pragma omp critical
        {
            std::cout << "Thread " << id
                      << " finished at "
                      << omp_get_wtime() - start
                      << " seconds\n";
        }
    }

    return 0;
}