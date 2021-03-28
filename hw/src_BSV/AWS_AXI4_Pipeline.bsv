package AWS_AXI4_Pipeline;

// ================================================================
// Bluespec library imports

import Connectable :: *;
import List        :: *;

// ================================================================
// Project imports

import AWS_AXI4_Types          :: *;
import AWS_AXI4_Register_Slice :: *;

// ================================================================
// A Pipeline has a master-side and a slave-side, and a reset

interface AXI4_Pipeline_IFC #(numeric type wd_id,
			      numeric type wd_addr,
			      numeric type wd_data,
			      numeric type wd_user);
   method Action reset;
   interface AXI4_Slave_IFC  #(wd_id, wd_addr, wd_data, wd_user) from_master;
   interface AXI4_Master_IFC #(wd_id, wd_addr, wd_data, wd_user) to_slave;
endinterface

// ----------------------------------------------------------------

module mkAXI4_Pipeline #(Integer num_stages)
		       (AXI4_Pipeline_IFC #(wd_id, wd_addr, wd_data, wd_user));

   List #(AXI4_Register_Slice_IFC #(wd_id, wd_addr, wd_data, wd_user))
      l_slices <- replicateM (num_stages, mkAXI4_Register_Slice);

   for (Integer i = 1; i < num_stages; i = i + 1)
      mkConnection (l_slices [i - 1].to_slave, l_slices [i].from_master);

   method Action reset;
      for (Integer i = 0; i < num_stages; i = i + 1)
	 l_slices [i].reset;
   endmethod

   interface from_master = head (l_slices).from_master;
   interface to_slave    = last (l_slices).to_slave;
endmodule

endpackage : AWS_AXI4_Pipeline
