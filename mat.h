#ifndef MAT_H
#define MAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int get_mat_from_file(const char* file_path, mat_t* m) {
  FILE* f = fopen(file_path, "r");
  if(!f) {
    fprintf(stderr, "File %s does not exists\n", file_path);
    return 1;
  }
    
  char buf[1024];
  int row = 0;
  while(fgets(buf, 1024, f)) {
    size_t str_len = strlen(buf);
    // printf("str_len: %lu, str: %s", str_len, buf);
    char* tk = strtok(buf, " ");
    int col = 0;
    while(tk) {
      MAT_AT(m, row, col) = atof(tk);
      tk = strtok(NULL, " ");
      col++;
    }
    row++;
  }

  fclose(f);

  return 0;
}


#endif
