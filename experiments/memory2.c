#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MATRIX_SIZE 1000 // Size of the square matrix

// Function to allocate memory for a square matrix
double** allocateMatrix(int size) {
    double** matrix = (double**)malloc(size * sizeof(double*));
    for (int i = 0; i < size; i++) {
        matrix[i] = (double*)malloc(size * sizeof(double));
    }
    return matrix;
}

// Function to fill a matrix with random values
void fillMatrix(double** matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i][j] = (double)rand() / RAND_MAX * 10.0; // Random doubles between 0 and 10
        }
    }
}

// Function to perform matrix multiplication
void multiplyMatrices(double** a, double** b, double** result, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            result[i][j] = 0.0;
            for (int k = 0; k < size; k++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

// Function to free the allocated memory for a matrix
void freeMatrix(double** matrix, int size) {
    for (int i = 0; i < size; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

int main() {
    // Seed the random number generator
    srand(time(NULL));

    printf("Allocating and filling matrices...\n");
    double** matrixA = allocateMatrix(MATRIX_SIZE);
    double** matrixB = allocateMatrix(MATRIX_SIZE);
    double** matrixResult = allocateMatrix(MATRIX_SIZE);

    fillMatrix(matrixA, MATRIX_SIZE);
    fillMatrix(matrixB, MATRIX_SIZE);

    printf("Performing matrix multiplication...\n");
    multiplyMatrices(matrixA, matrixB, matrixResult, MATRIX_SIZE);

    printf("Freeing matrices...\n");
    freeMatrix(matrixA, MATRIX_SIZE);
    freeMatrix(matrixB, MATRIX_SIZE);
    freeMatrix(matrixResult, MATRIX_SIZE);

    printf("Matrix multiplication completed.\n");

    return 0;
}
