#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define G 6.67430e-11  // Gravitational constant 
#define NUM_BODIES 1000    // Number of bodies in the system
#define DT 60*60*24     // Time step (1 day in seconds)
#define NUM_THREADS 4   // Number of threads

typedef struct {
    double x, y;      // Position (x, y)
    double vx, vy;    // Velocity (vx, vy)
    double mass;      // Mass
} Body;

typedef struct {
    Body* bodies;
    int start_idx;
    int end_idx;
    int num_bodies;
} ThreadArgs;

pthread_barrier_t barrier;

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

void* update_bodies_thread(void* args) {
    ThreadArgs* thread_args = (ThreadArgs*)args;
    Body* bodies = thread_args->bodies;
    int start_idx = thread_args->start_idx;
    int end_idx = thread_args->end_idx;
    int num_bodies = thread_args->num_bodies;

    for (int step = 0; step < 1000; step++) {
        // Compute forces
        for (int i = start_idx; i < end_idx; i++) {
            double fx = 0.0, fy = 0.0;
            for (int j = 0; j < num_bodies; j++) {
                if (i != j) {
                    double temp_fx, temp_fy;
                    compute_gravitational_force(&bodies[i], &bodies[j], &temp_fx, &temp_fy);
                    fx += temp_fx;
                    fy += temp_fy;
                }
            }
            bodies[i].vx += fx / bodies[i].mass * DT;
            bodies[i].vy += fy / bodies[i].mass * DT;
        }
        
        pthread_barrier_wait(&barrier);

        // Update positions
        for (int i = start_idx; i < end_idx; i++) {
            bodies[i].x += bodies[i].vx * DT;
            bodies[i].y += bodies[i].vy * DT;
        }

        pthread_barrier_wait(&barrier);
    }
    return NULL;
}

int main() {
    Body bodies[NUM_BODIES];
    pthread_t threads[NUM_THREADS];
    ThreadArgs thread_args[NUM_THREADS];

    // Initialize bodies
    for (int i = 0; i < NUM_BODIES; i++) {
        bodies[i].x = rand() % 1000000000;
        bodies[i].y = rand() % 1000000000;
        bodies[i].vx = (rand() % 100 - 50) * 1e3;
        bodies[i].vy = (rand() % 100 - 50) * 1e3;
        bodies[i].mass = (rand() % 100 + 1) * 1e24;
    }

    pthread_barrier_init(&barrier, NULL, NUM_THREADS);

    // Create threads
    int bodies_per_thread = NUM_BODIES / NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i].bodies = bodies;
        thread_args[i].start_idx = i * bodies_per_thread;
        thread_args[i].end_idx = (i + 1) * bodies_per_thread;
        thread_args[i].num_bodies = NUM_BODIES;
        pthread_create(&threads[i], NULL, update_bodies_thread, &thread_args[i]);
    }

    // Wait for threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier);

    return 0;
}
