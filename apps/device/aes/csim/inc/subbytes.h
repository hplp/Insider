#pragma once 

#include <insider_kernel.h>

#include "structure.h" 

inline void subbytes (Matrix &data, Elem table[][16]) {
#pragma HLS inline
  for (int i = 0; i < 16; i++) {
#pragma HLS unroll
    unsigned char row_id = (unsigned char) data(8 * i + 7, 8 * i + 4); // (7, 4);
    unsigned char col_id = (unsigned char) data(8 * i + 3, 8 * i + 0); // (3, 0);
    data(8 * i + 7, 8 * i + 0) = table[row_id][col_id];
  }
}
