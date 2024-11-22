#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1000 // Size of the matrices

void matrixMultiplyPartial(int **A, int **B, int **C, int startRow, int numRows, int n)
{
    for (int i = 0; i < numRows; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            C[i][j] = 0;
            for (int k = 0; k < n; ++k)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int **allocateMatrix(int n)
{
    int **matrix = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; ++i)
    {
        matrix[i] = (int *)malloc(n * sizeof(int));
    }
    return matrix;
}

void freeMatrix(int **matrix, int n)
{
    for (int i = 0; i < n; ++i)
    {
        free(matrix[i]);
    }
    free(matrix);
}

void initializeMatrix(int **matrix, int n, int value)
{
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            matrix[i][j] = value;
        }
    }
}
void displayMatrix(int **matrix, int n)
{
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);

    // get start time
    double start_time = MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int **A = NULL;
    int **B = NULL;
    int **C = NULL;

    if (rank == 0)
    {
        // Allocate and initialize matrices A and B on the root process
        A = allocateMatrix(N);
        B = allocateMatrix(N);
        C = allocateMatrix(N);
        initializeMatrix(A, N, 1);
        initializeMatrix(B, N, 1);
    }

    // Broadcast matrix B to all processes
    if (rank != 0)
    {
        B = allocateMatrix(N);
    }
    for (int i = 0; i < N; ++i)
    {
        if (rank == 0)
        {
            MPI_Bcast(B[i], N, MPI_INT, 0, MPI_COMM_WORLD);
        }
        else
        {
            B[i] = (int *)malloc(N * sizeof(int));
            MPI_Bcast(B[i], N, MPI_INT, 0, MPI_COMM_WORLD);
        }
    }

    // Scatter rows of matrix A to all processes
    int rowsPerProcess = N / size;
    int extraRows = N % size;

    int startRow = rank * rowsPerProcess + (rank < extraRows ? rank : extraRows);
    int numRows = rowsPerProcess + (rank < extraRows ? 1 : 0);

    int **localA = allocateMatrix(numRows);
    for (int i = 0; i < numRows; ++i)
    {
        localA[i] = (int *)malloc(N * sizeof(int));
    }

    if (rank == 0)
    {
        for (int i = 1; i < size; ++i)
        {
            int targetStartRow = i * rowsPerProcess + (i < extraRows ? i : extraRows);
            int targetNumRows = rowsPerProcess + (i < extraRows ? 1 : 0);
            for (int j = 0; j < targetNumRows; ++j)
            {
                MPI_Send(A[targetStartRow + j], N, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
        }
        for (int i = 0; i < numRows; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                localA[i][j] = A[startRow + i][j];
            }
        }
    }
    else
    {
        for (int i = 0; i < numRows; ++i)
        {
            MPI_Recv(localA[i], N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    // Allocate local result matrix
    int **localC = allocateMatrix(numRows);
    for (int i = 0; i < numRows; ++i)
    {
        localC[i] = (int *)malloc(N * sizeof(int));
    }

    // Perform local computation
    matrixMultiplyPartial(localA, B, localC, 0, numRows, N);

    // Gather results to root process
    if (rank == 0)
    {
        for (int i = 0; i < numRows; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                C[startRow + i][j] = localC[i][j];
            }
        }
        for (int i = 1; i < size; ++i)
        {
            int targetStartRow = i * rowsPerProcess + (i < extraRows ? i : extraRows);
            int targetNumRows = rowsPerProcess + (i < extraRows ? 1 : 0);
            for (int j = 0; j < targetNumRows; ++j)
            {
                MPI_Recv(C[targetStartRow + j], N, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
    }
    else
    {
        for (int i = 0; i < numRows; ++i)
        {
            MPI_Send(localC[i], N, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    // display matrix
    // if (rank == 0)
    // {
    //     printf("Resulting Matrix C:\n");
    //     displayMatrix(C, N);
    // }

    // Clean up memory
    freeMatrix(localA, numRows);
    freeMatrix(localC, numRows);
    if (rank != 0)
    {
        freeMatrix(B, N);
    }

    if (rank == 0)
    {
        freeMatrix(A, N);
        freeMatrix(B, N);
        freeMatrix(C, N);
    }

    // get end time
    double end_time = MPI_Wtime();

    // calculate and print elapsed time
    double elapsed_time = end_time - start_time;
    printf("Elapsed time: %f seconds\n", elapsed_time);

    MPI_Finalize();
    return 0;
}