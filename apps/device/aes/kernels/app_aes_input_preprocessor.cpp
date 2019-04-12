#ifndef APP_AES_INPUT_PREPROCESSOR_CPP_
#define APP_AES_INPUT_PREPROCESSOR_CPP_

#include <insider_kernel.h>

#include "constant.h"
#include "structure.h"
void app_aes_input_preprocessor(ST_Queue<APP_Data> &app_input_data,
                                ST_Queue<aes_inout> &app_aes_input_data_chan) {
  while (1) {
#pragma HLS pipeline
    APP_Data current_input_data;
    if (app_input_data.read_nb(current_input_data)) {

      unsigned char current_input_data_in_bytes[STT_LNG];
      aes_inout aes_input_data[MAX_AES_PARAM_SIZE];

      for (int i = 0; i < MAX_AES_PARAM_SIZE; i++) {
#pragma HLS unroll
      	      for (int j = 0; j < STT_LNG; j++) {
#pragma HLS unroll
        current_input_data_in_bytes[j] =
            current_input_data.data(j * 8 + 7, j * 8);
      }
      aes_input_data[i].data0 = current_input_data_in_bytes[0];
      aes_input_data[i].data1 = current_input_data_in_bytes[1];
      aes_input_data[i].data2 = current_input_data_in_bytes[2];
      aes_input_data[i].data3 = current_input_data_in_bytes[3];
      aes_input_data[i].data4 = current_input_data_in_bytes[4];
      aes_input_data[i].data5 = current_input_data_in_bytes[5];
      aes_input_data[i].data6 = current_input_data_in_bytes[6];
      aes_input_data[i].data7 = current_input_data_in_bytes[7];
      aes_input_data[i].data8 = current_input_data_in_bytes[8];
      aes_input_data[i].data9 = current_input_data_in_bytes[9];
      aes_input_data[i].data10 = current_input_data_in_bytes[10];
      aes_input_data[i].data11 = current_input_data_in_bytes[11];
      aes_input_data[i].data12 = current_input_data_in_bytes[12];
      aes_input_data[i].data13 = current_input_data_in_bytes[13];
      aes_input_data[i].data14 = current_input_data_in_bytes[14];
      aes_input_data[i].data15 = current_input_data_in_bytes[15];
      }
      app_aes_input_data_chan.write(aes_input_data);
    }
  }
}
#endif
