#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define G 6.67430e-11  // Gravitational constant 
#define NUM_BODIES 1000    // Number of bodies in the system
#define DT 60*60*24     // Time step (1 day in seconds)

// Position, velocity, and mass of each body
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

void update_bodies(Body bodies[], int num_bodies, double dt) {
    // Parallelized loop for force computation and velocity update
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < num_bodies; i++) {
        double fx = 0.0, fy = 0.0;
        
        // Calculate the net force on body[i] due to all other bodies
        for (int j = 0; j < num_bodies; j++) {
            if (i != j) {
                double temp_fx, temp_fy;
                compute_gravitational_force(&bodies[i], &bodies[j], &temp_fx, &temp_fy);

                // Use atomic to prevent race conditions when updating forces
                #pragma omp atomic
                fx += temp_fx;

                #pragma omp atomic
                fy += temp_fy;
            }
        }

        // Update velocity
        bodies[i].vx += fx / bodies[i].mass * dt;
        bodies[i].vy += fy / bodies[i].mass * dt;
    }

    // Parallelized loop for position updates
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < num_bodies; i++) {
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

int main() {
    Body bodies[NUM_BODIES];
    
    // Initialize bodies
    for (int i = 0; i < NUM_BODIES; i++) {
        bodies[i].x = rand() % 1000000000; 
        bodies[i].y = rand() % 1000000000;  
        bodies[i].vx = (rand() % 100 - 50) * 1e3; 
        bodies[i].vy = (rand() % 100 - 50) * 1e3; 
        bodies[i].mass = (rand() % 100 + 1) * 1e24; 
    }

    // Simulate for 1000 steps
    for (int step = 0; step < 1000; step++) {
        printf("Step %d:\n", step);
        print_positions(bodies, NUM_BODIES);
        update_bodies(bodies, NUM_BODIES, DT);
    }

    return 0;
}
