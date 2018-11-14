#ifndef INSIDER_COSIM_H_
#define INSIDER_COSIM_H_

#include <string.h>

#define TAG(x) (x << 2)
#define APP_FILE_INFO_TAG (10)
#define APP_BUF_ADDRS_TAG (11)
#define APP_FREE_BUF_TAG (12)
#define APP_INPUT_PARAM_TAG (13)
#define RESET_TAG (14)
#define DEBUG_TAG (15)

#define ALLOCATED_BUF_NUM (8)
#define APP_BUF_SIZE_LOG2 (21)
#define KBUF_SIZE (1 << 22)
#define BUF_METADATA_IDX (1 << 21)
#define VIRT_FILE_FD (0xFFFF)

#define COSIM_DRAMA_ADDR_OFFSET (4LL << 32)
#define COSIM_DRAMB_ADDR_OFFSET (8LL << 32)
#define COSIM_DRAMC_ADDR_OFFSET (12LL << 32)
#define COSIM_DRAMD_ADDR_OFFSET (16LL << 32)

int i;
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
static void kernel_user_memcpy(void *user_buf,
                               unsigned long long kernel_buf_addr,
                               size_t count);
static void reset();
ssize_t iread(int fd, void *buf, size_t count);
int iclose(int fd);
void set_physical_file(unsigned char *buf, size_t count);
void send_input_param(unsigned int data);
static void simulator();
void user_simulation_function();

static void dram_write(unsigned long long addr, unsigned char data) {
  int dram_id = (addr >> 12) & 3;
  int dram_idx = ((addr >> 14) << 12) + (addr & ((1 << 12) - 1));

  if (dram_id == 0) {
    host_memory_putc(dram_idx + COSIM_DRAMA_ADDR_OFFSET, data);
  } else if (dram_id == 1) {
    host_memory_putc(dram_idx + COSIM_DRAMB_ADDR_OFFSET, data);
  } else if (dram_id == 2) {
    host_memory_putc(dram_idx + COSIM_DRAMC_ADDR_OFFSET, data);
  } else if (dram_id == 3) {
    host_memory_putc(dram_idx + COSIM_DRAMD_ADDR_OFFSET, data);
  }
}

static void init() {
  for (i = 0; i < ALLOCATED_BUF_NUM; i++) {
    if (i == 0) {
      app_buf_phy_addrs[i] = 0;
    } else {
      app_buf_phy_addrs[i] =
          app_buf_phy_addrs[i - 1] + (1 << (APP_BUF_SIZE_LOG2 + 1));
    }
    cl_poke(TAG(APP_BUF_ADDRS_TAG), app_buf_phy_addrs[i] >> 32);
    cl_poke(TAG(APP_BUF_ADDRS_TAG), app_buf_phy_addrs[i] & 0xFFFFFFFF);
  }
}

int iopen(const char *pathname, int flags) {
  if (flags != 0) {
    puts("Error in iopen(): flags must be O_RDONLY, i.e. 0");
    exit(-1);
  }
  if (strcmp(pathname, "cosim_phy_file")) {
    puts("Error in iopen(): pathname must be cosim_phy_file");
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
    flag_addr = app_buf_phy_addrs[app_bufs_ptr] + BUF_METADATA_IDX +
                sizeof(unsigned int);
    flag = (host_memory_getc(flag_addr + 3) << 24) |
           (host_memory_getc(flag_addr + 2) << 16) |
           (host_memory_getc(flag_addr + 1) << 8) |
           (host_memory_getc(flag_addr + 0) << 0);
    metadata = (host_memory_getc(metadata_addr + 3) << 24) |
               (host_memory_getc(metadata_addr + 2) << 16) |
               (host_memory_getc(metadata_addr + 1) << 8) |
               (host_memory_getc(metadata_addr + 0) << 0);
  } while (!(flag));
  host_memory_putc(flag_addr + 0, 0);
  host_memory_putc(flag_addr + 1, 0);
  host_memory_putc(flag_addr + 2, 0);
  host_memory_putc(flag_addr + 3, 0);
  buf_len = metadata >> 1;
  is_eop = metadata & 0x1;
}

static void kernel_user_memcpy(void *user_buf,
                               unsigned long long kernel_buf_addr,
                               size_t count) {
  for (size_t i = 0; i < count; i++) {
    ((unsigned char *)user_buf)[i] = host_memory_getc(kernel_buf_addr + i);
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
    } else if (first) {
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
      } else {
        kernel_user_memcpy(buf, kbuf_addr + buf_idx, read_size);
        cl_poke(TAG(APP_FREE_BUF_TAG), 0);
        app_bufs_ptr = (app_bufs_ptr + 1) & (ALLOCATED_BUF_NUM - 1);
        buf_idx = 0;
        update_metadata();
      }
    } else {
      read_size = count;
      buf_idx += read_size;
      kernel_user_memcpy(buf, kbuf_addr + buf_idx, read_size);
    }
    return read_size;
  } else {
    return -1;
  }
}

int iclose(int fd) {
  if (fd == VIRT_FILE_FD) {
    cl_poke(TAG(RESET_TAG), 0);
    return 0;
  } else {
    return -1;
  }
}

void set_physical_file(unsigned char *buf, size_t count) {
  for (i = 0; i < count; i++) {
    dram_write(i, buf[i]);
  }
  cl_poke(TAG(APP_FILE_INFO_TAG), 0);
  cl_poke(TAG(APP_FILE_INFO_TAG), count >> 32);
  cl_poke(TAG(APP_FILE_INFO_TAG), count & 0xFFFFFFFF);
}

void send_input_param(unsigned int data) {
  cl_poke(TAG(APP_INPUT_PARAM_TAG), data);
}

static void simulator() {
  init();
  user_simulation_function();
}

#endif
