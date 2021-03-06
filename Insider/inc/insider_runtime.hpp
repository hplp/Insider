#ifndef INSIDER_RUNTIME_HPP
#define INSIDER_RUNTIME_HPP

#include <cstddef>
#include <sys/types.h>

extern "C" {
  void send_input_params(unsigned int data);
  void send_input_params_array(unsigned int *data_arr, size_t arr_len);
  int iopen(const char *pathname, int flags);
  ssize_t iread(int fd, void *buf, size_t count);
  int iclose(int fd);
  const char *reg_virt_file(const char *real_path);
}

#endif
