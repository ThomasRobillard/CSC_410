ABOUT
Code files for A5 CSC410

DATE: Nov 22 2024

Inluded files:
matrixMul.c // MPI parallel implementation of matrix multipliaction program
prime.c // MPI parallel implementation of prime numbers up to N program
integrate.c // MPI parallel version of a pi approximation program

To compile:
mpicc matrixMul.c 
mpicc prime.c -lm
mpicc integrate.c

To run:
mpirun -np 4 ./a.out
// replace 4 with desired number of threads

NOTE:
// Original sequential versions are included with an "S" appended to their name.
// They are compiled in the same manner since they use the MPI_Wtime() method for comparison.
// Run sequential version in this fashion:
mpirun -np 1 ./a.out