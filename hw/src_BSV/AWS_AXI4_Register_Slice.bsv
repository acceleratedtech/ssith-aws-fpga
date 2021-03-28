package AWS_AXI4_Register_Slice;

// ================================================================
// Bluespec library imports

import Connectable :: *;

// ================================================================
// Project imports

import AWS_AXI4_Types :: *;
import Semi_FIFOF     :: *;

// ================================================================
// A Register Slice has a master-side and a slave-side, and a reset

interface AXI4_Register_Slice_IFC #(numeric type wd_id,
				    numeric type wd_addr,
				    numeric type wd_data,
				    numeric type wd_user);
   method Action reset;
   interface AXI4_Slave_IFC  #(wd_id, wd_addr, wd_data, wd_user) from_master;
   interface AXI4_Master_IFC #(wd_id, wd_addr, wd_data, wd_user) to_slave;
endinterface

// ----------------------------------------------------------------

module mkAXI4_Register_Slice (AXI4_Register_Slice_IFC #(wd_id, wd_addr, wd_data, wd_user));
   AXI4_Slave_Xactor_IFC  #(wd_id, wd_addr, wd_data, wd_user) xactor_from_master <- mkAXI4_Slave_Xactor;
   AXI4_Master_Xactor_IFC #(wd_id, wd_addr, wd_data, wd_user) xactor_to_slave    <- mkAXI4_Master_Xactor;

   mkConnection (xactor_from_master.o_wr_addr, xactor_to_slave.i_wr_addr);
   mkConnection (xactor_from_master.o_wr_data, xactor_to_slave.i_wr_data);
   mkConnection (xactor_from_master.i_wr_resp, xactor_to_slave.o_wr_resp);
   mkConnection (xactor_from_master.o_rd_addr, xactor_to_slave.i_rd_addr);
   mkConnection (xactor_from_master.i_rd_data, xactor_to_slave.o_rd_data);

   method Action reset;
      xactor_from_master.reset;
      xactor_to_slave.reset;
   endmethod

   interface from_master = xactor_from_master.axi_side;
   interface to_slave    = xactor_to_slave   .axi_side;
endmodule

endpackage : AWS_AXI4_Register_Slice
