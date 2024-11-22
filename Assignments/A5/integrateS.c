#include <stdio.h>
#include <mpi.h>

#define N 1000000000 // intervals

double f(double x) {
    return 4.0 / (1.0 + x * x); // Function to integrate
}

double trapezoidalRule() {
    double h = 1.0 / N; 
    double sum = 0.5 * (f(0) + f(1)); 
    for (int i = 1; i < N; ++i) {
        sum += f(i * h);
    }
    sum *= h; 
    return sum;
}

int main() {

    // get start time
    double start_time = MPI_Wtime();

    double pi = trapezoidalRule();
    printf("Estimated value of π: %f\n", pi);

    // get end time
    double end_time = MPI_Wtime();

    // calculate and print elapsed time
    double elapsed_time = end_time - start_time;
    printf("Elapsed time: %f seconds\n", elapsed_time);

    return 0;
}
