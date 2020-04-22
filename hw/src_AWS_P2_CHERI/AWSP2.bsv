
import BRAMFIFO     :: *;
import BuildVector  :: *;
import ClientServer :: *;
import Connectable  :: *;
import FIFO         :: *;
import FIFOF        :: *;
import GetPut       :: *;
import Vector       :: *;

import AxiBits           :: *;
import ConnectalConfig   :: *;
import ConnectalMemTypes :: *;

// ================================================================
// Project imports

import P2_Core  :: *;
import SoC_Map  :: *;

// The basic core
import Core_IFC :: *;
import Core     :: *;

// External interrupt request interface
import PLIC :: *;    // for PLIC_Source_IFC type which is exposed at P2_Core interface

import Semi_FIFOF :: *;

// Main Fabric
import AXI4_Types   :: *;
`ifdef BLUESPEC_AXI
import AXI4_Fabric  :: *;
import AXI4_Deburster :: *;
import AXI_Mem_Controller :: *;
import Fabric_Defs  :: *;
`endif // BLUESPEC_AXI

`ifdef INCLUDE_TANDEM_VERIF
import TV_Info :: *;
import AXI4_Stream ::*;
`endif

`ifdef INCLUDE_GDB_CONTROL
import Debug_Module :: *;
`ifdef JTAG_TAP
import JtagTap      :: *;
`endif
import Giraffe_IFC  :: *;
`endif

import AWSP2_IFC   :: *;

`ifdef BOARD_awsf1
typedef 2 NumFabricMasters;
`else
typedef 2 NumFabricMasters;
`endif

`ifdef BOARD_awsf1
`ifdef AWSF1_DDR_A
`define USE_DDR
`endif
`endif

interface AWSP2;
  interface AWSP2_Request request;
  interface Vector#(1, MemReadClient#(DataBusWidth)) readClients;
  interface Vector#(1, MemWriteClient#(DataBusWidth)) writeClients;
`ifdef USE_DDR
   interface AWSP2_Pin_IFC pins;
`endif
endinterface


module mkAWSP2#(AWSP2_Response response)(AWSP2);

   let soc_map <- mkSoC_Map();
   P2_Core_IFC p2_core <- mkP2_Core();

   Reg#(Bit#(4)) rg_verbosity <- mkReg(0);
   Reg#(Bool) rg_ready <- mkReg(False);

   Reg#(Bit#(32)) rg_irq_levels[2] <- mkCReg(2, 0);

   Vector#(16, Reg#(Bit#(8)))    objIds <- replicateM(mkReg(0));

`ifdef BLUESPEC_AXI
   AXI4_Fabric_IFC#(NumFabricMasters, 2, 4, 64, 64, 0) axiFabric <- mkAXI4_Fabric_2x2();
   mkConnection(p2_core.master0, axiFabric.v_from_masters[0]);
   mkConnection(p2_core.master1, axiFabric.v_from_masters[1]);
   let to_slave0 = axiFabric.v_to_slaves[0];
   let to_slave1 = axiFabric.v_to_slaves[1];

   AXI4_Deburster_IFC #(Wd_Id, Wd_Addr, Wd_Data, Wd_User) deburster <- mkAXI4_Deburster();
   let memController <- mkAXI_Mem_Controller();

   AXI4_Fabric_IFC#(3, 2, 4, 64, 512, 0) memFabric <- mkMemFabric();
   mkConnection(to_slave0, deburster.from_master);
   mkConnection(deburster.to_slave, memController.slave);
   //let rawmem_xn <- mkConnection(memController.to_raw_mem, memFabric.v_from_masters[0]);
   //let to_ddr = memFabric.v_to_slaves[0];
   let to_ddr = memController.to_raw_mem;

   AXI4_Master_Xactor_IFC#(4, 64, 512, 0) ddr_master_xactor <- mkAXI4_Master_Xactor();
   //mkConnection(ddr_master_xactor.axi_side, memFabric.v_from_masters[1]);
   //let from_dma_pcis = memFabric.v_from_masters[2];
`endif

   FIFOF#(MemRequest) readReqFifo0 <- mkFIFOF();
   FIFOF#(MemRequest) writeReqFifo0 <- mkFIFOF();
   FIFOF#(MemData#(DataBusWidth))   readDataFifo0 <- mkSizedBRAMFIFOF(64);
   FIFOF#(MemData#(DataBusWidth))   writeDataFifo0 <- mkSizedBRAMFIFOF(64);
   FIFOF#(Bit#(MemTagSize)) doneFifo0 <- mkFIFOF();

