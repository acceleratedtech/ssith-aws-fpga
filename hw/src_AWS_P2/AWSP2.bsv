
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
import AXI4_Fabric  :: *;
import AXI4_Deburster :: *;
import AXI_Mem_Controller :: *;
import Fabric_Defs  :: *;

`ifdef INCLUDE_TANDEM_VERIF
import TV_Info :: *;
import AXI4_Stream ::*;
`endif

`ifdef INCLUDE_GDB_CONTROL
import Debug_Module :: *;
import JtagTap      :: *;
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


(* synthesize *)
module mkAXI4_Fabric_2x2(AXI4_Fabric_IFC#(NumFabricMasters, 2, 4, 64, 64, 0));

    let soc_map <- mkSoC_Map();

    function Tuple2 #(Bool, Bit #(TLog #(2))) fn_addr_to_slave_num(Bit #(64) addr);
        if ((soc_map.m_ddr4_0_uncached_addr_base <= addr) && (addr < soc_map.m_ddr4_0_uncached_addr_lim)) begin
           return tuple2(True, 0);
        end
        else if ((soc_map.m_ddr4_0_cached_addr_base <= addr) && (addr < soc_map.m_ddr4_0_cached_addr_lim)) begin
           return tuple2(True, 0);
        end
        else begin
           return tuple2(True, 1);
        end
    endfunction

   AXI4_Fabric_IFC#(NumFabricMasters, 2, 4, 64, 64, 0) axiFabric <- mkAXI4_Fabric(fn_addr_to_slave_num);

   method reset = axiFabric.reset;
   method set_verbosity = axiFabric.set_verbosity;
   interface v_from_masters = axiFabric.v_from_masters;
   interface v_to_slaves = axiFabric.v_to_slaves;
endmodule

(* synthesize *)
module mkMemFabric(AXI4_Fabric_IFC#(3, 1, 4, 64, 512, 0));

   function Tuple2 #(Bool, Bit #(0)) fn_mem_addr_to_slave_num(Bit #(64) addr);
      return tuple2(True, 0);
   endfunction
   let memFabric <- mkAXI4_Fabric(fn_mem_addr_to_slave_num);

   method reset = memFabric.reset;
   method set_verbosity = memFabric.set_verbosity;
   interface v_from_masters = memFabric.v_from_masters;
   interface v_to_slaves = memFabric.v_to_slaves;
endmodule

module mkAWSP2#(AWSP2_Response response)(AWSP2);

   let soc_map <- mkSoC_Map();
   P2_Core_IFC p2_core <- mkP2_Core();

   Reg#(Bit#(4)) rg_verbosity <- mkReg(0);
   Reg#(Bool) rg_ready <- mkReg(False);

   Reg#(Bit#(32)) rg_irq_levels[2] <- mkCReg(2, 0);

   Vector#(16, Reg#(Bit#(8)))    objIds <- replicateM(mkReg(0));

   // FIXME: add boot ROM slave interface
   AXI4_Fabric_IFC#(NumFabricMasters, 2, 4, 64, 64, 0) axiFabric <- mkAXI4_Fabric_2x2();
   mkConnection(p2_core.master0, axiFabric.v_from_masters[0]);
   mkConnection(p2_core.master1, axiFabric.v_from_masters[1]);
   let to_slave0 = axiFabric.v_to_slaves[0];
   let to_slave1 = axiFabric.v_to_slaves[1];

   FIFOF#(MemRequest) readReqFifo0 <- mkFIFOF();
   FIFOF#(MemRequest) writeReqFifo0 <- mkFIFOF();
   FIFOF#(MemData#(DataBusWidth))   readDataFifo0 <- mkSizedBRAMFIFOF(64);
   FIFOF#(MemData#(DataBusWidth))   writeDataFifo0 <- mkSizedBRAMFIFOF(64);
   FIFOF#(Bit#(MemTagSize)) doneFifo0 <- mkFIFOF();

   Wire#(Bool) w_arready0 <- mkDWire(False);
   Wire#(Bool) w_awready0 <- mkDWire(False);
   Wire#(Bool) w_wready0  <- mkDWire(False);
   Wire#(Bool) w_rready0  <- mkDWire(False);
   Wire#(Bool) w_rvalid0  <- mkDWire(False);

   FIFOF#(MemRequest) readReqFifo1 <- mkFIFOF();
   FIFOF#(MemRequest) writeReqFifo1 <- mkFIFOF();
   FIFOF#(MemData#(64)) readDataFifo1 <- mkSizedBRAMFIFOF(64);
   FIFOF#(MemData#(64)) writeDataFifo1 <- mkSizedBRAMFIFOF(64);
   FIFOF#(Bit#(MemTagSize)) doneFifo1 <- mkFIFOF();

   Wire#(Bool) w_arready1 <- mkDWire(False);
   Wire#(Bool) w_awready1 <- mkDWire(False);
   Wire#(Bool) w_wready1  <- mkDWire(False);
   Wire#(Bool) w_rready1  <- mkDWire(False);
   Wire#(Bool) w_rvalid1  <- mkDWire(False);

   AXI4_Deburster_IFC #(Wd_Id, Wd_Addr, Wd_Data, Wd_User) deburster <- mkAXI4_Deburster();
   let memController <- mkAXI_Mem_Controller();

   AXI4_Fabric_IFC#(3, 1, 4, 64, 512, 0) memFabric <- mkMemFabric();
   mkConnection(to_slave0, deburster.from_master);
   mkConnection(deburster.to_slave, memController.slave);
   //mkConnection(memController.to_raw_mem, memFabric.v_from_masters[0]);
   //let to_ddr = memFabric.v_to_slaves[0];
   let to_ddr = memController.to_raw_mem;

   AXI4_Master_Xactor_IFC#(4, 64, 512, 0) ddr_master_xactor <- mkAXI4_Master_Xactor();
   mkConnection(ddr_master_xactor.axi_side, memFabric.v_from_masters[1]);
   let from_pci = memFabric.v_from_masters[2];


`ifndef USE_DDR
   rule master0_handshake;
      to_ddr.m_awready(w_awready0);
      to_ddr.m_arready(w_arready0);
      to_ddr.m_wready(w_wready0);
   endrule

   rule debug0 if (rg_ready);
      if (to_ddr.m_arvalid()
         || w_rvalid0)
         if (rg_verbosity > 2) $display("master0 arvalid %d arready %d rvalid %d rready %d", to_ddr.m_arvalid(), w_arready0, w_rvalid0, to_ddr.m_rready());
   endrule

   rule master0_aw if (rg_ready);
      if (to_ddr.m_awvalid()) begin
          let awaddr = to_ddr.m_awaddr();
          let awlen    = to_ddr.m_awlen();
          let awsize   = to_ddr.m_awsize();
          let awid   = to_ddr.m_awid();

          Bit#(30) byteaddr = truncate(awaddr);
          let objId = objIds[8];
          let burstLen = fromInteger(valueOf(TDiv#(DataBusWidth,8))) * (awlen + 1);
          let req = MemRequest { sglId: extend(objId), offset: extend(byteaddr), burstLen: extend(burstLen), tag: extend(awid) };
          if (rg_verbosity > 1 || truncate(req.offset) != byteaddr)
             $display("master0 awaddr %h awlen=%d awsize=%d awid=%d byteaddr=%h objId=%d burstLen=%d",
                      awaddr, awlen, awsize, awid, byteaddr, objId, burstLen);

          writeReqFifo0.enq(req);
      end
      w_awready0 <= writeReqFifo0.notFull();
   endrule

   rule master0_wdata if (rg_ready);
      if (to_ddr.m_wvalid()) begin
          let wdata = to_ddr.m_wdata;
          let wstrb = to_ddr.m_wstrb;
          let wlast = to_ddr.m_wlast;
          if (rg_verbosity > 1) $display("master0 wdata %h wstrb %h", wdata, wstrb);
          writeDataFifo0.enq(MemData { data: wdata, tag: 0, byte_enables: wstrb, last: wlast});
       end
       w_wready0 <= writeDataFifo0.notFull();
    endrule

   rule master0_b if (rg_ready);
      let bvalid = doneFifo0.notEmpty();
      let bid    = doneFifo0.first();
      let bresp = 0;
      let buser = 0;
      to_ddr.m_bvalid(bvalid, truncate(bid), bresp, buser);
      if (to_ddr.m_bready()) begin
          doneFifo0.deq();
      end
   endrule

   rule master0_ar if (rg_ready);
      if (to_ddr.m_arvalid()) begin
          let araddr = to_ddr.m_araddr();
          let arlen    = to_ddr.m_arlen();
          let arsize   = to_ddr.m_arsize();
          let arid   = to_ddr.m_arid();

          Bit#(30) byteaddr = truncate(araddr);
          let objId = objIds[8];
          let burstLen = fromInteger(valueOf(TDiv#(DataBusWidth,8))) * (arlen + 1);
          let req = MemRequest { sglId: extend(objId), offset: extend(byteaddr), burstLen: extend(burstLen), tag: extend(arid) };
          if (rg_verbosity > 1 || truncate(req.offset) != byteaddr)
             $display("master0 araddr %h arlen=%d arsize=%d arid=%d byteaddr=%h objId=%d burstLen=%d",
                       araddr, arlen, arsize, arid, byteaddr, objId, burstLen);
          readReqFifo0.enq(req);
      end
      w_arready0 <= readReqFifo0.notFull();
   endrule

   rule master0_rdata if (rg_ready);

      w_rvalid0 <= readDataFifo0.notEmpty();
      if (readDataFifo0.notEmpty()) begin
	 let rdata = readDataFifo0.first;
	 if (rg_verbosity > 1 && to_ddr.m_rready()) $display("master0 rdata %h rid %d last %d", rdata.data, rdata.tag, rdata.last);
	 to_ddr.m_rvalid(readDataFifo0.notEmpty(),
				  truncate(rdata.tag),
				  rdata.data,
				  0,  // rresp
				  rdata.last,
				  0); // ruser


	 if (to_ddr.m_rready()) begin
	     if (rg_verbosity > 1) $display("master0_rdata_deq rvalid %d rready %d", readDataFifo0.notEmpty(), to_ddr.m_rready());
	    readDataFifo0.deq();
	 end
      end
   endrule
`endif // not AWSF1




   rule master1_handshake;
      to_slave1.m_awready(w_awready1);
      to_slave1.m_arready(w_arready1);
      to_slave1.m_wready(w_wready1);
   endrule

   rule debug1 if (rg_ready);
      if (to_slave1.m_arvalid()
         || w_rvalid1)
         if (rg_verbosity > 1) $display("master1 arvalid %d arready %d rvalid %d rready %d", to_slave1.m_arvalid(), w_arready1, w_rvalid1, to_slave1.m_rready());
   endrule

   rule master1_aw if (rg_ready);
      if (to_slave1.m_awvalid()) begin
          let awaddr = to_slave1.m_awaddr();
          let len    = to_slave1.m_awlen();
          let size   = to_slave1.m_awsize();
          let awid   = to_slave1.m_awid();

          Bit#(4)  objNumber = truncate(awaddr >> 28);
          Bit#(28) objOffset = truncate(awaddr);
          let objId = objIds[objNumber];
          let burstLen = 8 * (len + 1);
          if (rg_verbosity > 0)
              $display("master1 awaddr %h len=%d size=%d id=%d objId=%d objOffset=%h burstLen=%d", awaddr, len, size, awid, objId, objOffset, burstLen);
          writeReqFifo1.enq(MemRequest { sglId: extend(objId), offset: truncate(awaddr), burstLen: extend(burstLen), tag: extend(awid) });
      end
      w_awready1 <= writeReqFifo1.notFull();
   endrule

   rule master1_wdata if (rg_ready);
      if (to_slave1.m_wvalid()) begin
          let wdata = to_slave1.m_wdata;
          let wstrb = to_slave1.m_wstrb;
          let wlast = to_slave1.m_wlast;
          if (rg_verbosity > 0)
          $display("master1 wdata %h wstrb %h", wdata, wstrb);
          writeDataFifo1.enq(MemData { data: wdata, tag: 1, byte_enables: wstrb, last: wlast});
       end
       w_wready1 <= writeDataFifo1.notFull();
    endrule

   rule master1_b if (rg_ready);
      let bvalid = doneFifo1.notEmpty();
      let bid    = doneFifo1.first();
      let bresp = 0;
      let buser = 0;
      to_slave1.m_bvalid(bvalid, truncate(bid), bresp, buser);
      if (to_slave1.m_bready()) begin
          doneFifo1.deq();
      end
   endrule

   rule master1_ar if (rg_ready);
      if (to_slave1.m_arvalid()) begin
          let araddr = to_slave1.m_araddr();
          let len    = to_slave1.m_arlen();
          let size   = to_slave1.m_arsize();
          let arid   = to_slave1.m_arid();

          Bit#(4) objNumber = truncate(araddr >> 28);
          Bit#(28) objOffset = truncate(araddr);

          let objId = objIds[objNumber];
          let burstLen = 8 * (len + 1);
          if (rg_verbosity > 0)
              $display("master1 araddr %h len=%d size=%d id=%d objId=%d objOffset=%h", araddr, len, size, arid, objId, objOffset);
          readReqFifo1.enq(MemRequest { sglId: extend(objId), offset: truncate(araddr), burstLen: extend(burstLen), tag: extend(arid) });
      end
      w_arready1 <= readReqFifo1.notFull();

   endrule

   rule master1_rdata if (rg_ready);
      let rdata = readDataFifo1.first;
      if (rg_verbosity > 0)
          $display("master1 rdata %h rid %d last %d", rdata.data, rdata.tag, rdata.last);

      w_rvalid1 <= readDataFifo1.notEmpty();
      to_slave1.m_rvalid(readDataFifo1.notEmpty(),
                               truncate(rdata.tag),
                               rdata.data,
                               0,  // rresp
                               rdata.last,
                               0); // ruser

      if (to_slave1.m_rready()) begin
          //$display("master1_rdata_deq rvalid %d rready %d", w_rvalid1, to_slave1.m_rready());
         readDataFifo1.deq();
      end
   endrule

   rule master1_io_araddr;
       let req <- toGet(readReqFifo1).get();
       response.io_araddr(truncate(req.offset), extend(req.burstLen), extend(req.tag));
   endrule
   rule master1_io_awaddr;
       let req <- toGet(writeReqFifo1).get();
       response.io_awaddr(truncate(req.offset), extend(req.burstLen), extend(req.tag));
   endrule
   rule master1_io_wdata;
       let mdata <- toGet(writeDataFifo1).get();
       response.io_wdata(mdata.data, 0);
   endrule

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
      interface Put readData;
        method Action put(MemData#(DataBusWidth) rdata);
          readDataFifo0.enq(rdata);
        endmethod
      endinterface
   endinterface );
   MemWriteClient#(DataBusWidth) writeClient0 = (interface MemWriteClient;
      interface Get writeReq = toGet(writeReqFifo0);
      interface Get writeData = toGet(writeDataFifo0);
      interface Put writeDone = toPut(doneFifo0);
   endinterface );

   Reg#(Bool) rg_addr_map_set <- mkReg(False);
   function Action fn_reset();
   action
      axiFabric.reset();
      deburster.reset();
      //memController.server_reset.request.put(?);
      memFabric.reset();
      rg_addr_map_set <= False;
   endaction
   endfunction

   Reg#(Bool) rg_power_on_reset <- mkReg(False);
   rule rl_reset if (!rg_power_on_reset);
      fn_reset();
      rg_power_on_reset <= True;
   endrule
   rule rl_reset_done;
      $display("memController.server_reset.response.get()");
      let b <- memController.server_reset.response.get();
   endrule
   rule rl_set_addr_map if (!rg_addr_map_set);
      memController.set_addr_map(min(soc_map.m_ddr4_0_uncached_addr_base, soc_map.m_ddr4_0_cached_addr_base),
                                 max(soc_map.m_ddr4_0_uncached_addr_lim, soc_map.m_ddr4_0_cached_addr_lim));
      rg_addr_map_set <= True;
   endrule

   rule rl_ddr_rdata;
      let rdata <- pop_o(ddr_master_xactor.o_rd_data);
      response.ddr_data(unpack(rdata.rdata));
   endrule

   rule rl_wr_resp;
      let resp <- pop_o(ddr_master_xactor.o_wr_resp);
      response.ddr_data(unpack(0));
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
         status[15:8] = memController.status();
         response.dmi_status_data(status);
      endmethod

      method Action ddr_read(Bit#(32) addr);
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
      endmethod
      method Action ddr_write(Bit#(32) addr, Vector#(64, Bit#(8)) data, Bit#(64) byte_enables);
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
      endmethod

      method Action register_region(Bit#(32) region, Bit#(32) objectId);
         objIds[region] <= truncate(objectId);
      endmethod
      method Action memory_ready();
          $display("memory_ready");
          rg_ready <= True;
      endmethod
      method Action capture_tv_info(Bool c);
`ifdef INCLUDE_TANDEM_VERIF
         rg_capture_tv_info <= c;
`endif
      endmethod
      method Action set_watch_tohost (Bool watch_tohost, Bit#(32) tohost_addr);
        memController.set_watch_tohost(watch_tohost, extend(tohost_addr));
      endmethod

      method Action io_rdata(Bit#(64) data, Bit#(16) rid, Bit#(8) rresp, Bool last);
         readDataFifo1.enq(MemData { data: data, tag: truncate(rid), last: last });
      endmethod
      method Action io_bdone(Bit#(16) bid, Bit#(8) bresp);
         doneFifo1.enq(truncate(bid));
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
      interface pcis = from_pci;
   endinterface
`endif

endmodule
