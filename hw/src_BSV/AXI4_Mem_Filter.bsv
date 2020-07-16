
import FIFO            :: *;
import GetPut          :: *;
import Vector          :: *;

import Semi_FIFOF      :: *;
import AWS_AXI4_Types  :: *;
import AWS_Fabric_Defs :: *;

interface AXI4_Mem_Filter;
   method Action reset;

   interface AXI4_Slave_IFC #(Wd_Id, Wd_Addr, Wd_Data, Wd_User) from_control;

   // From master
   interface AXI4_Slave_IFC #(Wd_Id, Wd_Addr, 512, Wd_User) from_master;

   // To slave
   interface AXI4_Master_IFC #(Wd_Id, Wd_Addr, 512, Wd_User) to_slave;
endinterface

(* synthesize *)
module mkAXI4_Mem_Filter(AXI4_Mem_Filter);

   Reg#(Bit#(64)) rg_addr_filter <- mkReg(0);

   let xactor_from_control <- mkAXI4_Slave_Xactor;
   let xactor_from_master <- mkAXI4_Slave_Xactor;
   let xactor_to_slave <- mkAXI4_Master_Xactor;

   let rg_control_awcount <- mkReg(0);
   let rg_control_awid <- mkReg(0);
   rule rl_control_wr_addr if (rg_control_awcount == 0);
      let req <- pop_o(xactor_from_control.o_wr_addr);
      rg_control_awid <= req.awid;
      rg_control_awcount <= req.awlen + 1;
   endrule
   rule rl_control_wr_data if (rg_control_awcount != 0);
      let wdata <- pop_o(xactor_from_control.o_wr_data);
      rg_addr_filter <= wdata.wdata;
      if (rg_control_awcount == 1) begin
         xactor_from_control.i_wr_resp.enq(AXI4_Wr_Resp { bid: rg_control_awid, bresp: 0, buser: 0 } );
      end
      rg_control_awcount <= rg_control_awcount - 1;
   endrule

   let rg_control_arid <- mkReg(0);
   let rg_control_arcount <- mkReg(0);
   rule rl_control_rd_addr if (rg_control_arcount == 0);
      let req <- pop_o(xactor_from_control.o_rd_addr);
      rg_control_arid <= req.arid;
      rg_control_arcount <= req.arlen + 1;
   endrule
   rule rl_control_rd_data if (rg_control_arcount != 0);
      xactor_from_control.i_rd_data.enq(AXI4_Rd_Data { rid: rg_control_arid, rdata: rg_addr_filter, rresp: 0, rlast: (rg_control_arcount == 1), ruser: 0 });
      rg_control_arcount <= rg_control_arcount - 1;
   endrule

   let rg_master_awcount <- mkReg(0);
   let rg_master_awid <- mkReg(0);
   let f_wr_is_enclave_addr <- mkFIFO1();

   rule rl_master_wr_addr if (rg_master_awcount == 0);
      let req <- pop_o(xactor_from_master.o_wr_addr);
      Bit#(31) awaddr = truncate(req.awaddr);
      req.awaddr = zeroExtend(awaddr);
      rg_master_awid <= req.awid;
      rg_master_awcount <= req.awlen + 1;

      Bit#(6) addr_bits = req.awaddr[30:25];
      Bool is_enclave_addr = unpack(rg_addr_filter[addr_bits]);
      f_wr_is_enclave_addr.enq(is_enclave_addr);
      if (!is_enclave_addr)
         xactor_to_slave.i_wr_addr.enq(req);
   endrule

   let fifo_wr_resp <- mkFIFO();
   rule rl_master_wr_data if (rg_master_awcount != 0);
      let wdata <- pop_o(xactor_from_master.o_wr_data);
      if (!f_wr_is_enclave_addr.first())
         xactor_to_slave.i_wr_data.enq(wdata);
      rg_master_awcount <= rg_master_awcount - 1;
      if (rg_master_awcount == 1 && f_wr_is_enclave_addr.first())
         fifo_wr_resp.enq(AXI4_Wr_Resp { bid: rg_master_awid, bresp: 0, buser: 0 });
   endrule

   rule rl_master_wr_resp;
      let resp;
      if (f_wr_is_enclave_addr.first()) begin
         resp <- toGet(fifo_wr_resp).get();
      end
      else begin
         resp <- pop_o(xactor_to_slave.o_wr_resp);
      end
      f_wr_is_enclave_addr.deq();
      xactor_from_master.i_wr_resp.enq(resp);
   endrule

   let rg_master_arcount <- mkReg(0);
   let rg_master_arid <- mkReg(0);
   let f_rd_is_enclave_addr <- mkFIFO1();

   rule rl_master_rd_addr if (rg_master_arcount == 0);
      let req <- pop_o(xactor_from_master.o_rd_addr);
      Bit#(31) araddr = truncate(req.araddr);
      req.araddr = zeroExtend(araddr);
      rg_master_arid <= req.arid;
      rg_master_arcount <= req.arlen + 1;

      Bit#(6) addr_bits = req.araddr[30:25];
      Bool is_enclave_addr = unpack(rg_addr_filter[addr_bits]);
      f_rd_is_enclave_addr.enq(is_enclave_addr);
      if (!is_enclave_addr)
         xactor_to_slave.i_rd_addr.enq(req);
   endrule

   rule rl_master_rd_data if (rg_master_arcount != 0);
      Bit#(32) responseword = 32'h0badbeef;
      Vector#(16, Bit#(32)) responsewords = replicate(responseword);
      let rdata = AXI4_Rd_Data { rid: rg_master_arid, rdata: pack(responsewords), rresp: 0, rlast: (rg_master_arcount == 1), ruser: 0 };
      if (!f_rd_is_enclave_addr.first()) begin
         rdata <- pop_o(xactor_to_slave.o_rd_data);
      end
      rg_master_arcount <= rg_master_arcount - 1;
      if (rg_master_arcount == 1)
         f_rd_is_enclave_addr.deq();
      xactor_from_master.i_rd_data.enq(rdata);
   endrule

   method Action reset();
      rg_addr_filter <= 0;
      rg_control_awcount <= 0;
      rg_control_arcount <= 0;
      rg_master_awcount <= 0;
      rg_master_arcount <= 0;
      f_wr_is_enclave_addr.clear();
      f_rd_is_enclave_addr.clear();
      fifo_wr_resp.clear();
      xactor_from_control.reset;
      xactor_from_master.reset;
      xactor_to_slave.reset;
   endmethod

   interface AXI4_Slave_IFC from_control = xactor_from_control.axi_side;

   // From master
   interface AXI4_Slave_IFC from_master = xactor_from_master.axi_side;

   // To slave
   interface AXI4_Master_IFC to_slave = xactor_to_slave.axi_side;
endmodule
