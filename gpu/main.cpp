#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_TARGET_OPENCL_VERSION 120

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <stdbool.h>

#include <iostream>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "../mat.h"

#define MAX_SOURCE_SIZE (0x100000)
#define LIST_SIZE 1024

int main(int argc, char** argv) {
  if(argc < 2) {
    fprintf(stderr, "Usage: ./main <mat_size>\n");
  }

  srand(time(NULL));
  
  size_t sz = atoi(argv[1]);
  mat_t m = create_mat(sz, sz);
  mat_t m_g = create_mat(sz, sz*2);
  mat_t m_res = create_mat(sz, sz);

  fill_rand_mat(&m, 1.0, 10.0);
  get_extended_mat(&m, &m_g);

  // Load the kernel source code into the array source_str
  FILE* fp = fopen("matrix_kernel.cl", "r");
  if (!fp) {
    fprintf(stderr, "Failed to load kernel.\n");
    exit(1);
  }

  char* source_str = (char*)malloc(MAX_SOURCE_SIZE);
  size_t source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
  fclose(fp);

  // Get platform and device information
  cl_device_id device_id = NULL;   
  cl_uint ret_num_devices;
  cl_uint ret_num_platforms;


  cl_int ret = clGetPlatformIDs(0, NULL, &ret_num_platforms);
  cl_platform_id *platforms = NULL;
  platforms = (cl_platform_id*)malloc(ret_num_platforms*sizeof(cl_platform_id));

  ret = clGetPlatformIDs(ret_num_platforms, platforms, NULL);
  printf("ret at %d is %d\n", __LINE__, ret);

  ret = clGetDeviceIDs( platforms[1], CL_DEVICE_TYPE_ALL, 1, 
                       &device_id, &ret_num_devices);
  printf("ret at %d is %d\n", __LINE__, ret);
  // Create an OpenCL context
  cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
  printf("ret at %d is %d\n", __LINE__, ret);  

  // Create a command queue
  cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
  if(ret != CL_SUCCESS) std::cout << "create command q\n";

  size_t m_g_size = m_g.rows_*m_g.cols_ * sizeof(cl_double);
  // Create buffer for matrix
  cl_mem mat_mem = clCreateBuffer(context, CL_MEM_READ_WRITE, m_g_size, NULL, &ret);
  if(ret != CL_SUCCESS) std::cout << "create buffer\n";

  // Copy extended matrix to buffer
  ret = clEnqueueWriteBuffer(command_queue, mat_mem, CL_TRUE, 0, m_g_size, m_g.arr_, 0, NULL, NULL);
    if(ret != CL_SUCCESS) std::cout << "copy mat\n";

  // Create a program from the kernel source
  cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

  // Build the program
  ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
  if(ret != CL_SUCCESS) std::cout << "build prog\n";
  
  cl_event e = NULL, read_e = NULL;
  for(int i=0; i<m_g.rows_; i++) {
    // Create the OpenCL kernel
    std::cout << "i: " << i << "\n";
    cl_kernel kernel = clCreateKernel(program, "get_upper_triangular", &ret);
    if(kernel != CL_SUCCESS) std::cout << "setup kernel\n";

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(mat_mem), (void *)&mat_mem);
    if(ret != CL_SUCCESS) std::cout << "setup args\n";
    ret = clSetKernelArg(kernel, 1, sizeof(int), (int *)&m_g.cols_);
    if(ret != CL_SUCCESS) std::cout << "setup args\n";
    ret = clSetKernelArg(kernel, 2, sizeof(int), (int *)&i);
    if(ret != CL_SUCCESS) std::cout << "setup args\n";
    
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &m_g_size, NULL, 0, NULL, &e);
    clWaitForEvents(1, &e);

    ret = clEnqueueReadBuffer(command_queue, mat_mem, CL_TRUE, 0, m_g_size, m_g.arr_, 0, NULL, &read_e);
    clWaitForEvents(1, &read_e);

    clReleaseKernel(kernel);
  }

  print_mat(&m_g);

  return 0;
}
