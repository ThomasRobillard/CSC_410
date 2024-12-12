code = """
// Matrix Multiplication using OpenCL

#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

// OpenCL kernel as a string
const char *kernelSource = "__kernel void matrixMultiply(__global float* A, \\n"
                           "                             __global float* B, \\n"
                           "                             __global float* C, \\n"
                           "                             const int N) { \\n"
                           "    int row = get_global_id(0); \\n"
                           "    int col = get_global_id(1); \\n"
                           "    \\n"
                           "    float sum = 0.0; \\n"
                           "    for (int k = 0; k < N; k++) { \\n"
                           "        sum += A[row * N + k] * B[k * N + col]; \\n"
                           "    } \\n"
                           "    C[row * N + col] = sum; \\n"
                           "} \\n";

int main() {
    const int N = 1000; // Matrix dimension
    size_t bytes = N * N * sizeof(float);

    // Host data
    float *A = (float*)malloc(bytes);
    float *B = (float*)malloc(bytes);
    float *C = (float*)malloc(bytes);

    // Initialize matrices
    for (int i = 0; i < N * N; i++) {
        A[i] = 1.0f; 
        B[i] = 1.0f; 
        C[i] = 0.0f;
    }

    // OpenCL setup and matrix multiplication
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue command_queue;
    cl_program program;
    cl_kernel kernel;
    cl_mem bufferA, bufferB, bufferC;
    cl_int ret;

    // Get platform and device information
    clGetPlatformIDs(1, &platform, NULL);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

    // Create OpenCL context
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &ret);

    // Create command queue
    command_queue = clCreateCommandQueue(context, device, 0, &ret);

    // Create buffers
    bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, &ret);
    bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, &ret);
    bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bytes, NULL, &ret);

    // Copy matrices to buffers
    clEnqueueWriteBuffer(command_queue, bufferA, CL_TRUE, 0, bytes, A, 0, NULL, NULL);
    clEnqueueWriteBuffer(command_queue, bufferB, CL_TRUE, 0, bytes, B, 0, NULL, NULL);

    // Create program from kernel source
    program = clCreateProgramWithSource(context, 1, (const char**)&kernelSource, NULL, &ret);

    // Build program
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    // Create OpenCL kernel
    kernel = clCreateKernel(program, "matrixMultiply", &ret);

    // Set kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC);
    clSetKernelArg(kernel, 3, sizeof(int), &N);

    // Execute the kernel
    size_t global[2] = {N, N};
    clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global, NULL, 0, NULL, NULL);

    // Read result back to host
    clEnqueueReadBuffer(command_queue, bufferC, CL_TRUE, 0, bytes, C, 0, NULL, NULL);

    // Print result (commented for large matrices)
    // printf("Result Matrix:\\n");
    // for (int i = 0; i < N; i++) {
    //     for (int j = 0; j < N; j++) {
    //         printf("%f ", C[i * N + j]);
    //     }
    //     printf("\\n");
    // }

    // Cleanup
    // free buffers and allocated memory
    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
    free(A);
    free(B);
    free(C);

    return 0;
}
"""

# Write to a C file
with open("matrixOCL.c", "w") as f:
    f.write(code)
