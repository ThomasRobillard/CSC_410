#include <mpi.h>
#include <stdio.h>

#define N 1000000000 // Number of intervals

// Function to integrate
double f(double x) {
    return 4.0 / (1.0 + x * x);
}

// Compute the local sum for the trapezoidal rule
double localTrapezoidalSum(int start, int end, double h) {
    double localSum = 0.0;
    for (int i = start; i < end; ++i) {
        localSum += f(i * h);
    }
    return localSum;
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);

    // get start time
    double start_time = MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double h = 1.0 / N;  // Step size
    int intervalsPerProcess = N / size;  // Number of intervals per process
    int start = rank * intervalsPerProcess;
    int end = (rank + 1) * intervalsPerProcess;
    if (rank == size - 1) {
        end = N;  // The last process handles any remaining intervals
    }

    // Calculate the local sum
    double localSum = localTrapezoidalSum(start, end, h);

    // Add contributions from endpoints (only in root process)
    if (rank == 0) {
        localSum += 0.5 * (f(0) + f(1));
    }

    // Reduce all local sums to the root process
    double globalSum = 0.0;
    MPI_Reduce(&localSum, &globalSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Compute the final result in the root process
    if (rank == 0) {
        globalSum *= h;
        printf("Estimated value of π: %.15f\n", globalSum);
    }

    // get end time
    double end_time = MPI_Wtime();

    // calculate and print elapsed time
    double elapsed_time = end_time - start_time;
    printf("Elapsed time: %f seconds\n", elapsed_time);

    MPI_Finalize();
    return 0;
}
