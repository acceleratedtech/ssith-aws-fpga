// Galois, Inc
// 2019
//
// Dylan Hand

// This is a simple pass-through module to allow JTAG configuration to work

module DMITap(
  input clock,
  input reset,

  input dmi_in_dmiClock,
  input dmi_in_dmiReset,

  output        dmi_out_dmi_req_valid,
  input         dmi_out_dmi_req_ready,
  output [ 6:0] dmi_out_dmi_req_bits_addr,
  output [ 1:0] dmi_out_dmi_req_bits_op,
  output [31:0] dmi_out_dmi_req_bits_data,

  input         dmi_out_dmi_resp_valid,
  output        dmi_out_dmi_resp_ready,
  input  [ 1:0] dmi_out_dmi_resp_bits_resp,
  input  [31:0] dmi_out_dmi_resp_bits_data,

  input         dmi_in_dmi_req_valid,
  output        dmi_in_dmi_req_ready,
  input  [ 6:0] dmi_in_dmi_req_bits_addr,
  input  [ 1:0] dmi_in_dmi_req_bits_op,
  input  [31:0] dmi_in_dmi_req_bits_data,

  output        dmi_in_dmi_resp_valid,
  input         dmi_in_dmi_resp_ready,
  output [ 1:0] dmi_in_dmi_resp_bits_resp,
  output [31:0] dmi_in_dmi_resp_bits_data,

  output dmi_out_dmiClock,
  output dmi_out_dmiReset
);

assign dmi_out_dmiClock = dmi_in_dmiClock;
assign dmi_out_dmiReset = dmi_in_dmiReset;

assign dmi_out_dmi_req_valid     = dmi_in_dmi_req_valid;
assign dmi_out_dmi_req_bits_addr = dmi_in_dmi_req_bits_addr;
assign dmi_out_dmi_req_bits_op   = dmi_in_dmi_req_bits_op;
assign dmi_out_dmi_req_bits_data = dmi_in_dmi_req_bits_data;
assign dmi_out_dmi_resp_ready    = dmi_in_dmi_resp_ready;

assign dmi_in_dmi_req_ready      = dmi_out_dmi_req_ready;
assign dmi_in_dmi_resp_valid     = dmi_out_dmi_resp_valid;
assign dmi_in_dmi_resp_bits_resp = dmi_out_dmi_resp_bits_resp;
assign dmi_in_dmi_resp_bits_data = dmi_out_dmi_resp_bits_data;


endmodule
