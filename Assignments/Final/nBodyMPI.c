#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define G 6.67430e-11  // Gravitational constant 
#define NUM_BODIES 1000    // Number of bodies in the system
#define DT 60*60*24     // Time step (1 day in seconds)

typedef struct {
    double x, y;      // Position (x, y)
    double vx, vy;    // Velocity (vx, vy)
    double mass;      // Mass
} Body;

void compute_gravitational_force(Body *b1, Body *b2, double *fx, double *fy) {
    double dx = b2->x - b1->x;
    double dy = b2->y - b1->y;
    double distance = sqrt(dx * dx + dy * dy);
    
    if (distance == 0.0) {
        *fx = *fy = 0.0;
        return;
    }
    
    double force_magnitude = G * b1->mass * b2->mass / (distance * distance);
    *fx = force_magnitude * dx / distance;
    *fy = force_magnitude * dy / distance;
}

void update_bodies(Body bodies[], int start, int end, int num_bodies, double dt) {
    for (int i = start; i < end; i++) {
        double fx = 0.0, fy = 0.0;
        
        for (int j = 0; j < num_bodies; j++) {
            if (i != j) {
                double temp_fx, temp_fy;
                compute_gravitational_force(&bodies[i], &bodies[j], &temp_fx, &temp_fy);
                fx += temp_fx;
                fy += temp_fy;
            }
        }

        // Update velocity
        bodies[i].vx += fx / bodies[i].mass * dt;
        bodies[i].vy += fy / bodies[i].mass * dt;
    }

    for (int i = start; i < end; i++) {
        // Update position
        bodies[i].x += bodies[i].vx * dt;
        bodies[i].y += bodies[i].vy * dt;
    }
}

void print_positions(Body bodies[], int num_bodies) {
    for (int i = 0; i < num_bodies; i++) {
        printf("Body %d: Position = (%.2f, %.2f), Velocity = (%.2f, %.2f)\n", 
               i, bodies[i].x, bodies[i].y, bodies[i].vx, bodies[i].vy);
    }
    printf("\n");
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    Body bodies[NUM_BODIES];
    int num_bodies_per_proc = NUM_BODIES / size;

    if (rank == 0) {
        // Initialize bodies only on the root process
        for (int i = 0; i < NUM_BODIES; i++) {
            bodies[i].x = rand() % 1000000000; 
            bodies[i].y = rand() % 1000000000;  
            bodies[i].vx = (rand() % 100 - 50) * 1e3; 
            bodies[i].vy = (rand() % 100 - 50) * 1e3; 
            bodies[i].mass = (rand() % 100 + 1) * 1e24; 
        }
    }

    // Broadcast the bodies array to all processes
    MPI_Bcast(bodies, NUM_BODIES * sizeof(Body) / sizeof(double), MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int step = 0; step < 1000; step++) {
        // Each process updates a subset of bodies
        int start = rank * num_bodies_per_proc;
        int end = (rank + 1) * num_bodies_per_proc;
        update_bodies(bodies, start, end, NUM_BODIES, DT);

        // Gather updated bodies to the root process
        MPI_Gather(&bodies[start], num_bodies_per_proc * sizeof(Body) / sizeof(double), MPI_DOUBLE,
                   bodies, num_bodies_per_proc * sizeof(Body) / sizeof(double), MPI_DOUBLE,
                   0, MPI_COMM_WORLD);

        // Root process prints positions
        if (rank == 0) {
            printf("Step %d:\n", step);
            print_positions(bodies, NUM_BODIES);
        }

        // Broadcast the updated bodies array for the next iteration
        MPI_Bcast(bodies, NUM_BODIES * sizeof(Body) / sizeof(double), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
