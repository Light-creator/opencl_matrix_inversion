#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "../mat.h"

// Gaussian
void create_mat_with_cols_rows(size_t rows, size_t cols);
bool is_determinant_zero(mat_t* m);
void gausian_inverse(mat_t* m, mat_t* res);

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
  // print_mat(&m);

  get_extended_mat(&m, &m_g);
  // print_mat(&m_g);

  clock_t begin = clock();

  gausian_inverse(&m_g, &m_res);

  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  
  printf("matrix size = %lux%lu | execution time = %5.8f\n\n\n", sz, sz, time_spent);

  // print_mat(&m_res);

  free_mat(&m);
  free_mat(&m_g);
  free_mat(&m_res);

  return 0;
}


// Gaussian
bool is_determinant_zero(mat_t* m) {
  for(int i=0; i<m->rows_; i++) {
    if(MAT_AT(m, i, i) == 0) return true;
  }

  return false;
}

void get_upper_triangular(mat_t* m) {
  for(int i=0; i<m->rows_; i++) {
    float val_div = MAT_AT(m, i, i);
    for(int j=i+1; j<m->rows_; j++) {
      float val_mul = MAT_AT(m, j, i);
      for(int k=i; k<m->cols_; k++) {
        MAT_AT(m, j, k) -= MAT_AT(m, i, k) * val_mul / val_div;
      }
    }
  }
}

void gausian_inverse(mat_t* m, mat_t* res) {
  // get upper triangular matrix
  get_upper_triangular(m);

  if(is_determinant_zero(m)) {
    fprintf(stderr, "Determinant equals zero\n");
    return;
  }
  
  // make ones on main diag
  for(int i=0; i<m->rows_; i++) {
    float val = MAT_AT(m, i, i);
    for(int j=0; j<m->cols_; j++) MAT_AT(m, i, j) /= val;
      
    // proccess upper rows
    for(int j=i-1; j>=0; j--) {
      float val_mul = MAT_AT(m, j, i);
      for(int k=i; k<m->cols_; k++) {
        MAT_AT(m, j, k) -= MAT_AT(m, i, k) * val_mul;
      }
    }
  }

  copy_inverse_mat(m, res);
}


// __kernel void get_upper_triangular(__global float* m, const int cols, const int idx) {
//     int i = get_global_id(0);
//     int j = get_global_id(1);
//  
//     if(i < cols/2 && i > idx) {
//       float div_val = m[idx*cols + idx];
//       float mul_val = m[i*cols + idx];
//       m[i*cols + j] -= m[idx*cols +j] * mul_val / div_val;
//     }
// }

// __kernel void process_upper_rows(__global float* m, const int cols, const int idx) {
//     int i = get_global_id(0);
//     int j = get_global_id(1);
//   
//     float div_ = m[i*cols + i];
//     m[i*cols + j] /= div_;

//     if(i >= 0 && i < idx) {
//       float mul_val = m[i*cols + idx];
//       m[i*cols + j] -= m[idx*cols + j] * mul_val;
//     }
// }
