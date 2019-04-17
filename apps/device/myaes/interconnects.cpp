#include <insider_itc.h>

#include "constant.h"
#include "structure.h"

#include "app_AES_writer.cpp"

void interconnects() {

  ST_Queue<aes_inout> app_aes_input_data_chan(16);
  ST_Queue<aes_inout> app_aes_output_data_chan(16);

  app_aes_input_preprocessor(app_input_data, app_aes_input_data_chan);
  app_aes_do(app_input_params, app_aes_input_data_chan, app_aes_output_data_chan);
  app_aes_writer(app_output_data, app_aes_output_data_chan);

}
