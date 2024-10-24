ABOUT
Code files for Midterm part 2 CSC410

DATE: Oct 28 2024

Included files:
NqueensT.c  // Solves the n queens problem using pthreads running in parallel
NqueensT_OpenMP.c // Solves the n queens problem using NqueensT_OpenMP

To compile:
gcc NqueensT.c -o NqueensT -lpthread
gcc NqueensT_OpenMP.c -o NqueensT_OpenMP -fopenmp