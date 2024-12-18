ABOUT
Code files for Final Part 2

DATE: 12/17 2024

TO COMPILE:
gcc nBodyPthread.c -o -lpthread -lm
gcc -fopenmp nBodyOMP.c -o nBodyOMP -lm

mpicc nBodyMPI.c -o nBodyMPI -lm
mpirun -np 4 ./nBodyMPI