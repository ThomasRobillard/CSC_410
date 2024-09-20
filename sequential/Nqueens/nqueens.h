#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

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
    // Base case
    if (col >= n)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (board[i] == j)
                {
                    printf("Q ");
                }
                else
                {
                    printf(". ");
                }
            }
            printf("\n");
        }
        printf("\n");
        return;
    }
    // Recursive case
    for (int i = 0; i < n; i++)
    {
        if (isSafe(board, i, col, n))
        {
            board[col] = i;
            solveNQueensUtil(board, col + 1, n);
            // Backtrack
            board[col] = -1;
        }
    }
}