// ==============================================================
// RTL generated by Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC
// Version: 2017.1
// Copyright (C) 1986-2017 Xilinx, Inc. All Rights Reserved.
// 
// ===========================================================

`timescale 1 ns / 1 ps 

(* CORE_GENERATION_INFO="dramA_read_req_multiplexer,hls_ip_2017_1,{HLS_INPUT_TYPE=cxx,HLS_INPUT_FLOAT=0,HLS_INPUT_FIXED=0,HLS_INPUT_PART=xcvu9p-flgb2104-2-i,HLS_INPUT_CLOCK=4.000000,HLS_INPUT_ARCH=others,HLS_SYN_CLOCK=0.594000,HLS_SYN_LAT=-1,HLS_SYN_TPT=none,HLS_SYN_MEM=0,HLS_SYN_DSP=0,HLS_SYN_FF=150,HLS_SYN_LUT=168}" *)

module dramA_read_req_multiplexer (
        ap_clk,
        ap_rst,
        ap_start,
        ap_done,
ap_continue,
        ap_idle,
        ap_ready,
        dramA_read_req_V_num_din,
        dramA_read_req_V_num_full_n,
        dramA_read_req_V_num_write,
        dramA_read_req_V_addr_din,
        dramA_read_req_V_addr_full_n,
        dramA_read_req_V_addr_write,
        host_dramA_read_req_V_num_dout,
        host_dramA_read_req_V_num_empty_n,
        host_dramA_read_req_V_num_read,
        host_dramA_read_req_V_addr_dout,
        host_dramA_read_req_V_addr_empty_n,
        host_dramA_read_req_V_addr_read,
        device_dramA_read_req_V_num_dout,
        device_dramA_read_req_V_num_empty_n,
        device_dramA_read_req_V_num_read,
        device_dramA_read_req_V_addr_dout,
        device_dramA_read_req_V_addr_empty_n,
        device_dramA_read_req_V_addr_read,
        dramA_read_context_V_din,
        dramA_read_context_V_full_n,
        dramA_read_context_V_write
);

parameter    ap_ST_fsm_state1 = 2'd1;
parameter    ap_ST_fsm_pp0_stage0 = 2'd2;

input   ap_clk;
input   ap_rst;
input   ap_start;
output   ap_done;
output   ap_idle;
output ap_ready; output ap_continue;
output  [7:0] dramA_read_req_V_num_din;
input   dramA_read_req_V_num_full_n;
output   dramA_read_req_V_num_write;
output  [63:0] dramA_read_req_V_addr_din;
input   dramA_read_req_V_addr_full_n;
output   dramA_read_req_V_addr_write;
input  [7:0] host_dramA_read_req_V_num_dout;
input   host_dramA_read_req_V_num_empty_n;
output   host_dramA_read_req_V_num_read;
input  [63:0] host_dramA_read_req_V_addr_dout;
input   host_dramA_read_req_V_addr_empty_n;
output   host_dramA_read_req_V_addr_read;
input  [7:0] device_dramA_read_req_V_num_dout;
input   device_dramA_read_req_V_num_empty_n;
output   device_dramA_read_req_V_num_read;
input  [63:0] device_dramA_read_req_V_addr_dout;
input   device_dramA_read_req_V_addr_empty_n;
output   device_dramA_read_req_V_addr_read;
output   dramA_read_context_V_din;
input   dramA_read_context_V_full_n;
output   dramA_read_context_V_write;

reg ap_idle;
reg[7:0] dramA_read_req_V_num_din;
reg[63:0] dramA_read_req_V_addr_din;
reg dramA_read_context_V_din;
reg dramA_read_context_V_write;

(* fsm_encoding = "none" *) reg   [1:0] ap_CS_fsm;
wire    ap_CS_fsm_state1;
reg    dramA_read_req_V_num_blk_n;
wire    ap_CS_fsm_pp0_stage0;
reg    ap_enable_reg_pp0_iter1;
wire    ap_block_pp0_stage0;
reg   [0:0] empty_n_2_reg_197;
reg   [0:0] empty_n_3_reg_211;
reg    dramA_read_req_V_addr_blk_n;
reg    dramA_read_context_V_blk_n;
wire   [0:0] empty_n_2_fu_173_p1;
wire    ap_block_state2_pp0_stage0_iter0;
wire    dramA_read_req_V_num1_status;
reg    ap_predicate_op32_write_state3;
reg    ap_block_state3_pp0_stage0_iter1;
reg    ap_block_pp0_stage0_11001;
reg   [7:0] tmp_num_reg_201;
reg   [63:0] tmp_addr_reg_206;
reg   [7:0] tmp_num_2_reg_215;
reg   [63:0] tmp_addr_2_reg_220;
reg    ap_enable_reg_pp0_iter0;
reg    ap_block_pp0_stage0_subdone;
reg    host_dramA_read_req_V_num0_update;
wire   [0:0] empty_n_nbread_fu_138_p3_0;
reg    device_dramA_read_req_V_num0_update;
wire   [0:0] empty_n_1_nbread_fu_146_p3_0;
reg    dramA_read_req_V_num1_update;
reg    ap_block_pp0_stage0_01001;
reg   [1:0] ap_NS_fsm;
reg    ap_idle_pp0;
wire    ap_enable_pp0;
reg    ap_condition_143;

// power-on initialization
initial begin
#0 ap_CS_fsm = 2'd1;
#0 ap_enable_reg_pp0_iter1 = 1'b0;
#0 ap_enable_reg_pp0_iter0 = 1'b0;
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_CS_fsm <= ap_ST_fsm_state1;
    end else begin
        ap_CS_fsm <= ap_NS_fsm;
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_enable_reg_pp0_iter0 <= 1'b0;
    end else begin
        if (((1'b1 == ap_CS_fsm_state1) & (ap_start == 1'b1))) begin
            ap_enable_reg_pp0_iter0 <= 1'b1;
        end
    end
end

always @ (posedge ap_clk) begin
    if (ap_rst == 1'b1) begin
        ap_enable_reg_pp0_iter1 <= 1'b0;
    end else begin
        if ((ap_block_pp0_stage0_subdone == 1'b0)) begin
            ap_enable_reg_pp0_iter1 <= ap_enable_reg_pp0_iter0;
        end else if (((1'b1 == ap_CS_fsm_state1) & (ap_start == 1'b1))) begin
            ap_enable_reg_pp0_iter1 <= 1'b0;
        end
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_block_pp0_stage0_11001 == 1'b0))) begin
        empty_n_2_reg_197 <= empty_n_nbread_fu_138_p3_0;
        tmp_addr_reg_206 <= host_dramA_read_req_V_addr_dout;
        tmp_num_reg_201 <= host_dramA_read_req_V_num_dout;
    end
end

always @ (posedge ap_clk) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_block_pp0_stage0_11001 == 1'b0) & (1'd0 == empty_n_2_fu_173_p1))) begin
        empty_n_3_reg_211 <= empty_n_1_nbread_fu_146_p3_0;
        tmp_addr_2_reg_220 <= device_dramA_read_req_V_addr_dout;
        tmp_num_2_reg_215 <= device_dramA_read_req_V_num_dout;
    end
end

always @ (*) begin
    if (((1'b0 == ap_start) & (1'b1 == ap_CS_fsm_state1))) begin
        ap_idle = 1'b1;
    end else begin
        ap_idle = 1'b0;
    end
end

always @ (*) begin
    if (((1'b0 == ap_enable_reg_pp0_iter0) & (1'b0 == ap_enable_reg_pp0_iter1))) begin
        ap_idle_pp0 = 1'b1;
    end else begin
        ap_idle_pp0 = 1'b0;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_block_pp0_stage0_11001 == 1'b0) & (1'd0 == empty_n_2_fu_173_p1) & (1'b1 == ap_enable_reg_pp0_iter0) & (1'b1 == (device_dramA_read_req_V_num_empty_n & device_dramA_read_req_V_addr_empty_n)))) begin
        device_dramA_read_req_V_num0_update = 1'b1;
    end else begin
        device_dramA_read_req_V_num0_update = 1'b0;
    end
end

always @ (*) begin
    if ((((1'b1 == ap_CS_fsm_pp0_stage0) & (1'b1 == ap_enable_reg_pp0_iter1) & (ap_block_pp0_stage0 == 1'b0) & (empty_n_2_reg_197 == 1'd1)) | ((1'b1 == ap_CS_fsm_pp0_stage0) & (1'b1 == ap_enable_reg_pp0_iter1) & (ap_block_pp0_stage0 == 1'b0) & (empty_n_2_reg_197 == 1'd0) & (1'd1 == empty_n_3_reg_211)))) begin
        dramA_read_context_V_blk_n = dramA_read_context_V_full_n;
    end else begin
        dramA_read_context_V_blk_n = 1'b1;
    end
end

always @ (*) begin
    if ((ap_condition_143 == 1'b1)) begin
        if ((empty_n_2_reg_197 == 1'd1)) begin
            dramA_read_context_V_din = 1'd0;
        end else if ((1'b1 == ap_predicate_op32_write_state3)) begin
            dramA_read_context_V_din = 1'd1;
        end else begin
            dramA_read_context_V_din = 'bx;
        end
    end else begin
        dramA_read_context_V_din = 'bx;
    end
end

always @ (*) begin
    if ((((1'b1 == ap_CS_fsm_pp0_stage0) & (1'b1 == ap_enable_reg_pp0_iter1) & (1'b1 == ap_predicate_op32_write_state3) & (ap_block_pp0_stage0_11001 == 1'b0)) | ((1'b1 == ap_CS_fsm_pp0_stage0) & (1'b1 == ap_enable_reg_pp0_iter1) & (empty_n_2_reg_197 == 1'd1) & (ap_block_pp0_stage0_11001 == 1'b0)))) begin
        dramA_read_context_V_write = 1'b1;
    end else begin
        dramA_read_context_V_write = 1'b0;
    end
end

always @ (*) begin
    if ((((1'b1 == ap_CS_fsm_pp0_stage0) & (1'b1 == ap_enable_reg_pp0_iter1) & (ap_block_pp0_stage0 == 1'b0) & (empty_n_2_reg_197 == 1'd1)) | ((1'b1 == ap_CS_fsm_pp0_stage0) & (1'b1 == ap_enable_reg_pp0_iter1) & (ap_block_pp0_stage0 == 1'b0) & (empty_n_2_reg_197 == 1'd0) & (1'd1 == empty_n_3_reg_211)))) begin
        dramA_read_req_V_addr_blk_n = dramA_read_req_V_addr_full_n;
    end else begin
        dramA_read_req_V_addr_blk_n = 1'b1;
    end
end

always @ (*) begin
    if ((ap_condition_143 == 1'b1)) begin
        if ((empty_n_2_reg_197 == 1'd1)) begin
            dramA_read_req_V_addr_din = tmp_addr_reg_206;
        end else if ((1'b1 == ap_predicate_op32_write_state3)) begin
            dramA_read_req_V_addr_din = tmp_addr_2_reg_220;
        end else begin
            dramA_read_req_V_addr_din = 'bx;
        end
    end else begin
        dramA_read_req_V_addr_din = 'bx;
    end
end

always @ (*) begin
    if ((((1'b1 == ap_CS_fsm_pp0_stage0) & (1'b1 == ap_enable_reg_pp0_iter1) & (1'b1 == ap_predicate_op32_write_state3) & (ap_block_pp0_stage0_11001 == 1'b0)) | ((1'b1 == ap_CS_fsm_pp0_stage0) & (1'b1 == ap_enable_reg_pp0_iter1) & (empty_n_2_reg_197 == 1'd1) & (ap_block_pp0_stage0_11001 == 1'b0)))) begin
        dramA_read_req_V_num1_update = 1'b1;
    end else begin
        dramA_read_req_V_num1_update = 1'b0;
    end
end

always @ (*) begin
    if ((((1'b1 == ap_CS_fsm_pp0_stage0) & (1'b1 == ap_enable_reg_pp0_iter1) & (ap_block_pp0_stage0 == 1'b0) & (empty_n_2_reg_197 == 1'd1)) | ((1'b1 == ap_CS_fsm_pp0_stage0) & (1'b1 == ap_enable_reg_pp0_iter1) & (ap_block_pp0_stage0 == 1'b0) & (empty_n_2_reg_197 == 1'd0) & (1'd1 == empty_n_3_reg_211)))) begin
        dramA_read_req_V_num_blk_n = dramA_read_req_V_num_full_n;
    end else begin
        dramA_read_req_V_num_blk_n = 1'b1;
    end
end

always @ (*) begin
    if ((ap_condition_143 == 1'b1)) begin
        if ((empty_n_2_reg_197 == 1'd1)) begin
            dramA_read_req_V_num_din = tmp_num_reg_201;
        end else if ((1'b1 == ap_predicate_op32_write_state3)) begin
            dramA_read_req_V_num_din = tmp_num_2_reg_215;
        end else begin
            dramA_read_req_V_num_din = 'bx;
        end
    end else begin
        dramA_read_req_V_num_din = 'bx;
    end
end

always @ (*) begin
    if (((1'b1 == ap_CS_fsm_pp0_stage0) & (ap_block_pp0_stage0_11001 == 1'b0) & (1'b1 == ap_enable_reg_pp0_iter0) & (1'b1 == (host_dramA_read_req_V_num_empty_n & host_dramA_read_req_V_addr_empty_n)))) begin
        host_dramA_read_req_V_num0_update = 1'b1;
    end else begin
        host_dramA_read_req_V_num0_update = 1'b0;
    end
end

always @ (*) begin
    case (ap_CS_fsm)
        ap_ST_fsm_state1 : begin
            if (((1'b1 == ap_CS_fsm_state1) & (ap_start == 1'b1))) begin
                ap_NS_fsm = ap_ST_fsm_pp0_stage0;
            end else begin
                ap_NS_fsm = ap_ST_fsm_state1;
            end
        end
        ap_ST_fsm_pp0_stage0 : begin
            ap_NS_fsm = ap_ST_fsm_pp0_stage0;
        end
        default : begin
            ap_NS_fsm = 'bx;
        end
    endcase
end

assign ap_CS_fsm_pp0_stage0 = ap_CS_fsm[32'd1];

assign ap_CS_fsm_state1 = ap_CS_fsm[32'd0];

assign ap_block_pp0_stage0 = ~(1'b1 == 1'b1);

always @ (*) begin
    ap_block_pp0_stage0_01001 = ((1'b1 == ap_enable_reg_pp0_iter1) & (((1'b0 == dramA_read_req_V_num1_status) & (1'b1 == ap_predicate_op32_write_state3)) | ((1'b1 == ap_predicate_op32_write_state3) & (1'b0 == dramA_read_context_V_full_n)) | ((empty_n_2_reg_197 == 1'd1) & (1'b0 == dramA_read_req_V_num1_status)) | ((empty_n_2_reg_197 == 1'd1) & (1'b0 == dramA_read_context_V_full_n))));
end

always @ (*) begin
    ap_block_pp0_stage0_11001 = ((1'b1 == ap_enable_reg_pp0_iter1) & (((1'b0 == dramA_read_req_V_num1_status) & (1'b1 == ap_predicate_op32_write_state3)) | ((1'b1 == ap_predicate_op32_write_state3) & (1'b0 == dramA_read_context_V_full_n)) | ((empty_n_2_reg_197 == 1'd1) & (1'b0 == dramA_read_req_V_num1_status)) | ((empty_n_2_reg_197 == 1'd1) & (1'b0 == dramA_read_context_V_full_n))));
end

always @ (*) begin
    ap_block_pp0_stage0_subdone = ((1'b1 == ap_enable_reg_pp0_iter1) & (((1'b0 == dramA_read_req_V_num1_status) & (1'b1 == ap_predicate_op32_write_state3)) | ((1'b1 == ap_predicate_op32_write_state3) & (1'b0 == dramA_read_context_V_full_n)) | ((empty_n_2_reg_197 == 1'd1) & (1'b0 == dramA_read_req_V_num1_status)) | ((empty_n_2_reg_197 == 1'd1) & (1'b0 == dramA_read_context_V_full_n))));
end

assign ap_block_state2_pp0_stage0_iter0 = ~(1'b1 == 1'b1);

always @ (*) begin
    ap_block_state3_pp0_stage0_iter1 = (((1'b0 == dramA_read_req_V_num1_status) & (1'b1 == ap_predicate_op32_write_state3)) | ((1'b1 == ap_predicate_op32_write_state3) & (1'b0 == dramA_read_context_V_full_n)) | ((empty_n_2_reg_197 == 1'd1) & (1'b0 == dramA_read_req_V_num1_status)) | ((empty_n_2_reg_197 == 1'd1) & (1'b0 == dramA_read_context_V_full_n)));
end

always @ (*) begin
    ap_condition_143 = ((1'b1 == ap_CS_fsm_pp0_stage0) & (1'b1 == ap_enable_reg_pp0_iter1) & (ap_block_pp0_stage0_01001 == 1'b0));
end

assign ap_done = 1'b0;

assign ap_enable_pp0 = (ap_idle_pp0 ^ 1'b1);

always @ (*) begin
    ap_predicate_op32_write_state3 = ((empty_n_2_reg_197 == 1'd0) & (1'd1 == empty_n_3_reg_211));
end

assign ap_ready = 1'b0;

assign device_dramA_read_req_V_addr_read = device_dramA_read_req_V_num0_update;

assign device_dramA_read_req_V_num_read = device_dramA_read_req_V_num0_update;

assign dramA_read_req_V_addr_write = dramA_read_req_V_num1_update;

assign dramA_read_req_V_num1_status = (dramA_read_req_V_num_full_n & dramA_read_req_V_addr_full_n);

assign dramA_read_req_V_num_write = dramA_read_req_V_num1_update;

assign empty_n_1_nbread_fu_146_p3_0 = (device_dramA_read_req_V_num_empty_n & device_dramA_read_req_V_addr_empty_n);

assign empty_n_2_fu_173_p1 = empty_n_nbread_fu_138_p3_0;

assign empty_n_nbread_fu_138_p3_0 = (host_dramA_read_req_V_num_empty_n & host_dramA_read_req_V_addr_empty_n);

assign host_dramA_read_req_V_addr_read = host_dramA_read_req_V_num0_update;

assign host_dramA_read_req_V_num_read = host_dramA_read_req_V_num0_update;

endmodule //dramA_read_req_multiplexer
