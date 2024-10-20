#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 1000        // Size of the matrix
#define NUM_THREADS 4 // Number of threads

int **A, **B, **C; // Global matrices

// Information holder for each thread
typedef struct
{
    int thread_id;
    int num_rows;
} thread_data_t;

// Entry function for each thread
void *matrixMultiplyThread(void *arg)
{
    // Extract thread info from the passed argument
    thread_data_t *data = (thread_data_t *)arg;

    // Calculate the start and ending row chunk for each thread to handle
    int start = data->num_rows * data->thread_id;
    int end = data->num_rows * (data->thread_id + 1);

    // Loop through the start and end row assigned to the thread and compute matrix multiplication
    for (int i = start; i < end; i++)
    {
        for (int j = 0; j < N; j++)
        {
            C[i][j] = 0;
            for (int k = 0; k < N; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    pthread_exit(NULL);
}
void displayMatrix(int **matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            printf("%5d ", matrix[i][j]); // Adjust the width based on the number size
        }
        printf("\n");
    }
    printf("\n");
}
