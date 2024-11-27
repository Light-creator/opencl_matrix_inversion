#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <OpenCL/opencl.h>

#include "../mat.h"

int main(int argc, char** argv) {
  if(argc < 2) {
    fprintf(stdout, "Usage: ./main <mat_size>\n");
    return 1;
  }

  srand(time(NULL));

  int success;                          
    
  int sz = atoi(argv[1]);
  mat_t m = create_mat(sz, sz);
  mat_t m_g = create_mat(sz, sz*2);
  mat_t m_res = create_mat(sz, sz);

  fill_rand_mat(&m, 1.0, 10.0);
  get_extended_mat(&m, &m_g);
  // print_mat(&m);
  int DATA_SIZE = sz*sz*2;

  size_t count = m_g.cols_ * m_g.rows_;

  size_t local;                       

  cl_device_id device_id;             
  cl_context context;                 
  cl_command_queue commands;          
  cl_program program;                 
  cl_kernel kernel, kernel_2;                   

  cl_mem input;                       


  clock_t begin = clock();
  success = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
  if (success != CL_SUCCESS) {
    fprintf(stderr, "Error while getting device id\n");
    return 1;
  }

  context = clCreateContext(0, 1, &device_id, NULL, NULL, &success);
  if (!context) {
    fprintf(stderr, "Error while creating context\n");
    return 1;
  }

  commands = clCreateCommandQueue(context, device_id, 0, &success);
  if (!commands) {
    fprintf(stderr, "Error while creating queue\n");
    return 1;
  }


  FILE* fp = fopen("./gpu/matrix_kernel.cl", "r");
  if (!fp) {
    fprintf(stderr, "Failed to load kernel.\n");
    return 1;
  }

  char* src = (char*)malloc(4096);
  size_t src_sz = fread(src, 1, 4096, fp);
  fclose(fp);

  program = clCreateProgramWithSource(context, 1, (const char **) &src, (const size_t*)&src_sz, &success);
  if (!program) {
    fprintf(stderr, "Error while creating progran\n");
    return 1;
  }

  success = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if (success != CL_SUCCESS) {
    fprintf(stderr, "Error while building program\n");
    return 1;
  }

  kernel = clCreateKernel(program, "get_upper_triangular", &success);
  if (!kernel || success != CL_SUCCESS) {
    fprintf(stderr, "Error while creating kernel\n");
    return 1;
  }
  
  // Create Buffer 
  input = clCreateBuffer(context,  CL_MEM_READ_WRITE,  sizeof(float) * count, NULL, NULL);
  if (!input) {
    fprintf(stderr, "Error while creating buffer\n");
    return 1;
  }    

  
  cl_event e_run, e_read;
  for(int idx=0; idx<m_g.rows_; idx++) {
    success = clEnqueueWriteBuffer(commands, input, CL_TRUE, 0, sizeof(float) * count, m_g.arr_, 0, NULL, NULL);
    if (success != CL_SUCCESS) {
      fprintf(stderr, "Error while copying buffer to the kernel\n");
      return 1;
    }

    success = 0;
    success  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
    success |= clSetKernelArg(kernel, 1, sizeof(int), &m_g.cols_);
    success |= clSetKernelArg(kernel, 2, sizeof(int), &idx);
    if (success != CL_SUCCESS) {
      fprintf(stderr, "Error while setting up args!\n");
      return 1;
    }

    // size_t global[2] = {m_g.rows_, m_g.cols_};
    size_t global = m_g.rows_;
    // success = clEnqueueNDRangeKernel(commands, kernel, 2, NULL, global, NULL, 0, NULL, &e_run);
    success = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, NULL, 0, NULL, &e_run);
    if (success != CL_SUCCESS) {
      fprintf(stderr, "Error while running kernel!\n");
      return 1;
    }
    clWaitForEvents(1, &e_run);

    success = clEnqueueReadBuffer(commands, input, CL_TRUE, 0, sizeof(float) * count, m_g.arr_, 0, NULL, &e_read);  
    if (success != CL_SUCCESS) {
      fprintf(stderr, "Error while copying buffer\n");
      return 1;
    }
    clWaitForEvents(1, &e_read);
  }
  
  kernel_2 = clCreateKernel(program, "process_upper_rows", &success);
  if (!kernel_2 || success != CL_SUCCESS) {
    fprintf(stderr, "Error while creating kernel\n");
    return 1;
  }

  for(int idx=0; idx<m_g.rows_; idx++) {
   success = clEnqueueWriteBuffer(commands, input, CL_TRUE, 0, sizeof(float) * count, m_g.arr_, 0, NULL, NULL);
    if (success != CL_SUCCESS) {
      fprintf(stderr, "Error while copying buffer to the kernel memory\n");
      return 1;
    }

    success = 0;
    success  = clSetKernelArg(kernel_2, 0, sizeof(cl_mem), &input);
    success |= clSetKernelArg(kernel_2, 1, sizeof(int), &m_g.cols_);
    success |= clSetKernelArg(kernel_2, 2, sizeof(int), &idx);
    if (success != CL_SUCCESS) {
      fprintf(stderr, "Error while setting up args\n");
      return 1;
    }

    // size_t global[2] = {m_g.rows_, m_g.cols_};
    size_t global = m_g.rows_;
    // success = clEnqueueNDRangeKernel(commands, kernel_2, 2, NULL, global, NULL, 0, NULL, &e_run);
    success = clEnqueueNDRangeKernel(commands, kernel_2, 1, NULL, &global, NULL, 0, NULL, &e_run);
    if (success) {
      fprintf(stderr, "Error while running kernel_2\n");
      return 1;
    }
    clWaitForEvents(1, &e_run);

    success = clEnqueueReadBuffer( commands, input, CL_TRUE, 0, sizeof(float) * count, m_g.arr_, 0, NULL, &e_read);  
    if (success != CL_SUCCESS) {
      fprintf(stderr, "Error while copying buffer");
      return 1;
    }
    clWaitForEvents(1, &e_read);
  } 

  copy_inverse_mat(&m_g, &m_res);
  // print_mat(&m_res);

  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  
  printf("matrix size = %dx%d | execution time = %5.8f\n\n\n", sz, sz, time_spent);

  clReleaseMemObject(input);
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  clReleaseCommandQueue(commands);
  clReleaseContext(context);

  free_mat(&m_g);
  free_mat(&m_res);
  free_mat(&m);

  return 0;
}
