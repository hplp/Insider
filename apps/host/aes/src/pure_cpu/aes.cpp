#include "../../inc/const.h"
#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <omp.h>
#include <queue>
#include <unistd.h>

//#define NDEBUG
// #define VERIFY
using namespace std;

volatile unsigned char expected_result[READ_BUF_SIZE];

class Timer {
public:
  Timer() : m_beg(clock_::now()) {}
  void reset() { m_beg = clock_::now(); }

  double elapsed() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(clock_::now() -
                                                                 m_beg)
        .count();
  }

private:
  typedef std::chrono::high_resolution_clock clock_;
  typedef std::chrono::duration<double, std::ratio<1>> second_;
  std::chrono::time_point<clock_> m_beg;
};

inline unsigned char GFMul2(unsigned char a){
  unsigned char msb = (a >> 7)& 1;
  a <<= 1;
  if (!msb) a ^= 0x1B;
  return a;    
}

inline unsigned char GFMul3(unsigned char a){
    return GFMul2(a) ^ a;
}

inline unsigned char GFMul(unsigned char a, unsigned char b){
  if (a == 1) return b;
  else if(a == 2) return GFMul2(b);
  else if (a == 3) return GFMul3(b);
}

inline void subbytes(unsigned char *data_chuck) {
//#pragma omp parallel for schedule(static)
  for (int i = 0; i < 16; i++) {
    unsigned int row_id = ((*(data_chuck + i)) >> 4) & 0xf;
    unsigned int col_id = (*(data_chuck + i)) & 0xf;
    *(data_chuck + i) = s_matrix[row_id][col_id];
  }
}

inline void shiftrows(unsigned char *data_chuck) {
  unsigned char tmp_data[16];
//#pragma omp parallel for schedule(static) collapse(2)
  for (int i = 0; i < 4; i++) { 
    for (int j = 0; j < 4; j++) {
      tmp_data[4 * i + j] = *(data_chuck + 4 * i + ((j + i) & 3));
    }
  }
  memcpy(data_chuck, tmp_data, 16);
}

inline void mixcols(unsigned char *data_chuck) {
  unsigned char new_data_chuck[4][4]= {0};
//#pragma omp parallel for reduction(^:new_data_chuck[i][j]) collapse(3)
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      for (int k = 0; k < 4; k++) {
        new_data_chuck[i][j] ^= GFMul(mix_matrix[i][k], data_chuck[4 * k + j]); 
      }
    }
  }
  memcpy(data_chuck, new_data_chuck, 16);
}

inline void addkeys(unsigned char *data_chuck) {
  for (int i = 0; i < 16; i++) {
    data_chuck[i] = data_chuck[i] ^ key_matrix[i];
  }
}

void round_proncess(unsigned char *data_chuck, int round_id) {
  if (round_id == 0) {
    addkeys(data_chuck);
  } else if (round_id == 10) {
    subbytes(data_chuck);
    shiftrows(data_chuck);
    addkeys(data_chuck);
  } else if (round_id > 0 && round_id < 10) { 
    subbytes(data_chuck);
    shiftrows(data_chuck);
    mixcols(data_chuck);
    addkeys(data_chuck);
  }
}

void aes(unsigned char *data_chuck, volatile unsigned char *expected_ptr) {
  // sequential
  for (int rd = 0; rd < 11; rd++) {
    round_proncess(data_chuck, rd);
  }
  // prevent optimization
  memcpy((unsigned char*) expected_ptr, data_chuck, 16);
#ifdef VERIFY
  for (int i = 0; i < 16; i++) {
    std::cout << data_chuck[i];
  }
#endif
}

void read_encrypt(void) {
  int fd;
  //fd = fd = open("../../data_gen/aes_data.txt", O_RDWR);
  fd = open("/mnt/aes_data.txt", O_RDWR);
  unsigned char read_buf[READ_BUF_SIZE] __attribute__((aligned (16))); // for SSE
  int read_cnt = -1;
  long long total_read_bytes = 0;
  while (read_cnt != DATA_SIZE / READ_BUF_SIZE) {
    int read_bytes = 0;
    while (read_bytes != READ_BUF_SIZE) {
      int curReadBytes = read(fd, read_buf, READ_BUF_SIZE - read_bytes);
      if (curReadBytes == 0)

        break;
      read_bytes += curReadBytes;
    }
    read_cnt++;
    total_read_bytes += read_bytes;
#pragma omp parallel for num_threads(8)
    for (int i = 0; i < read_bytes; i += 16) {
      aes(read_buf + i, expected_result + i);
    }
    std::cout << "total read bytes = " << total_read_bytes << std::endl;
  }
  close(fd);
}


int main(int argc, char **argv) {
  puts("read_encrypt().....");
  Timer timer;
  read_encrypt();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  return 0;
}
