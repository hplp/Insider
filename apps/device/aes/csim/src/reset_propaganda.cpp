#ifndef RESET_PROPAGANDA_CPP_
#define RESET_PROPAGANDA_CPP_

#include <insider_kernel.h>

void reset_propaganda(ST_Queue<bool> &reset_app_aes_end,
                      ST_Queue<bool> &reset_app_aes_process_0,
                      ST_Queue<bool> &reset_app_aes_process_1,
                      ST_Queue<bool> &reset_app_aes_process_2,
                      ST_Queue<bool> &reset_app_aes_process_3,
                      ST_Queue<bool> &reset_app_aes_process_4,
                      ST_Queue<bool> &reset_app_aes_process_5,
                      ST_Queue<bool> &reset_app_aes_process_6,
                      ST_Queue<bool> &reset_app_aes_process_7,
                      ST_Queue<bool> &reset_app_aes_process_8,
                      ST_Queue<bool> &reset_app_aes_start,
                      ST_Queue<bool> &reset_sigs,
                      ST_Queue<bool> &reset_dram_helper_app,
                      ST_Queue<bool> &reset_pcie_helper_app,
                      ST_Queue<bool> &reset_pcie_data_splitter_app) {
  while (1) {
#pragma HLS pipeline
    bool dummy;
    if (reset_sigs.read_nb(dummy)) {
      reset_dram_helper_app.write(0);
      reset_pcie_helper_app.write(0);
      reset_pcie_data_splitter_app.write(0);

      reset_app_aes_end.write(0);
      reset_app_aes_process_0.write(0);
      reset_app_aes_process_1.write(0);
      reset_app_aes_process_2.write(0);
      reset_app_aes_process_3.write(0);
      reset_app_aes_process_4.write(0);
      reset_app_aes_process_5.write(0);
      reset_app_aes_process_6.write(0);
      reset_app_aes_process_7.write(0);
      reset_app_aes_process_8.write(0);
      reset_app_aes_start.write(0);
    }
  }
}
#endif