`ifndef USE_DDR
`ifdef BLUESPEC_AXI
   AXI4_Slave_Xactor_IFC#(4, 64, 512, 0) ddr_slave_xactor <- mkAXI4_Slave_Xactor();
   let ddr_xn <- mkConnectionVerbose(to_ddr, ddr_slave_xactor.axi_side);

   rule master0_aw if (rg_ready);
      let req <- pop_o(ddr_slave_xactor.o_wr_addr);
      let awaddr = req.awaddr;
      let awlen  = req.awlen;
      let awsize = req.awsize;
      let awid   = req.awid;

      Bit#(30) byteaddr = truncate(awaddr);
      let objId = objIds[8];
      let burstLen = fromInteger(valueOf(TDiv#(DataBusWidth,8))) * (awlen + 1);
      let memreq = MemRequest { sglId: extend(objId), offset: extend(byteaddr), burstLen: extend(burstLen), tag: extend(awid) };
      if (rg_verbosity > 1 || truncate(awaddr) != byteaddr)
	 $display("master0 awaddr %h awlen=%d awsize=%d awid=%d byteaddr=%h objId=%d burstLen=%d",
		  awaddr, awlen, awsize, awid, byteaddr, objId, burstLen);

      writeReqFifo0.enq(memreq);
   endrule

   rule master0_wdata if (rg_ready);
      let req <- pop_o(ddr_slave_xactor.o_wr_data);
      let wdata = req.wdata;
      let wstrb = req.wstrb;
      let wlast = req.wlast;
      if (rg_verbosity > 1) $display("master0 wdata %h wstrb %h", wdata, wstrb);
      writeDataFifo0.enq(MemData { data: wdata, tag: 0, byte_enables: wstrb, last: wlast});
    endrule

   rule master0_b if (rg_ready);
      let bvalid = doneFifo0.notEmpty();
      let bid    = doneFifo0.first();
      let bresp = 0;
      let buser = 0;
      ddr_slave_xactor.i_wr_resp.enq(AXI4_Wr_Resp { bid: truncate(bid), bresp: bresp, buser: buser });
      doneFifo0.deq();
   endrule

   rule master0_ar if (rg_ready);
      let req <- pop_o(ddr_slave_xactor.o_rd_addr);
      let araddr = req.araddr();
      let arlen  = req.arlen();
      let arsize = req.arsize();
      let arid   = req.arid();

      Bit#(30) byteaddr = truncate(araddr);
      let objId = objIds[8];
      let burstLen = fromInteger(valueOf(TDiv#(DataBusWidth,8))) * (arlen + 1);
      let memreq = MemRequest { sglId: extend(objId), offset: extend(byteaddr), burstLen: extend(burstLen), tag: extend(arid) };
      if (rg_verbosity > 1 || truncate(araddr) != byteaddr)
	 $display("master0 araddr %h arlen=%d arsize=%d arid=%d byteaddr=%h objId=%d burstLen=%d",
		   araddr, arlen, arsize, arid, byteaddr, objId, burstLen);
      readReqFifo0.enq(memreq);
   endrule

   rule master0_rdata if (rg_ready);
      let rdata = readDataFifo0.first;
      readDataFifo0.deq();
      if (rg_verbosity > 1) $display("master0 rdata %h rid %d last %d", rdata.data, rdata.tag, rdata.last);
      ddr_slave_xactor.i_rd_data.enq(AXI4_Rd_Data { rid: truncate(rdata.tag),
                                                    rdata: rdata.data,
			                            rresp: 0,
			                            rlast: rdata.last,
			                            ruser: 0 }); // ruser

   endrule
`endif // BLUESPEC_AXI
`endif // not USE_DDR

`ifdef BLUESPEC_AXI
   AXI4_Slave_Xactor_IFC#(4, 64, 64, 0) io_slave_xactor <- mkAXI4_Slave_Xactor();
   mkConnection(to_slave1, io_slave_xactor.axi_side);

   rule master1_aw if (rg_ready);
      let req <- pop_o(io_slave_xactor.o_wr_addr);
      let awaddr = req.awaddr;
      let len    = req.awlen;
      let size   = req.awsize;
      let awid   = req.awid;

      Bit#(4)  objNumber = truncate(awaddr >> 28);
      Bit#(28) objOffset = truncate(awaddr);
      let objId = objIds[objNumber];
      let burstLen = 8 * (len + 1);
      if (rg_verbosity > 0)
	  $display("master1 awaddr %h len=%d size=%d id=%d objId=%d objOffset=%h burstLen=%d", awaddr, len, size, awid, objId, objOffset, burstLen);
      response.io_awaddr(truncate(awaddr), extend(burstLen), extend(awid));
   endrule

   rule master1_wdata if (rg_ready);
      let req <- pop_o(io_slave_xactor.o_wr_data);
      let wdata = req.wdata;
      let wstrb = req.wstrb;
      let wlast = req.wlast;
      if (rg_verbosity > 0) $display("master1 wdata %h wstrb %h", wdata, wstrb);
      response.io_wdata(wdata, 0);
    endrule

   rule master1_ar if (rg_ready);
      let req <- pop_o(io_slave_xactor.o_rd_addr);
      let araddr = req.araddr();
      let len    = req.arlen();
      let size   = req.arsize();
      let arid   = req.arid();

      Bit#(4) objNumber = truncate(araddr >> 28);
      Bit#(28) objOffset = truncate(araddr);

      let objId = objIds[objNumber];
      let burstLen = 8 * (len + 1);
      if (rg_verbosity > 0)
	  $display("master1 araddr %h len=%d size=%d id=%d objId=%d objOffset=%h", araddr, len, size, arid, objId, objOffset);
      response.io_araddr(truncate(araddr), extend(burstLen), extend(arid));
   endrule
`endif

`ifdef INCLUDE_GDB_CONTROL
   let dmiReadFifo <- mkFIFOF();
   let dmiWriteFifo <- mkFIFOF();
   let dmiDataFifo <- mkFIFOF();
   rule dmi_read_data_rl;
      let rdata <- p2_core.dmi.read_data();
      //$display("dmi_read_data %h", rdata);
      dmiDataFifo.enq(rdata);
   endrule
   rule dmi_read_rsp_rl;
      let rdata <- toGet(dmiDataFifo).get();
      response.dmi_read_data(rdata);
   endrule
   rule dmi_read_rl;
      let addr <- toGet(dmiReadFifo).get();
      //$display("dmi_read addr %h", addr);
      p2_core.dmi.read_addr(addr);
   endrule
   rule dmi_write_rl;
      let req <- toGet(dmiWriteFifo).get();
      //$display("dmi_write addr %h data %h", tpl_1(req), tpl_2(req));
      p2_core.dmi.write(tpl_1(req), tpl_2(req));
      dmiDataFifo.enq(tpl_2(req));
   endrule
`endif

`ifdef INCLUDE_TANDEM_VERIF
   Reg#(Bool) rg_capture_tv_info <- mkReg(False);
   let tvFifo <- mkFIFOF();
   rule tv_out;
      if (p2_core.tv_verifier_info_tx.m_tvalid() && rg_capture_tv_info) begin
          let tv_bits = p2_core.tv_verifier_info_tx.m_tdata();
          let tv_strb = p2_core.tv_verifier_info_tx.m_tstrb();
          tvFifo.enq(tv_bits);
      end
      p2_core.tv_verifier_info_tx.m_tready(tvFifo.notFull());
   endrule
   rule tv_ready;
      let tv_bits <- toGet(tvFifo).get();
      Info_CPU_to_Verifier info = unpack(tv_bits);
      response.tandem_packet(info.num_bytes, info.vec_bytes);
   endrule
`endif

   MemReadClient#(DataBusWidth) readClient0 = (interface MemReadClient;
      interface Get readReq = toGet(readReqFifo0);
      interface Put readData = toPut(readDataFifo0);
   endinterface );
   MemWriteClient#(DataBusWidth) writeClient0 = (interface MemWriteClient;
      interface Get writeReq = toGet(writeReqFifo0);
      interface Get writeData = toGet(writeDataFifo0);
      interface Put writeDone = toPut(doneFifo0);
   endinterface );

   Reg#(Bool) rg_addr_map_set <- mkReg(False);
   function Action fn_reset();
   action
      rg_addr_map_set <= False;
`ifdef BLUESPEC_AXI
      axiFabric.reset();
      deburster.reset();
      //memController.server_reset.request.put(?);
      memFabric.reset();
      ddr_master_xactor.reset();
`ifndef USE_DDR
      ddr_slave_xactor.reset();
`endif
      io_slave_xactor.reset();
`endif // BLUESPEC_AXI
   endaction
   endfunction

   Reg#(Bool) rg_power_on_reset <- mkReg(False);
   rule rl_reset if (!rg_power_on_reset);
      fn_reset();
      rg_power_on_reset <= True;
   endrule
`ifdef BLUESPEC_AXI
   rule rl_reset_done;
      $display("memController.server_reset.response.get()");
      let b <- memController.server_reset.response.get();
   endrule
`endif // BLUESPEC_AXI

   rule rl_set_addr_map if (!rg_addr_map_set);
`ifdef BLUESPEC_AXI
      memController.set_addr_map(min(soc_map.m_ddr4_0_uncached_addr_base, soc_map.m_ddr4_0_cached_addr_base),
                                 max(soc_map.m_ddr4_0_uncached_addr_lim, soc_map.m_ddr4_0_cached_addr_lim));
`endif
      rg_addr_map_set <= True;
   endrule

   rule rl_ddr_rdata;
`ifdef BLUESPEC_AXI
      let rdata <- pop_o(ddr_master_xactor.o_rd_data);
      response.ddr_data(unpack(rdata.rdata));
`endif
   endrule

   rule rl_wr_resp;
`ifdef BLUESPEC_AXI
      let resp <- pop_o(ddr_master_xactor.o_wr_resp);
      response.ddr_data(unpack(0));
`endif
   endrule

   rule rl_irq_levels;
      p2_core.interrupt_reqs(truncate(rg_irq_levels[0]));
   endrule

   interface AWSP2_Request request;
      method Action dmi_read(Bit#(7) addr);
        //$display("dmi_read req addr %h", addr);
`ifdef INCLUDE_GDB_CONTROL
         dmiReadFifo.enq(addr);
`endif
      endmethod
      method Action dmi_write(Bit#(7) addr, Bit#(32) data);
        //$display("dmi_write req addr %h data %h", addr, data);
`ifdef INCLUDE_GDB_CONTROL
        dmiWriteFifo.enq(tuple2(addr, data));
`endif
      endmethod
      method Action dmi_status();
         Bit#(16) status = 0;
`ifdef INCLUDE_GDB_CONTROL
         status[0] = pack(dmiReadFifo.notEmpty());
         status[1] = pack(dmiWriteFifo.notEmpty());
         status[2] = pack(dmiDataFifo.notEmpty());
`endif
`ifdef BLUESPEC_AXI
         status[15:8] = memController.status();
`endif // BLUESPEC_AXI
         response.dmi_status_data(status);
      endmethod

      method Action ddr_read(Bit#(32) addr);
`ifdef BLUESPEC_AXI
         let req = AXI4_Rd_Addr {
            arid: 0,
	    araddr: extend(addr),
	    arlen: 0,
	    arsize: axsize_64,
	    arburst: 1,
	    arlock: 0,
	    arcache: 0,
	    arprot: 0,
	    arqos: 0,
	    arregion: 0,
	    aruser: 0
	 };
         ddr_master_xactor.i_rd_addr.enq(req);
`endif
      endmethod
      method Action ddr_write(Bit#(32) addr, Vector#(64, Bit#(8)) data, Bit#(64) byte_enables);
`ifdef BLUESPEC_AXI
         let req = AXI4_Wr_Addr {
            awid: 0,
	    awaddr: extend(addr),
	    awlen: 0,
	    awsize: axsize_64,
	    awburst: 1,
	    awlock: 0,
	    awcache: 0,
	    awprot: 0,
	    awqos: 0,
	    awregion: 0,
	    awuser: 0
	 };
         ddr_master_xactor.i_wr_addr.enq(req);
	 let wdata = AXI4_Wr_Data {
	    wdata: pack(data),
	    wstrb: byte_enables,
	    wlast: True,
	    wuser: 0
	 };
	 ddr_master_xactor.i_wr_data.enq(wdata);
`endif
      endmethod

      method Action register_region(Bit#(32) region, Bit#(32) objectId);
         objIds[region] <= truncate(objectId);
      endmethod
      method Action memory_ready();
          $display("memory_ready");
          rg_ready <= True;
`ifdef BLUESPEC_AXI
	  memFabric.set_verbosity(2);
`endif // BLUESPEC_AXI
      endmethod
      method Action capture_tv_info(Bool c);
`ifdef INCLUDE_TANDEM_VERIF
         rg_capture_tv_info <= c;
`endif
      endmethod
      method Action set_watch_tohost (Bool watch_tohost, Bit#(32) tohost_addr);
`ifdef BLUESPEC_AXI
        memController.set_watch_tohost(watch_tohost, extend(tohost_addr));
`endif // BLUESPEC_AXI
      endmethod

      method Action io_rdata(Bit#(64) rdata, Bit#(16) rid, Bit#(8) rresp, Bool rlast);
         if (rg_verbosity > 0) $display("master1 rdata %h rid %d last %d", rdata, rid, rlast);
`ifdef BLUESPEC_AXI
         io_slave_xactor.i_rd_data.enq(AXI4_Rd_Data { rdata: rdata, rid: truncate(rid), rlast: rlast, rresp: 0 });
`endif
      endmethod
      method Action io_bdone(Bit#(16) bid, Bit#(8) bresp);
`ifdef BLUESPEC_AXI
         io_slave_xactor.i_wr_resp.enq(AXI4_Wr_Resp { bid: truncate(bid), bresp: truncate(bresp), buser: 0});
`endif
      endmethod

      method Action irq_set_levels(Bit#(32) w1s);
         rg_irq_levels[1] <= w1s | rg_irq_levels[1];
      endmethod
      method Action irq_clear_levels(Bit#(32) w1c);
         rg_irq_levels[1] <= ~w1c & rg_irq_levels[1];
      endmethod
      method Action read_irq_status();
         response.irq_status(rg_irq_levels[0]);
      endmethod

   endinterface

   interface readClients = vec(readClient0);
   interface writeClients = vec(writeClient0);

`ifdef USE_DDR
   interface AWSP2_Pin_IFC pins;
      interface ddr = to_ddr;
      interface pcis = from_dma_pcis;
   endinterface
`endif

endmodule
