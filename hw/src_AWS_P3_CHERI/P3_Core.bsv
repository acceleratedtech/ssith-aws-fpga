// Copyright (c) 2018-2020 Bluespec, Inc. All Rights Reserved.

package P3_Core;

// ================================================================
// This package defines the interface and implementation of the 'P3 Core'
// for the DARPA SSITH project.
// This P3 core contains:
//    - MIT's RISCY-OOO CPU, including
//        - Near_Mem (ICache and DCache)
//        - Near_Mem_IO (Timer, Software-interrupt, and other mem-mapped-locations)
//        - External interrupt request lines
//        - 2 x AXI4 Master interfaces (from DM and ICache, and from DCache)
//    - RISC-V Debug Module (DM)
//    - JTAG TAP interface for DM
//    - Optional Tandem Verification trace stream output interface

// ================================================================
// BSV library imports

import Vector        :: *;
import FIFO          :: *;
import GetPut        :: *;
import ClientServer  :: *;
import Connectable   :: *;
import Bus           :: *;
import Clocks        :: *;
import Vector        :: *;

// ----------------
// BSV additional libs

import GetPut_Aux :: *;
import Routable   :: *;
import AXI4       :: *;
import Semi_FIFOF :: *;
import Cur_Cycle  :: *;

`ifdef INCLUDE_DMEM_SLAVE
import AXI4Lite :: *;
`endif

// ================================================================
// Project imports

import SoC_Map  :: *;
import Fabric_Defs :: *;
import PowerOnReset :: *;

// The basic core
import CoreW_IFC :: *;
import CoreW     :: *;

// External interrupt request interface
import PLIC :: *;    // for PLIC_Source_IFC type which is exposed at P3_Core interface

`ifdef INCLUDE_TANDEM_VERIF
import TV_Info :: *;
import AXI4_Stream ::*;
`endif

`ifdef INCLUDE_GDB_CONTROL
import Debug_Module :: *;
`ifdef JTAG_TAP
import Giraffe_IFC  :: *;
import JtagTap      :: *;
`endif
`endif

// ================================================================
// The P3_Core interface

