#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

void sieveSegment(int start, int end, bool *segment, int sqrt_n, bool *smallPrimes) {
    for (int i = 2; i <= sqrt_n; i++) {
        if (smallPrimes[i]) {
            int startMultiple = (start / i) * i;
            if (startMultiple < start) {
                startMultiple += i;
            }
            if (startMultiple == i) {
                startMultiple += i;
            }
            for (int j = startMultiple; j <= end; j += i) {
                segment[j - start] = false;
            }
        }
    }
}

int main(int argc, char **argv) {
    int rank, size, n = 10000000;
    MPI_Init(&argc, &argv);

    // get start time
    double start_time = MPI_Wtime();
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int sqrt_n = (int)sqrt(n);
    bool *smallPrimes = NULL;

    if (rank == 0) {
        smallPrimes = (bool *)malloc((sqrt_n + 1) * sizeof(bool));
        for (int i = 0; i <= sqrt_n; i++) {
            smallPrimes[i] = true;
        }
        smallPrimes[0] = smallPrimes[1] = false;

        for (int p = 2; p <= sqrt_n; p++) {
            if (smallPrimes[p]) {
                for (int i = p * p; i <= sqrt_n; i += p) {
                    smallPrimes[i] = false;
                }
            }
        }
    }

    // Broadcast the small primes array to all processes
    if (rank != 0) {
        smallPrimes = (bool *)malloc((sqrt_n + 1) * sizeof(bool));
    }
    MPI_Bcast(smallPrimes, sqrt_n + 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);

    // Determine the range each process will handle
    int rangeSize = (n - 1) / size + 1; // Divide [2, n] among processes
    int start = rank * rangeSize + 2;
    int end = (rank + 1) * rangeSize + 1;
    if (end > n) {
        end = n;
    }

    // Allocate memory for the local segment
    bool *segment = (bool *)malloc((end - start + 1) * sizeof(bool));
    for (int i = 0; i <= end - start; i++) {
        segment[i] = true;
    }

    // Perform the sieve on the segment
    sieveSegment(start, end, segment, sqrt_n, smallPrimes);

    // Gather results at the root process
    if (rank == 0) {
        printf("Prime numbers up to %d are:\n", n);
        for (int i = 2; i <= sqrt_n; i++) {
            if (smallPrimes[i]) {
                printf("%d ", i);
            }
        }

        for (int i = 1; i < size; i++) {
            int procStart = i * rangeSize + 2;
            int procEnd = (i + 1) * rangeSize + 1;
            if (procEnd > n) {
                procEnd = n;
            }
            bool *recvSegment = (bool *)malloc((procEnd - procStart + 1) * sizeof(bool));
            MPI_Recv(recvSegment, procEnd - procStart + 1, MPI_C_BOOL, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int j = 0; j <= procEnd - procStart; j++) {
                if (recvSegment[j]) {
                    printf("%d ", procStart + j);
                }
            }
            free(recvSegment);
        }
        printf("\n");
    } else {
        MPI_Send(segment, end - start + 1, MPI_C_BOOL, 0, 0, MPI_COMM_WORLD);
    }

    // Free allocated memory
    free(segment);
    free(smallPrimes);

    // get end time
    double end_time = MPI_Wtime();

    // calculate and print elapsed time
    double elapsed_time = end_time - start_time;
    printf("Elapsed time: %f seconds\n", elapsed_time);

    MPI_Finalize();
    return 0;
}
