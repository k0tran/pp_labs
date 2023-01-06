#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv)
{
    const int count = 10000000;     ///< Number of array elements
    const int random_seed = 920214; ///< RNG seed
    const int target = 16;          ///< Number to look for

    int* array = 0;                 ///< The array we need to find the max in
    int  index = -1;                ///< The index of the element we need

    /* Initialize the RNG */
    srand(random_seed);

    /* Generate the random array */
    array = (int*)malloc(count*sizeof(int));
    for(int i=0; i<count; i++) { array[i] = rand(); }

    clock_t start = clock();
    /* Find the index of the element */
    
    #pragma omp parallel num_threads(THREADS) shared(array, count, index, target) default(none) 
    {
        #pragma omp for
        for(int i=0; i<count; i++) {
        if(array[i] == target) {
            #pragma omp critical
            index = i;
            #pragma omp cancel for
        }
    }
    }
    clock_t end = clock();

    const double CLOCKS_PER_MS = (double)CLOCKS_PER_SEC / 1000;
    double total = (double)(end - start) / CLOCKS_PER_MS;
    printf("%.3f", total);

    return 0;
}