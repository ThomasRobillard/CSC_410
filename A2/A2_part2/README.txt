ABOUT
Code files for A2 part 2 CSC410

DATE: Sep 26 2024

Included files:
sum_T.c // adds every element of an array together and returns the sum, and having a 
        // defined number of threads adding different portions of the array in parallel
matrixT.c // multiplies two matrices of varying size N, and having a defined # of
          //  threads calculating different parts of the matrix in parallel
matrixT.h // header file containing helper funcgtions for matrixT.c

To compile:
gcc sum_T.c -o sum_T
gcc matrixT.c -o matrixT