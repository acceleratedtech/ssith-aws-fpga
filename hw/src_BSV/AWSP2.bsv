
import BRAMFIFO     :: *;
import BuildVector  :: *;
import ClientServer :: *;
import Connectable  :: *;
import FIFO         :: *;
import FIFOF        :: *;
import FIFOLevel    :: *;
import GetPut       :: *;
import Vector       :: *;
import Clocks       :: *;

import AxiBits           :: *;
import ConnectalConfig   :: *;
import ConnectalMemTypes :: *;

// ================================================================
// Project imports

import P2_Core  :: *;
import SoC_Map  :: *;
import UART_Model :: *;

// The basic core
import Core_IFC :: *;
import Core     :: *;

// External interrupt request interface
import PLIC :: *;    // for PLIC_Source_IFC type which is exposed at P2_Core interface

import Semi_FIFOF :: *;

// Main Fabric
import AWS_AXI4_Types   :: *;
import AWS_AXI4_Fabric  :: *;
import AWS_Fabric_Defs  :: *;
import AWS_AXI4_Deburster :: *;
import AWS_AXI4_Connection :: *;
import AWS_AXI4_Id_Reflector :: *;
import AWS_AXI4_Downsizer :: *;
import AWS_AXI4_Crossing :: *;
import AXI_Mem_Controller :: *;
import AXI4_Mem_Filter :: *;
import AXI_RAM        :: *;

`ifdef INCLUDE_TANDEM_VERIF
import TV_Info :: *;
import AXI4_Stream ::*;
`endif

`ifdef INCLUDE_GDB_CONTROL
import Debug_Module :: *;
`ifdef JTAG_TAP
import JtagTap      :: *;
import Giraffe_IFC  :: *;
`endif
`endif

import AWSP2_IFC   :: *;

`ifdef BOARD_awsf1
`ifdef AWSF1_DDR_A
`define USE_DDR
`endif
`endif

`ifdef HAVE_BLUESTUFF_ROUTABLE
import Routable :: *;
`define SOC_MAP_BASE(soc_map, field) rangeBase(soc_map.m_``field``_range)
`define SOC_MAP_SIZE(soc_map, field) rangeSize(soc_map.m_``field``_range)
`define SOC_MAP_LIM (soc_map, field) rangeTop (soc_map.m_``field``_range)
`else
`define SOC_MAP_BASE(soc_map, field) soc_map.m_``field``_base
`define SOC_MAP_SIZE(soc_map, field) soc_map.m_``field``_size
`define SOC_MAP_LIM (soc_map, field) soc_map.m_``field``_lim
`endif

interface AWSP2;
  interface AWSP2_Request request;
  interface Vector#(1, MemReadClient#(DataBusWidth)) readClients;
  interface Vector#(1, MemWriteClient#(DataBusWidth)) writeClients;
`ifdef USE_DDR
   interface AWSP2_Pin_IFC pins;
`endif
endinterface

typedef 2 IOFABRIC_NUM_MASTERS;

typedef 1 MEMFABRIC_NUM_SLAVES;
`ifdef HAVE_P2_L1_SLAVE_PORT
typedef 1 MEMFABRIC_NUM_MASTERS;
`else
typedef 2 MEMFABRIC_NUM_MASTERS;
`endif
`ifndef USE_MEM_FILTER
typedef 3 IOFABRIC_NUM_SLAVES;
`else
typedef 4 IOFABRIC_NUM_SLAVES;
`endif

