__kernel void get_upper_triangular(__global float* m, const int cols, const int idx) {
    int i = get_global_id(0);
 
    if(i < cols/2 && i > idx) {
      float div_val = m[idx*cols + idx];
      float mul_val = m[i*cols + idx];
      for(int k=idx; k<cols; k++) m[i*cols + k] -= m[idx*cols +k] * mul_val / div_val;
    }
}

__kernel void process_upper_rows(__global float* m, const int cols, const int idx) {
    int i = get_global_id(0);
  
    float div_ = m[i*cols + i];
    for(int j=i; j<cols; j++) m[i*cols + j] /= div_;

    if(i >= 0 && i < idx) {
      float mul_val = m[i*cols + idx];
      for(int k=0; k<cols; k++) m[i*cols + k] -= m[idx*cols + k] * mul_val;
    }
}