interface P3_Core_IFC;

   // ----------------------------------------------------------------
   // Core CPU interfaces

   // CPU IMem to Fabric master interface
   interface AXI4_Master_Synth#(TAdd#(Wd_MId,1), Wd_Addr, Wd_Data,
                                0, 0, 0, 0, 0)  master0;

   interface AXI4_Master_Synth#(TAdd#(Wd_MId,1), Wd_Addr, Wd_Data,
                                0, 0, 0, 0, 0)  master1;

   // External interrupt sources
   (* always_ready, always_enabled, prefix="" *)
   method  Action interrupt_reqs ((* port="cpu_external_interrupt_req" *)
				  Bit #(N_External_Interrupt_Sources)  reqs);

   // ----------------
   // External interrupt [14] to go into Debug Mode

`ifndef BSIM
   (* always_ready, always_enabled *)
   method Action  debug_external_interrupt_req (Bool set_not_clear);
`endif

`ifdef INCLUDE_DMEM_SLAVE
   // ----------------------------------------------------------------
   // Optional AXI4-Lite DMA slave interface

   interface AXI4Lite_Slave_Synth #(Wd_Addr, Wd_Data, 0, 0, 0, 0, 0) slave0;
`endif

`ifdef INCLUDE_GDB_CONTROL
   // ----------------
   // JTAG interface

`ifdef JTAG_TAP
   interface JTAG_IFC jtag;
`else
   interface DM_Common::DMI dmi;
`endif
`endif

`ifdef INCLUDE_TANDEM_VERIF
   // ----------------------------------------------------------------
   // Optional Tandem Verifier interface.  The data signal is
   // packed output tuples (n,vb),/ where 'vb' is a vector of
   // bytes with relevant bytes in locations [0]..[n-1]

      interface AXI4_Stream_Master_IFC #(Wd_SId, Wd_SDest, Wd_SData, Wd_SUser)
                tv_verifier_info_tx;
`endif

endinterface

// ================================================================

(* synthesize *)
module mkP3_Core (P3_Core_IFC);

   // ================================================================
   // The RISC-V Debug Module is at the following point in the module hierarchy:
   //     p3_core.corew.debug_module
   //     (instances of mkP3_Core, mkCoreW, mkDebug_Module)

   // The Debug Module is reset only once, on power-up, hence we pass
   // its reset down from here.

   // (power-on reset) and the Debug Module's 'hart_reset' control.
   let default_reset <- exposeCurrentReset();

   let por_ifc <- mkPowerOnReset();
   let power_on_reset = por_ifc.gen_rst;  // This line and the next
   //let power_on_reset <- default_reset; //are alternatives.
   let dm_power_on_reset = power_on_reset;

   // The rest of the system (corew minus the Debug Module) are reset:
   // - on power-on, and
   // - when the Debug Module requests an NDM reset (for non-DebugModule).

`ifdef INCLUDE_GDB_CONTROL
   let clk <- exposeCurrentClock;
   // Setting initial_reset_val to True ensures that if the mkReset is itself in
   // reset (controlled by mkP3_Core's default reset), its output reset will be
   // asserted.  Thus ndm_reset will be asserted if any of
   //    power_on_reset, default_reset, ndm_reset from debug module
   // is asserted.

   // Currently initial_reset_val=False -- thus ndm_reset will be asserted only
   // if any of
   //   power_on_reset, ndm_reset from debug module
   // is asserted.
   Bool    initial_reset_val  = False; // was True;
   Integer ndm_reset_duration = 10;    // NOTE: assuming 10 cycle reset enough for NDM
   let ndm_reset_controller <- mkReset(ndm_reset_duration, initial_reset_val, clk);

   let ndm_reset <- mkResetEither (power_on_reset, ndm_reset_controller.new_rst);
`else
   let rstn <- exposeCurrentReset;
   let ndm_reset <- mkResetEither (power_on_reset, rstn);
`endif

   // ================================================================
   // CoreW
   //     CPU + Near_Mem_IO (CLINT) + PLIC + Debug module (optional) + TV (optional)
   CoreW_IFC #(N_External_Interrupt_Sources)  corew <- mkCoreW (dm_power_on_reset,
								reset_by ndm_reset);

   // ================================================================
   // Tie-offs (not used in SSITH GFE)

   // Set core's verbosity
   rule rl_never (False);
      corew.set_verbosity (?, ?);
   endrule

   // Tie-offs
   rule rl_always (True);
      // Non-maskable interrupt request.
      corew.nmi_req (False);
   endrule

