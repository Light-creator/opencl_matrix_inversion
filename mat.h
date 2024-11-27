#ifndef MAT_H
#define MAT_H

#include <stdio.h>
#include <stdlib.h>

#define MAT_AT(m, i, j) (m)->arr_[(i)*(m)->cols_+(j)]


typedef struct mat_t_ {
  size_t rows_;
  size_t cols_;
  float* arr_;
} mat_t;

void copy_inverse_mat(mat_t* m, mat_t* res) {
  for(int i=0; i<res->rows_; i++) {
    for(int j=0; j<res->cols_; j++) {
      MAT_AT(res, i, j) = MAT_AT(m, i, j+res->cols_);
    }
  }
}

void get_extended_mat(mat_t* src, mat_t* dst) {
  for(int i=0; i<src->rows_; i++) {
    for(int j=0; j<src->cols_; j++) {
      MAT_AT(dst, i, j) = MAT_AT(src, i, j);
    }
  }

  for(int i=0; i<src->rows_; i++) {
    int idx = src->cols_ + i; 
    MAT_AT(dst, i, idx) = 1.0;
  }
}

mat_t create_mat(size_t rows, size_t cols) {
  mat_t m;
  m.rows_ = rows;
  m.cols_ = cols;
  m.arr_ = (float*)malloc(sizeof(float)*rows*cols);

  return m;
}

void free_mat(mat_t* m) {
  free(m->arr_);
}

float get_random_value(float from, float to) {
  float range = to-from;
  float div = RAND_MAX / range;
  return from + (rand()/div);
}

void fill_rand_mat(mat_t* m, float from, float to) {
  for(int i=0; i<m->rows_; i++) {
    for(int j=0; j<m->cols_; j++) {
      MAT_AT(m, i, j) = get_random_value(from, to);
    }
  }
}

void transpose_mat(mat_t* src, mat_t* dst) {
  for(int i=0; i<src->rows_; i++) {
    for(int j=0; j<src->cols_; j++) {
      MAT_AT(dst, i, j) = MAT_AT(src, j, i); 
    }
  }
}

void del_mat_val(mat_t* m, float val) {
  for(int i=0; i<m->rows_; i++) {
    for(int j=0; j<m->cols_; j++) {
      MAT_AT(m, i, j) /= val;
    } 
  }
}

void print_mat(mat_t* m) {
  for(int i=0; i<m->rows_; i++) {
    for(int j=0; j<m->cols_; j++) {
      printf("%2.2f ", MAT_AT(m, i, j));
    } 
    printf("\n");
  }
  printf("\n");
}


#endif
