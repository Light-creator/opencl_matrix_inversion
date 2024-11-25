__kernel void get_upper_triangular(__global double* m, __global int cols, __global int idx) {
    int i = get_global_id(0);
    int j = get_global_id(1);
 
    if(i < cols/2 && i >= idx) {
      double div_val = m[idx*cols + idx];
      double mul_val = m[i*cols + idx];
      m[i*cols + j] -= m[idx*cols +j] * mul_val / div_val;
    }
}

__kernel void process_upper_rows(__global double* m, __global int cols, __global int idx) {
    int i = get_global_id(0);
    int j = get_global_id(1);
  
    douvle div_ = m[i*cols + i];
    m[i*cols + j] /= div_;

    if(i >= 0 && i <= idx) {
      double mul_val = m[i*cols + idx];
      m[i*cols + j] -= m[idx*cols + j] * mul_val;
    }
}