`ifdef INCLUDE_GDB_CONTROL
   // ================================================================
   // NDM reset (reset for non-DebugModule)

   Reg #(Bit #(8))  rg_ndm_reset_delay <- mkReg (0);
   Reg #(Bool)      rg_running         <- mkRegU;

   // Get an NDM-reset request from the Debug Module, assert ndm-reset,
   // and then wait for a suitable delay.
   rule rl_ndm_reset (rg_ndm_reset_delay == 0);
      let x <- corew.ndm_reset_client.request.get;
      rg_running <= x;
      ndm_reset_controller.assertReset;
      rg_ndm_reset_delay <= fromInteger (ndm_reset_duration + 100);    // NOTE: heuristic

      $display ("%0d: %m.rl_ndm_reset: asserting NDM reset (for non-DebugModule) for %0d cycles",
		cur_cycle, ndm_reset_duration);
   endrule

   // Wait for suitable delay, then send ack response to Debug Module for NDM-reset request
   rule rl_ndm_reset_wait (rg_ndm_reset_delay != 0);
      if (rg_ndm_reset_delay == 1) begin
	 Bool is_running = rg_running;
         // Restart the corew
         corew.start (rg_running, 0, 0);
	 corew.ndm_reset_client.response.put (is_running);
	 $display ("%0d: %m.rl_ndm_reset_wait: sent NDM reset ack (for non-DebugModule) to Debug Module",
		   cur_cycle);
      end
      rg_ndm_reset_delay <= rg_ndm_reset_delay - 1;
   endrule

   // ================================================================
   // Start the corew a suitable time after a PoR
   UInt#(8) initial_wait = 100; // heuristic -- better to wait till "all out of reset" received from corew
   Reg #(UInt#(8)) rg_corew_start_after_por <- mkReg(initial_wait);
   rule rl_step_0 (rg_corew_start_after_por != 0);
      let n = rg_corew_start_after_por - 1;
      rg_corew_start_after_por <= n;
      if (n==0) corew.start (True, 0, 0); // initial start leaves proc running
   endrule
   // ================================================================



   // ================================================================
   // Instantiate JTAG TAP controller,
   // connect to corew.dmi;
   // and export its JTAG interface

   Wire#(Bit#(7)) w_dmi_req_addr <- mkDWire(0);
   Wire#(Bit#(32)) w_dmi_req_data <- mkDWire(0);
   Wire#(Bit#(2)) w_dmi_req_op <- mkDWire(0);

   Wire#(Bit#(32)) w_dmi_rsp_data <- mkDWire(0);
   Wire#(Bit#(2)) w_dmi_rsp_response <- mkDWire(0);

   BusReceiver#(Tuple3#(Bit#(7),Bit#(32),Bit#(2))) bus_dmi_req <- mkBusReceiver;
   BusSender#(Tuple2#(Bit#(32),Bit#(2))) bus_dmi_rsp <- mkBusSender(unpack(0));

`ifdef JTAG_TAP
   let jtagtap <- mkJtagTap(reset_by power_on_reset);

   mkConnection(jtagtap.dmi.req_ready, pack(bus_dmi_req.in.ready));
   mkConnection(jtagtap.dmi.req_valid, compose(bus_dmi_req.in.valid, unpack));
   mkConnection(jtagtap.dmi.req_addr, w_dmi_req_addr._write);
   mkConnection(jtagtap.dmi.req_data, w_dmi_req_data._write);
   mkConnection(jtagtap.dmi.req_op, w_dmi_req_op._write);
   mkConnection(jtagtap.dmi.rsp_valid, pack(bus_dmi_rsp.out.valid));
   mkConnection(jtagtap.dmi.rsp_ready, compose(bus_dmi_rsp.out.ready, unpack));
   mkConnection(jtagtap.dmi.rsp_data, w_dmi_rsp_data);
   mkConnection(jtagtap.dmi.rsp_response, w_dmi_rsp_response);
`endif

   rule rl_dmi_req;
      bus_dmi_req.in.data(tuple3(w_dmi_req_addr, w_dmi_req_data, w_dmi_req_op));
   endrule

   rule rl_dmi_rsp;
      match {.data, .response} = bus_dmi_rsp.out.data;
      w_dmi_rsp_data <= data;
      w_dmi_rsp_response <= response;
   endrule

   (* preempts = "rl_dmi_req_cpu, rl_dmi_rsp_cpu" *)
   rule rl_dmi_req_cpu;
      match {.addr, .data, .op} = bus_dmi_req.out.first;
      bus_dmi_req.out.deq;
      case (op)
	 1: corew.dmi.read_addr(addr);
	 2: begin
	       corew.dmi.write(addr, data);
	       bus_dmi_rsp.in.enq(tuple2(?, 0));
	    end
	 default: bus_dmi_rsp.in.enq(tuple2(?, 2));
      endcase
   endrule

   rule rl_dmi_rsp_cpu;
      let data <- corew.dmi.read_data;
      bus_dmi_rsp.in.enq(tuple2(data, 0));
   endrule

   // ================================================================
`endif

