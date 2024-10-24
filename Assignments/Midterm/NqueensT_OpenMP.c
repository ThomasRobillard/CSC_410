#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <omp.h>

// Define number of threads
#define NUM_THREADS 4

typedef struct
{
    int *board;
    int n;
    int col;
    int start_row;
    int end_row;
} ThreadData;

bool isSafe(int board[], int row, int col, int n)
{
    for (int i = 0; i < col; i++)
    {
        if (board[i] == row || abs(board[i] - row) == abs(i - col))
        {
            return false;
        }
    }
    return true;
}

void solveNQueensUtil(int board[], int col, int n)
{
    if (col >= n)
    {
        // omp critical section for safe printing :)
        #pragma omp critical
        {
            for (int i = 0; i < n; i++)
            {
                printf("%d ", board[i]);
            }
            printf("\n");
        }
        return;
    }

    for (int i = 0; i < n; i++)
    {
        if (isSafe(board, i, col, n))
        {
            board[col] = i;
            solveNQueensUtil(board, col + 1, n);
            board[col] = -1; // backtrack
        }
    }
}

// Complete this function
void *solveNQueensParallel(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    int *board = data->board;
    int n = data->n;
    int col = data->col;
    int start_row = data->start_row;
    int end_row = data->end_row;

    // complete this
    for (int i = start_row; i <= end_row; i++)
    {
        board[col] = i;
        if (isSafe(board, i, col, n))
        {
            solveNQueensUtil(board, col + 1, n); // next column
            board[col] = -1;                     // recursively backtrack
        }
    }
}

int main()
{
    omp_set_num_threads(NUM_THREADS);
    int n = 15;

    int rows_per_thread = n / NUM_THREADS;

    #pragma omp parallel
    {
        ThreadData thread_data;
        thread_data.board = (int *)malloc(n * sizeof(int));

        if (thread_data.board == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }

        // Initialize board
        for (int j = 0; j < n; j++)
        {
            thread_data.board[j] = -1;
        }
        thread_data.n = n;
        thread_data.col = 0;

        int tID = omp_get_thread_num();
        thread_data.start_row = tID * rows_per_thread;
        thread_data.end_row = thread_data.start_row + rows_per_thread - 1;

        if (tID == NUM_THREADS - 1)
        {
            thread_data.end_row = n - 1;
        }

        solveNQueensParallel(&thread_data);

        free(thread_data.board);
    }

    

    return 0;
}
