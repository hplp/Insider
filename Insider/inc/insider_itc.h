#ifndef INSIDER_ITC_H_
#define INSIDER_ITC_H_

#include <staccel_itc.h>
#include <insider_common.h>

#ifdef CSIM
#include <vector>
#include <cstdlib>
#include <iostream>
#include <insider_itc.h>

#define ALLOCATED_BUF_NUM (8)
#define KBUF_SIZE (1 << 22)
#define BUF_METADATA_IDX (1 << 21)
#define VIRT_FILE_FD (0xFFFF)

#define COSIM_DRAMA_ADDR_OFFSET (4LL << 32)
#define COSIM_DRAMB_ADDR_OFFSET (8LL << 32)
#define COSIM_DRAMC_ADDR_OFFSET (12LL << 32)
#define COSIM_DRAMD_ADDR_OFFSET (16LL << 32)

int first = 1;
unsigned long long app_buf_phy_addrs[ALLOCATED_BUF_NUM];
unsigned int app_bufs_ptr = 0;
int is_eop = 0;
int buf_idx = 0;
int buf_len = 0;
int file_finish_reading = 0;

static void dram_write(unsigned long long addr, unsigned char data);
static void init();
int iopen(const char *pathname, int flags);
static void update_metadata();
static void kernel_user_memcpy(void *user_buf, unsigned long long kernel_buf_addr, size_t count);
static void reset();
ssize_t iread(int fd, void *buf, size_t count);
int iclose(int fd);
void set_physical_file(unsigned char *buf, size_t count);
void send_input_param(unsigned int data);
static void simulator();
void user_simulation_function();

static void init() {
  for (int i = 0; i < ALLOCATED_BUF_NUM; i ++) {
    if (i == 0) {
      app_buf_phy_addrs[i] = 0;
    }
    else {
      app_buf_phy_addrs[i] = app_buf_phy_addrs[i - 1] + (1 << (APP_BUF_SIZE_LOG2 + 1));
    }
    send_control_msg(TAG(APP_BUF_ADDRS_TAG), app_buf_phy_addrs[i] >> 32);
    send_control_msg(TAG(APP_BUF_ADDRS_TAG), app_buf_phy_addrs[i] & 0xFFFFFFFF);
  }  
}

void dram_write(unsigned long long addr, unsigned char data) {
  int dram_id = (addr >> 12) & 3;
  int dram_idx = ((addr >> 14) << 12) + (addr & ((1 << 12) - 1));

  if (dram_id == 0) {
    dramA_space_mutex.lock();
    dramA_space[dram_idx] = data;
    dramA_space_mutex.unlock();
  }
  else if (dram_id == 1) {
    dramB_space_mutex.lock();
    dramB_space[dram_idx] = data;
    dramB_space_mutex.unlock();
  }
  else if (dram_id == 2) {
    dramC_space_mutex.lock();
    dramC_space[dram_idx] = data;
    dramC_space_mutex.unlock();
  }
  else if (dram_id == 3) {
    dramD_space_mutex.lock();
    dramD_space[dram_idx] = data;
    dramD_space_mutex.unlock();
  }
}

int iopen(const char *pathname, int flags) {
  if (flags != 0) {
    std:: cout << "Error in iopen(): flags must be O_RDONLY, i.e. 0" << std::endl;
    exit(-1);
  }
  return VIRT_FILE_FD;
}

static void update_metadata() {
  unsigned int metadata = 0, flag = 0;
  unsigned long long metadata_addr, flag_addr;
  volatile unsigned char *metadata_ptr;
  do {
    metadata_addr = app_buf_phy_addrs[app_bufs_ptr] + BUF_METADATA_IDX;
    flag_addr = app_buf_phy_addrs[app_bufs_ptr] + BUF_METADATA_IDX + sizeof(unsigned int);
    flag = 
      (pcie_space_read(flag_addr + 3) << 24) |
      (pcie_space_read(flag_addr + 2) << 16) |
      (pcie_space_read(flag_addr + 1) << 8)  |
      (pcie_space_read(flag_addr + 0) << 0);
    metadata = 
      (pcie_space_read(metadata_addr + 3) << 24) |
      (pcie_space_read(metadata_addr + 2) << 16) |
      (pcie_space_read(metadata_addr + 1) << 8)  |
      (pcie_space_read(metadata_addr + 0) << 0);
  }
  while (!(flag));
  pcie_space_write(flag_addr + 0, 0);
  pcie_space_write(flag_addr + 1, 0);
  pcie_space_write(flag_addr + 2, 0);
  pcie_space_write(flag_addr + 3, 0);
  buf_len = metadata >> 1;
  is_eop = metadata & 0x1;
}

static void kernel_user_memcpy(void *user_buf, unsigned long long kernel_buf_addr, size_t count) {
  for (size_t i = 0; i < count; i ++) {
    ((unsigned char *)user_buf)[i] = pcie_space_read(kernel_buf_addr + i);
  }
}

static void reset() {
  app_bufs_ptr = is_eop = buf_idx = buf_len = 0;
  first = 1;
}

ssize_t iread(int fd, void *buf, size_t count) {
  if (fd == VIRT_FILE_FD) {
    if (file_finish_reading) {
      return 0;
    }
    else if (first) {
      update_metadata();
      first = 0;
    }
    unsigned long long kbuf_addr = app_buf_phy_addrs[app_bufs_ptr];
    ssize_t read_size;
    if (count >= buf_len - buf_idx) {
      read_size = buf_len - buf_idx;
      if (is_eop) {
	file_finish_reading = 1;
	reset();
	kernel_user_memcpy(buf, kbuf_addr + buf_idx, read_size);
      }
      else {
	kernel_user_memcpy(buf, kbuf_addr + buf_idx, read_size);
	send_control_msg(TAG(APP_FREE_BUF_TAG), 0);
	app_bufs_ptr = (app_bufs_ptr + 1) & (ALLOCATED_BUF_NUM - 1);
	buf_idx = 0;
	update_metadata();
      }
    }
    else {
      read_size = count;
      buf_idx += read_size;
      kernel_user_memcpy(buf, kbuf_addr + buf_idx, read_size);
    }
    return read_size;
  }
  else {
    return -1;
  }
}

int iclose(int fd) {
  if (fd == VIRT_FILE_FD) {
    send_control_msg(TAG(RESET_TAG), 0);
    file_finish_reading = 0;
    return 0;
  }
  else {
    return -1;
  }
}

void set_physical_file(unsigned char *buf, size_t count) {
  for (size_t i = 0; i < count; i ++) {
    dram_write(i, buf[i]);
  }
  send_control_msg(TAG(APP_FILE_INFO_TAG), 1);
  send_control_msg(TAG(APP_FILE_INFO_TAG), count >> 32);
  send_control_msg(TAG(APP_FILE_INFO_TAG), count & 0xFFFFFFFF);
  send_control_msg(TAG(APP_FILE_INFO_TAG), 0);
  send_control_msg(TAG(APP_FILE_INFO_TAG), 0);
  send_control_msg(TAG(APP_FILE_INFO_TAG), count >> 32);
  send_control_msg(TAG(APP_FILE_INFO_TAG), count & 0xFFFFFFFF);
}

static void simulator() {
  init();
  user_simulation_function();
}

void send_input_param(unsigned int data) {
  send_control_msg(TAG(APP_INPUT_PARAM_TAG), data);
}
#endif

#ifdef INSIDER_COMPILER
ST_Queue<APP_Data> app_input_data(1);
ST_Queue<APP_Data> app_output_data(1);
ST_Queue<unsigned int> app_input_params(1);
#endif

#endif
