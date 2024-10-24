#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>

#define NUM_THREADS 3

pthread_mutex_t print_lock; // Mutex for safe printing :)

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
        // mutex lock for safe printing :)
        // pthread_mutex_lock(&print_lock);
        // for (int i = 0; i < n; i++)
        // {
        //     printf("%d ", board[i]);
        // }
        // printf("\n");
        // pthread_mutex_unlock(&print_lock);
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

    for (int i = start_row; i <= end_row; i++)
    {
        board[col] = i;
        if (isSafe(board, i, col, n))
        {
            solveNQueensUtil(board, col + 1, n); // next column
            board[col] = -1; // recursively backtrack
        }
    }

    pthread_exit(NULL);
}

int main()
{
    int n = 15;
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    // Initialize mutex for thread-safe printing
    pthread_mutex_init(&print_lock, NULL);

    // Each thread will have its own board to avoid race conditions
    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread_data[i].board = (int *)malloc(n * sizeof(int));
        if (thread_data[i].board == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            return 1;
        }

        // Initialize board
        for (int j = 0; j < n; j++)
        {
            thread_data[i].board[j] = -1;
        }
        thread_data[i].n = n;
        thread_data[i].col = 0;
    }

    // Split the first row's work among threads
    int rows_per_thread = n / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread_data[i].start_row = i * rows_per_thread;
        thread_data[i].end_row = thread_data[i].start_row + rows_per_thread - 1;

        // assign extra rows to last thread
        if (i == NUM_THREADS - 1)
        {
            thread_data[i].end_row = n - 1;
        }
    }

    // Create threads and distribute rows among them

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, solveNQueensParallel, &thread_data[i]);
    }
    // Make sure to handle extra rows for some threads (if n is not divisible by NUM_THREADS)

    // Wait for all threads to complete and free memory for each thread
    for (int i = NUM_THREADS - 1; i >= 0; i--)
    {
        pthread_join(threads[i], NULL);
        free(thread_data[i].board);
    }

    // Destroy the mutex after all threads have finished
    pthread_mutex_destroy(&print_lock);

    return 0;
}
