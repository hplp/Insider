#include "../../inc/const.h"
#include <cassert>
#include <chrono>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <omp.h>
#include <queue>
#include <unistd.h>

#include <insider_runtime.hpp>
#include <stdio.h>
#include <time.h>
//#define NDEBUG

using namespace std;

int fd;

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

// TODO
// This is same as pure CPU for now

void read_data(void) {
  // int fd = open("/mnt/aes_input.txt", O_RDONLY);
  int fd = open("../../data_gen/aes_input.txt", O_RDONLY);
  unsigned char *read_buf = (unsigned char *)&buf;
  int read_buf_cnt = 0;

  while (read_buf_cnt != (int)READ_BUF_ITERATIONS) {
    int read_bytes = 0;
    //    cout << "read_buf_cnt=" << read_buf_cnt << endl;
    while (read_bytes != READ_BUF_SIZE) {
      read_bytes += read(fd, read_buf, READ_BUF_SIZE - read_bytes);
      //      cout << "read_bytes=" << read_bytes << endl;
    }
    read_buf += READ_BUF_SIZE;
    read_buf_cnt++;
  }
  close(fd);
}

unsigned short random_Nr() {
  unsigned short NkVals[3] = {4, 6, 8};
  unsigned short Nr, Nk;
  Nk = NkVals[rand() % 3];
  Nr = Nk + 6; // = 10, 12 or 14 rounds
  return Nr;
}

void generate_Nr() {
  for (int i = 0; i < MAX_AES_Nr_PARAM_SIZE; i++) {
    Nr[i] = random_Nr();
  }
}

void encrypting() {
  for (int i = 0; i < FILE_ROW_NUM; i++) {
    unsigned char plaintext[FILE_COL_NUM - 1];
    unsigned char ciphertext[FILE_COL_NUM - 1];
    // unsigned char d_plaintext[FILE_COL_NUM-1];
    for (int j = 0; j < FILE_COL_NUM - 1; j++) {
      plaintext[j] = buf[i][j];
    }
    // cout << plaintext << endl;
    AES_Encrypt(plaintext, Nr[i % MAX_AES_Nr_PARAM_SIZE], ciphertext);
    // AES_Decrypt(ciphertext, Nr[i % MAX_AES_Nr_PARAM_SIZE], d_plaintext);
    // cout << "D:" << d_plaintext << endl;
  }
}

void decrypting() {
  for (int i = 0; i < FILE_ROW_NUM; i++) {
    unsigned char ciphertext[FILE_COL_NUM - 1];
    unsigned char d_plaintext[FILE_COL_NUM - 1];
    for (int j = 0; j < FILE_COL_NUM - 1; j++) {
      ciphertext[j] = buf[i][j];
    }
    AES_Decrypt(ciphertext, Nr[i % MAX_AES_Nr_PARAM_SIZE], d_plaintext);
  }
}

int main(int argc, char **argv) {
  puts("read_data().....");
  Timer timer;
  read_data();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  puts("generate_Nr().....");
  timer.reset();
  generate_Nr();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  puts("encrypting().....");
  timer.reset();
  encrypting();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  puts("decrypting().....");
  timer.reset();
  decrypting();
  cout << "elapsed time = " << timer.elapsed() / 1000 << endl;

  return 0;
}