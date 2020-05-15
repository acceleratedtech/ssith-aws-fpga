package AWS_AXI4_Id_Reflector;

// ================================================================
// Bluespec library imports

import Connectable  :: *;
import FIFO         :: *;
import SpecialFIFOs :: *;

// ================================================================
// Project imports

import AWS_AXI4_Types     :: *;
import Semi_FIFOF         :: *;

// ================================================================
// The interface for the ID reflector module

interface AXI4_Id_Reflector_IFC #(numeric type wd_id,
				  numeric type wd_addr,
				  numeric type wd_data,
				  numeric type wd_user);
   interface AXI4_Slave_IFC  #(wd_id, wd_addr, wd_data, wd_user) from_master;
   interface AXI4_Master_IFC #(    0, wd_addr, wd_data, wd_user) to_slave;
endinterface

// ================================================================
// The ID reflector module

module mkAXI4_Id_Reflector (AXI4_Id_Reflector_IFC #(wd_id, wd_addr, wd_data, wd_user));
   AXI4_Slave_Xactor_IFC  #(wd_id, wd_addr, wd_data, wd_user) xactor_from_master <- mkAXI4_Slave_Xactor;
   AXI4_Master_Xactor_IFC #(    0, wd_addr, wd_data, wd_user) xactor_to_slave    <- mkAXI4_Master_Xactor;

   FIFO #(Bit #(wd_id)) f_wr_id <- mkSizedFIFO (4);
   FIFO #(Bit #(wd_id)) f_rd_id <- mkSizedFIFO (4);

   rule rl_wr_addr;
      AXI4_Wr_Addr #(wd_id, wd_addr, wd_user) wra_in <- pop_o (xactor_from_master.o_wr_addr);
      let wra_out = AXI4_Wr_Addr {
	 awid:     0'b0,
	 awaddr:   wra_in.awaddr,
	 awlen:    wra_in.awlen,
	 awsize:   wra_in.awsize,
	 awburst:  wra_in.awburst,
	 awlock:   wra_in.awlock,
	 awcache:  wra_in.awcache,
	 awprot:   wra_in.awprot,
	 awqos:    wra_in.awqos,
	 awregion: wra_in.awregion,
	 awuser:   wra_in.awuser
      };

      xactor_to_slave.i_wr_addr.enq (wra_out);
      f_wr_id.enq (wra_in.awid);
   endrule

   rule rl_wr_data;
      AXI4_Wr_Data #(wd_data, wd_user) wrd <- pop_o (xactor_from_master.o_wr_data);
      xactor_to_slave.i_wr_data.enq (wrd);
   endrule

   rule rl_wr_resp;
      AXI4_Wr_Resp #(    0, wd_user) wrr_in <- pop_o (xactor_to_slave.o_wr_resp);
      let wrr_out = AXI4_Wr_Resp {
	 bid:   f_wr_id.first,
	 bresp: wrr_in.bresp,
	 buser: wrr_in.buser
      };

      xactor_from_master.i_wr_resp.enq (wrr_out);
      f_wr_id.deq;
   endrule

   rule rl_rd_addr;
      AXI4_Rd_Addr #(wd_id, wd_addr, wd_user) rda_in <- pop_o (xactor_from_master.o_rd_addr);
      let rda_out = AXI4_Rd_Addr {
	 arid:     0'b0,
	 araddr:   rda_in.araddr,
	 arlen:    rda_in.arlen,
	 arsize:   rda_in.arsize,
	 arburst:  rda_in.arburst,
	 arlock:   rda_in.arlock,
	 arcache:  rda_in.arcache,
	 arprot:   rda_in.arprot,
	 arqos:    rda_in.arqos,
	 arregion: rda_in.arregion,
	 aruser:   rda_in.aruser
      };

      xactor_to_slave.i_rd_addr.enq (rda_out);
      f_rd_id.enq (rda_in.arid);
   endrule

   rule rl_rd_data;
      AXI4_Rd_Data #(    0, wd_data, wd_user) rdr_in <- pop_o (xactor_to_slave.o_rd_data);
      let rdr_out = AXI4_Rd_Data {
	 rid:   f_rd_id.first,
	 rdata: rdr_in.rdata,
	 rresp: rdr_in.rresp,
	 rlast: rdr_in.rlast,
	 ruser: rdr_in.ruser
      };

      xactor_from_master.i_rd_data.enq (rdr_out);
      if (rdr_out.rlast)
	 f_rd_id.deq;
   endrule

   interface from_master = xactor_from_master.axi_side;
   interface to_slave    = xactor_to_slave   .axi_side;
endmodule

endpackage : AWS_AXI4_Id_Reflector