(* synthesize *)
module mkIOFabric(AXI4_Fabric_IFC#(IOFABRIC_NUM_MASTERS, IOFABRIC_NUM_SLAVES, 6, 64, 64, 0));

    let soc_map <- mkSoC_Map();

    function Tuple2 #(Bool, Bit #(TLog #(3))) fn_addr_to_slave_num(Bit #(64) addr);
        if ((`SOC_MAP_BASE(soc_map, ddr4_0_uncached_addr) <= addr) && (addr < `SOC_MAP_LIM(soc_map, ddr4_0_uncached_addr))) begin
           return tuple2(True, 0);
        end
        else if ((`SOC_MAP_BASE(soc_map, ddr4_0_cached_addr) <= addr) && (addr < `SOC_MAP_LIM(soc_map, ddr4_0_cached_addr))) begin
           return tuple2(True, 0);
        end
        else if ((`SOC_MAP_BASE(soc_map, uart16550_0_addr) <= addr) && (addr < `SOC_MAP_LIM(soc_map, uart16550_0_addr))) begin
           return tuple2(True, 1);
        end
`ifdef USE_MEM_FILTER
        else if ((`SOC_MAP_BASE(soc_map, mem_filter_0_addr) <= addr) && (addr < `SOC_MAP_LIM(soc_map, mem_filter_0_addr))) begin
           return tuple2(True, 3);
        end
`endif
        else if ((`SOC_MAP_BASE(soc_map, host_io_addr) <= addr) && (addr < `SOC_MAP_LIM(soc_map, host_io_addr))) begin
           return tuple2(True, 2);
        end
        else begin
           return tuple2(False, 0);
        end
   endfunction

   AXI4_Fabric_IFC#(IOFABRIC_NUM_MASTERS, IOFABRIC_NUM_SLAVES, 6, 64, 64, 0) axiFabric <- mkAXI4_Fabric(fn_addr_to_slave_num);

   method reset = axiFabric.reset;
   method set_verbosity = axiFabric.set_verbosity;
   interface v_from_masters = axiFabric.v_from_masters;
   interface v_to_slaves = axiFabric.v_to_slaves;
endmodule

(* synthesize *)
module mkMemFabric(AXI4_Fabric_IFC#(MEMFABRIC_NUM_MASTERS, MEMFABRIC_NUM_SLAVES, 6, 64, 512, 0));
   let soc_map <- mkSoC_Map();
   function Tuple2 #(Bool, Bit #(TLog#(MEMFABRIC_NUM_SLAVES))) fn_mem_addr_to_slave_num(Bit #(64) addr);
      let min_mem_addr = min(`SOC_MAP_BASE(soc_map, ddr4_0_uncached_addr), `SOC_MAP_BASE(soc_map, ddr4_0_cached_addr));
      let uncached_mem_base = `SOC_MAP_BASE(soc_map, ddr4_0_uncached_addr) - min_mem_addr;
      let uncached_mem_lim = `SOC_MAP_LIM(soc_map, ddr4_0_uncached_addr) - min_mem_addr;
      // cached memory base has been subtracted from the address
      begin
          return tuple2(True, 0);
      end
   endfunction
   let memFabric <- mkAXI4_Fabric(fn_mem_addr_to_slave_num);

   method reset = memFabric.reset;
   method set_verbosity = memFabric.set_verbosity;
   interface v_from_masters = memFabric.v_from_masters;
   interface v_to_slaves = memFabric.v_to_slaves;
endmodule

(* synthesize *)
module mkDeburster(AXI4_Deburster_IFC #(Wd_Id, Wd_Addr, Wd_Data, Wd_User));
    AXI4_Deburster_IFC #(Wd_Id, Wd_Addr, Wd_Data, Wd_User) deburster <- mkAXI4_Deburster();
    return deburster;
endmodule

module mkAWSP2#(Clock derivedClock, Reset derivedReset, AWSP2_Response response)(AWSP2);
   let defaultClock <- exposeCurrentClock;
   let defaultReset <- exposeCurrentReset;

   let soc_map <- mkSoC_Map();
   P2_Core_IFC p2_core <- mkP2_Core(clocked_by derivedClock, reset_by derivedReset);

   Reg#(Bit#(4)) rg_verbosity <- mkReg(0);
   Reg#(Bool) rg_ready <- mkReg(False);

   Reg#(Bit#(32)) rg_irq_levels[3] <- mkCReg(3, 0);

   Vector#(16, Reg#(Bit#(8)))    objIds <- replicateM(mkReg(0));

   AXI4_Crossing_IFC #(6, 64, 64, 0) master0Crossing <- mkAXI4_CrossingToCC(derivedClock, derivedReset);
   AXI4_Crossing_IFC #(6, 64, 64, 0) master1Crossing <- mkAXI4_CrossingToCC(derivedClock, derivedReset);
   mkConnection(p2_core.master0, master0Crossing.from_master, clocked_by derivedClock, reset_by derivedReset);
   mkConnection(p2_core.master1, master1Crossing.from_master, clocked_by derivedClock, reset_by derivedReset);

   AXI4_Fabric_IFC#(IOFABRIC_NUM_MASTERS, IOFABRIC_NUM_SLAVES, 6, 64, 64, 0) axiFabric <- mkIOFabric();
   mkConnection(master0Crossing.to_slave, axiFabric.v_from_masters[0]);
   mkConnection(master1Crossing.to_slave, axiFabric.v_from_masters[1]);
   let to_slave0 = axiFabric.v_to_slaves[0];
   let to_slave1 = axiFabric.v_to_slaves[1];
   let to_slave2 = axiFabric.v_to_slaves[2];

   AXI4_Deburster_IFC #(Wd_Id, Wd_Addr, Wd_Data, Wd_User) deburster <- mkDeburster();
   let memController <- mkAXI_Mem_Controller();

   AXI4_Fabric_IFC#(MEMFABRIC_NUM_MASTERS, MEMFABRIC_NUM_SLAVES, 6, 64, 512, 0) memFabric <- mkMemFabric();
   mkConnection(to_slave0, deburster.from_master);
   mkConnection(deburster.to_slave, memController.slave);
   let rawmem_xn <- mkConnection(memController.to_raw_mem, memFabric.v_from_masters[0]);
   let to_ddr = memFabric.v_to_slaves[0];


`ifdef HAVE_P2_L1_SLAVE_PORT
   AXI4_Crossing_IFC #(0, 64, 64, 0) slave0Crossing <- mkAXI4_CrossingFromCC(derivedClock, derivedReset);
   mkConnection(slave0Crossing.to_slave, p2_core.slave0, clocked_by derivedClock, reset_by derivedReset);

   AXI4_Downsizer_IFC #(0, 64, 512, 64, 0) downsizer <- mkAXI4_Downsizer();
   mkConnection(downsizer.to_slave, slave0Crossing.from_master);
   AXI4_Id_Reflector_IFC #(6, 64, 512, 0) id_reflector <- mkAXI4_Id_Reflector();
   mkConnection(id_reflector.to_slave, downsizer.from_master);
   let from_dma_pcis = id_reflector.from_master;
`else
`ifdef USE_MEM_FILTER
   let axiMemFilter <- mkAXI4_Mem_Filter();
   mkConnection(axiFabric.v_to_slaves[3], axiMemFilter.from_control);
   mkConnection(axiMemFilter.to_slave, memFabric.v_from_masters[1]);
   let from_dma_pcis = axiMemFilter.from_master;
`else
   let from_dma_pcis = memFabric.v_from_masters[1];
`endif // !USE_MEM_FILTER
`endif // !HAVE_P2_L1_SLAVE_PORT

`ifndef BOARD_awsf1
    AXI4_Master_Xactor_IFC#(6, 64, 512, 0) dma_pcis_master_xactor <- mkAXI4_Master_Xactor();
    mkConnection(dma_pcis_master_xactor.axi_side, from_dma_pcis);
`endif

   FIFOF#(Bit#(8)) uartToHostFifo <- mkFIFOF();
   FIFOF#(Bit#(8)) uartFromHostFifo <- mkFIFOF();
   let uart <- mkUART();
   mkConnection(to_slave1, uart.slave);
   mkConnection(toGet(uartFromHostFifo), uart.put_from_console);
   mkConnection(uart.get_to_console, toPut(uartToHostFifo));

   FIFOF#(MemRequest) readReqFifo0 <- mkFIFOF();
   FIFOF#(MemRequest) writeReqFifo0 <- mkFIFOF();
   FIFOF#(MemData#(DataBusWidth))   readDataFifo0 <- mkSizedBRAMFIFOF(64);
   FIFOF#(MemData#(DataBusWidth))   writeDataFifo0 <- mkSizedBRAMFIFOF(64);
   FIFOF#(Bit#(MemTagSize)) doneFifo0 <- mkFIFOF();

`ifndef USE_DDR
   AXI4_Slave_Xactor_IFC#(6, 64, 512, 0) ddr_slave_xactor <- mkAXI4_Slave_Xactor();
   let ddr_xn <- mkConnection/*Verbose*/(to_ddr, ddr_slave_xactor.axi_side);

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
`endif // not USE_DDR

   AXI4_Slave_Xactor_IFC#(6, 64, 64, 0) io_slave_xactor <- mkAXI4_Slave_Xactor();
   mkConnection(to_slave2, io_slave_xactor.axi_side);
   let rg_io_slave_awaddr_count <- mkReg(0);

   rule master1_aw if (rg_ready && rg_io_slave_awaddr_count == 0);
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
      rg_io_slave_awaddr_count <= len + 1;
   endrule

   rule master1_wdata if (rg_ready && rg_io_slave_awaddr_count > 0);
      let req <- pop_o(io_slave_xactor.o_wr_data);
      let wdata = req.wdata;
      let wstrb = req.wstrb;
      let wlast = req.wlast;
      if (rg_verbosity > 0) $display("master1 wdata %h wstrb %h", wdata, wstrb);
      response.io_wdata(wdata, 0);
      rg_io_slave_awaddr_count <= rg_io_slave_awaddr_count - 1;
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

`ifdef INCLUDE_GDB_CONTROL
   SyncFIFOLevelIfc#(Bit#(7), 4) dmiReadFifo <- mkSyncFIFOLevel(defaultClock, defaultReset, derivedClock);
   SyncFIFOLevelIfc#(Tuple2#(Bit#(7), Bit#(32)), 4) dmiWriteFifo <- mkSyncFIFOLevel(defaultClock, defaultReset, derivedClock);
   SyncFIFOLevelIfc#(Bit#(32), 4) dmiDataFifo <- mkSyncFIFOLevel(derivedClock, derivedReset, defaultClock);

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
   Reg#(Bool) rg_capture_tv_info <- mkSyncRegFromCC(False, derivedClock);
   AXI4_Stream_Slave_Xactor_IFC#(Wd_SId, Wd_SDest, Wd_SData, Wd_SUser) tv_xactor <- mkAXI4_Stream_Slave_Xactor(clocked_by derivedClock, reset_by derivedReset);
   mkConnection(p2_core.tv_verifier_info_tx, tv_xactor.axi_side, clocked_by derivedClock, reset_by derivedReset);

   let tvFifo <- mkSyncFIFOToCC(4, derivedClock, derivedReset);

   rule rl_tv_data_channel;
      let packet <- pop_o(tv_xactor.o_stream);
      if (rg_capture_tv_info) begin
          tvFifo.enq(packet.tdata);
      end
   endrule

   rule rl_tv_out;
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
   rule rl_set_addr_map if (!rg_addr_map_set);
      $display("memController.set_addr_map: %h %h",
                min(`SOC_MAP_BASE(soc_map, ddr4_0_uncached_addr), `SOC_MAP_BASE(soc_map, ddr4_0_cached_addr)),
                max(`SOC_MAP_LIM(soc_map, ddr4_0_uncached_addr), `SOC_MAP_LIM(soc_map, ddr4_0_cached_addr)));
      memController.set_addr_map(min(`SOC_MAP_BASE(soc_map, ddr4_0_uncached_addr), `SOC_MAP_BASE(soc_map, ddr4_0_cached_addr)),
                                 max(`SOC_MAP_LIM(soc_map, ddr4_0_uncached_addr), `SOC_MAP_LIM(soc_map, ddr4_0_cached_addr)));
      uart.set_addr_map(`SOC_MAP_BASE(soc_map, uart16550_0_addr), `SOC_MAP_LIM(soc_map, uart16550_0_addr));
      rg_addr_map_set <= True;
   endrule

   // Interrupt lines are independent (no inter-bit consistency issues), so we
   // can get away with a per-bit synchronizer. The Verilog is parameterised on
   // the reset value, though this is not exposed, but the default of 0 is what
   // we want.
   Vector#(32, SyncBitIfc#(Bit#(1))) v_sync_irq_levels <- replicateM(mkSyncBitFromCC(derivedClock));

   (* no_implicit_conditions, fire_when_enabled *)
   rule rl_sync_irq_levels;
      function Action sb_send(SyncBitIfc#(a) sb, a data) = sb.send(data);
      joinActions(zipWith(sb_send, tail(v_sync_irq_levels), unpack(rg_irq_levels[0][31:1])));
   endrule

   (* no_implicit_conditions, fire_when_enabled *)
   rule rl_sync_uart_intr;
      v_sync_irq_levels[0].send(pack(uart.intr));
   endrule

   rule rl_core_interrupt_reqs;
      function a sb_read(SyncBitIfc#(a) sb) = sb.read();
      p2_core.interrupt_reqs(truncate(pack(map(sb_read, v_sync_irq_levels))));
   endrule

   rule rl_uart_tohost;
      let ch <- toGet(uartToHostFifo).get();
      response.uart_tohost(ch);
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
         status[0] = pack(dmiReadFifo.sNotEmpty());
         status[1] = pack(dmiWriteFifo.sNotEmpty());
         status[2] = pack(dmiDataFifo.dNotEmpty());
`endif
         status[15:8] = memController.status();
         response.dmi_status_data(status);
      endmethod

      method Action ddr_read(Bit#(32) addr);
      endmethod
      method Action ddr_write(Bit#(32) addr, Vector#(64, Bit#(8)) data, Bit#(64) byte_enables);
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

      method Action io_rdata(Bit#(64) rdata, Bit#(16) rid, Bit#(8) rresp, Bool rlast);
         if (rg_verbosity > 0) $display("master1 rdata %h rid %d last %d", rdata, rid, rlast);
         io_slave_xactor.i_rd_data.enq(AXI4_Rd_Data { rdata: rdata, rid: truncate(rid), rlast: rlast, rresp: truncate(rresp) });
      endmethod
      method Action io_bdone(Bit#(16) bid, Bit#(8) bresp);
         io_slave_xactor.i_wr_resp.enq(AXI4_Wr_Resp { bid: truncate(bid), bresp: truncate(bresp), buser: 0});
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
      method Action uart_fromhost(Bit#(8) char);
         uartFromHostFifo.enq(char);
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
