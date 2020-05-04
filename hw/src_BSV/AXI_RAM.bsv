
import BRAM         :: *;
import Clocks       :: *;
import DefaultValue :: *;
import FIFO         :: *;
import GetPut       :: *;

import Semi_FIFOF   :: *;
import AXI4_Types   :: *;

interface AXI_BRAM;
   interface AXI4_Slave_IFC#(6, 64, 512, 0) portA;
   interface AXI4_Slave_IFC#(6, 64, 512, 0) portB;
endinterface

module mkAXI_BRAM(AXI_BRAM);
   AXI4_Slave_Xactor_IFC#(6, 64, 512, 0) bram_slave_xactor_portA <- mkAXI4_Slave_Xactor();
   AXI4_Slave_Xactor_IFC#(6, 64, 512, 0) bram_slave_xactor_portB <- mkAXI4_Slave_Xactor();

   let addrShift = valueOf(TLog#(TDiv#(512, 8)));
   let bramConfig = defaultValue;
   bramConfig.memorySize = 1024;
   bramConfig.latency = 2;
   BRAM2PortBE #(Bit#(32), Bit#(512), 64)  bram <- mkBRAM2ServerBE (bramConfig); // latency 1
   let bankaFifo <- mkFIFO();
   Reg#(Bool) rg_valid1 <- mkReg(False);
   rule rl_banka_aw;
      let req <- pop_o(bram_slave_xactor_portA.o_wr_addr);
      let wdata <- pop_o(bram_slave_xactor_portA.o_wr_data);
      bram.portA.request.put(BRAMRequestBE { writeen: wdata.wstrb,
                                             address: truncate(req.awaddr >> addrShift),
                                              responseOnWrite: False,
                                              datain: wdata.wdata });
      $display("banka write %h", req.awaddr);
      bram_slave_xactor_portA.i_wr_resp.enq(AXI4_Wr_Resp { bid: req.awid, bresp: 0, buser: req.awuser });
   endrule
   rule rl_banka_ar;
      let req <- pop_o(bram_slave_xactor_portA.o_rd_addr);
      bram.portA.request.put(BRAMRequestBE { writeen: 0,
                                             address: truncate(req.araddr >> addrShift),
                                             responseOnWrite: False,
                                             datain: 0 });
      $display("banka read %h", req.araddr);
      bankaFifo.enq(req.arid);
   endrule
   rule rl_banka_rdata;
      let arid <- toGet(bankaFifo).get();
      let data <- bram.portA.response.get();
      bram_slave_xactor_portA.i_rd_data.enq(AXI4_Rd_Data { rid: arid, rdata: data, rresp: 0, rlast: True, ruser: 0 });
   endrule

   let bankbFifo <- mkFIFO();
   rule rl_bankb_aw;
      let req <- pop_o(bram_slave_xactor_portB.o_wr_addr);
      let wdata <- pop_o(bram_slave_xactor_portB.o_wr_data);
      bram.portB.request.put(BRAMRequestBE { writeen: wdata.wstrb,
                                             address: truncate(req.awaddr >> addrShift),
                                             responseOnWrite: False,
                                             datain: wdata.wdata });
      $display("bankb write %h", req.awaddr);
      bram_slave_xactor_portB.i_wr_resp.enq(AXI4_Wr_Resp { bid: req.awid, bresp: 0, buser: req.awuser });
   endrule
   rule rl_bankb_ar;
      let req <- pop_o(bram_slave_xactor_portB.o_rd_addr);
      bram.portB.request.put(BRAMRequestBE { writeen: 0,
                                             address: truncate(req.araddr >> addrShift),
                                             responseOnWrite: False,
                                             datain: 0 });
      $display("bankb read %h", req.araddr);
      bankbFifo.enq(req.arid);
   endrule
   rule rl_bankb_rdata;
      let arid <- toGet(bankbFifo).get();
      let data <- bram.portB.response.get();
      bram_slave_xactor_portB.i_rd_data.enq(AXI4_Rd_Data { rid: arid, rdata: data, rresp: 0, rlast: True, ruser: 0 });
   endrule

   interface portA = bram_slave_xactor_portA.axi_side;
   interface portB = bram_slave_xactor_portB.axi_side;
endmodule
