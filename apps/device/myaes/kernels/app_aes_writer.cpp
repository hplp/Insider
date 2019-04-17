#ifndef APP_AES_WRITER_CPP_
#define APP_AES_WRITER_CPP_
#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"

void app_aes_writer(ST_Queue<APP_Data> &app_output_data,
                    ST_Queue<aes_inout> &app_aes_output_data_chan) {
  while (1) {
#pragma HLS pipeline

    aes_inout data_out;
    if (app_aes_output_data_chan.read_nb(data_out)) {

      APP_Data current_output_data;

      current_output_data = data_out; // TODO
      app_output_data.write(current_output_data);
    }
  }
}
#endif
