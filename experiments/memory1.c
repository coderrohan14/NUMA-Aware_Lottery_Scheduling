#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 10000000 // Adjust the size to control memory usage
#define NUM_ITERATIONS 10

int main() {
    int *large_array;
    int i, j;

    for (j = 0; j < NUM_ITERATIONS; j++) {
        // Allocate a large array
        large_array = malloc(ARRAY_SIZE * sizeof(int));
        if (large_array == NULL) {
            fprintf(stderr, "Memory allocation failed!\n");
            return 1;
        }

        // Fill the array with random data
        for (i = 0; i < ARRAY_SIZE; i++) {
            large_array[i] = rand();
        }

        // Simulate some processing
        for (i = 0; i < ARRAY_SIZE; i++) {
            large_array[i] += i % 37; // Some arbitrary computation
        }

        // Free the allocated memory
        free(large_array);
    }

    printf("Memory-intensive task completed.\n");
    return 0;
}
