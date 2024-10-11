ABOUT
Code files for A3 part 1 CSC410

DATE: Oct 11 2024

Included files:
sum_T.c // adds every element of an array together and returns the sum, and having a 
        // defined number of threads adding different portions of the array in parallel
        // Solved syncronization errors with mutex lock
matrixT.c // multiplies two matrices of varying size N, and having a defined # of
          //  threads calculating different parts of the matrix in parallel
          // modified to fix uneven chunk size
matrixT.h // header file containing helper funcgtions for matrixT.c

To compile:
gcc sum_T.c -o sum_T
gcc matrixT.c -o matrixT