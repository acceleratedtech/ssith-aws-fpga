package AWS_AXI4_Downsizer;

// ================================================================
// Bluespec library imports

import Connectable  :: *;
import FIFO         :: *;
import SpecialFIFOs :: *;

// ================================================================
// Project imports

import AWS_AXI4_Deburster :: *;
import AWS_AXI4_Types     :: *;
import Semi_FIFOF         :: *;

// ================================================================
// The interface for the downsizer module

interface AXI4_Downsizer_IFC #(numeric type wd_id,
			       numeric type wd_addr,
			       numeric type m_wd_data,
			       numeric type s_wd_data,
			       numeric type wd_user);
   interface AXI4_Slave_IFC  #(wd_id, wd_addr, m_wd_data, wd_user) from_master;
   interface AXI4_Master_IFC #(wd_id, wd_addr, s_wd_data, wd_user) to_slave;
endinterface

// ================================================================
// The downsizer module. This implementation relies on in-order response
// delivery and so its ID width must be 0 to statically enforce that.
//
module mkAXI4_Downsizer (AXI4_Downsizer_IFC #(0, wd_addr, m_wd_data, s_wd_data, wd_user))
   // Many of these should be completely unnecessary assertions, but bsc is
   // unable to infer most of it so we need to hold its hand and tell it
   // everything it could possibly want.
   provisos (Add #(a__, s_wd_data, m_wd_data),
	     Mul #(s_wd_data, ratio, m_wd_data),
	     Div #(m_wd_data, 8, m_wd_data_bytes),
	     Div #(s_wd_data, 8, s_wd_data_bytes),
	     Log #(m_wd_data_bytes, log_m_wd_data_bytes),
	     Log #(s_wd_data_bytes, log_s_wd_data_bytes),
	     Add #(b__, s_wd_data_bytes, m_wd_data_bytes),
	     Add #(c__, 2, ratio),
	     Add #(d__, ratio, 256),
	     Log #(ratio, log_ratio),
	     Add #(log_s_wd_data_bytes, log_ratio, log_m_wd_data_bytes),
	     Add #(e__, s_wd_data_bytes, TDiv#(m_wd_data, 8)),
	     Add #(f__, 8, wd_addr),
	     Add #(g__, log_ratio, 8));

   // So that, for simplicitly, we can always turn transactions into bursts
   // with no need to split, and since the only user of this module is to
   // connect to an AXI4-Lite slave so we'd need to deburst anyway, we feed all
   // the input through a deburster before downsizing it.
   AXI4_Deburster_IFC #(0, wd_addr, m_wd_data, wd_user) deburster <- mkAXI4_Deburster;
   AXI4_Slave_Xactor_IFC  #(0, wd_addr, m_wd_data, wd_user) xactor_from_deburster <- mkAXI4_Slave_Xactor;
   mkConnection (deburster.to_slave, xactor_from_deburster.axi_side);

   AXI4_Master_Xactor_IFC #(0, wd_addr, s_wd_data, wd_user) xactor_to_slave <- mkAXI4_Master_Xactor;

   FIFO #(Bit #(log_ratio)) f_wr_beat_start <- mkBypassFIFO;
   FIFO #(Bit #(log_ratio)) f_wr_len        <- mkBypassFIFO;
   Reg #(Bit #(log_ratio)) rg_wr_beat_num <- mkReg (0);

   FIFO #(Bit #(log_ratio)) f_rd_beat_start <- mkSizedFIFO (4);
   Reg #(AXI4_Resp)        rg_rd_resp     <- mkReg (axi4_resp_okay);
   Reg #(Bit #(m_wd_data)) rg_rd_data     <- mkReg (0);
   Reg #(Bit #(log_ratio)) rg_rd_beat_num <- mkReg (0);

   rule rl_wr_addr;
      let wra_in <- pop_o (xactor_from_deburster.o_wr_addr);
      let wra_out = wra_in;

      Bit #(8) awbytes = (8'b1 << wra_in.awsize);
      if (awbytes > fromInteger (valueOf (s_wd_data_bytes))) begin
	 let conv_ratio = awbytes / fromInteger (valueOf (s_wd_data_bytes));
	 let downsize_len = conv_ratio - 1;
	 let align_adjust = (wra_in.awaddr & (zeroExtend (awbytes - 1))) / valueOf (s_wd_data_bytes);
	 wra_out.awlen = downsize_len - truncate (align_adjust);
	 wra_out.awsize = fromInteger (valueOf (log_s_wd_data_bytes));
	 wra_out.awburst = axburst_incr;
      end

      xactor_to_slave.i_wr_addr.enq (wra_out);
      f_wr_beat_start.enq (wra_in.awaddr [valueOf (log_m_wd_data_bytes) - 1 : valueOf (log_s_wd_data_bytes)]);
      f_wr_len.enq (truncate (wra_out.awlen));
   endrule

   rule rl_wr_data;
      let wrd_in = xactor_from_deburster.o_wr_data.first;
      Bit #(log_ratio) awlen = f_wr_len.first;

      Bit #(log_m_wd_data_bytes) byte_shift = zeroExtend (f_wr_beat_start.first + rg_wr_beat_num) * fromInteger (valueOf (s_wd_data_bytes));
      Bit #(s_wd_data) wdata = truncate (wrd_in.wdata >> {byte_shift, 3'b0});
      Bit #(s_wd_data_bytes) wstrb = truncate (wrd_in.wstrb >> byte_shift);
      Bool wlast = (awlen == rg_wr_beat_num);

      let wrd_out = AXI4_Wr_Data {
	 wdata: wdata,
	 wstrb: wstrb,
	 wlast: wlast,
	 wuser: wrd_in.wuser
      };
      xactor_to_slave.i_wr_data.enq (wrd_out);

      if (wlast) begin
	 xactor_from_deburster.o_wr_data.deq;
	 f_wr_beat_start.deq;
	 f_wr_len.deq;
	 rg_wr_beat_num <= 0;
      end
      else begin
	 rg_wr_beat_num <= rg_wr_beat_num + 1;
      end
   endrule

   rule rl_wr_resp;
      let wrr <- pop_o (xactor_to_slave.o_wr_resp);
      xactor_from_deburster.i_wr_resp.enq (wrr);
   endrule

   rule rl_rd_addr;
      let rda_in <- pop_o (xactor_from_deburster.o_rd_addr);
      let rda_out = rda_in;

      Bit #(8) arbytes = (8'b1 << rda_in.arsize);
      if (arbytes > fromInteger (valueOf (s_wd_data_bytes))) begin
	 let conv_ratio = arbytes / fromInteger (valueOf (s_wd_data_bytes));
	 let downsize_len = conv_ratio - 1;
	 let align_adjust = (rda_in.araddr & (zeroExtend (arbytes - 1))) / valueOf (s_wd_data_bytes);
	 rda_out.arlen = downsize_len - truncate (align_adjust);
	 rda_out.arsize = fromInteger (valueOf (log_s_wd_data_bytes));
	 rda_out.arburst = axburst_incr;
      end

      xactor_to_slave.i_rd_addr.enq (rda_out);
      f_rd_beat_start.enq (rda_in.araddr [valueOf (log_m_wd_data_bytes) - 1 : valueOf (log_s_wd_data_bytes)]);
   endrule

   rule rl_rd_data;
      let rdr_in <- pop_o (xactor_to_slave.o_rd_data);

      Bit #(log_m_wd_data_bytes) byte_shift = zeroExtend (f_rd_beat_start.first + rg_rd_beat_num) * fromInteger (valueOf (s_wd_data_bytes));
      Bit #(m_wd_data) rdata = rg_rd_data | (zeroExtend (rdr_in.rdata) << {byte_shift, 3'b0});

      AXI4_Resp rresp = rg_rd_resp;
      if (rresp == axi4_resp_okay)
	 rresp = rdr_in.rresp;

      if (rdr_in.rlast) begin
	 let rdr_out = AXI4_Rd_Data {
	    rid:   rdr_in.rid,
	    rdata: rdata,
	    rresp: rresp,
	    rlast: True,
	    ruser: rdr_in.ruser
	 };
	 xactor_from_deburster.i_rd_data.enq (rdr_out);

	 rg_rd_resp <= axi4_resp_okay;
	 rg_rd_data <= 0;
	 rg_rd_beat_num <= 0;
	 f_rd_beat_start.deq;
      end
      else begin
	 rg_rd_resp <= rresp;
	 rg_rd_data <= rdata;
	 rg_rd_beat_num <= rg_rd_beat_num + 1;
      end
   endrule

   interface from_master = deburster.from_master;
   interface to_slave    = xactor_to_slave.axi_side;
endmodule

endpackage : AWS_AXI4_Downsizer
