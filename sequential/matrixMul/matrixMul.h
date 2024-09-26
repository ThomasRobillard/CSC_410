#include <stdio.h>
#include <stdlib.h>
#define N 1000 // Size of the matrices


void displayMatrix(int matrix[N][N]) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void matrixMultiply(int A[N][N], int B[N][N], int C[N][N]) {
    // With initialization of all 1s for A and B, output of C will be 10 in each element
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}