`ifdef INCLUDE_TANDEM_VERIF
   // ================================================================
   let tv_xactor <- mkTV_Xactor;

   mkConnection (corew.tv_verifier_info_get, tv_xactor.tv_in);
   // ================================================================
`endif

   let master0_synth <- toAXI4_Master_Synth (corew.cpu_imem_master);
   let master1_synth <- toAXI4_Master_Synth (corew.cpu_dmem_master);

`ifdef INCLUDE_DMEM_SLAVE
   let slave0_synth  <- toAXI4Lite_Slave_Synth (corew.cpu_dma_slave);
`endif

   // ================================================================
   // INTERFACE

   // ----------------------------------------------------------------
   // Core CPU interfaces

   // CPU IMem to Fabric master interface
   interface AXI4_Master_Synth master0 = master0_synth;

   // CPU DMem to Fabric master interface
   interface AXI4_Master_Synth master1 = master1_synth;

   // External interrupts
   method  Action interrupt_reqs (Bit #(N_External_Interrupt_Sources) reqs);
      for (Integer j = 0; j < valueOf (N_External_Interrupt_Sources); j = j + 1) begin
	 Bool req_j = unpack (reqs [j]);
	 corew.core_external_interrupt_sources [j].m_interrupt_req (req_j);
      end
   endmethod

`ifdef INCLUDE_DMEM_SLAVE
   // ----------------------------------------------------------------
   // Optional AXI4-Lite DMA slave interface

   interface AXI4Lite_Slave_Synth slave0 = slave0_synth;
`endif

`ifdef INCLUDE_GDB_CONTROL
   // ----------------------------------------------------------------
   // Optional Debug Module interfaces

`ifdef JTAG_TAP
   interface JTAG_IFC jtag = jtagtap.jtag;
`else
   interface DMI dmi = corew.dmi;
`endif
`endif

`ifdef INCLUDE_TANDEM_VERIF
   // ----------------------------------------------------------------
   // Optional Tandem Verifier interface.  The data signal is
   // packed output tuples (n,vb),/ where 'vb' is a vector of
   // bytes with relevant bytes in locations [0]..[n-1]

   interface tv_verifier_info_tx = tv_xactor.axi_out;
`endif

endmodule

// ================================================================
// The TV to AXI4 Stream transactor

`ifdef INCLUDE_TANDEM_VERIF

// ================================================================
// TV AXI4 Stream Parameters

typedef SizeOf #(Info_CPU_to_Verifier)Wd_SData;
typedef 0 Wd_SDest;
typedef 0 Wd_SUser;
typedef 0 Wd_SId;

// ================================================================

interface TV_Xactor;
   interface Put #(Info_CPU_to_Verifier) tv_in;
   interface AXI4_Stream_Master_IFC #(Wd_SId, Wd_SDest, Wd_SData, Wd_SUser)  axi_out;
endinterface

function AXI4_Stream #(Wd_SId, Wd_SDest, Wd_SData, Wd_SUser) fn_TVToAxiS (Info_CPU_to_Verifier x);
   return AXI4_Stream {tid: ?,
		       tdata: pack(x),
		       tstrb: '1,
		       tkeep: '1,
		       tlast: True,
		       tdest: ?,
		       tuser: ? };
endfunction

(*synthesize*)
module mkTV_Xactor (TV_Xactor);
   AXI4_Stream_Master_Xactor_IFC #(Wd_SId, Wd_SDest, Wd_SData, Wd_SUser)
                               tv_xactor <- mkAXI4_Stream_Master_Xactor;

   interface Put tv_in;
      method Action put(x);
	 toPut(tv_xactor.i_stream).put(fn_TVToAxiS(x));
      endmethod
   endinterface

   interface axi_out = tv_xactor.axi_side;
endmodule
`endif

// ================================================================

endpackage
