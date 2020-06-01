package AWS_AXI4_Crossing;

// ================================================================
// Bluespec library imports

import Clocks :: *;

// ================================================================
// Project imports

import AWS_AXI4_Types :: *;
import GetPut_Aux     :: *;
import Semi_FIFOF     :: *;

// ================================================================
// A Crossing has a master-side and a slave-side, but no reset

interface AXI4_Crossing_IFC #(numeric type wd_id,
			      numeric type wd_addr,
			      numeric type wd_data,
			      numeric type wd_user);
   interface AXI4_Slave_IFC  #(wd_id, wd_addr, wd_data, wd_user) from_master;
   interface AXI4_Master_IFC #(wd_id, wd_addr, wd_data, wd_user) to_slave;
endinterface

// ----------------------------------------------------------------

module mkAXI4_Crossing #(Clock master_clock,
			 Reset master_reset,
			 Clock slave_clock,
			 Reset slave_reset)
		       (AXI4_Crossing_IFC #(wd_id, wd_addr, wd_data, wd_user));
   AXI4_Slave_Xactor_IFC  #(wd_id, wd_addr, wd_data, wd_user) xactor_from_master <- mkAXI4_Slave_Xactor  (clocked_by master_clock, reset_by master_reset);
   AXI4_Master_Xactor_IFC #(wd_id, wd_addr, wd_data, wd_user) xactor_to_slave    <- mkAXI4_Master_Xactor (clocked_by  slave_clock, reset_by  slave_reset);

   SyncFIFOIfc #(AXI4_Wr_Addr #(wd_id, wd_addr, wd_user)) f_wr_addr <- mkSyncFIFO (4, master_clock, master_reset,  slave_clock);
   SyncFIFOIfc #(AXI4_Wr_Data #(wd_data, wd_user))        f_wr_data <- mkSyncFIFO (4, master_clock, master_reset,  slave_clock);
   SyncFIFOIfc #(AXI4_Wr_Resp #(wd_id, wd_user))          f_wr_resp <- mkSyncFIFO (4,  slave_clock,  slave_reset, master_clock);

   SyncFIFOIfc #(AXI4_Rd_Addr #(wd_id, wd_addr, wd_user)) f_rd_addr <- mkSyncFIFO (4, master_clock, master_reset,  slave_clock);
   SyncFIFOIfc #(AXI4_Rd_Data #(wd_id, wd_data, wd_user)) f_rd_data <- mkSyncFIFO (4,  slave_clock,  slave_reset, master_clock);

   rule rl_master_wr_addr;
      let wra <- pop_o (xactor_from_master.o_wr_addr);
      f_wr_addr.enq (wra);
   endrule

   rule rl_master_wr_data;
      let wrd <- pop_o (xactor_from_master.o_wr_data);
      f_wr_data.enq (wrd);
   endrule

   rule rl_master_wr_resp;
      let wrr <- pop (f_wr_resp);
      xactor_from_master.i_wr_resp.enq (wrr);
   endrule

   rule rl_master_rd_addr;
      let rda <- pop_o (xactor_from_master.o_rd_addr);
      f_rd_addr.enq (rda);
   endrule

   rule rl_master_rd_data;
      let rdr <- pop (f_rd_data);
      xactor_from_master.i_rd_data.enq (rdr);
   endrule

   rule rl_slave_wr_addr;
      let wra <- pop (f_wr_addr);
      xactor_to_slave.i_wr_addr.enq (wra);
   endrule

   rule rl_slave_wr_data;
      let wrd <- pop (f_wr_data);
      xactor_to_slave.i_wr_data.enq (wrd);
   endrule

   rule rl_slave_wr_resp;
      let wrr <- pop_o (xactor_to_slave.o_wr_resp);
      f_wr_resp.enq (wrr);
   endrule

   rule rl_slave_rd_addr;
      let rda <- pop (f_rd_addr);
      xactor_to_slave.i_rd_addr.enq (rda);
   endrule

   rule rl_slave_rd_data;
      let rdr <- pop_o (xactor_to_slave.o_rd_data);
      f_rd_data.enq (rdr);
   endrule

   interface from_master = xactor_from_master.axi_side;
   interface to_slave    = xactor_to_slave   .axi_side;
endmodule

module mkAXI4_CrossingFromCC #(Clock slave_clock, Reset slave_reset)
			     (AXI4_Crossing_IFC #(wd_id, wd_addr, wd_data, wd_user));
   let master_clock <- exposeCurrentClock;
   let master_reset <- exposeCurrentReset;
   let crossing <- mkAXI4_Crossing (master_clock, master_reset, slave_clock, slave_reset);

   return crossing;
endmodule

module mkAXI4_CrossingToCC #(Clock master_clock, Reset master_reset)
			   (AXI4_Crossing_IFC #(wd_id, wd_addr, wd_data, wd_user));
   let slave_clock <- exposeCurrentClock;
   let slave_reset <- exposeCurrentReset;
   let crossing <- mkAXI4_Crossing (master_clock, master_reset, slave_clock, slave_reset);

   return crossing;
endmodule

endpackage : AWS_AXI4_Crossing
