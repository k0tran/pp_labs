#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

const int N = 1000000;
const int MAX_THREADS = 16;
const int RUNS_PER_THREAD = 10;

void randArr(int *array, int size) {
    for (int i = 0; i < size; ++i)
        array[i] = rand();
}

// run algo and return time elapsed
double run(const int threads, int *array, const int size) {
    double start = omp_get_wtime();
    for (int d = size / 2; d > 0; d /= 2) {
        const int cd = d;
        #pragma omp parallel for num_threads(threads) shared(array, size, cd) default(none)
        for (int i = 0; i < cd; ++i) {
            // insertion sort
            for (int j = cd + i; j < size; j += cd) {
                int key = array[j];
                int k = j - cd;

                while (k >= i && array[k] > key) {
                    array[k + cd] = array[k];
                    k -= cd;
                }
                array[k + cd] = key;
            }
        }
    }

    double end = omp_get_wtime();
    return (end - start) * 1000;
}

int main(int argc, char **argv) {
    // set constant seeds
    int seed[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; ++i)
        seed[i] = rand();
    
    
    int *array = (int *)malloc(N * sizeof(int));

    puts("Threads,Worst (ms),Best (ms),Avg (ms)");

    for (int threads = 1; threads < MAX_THREADS + 1; ++threads) {
        double sum = 0, max_time = -1, min_time = 100000;
        for (int i = 0; i < RUNS_PER_THREAD; ++i) {
            // gen array with special seed
            srand(seed[i]);
            randArr(array, N);

            // calc value
            double time = run(threads, array, N);
            if (time > max_time)
                max_time = time;
            if (time < min_time)
                min_time = time;
            sum += time;
        }
        
        printf("%d,%.3f,%.3f,%.3f\n", threads, max_time, min_time, sum / RUNS_PER_THREAD);
    }

    free(array);

    return 0;
}