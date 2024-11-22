ABOUT
Code files for A5 CSC410

DATE: Nov 22 2024

Inluded files:
matrixMul.c // MPI parallel implementation of matrix multipliaction program
prime.c // MPI parallel implementation of prime numbers up to N program

To compile:
mpicc matrixMul.c 
mpicc prime.c -lm



To run:
mpirun -np 4 ./a.out
// replace 4 with desired number of threads