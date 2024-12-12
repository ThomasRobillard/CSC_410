ABOUT
Code files for A6 CSC410

DATE: Dec 11 2024

Inluded files:
matrixOCL.c

To compile:
1. paste into google colab folder
2. change runtime time to T4
3. Add the following line to a new cell:
    !gcc matrixOCL.c -o matrixOCL -lOpenCL

To run:
Add the following line to a new cell:
    !./matrix_multiply_opencl