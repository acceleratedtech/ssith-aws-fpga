import AWS_AXI4_Types :: *;
import AWS_AXI4_Deburster :: *;
import Connectable :: *;

`ifdef HAVE_BLUESTUFF_AXI
import AXI4_Types :: *;
import AXI4Lite_Types :: *;

instance Connectable #(AXI4_Types::AXI4_Master_Synth  #(m_wd_id, wd_addr, wd_data, wd_user, wd_user, wd_user, wd_user, wd_user),
		       AWS_AXI4_Types::AXI4_Slave_IFC #(s_wd_id, wd_addr, wd_data, wd_user))
   provisos (Add #(a__, m_wd_id, s_wd_id));

   module mkConnection #(AXI4_Types::AXI4_Master_Synth   #(m_wd_id, wd_addr, wd_data, wd_user, wd_user, wd_user, wd_user, wd_user) axim,
			 AWS_AXI4_Types::AXI4_Slave_IFC  #(s_wd_id, wd_addr, wd_data, wd_user) axis)
		       (Empty);

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_awflit;
	 axis.m_awvalid (axim.aw.awvalid,
			 zeroExtend(axim.aw.awid),
			 axim.aw.awaddr,
			 axim.aw.awlen,
			 unpack(pack(axim.aw.awsize)),
			 unpack(pack(axim.aw.awburst)),
			 unpack(pack(axim.aw.awlock)),
			 axim.aw.awcache,
			 axim.aw.awprot,
			 axim.aw.awqos,
			 axim.aw.awregion,
			 axim.aw.awuser);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_awready;
	 axim.aw.awready (axis.m_awready);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_wflit;
	 axis.m_wvalid (axim.w.wvalid,
			axim.w.wdata,
			axim.w.wstrb,
			axim.w.wlast,
			axim.w.wuser);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_wready;
	 axim.w.wready (axis.m_wready);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_bflit;
	 axim.b.bflit (axis.m_bvalid,
		       truncate(axis.m_bid),
		       unpack(pack(axis.m_bresp)),
		       axis.m_buser);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_bready;
	 axis.m_bready (axim.b.bready);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_arflit;
	 axis.m_arvalid (axim.ar.arvalid,
			 zeroExtend(axim.ar.arid),
			 axim.ar.araddr,
			 axim.ar.arlen,
			 unpack(pack(axim.ar.arsize)),
			 unpack(pack(axim.ar.arburst)),
			 unpack(pack(axim.ar.arlock)),
			 axim.ar.arcache,
			 axim.ar.arprot,
			 axim.ar.arqos,
			 axim.ar.arregion,
			 axim.ar.aruser);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_arready;
	 axim.ar.arready (axis.m_arready);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_rflit;
	 axim.r.rflit (axis.m_rvalid,
		       truncate(axis.m_rid),
		       axis.m_rdata,
		       unpack(pack(axis.m_rresp)),
		       axis.m_rlast,
		       axis.m_ruser);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_rready;
	 axis.m_rready (axim.r.rready);
      endrule
   endmodule
endinstance

instance Connectable #(AWS_AXI4_Types::AXI4_Master_IFC #(0, wd_addr, wd_data, wd_user),
		       AXI4Lite_Types::AXI4Lite_Slave_Synth #(wd_addr, wd_data, wd_user, wd_user, wd_user, wd_user, wd_user))
   provisos (Add #(a__, 8, wd_addr));

   module mkConnection #(AWS_AXI4_Types::AXI4_Master_IFC #(0, wd_addr, wd_data, wd_user) axim,
			 AXI4Lite_Types::AXI4Lite_Slave_Synth #(wd_addr, wd_data, wd_user, wd_user, wd_user, wd_user, wd_user) axis)
		       (Empty);

      AXI4_Deburster_IFC #(0, wd_addr, wd_data, wd_user) deburster <- mkAXI4_Deburster();
      mkConnection(axim, deburster.from_master);

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_awflit;
	 axis.aw.awflit (deburster.to_slave.m_awvalid,
			 deburster.to_slave.m_awaddr,
			 deburster.to_slave.m_awprot,
			 deburster.to_slave.m_awuser);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_awready;
	 deburster.to_slave.m_awready (axis.aw.awready);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_wflit;
	 axis.w.wflit (deburster.to_slave.m_wvalid,
		       deburster.to_slave.m_wdata,
		       deburster.to_slave.m_wstrb,
		       deburster.to_slave.m_wuser);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_wready;
	 deburster.to_slave.m_wready (axis.w.wready);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_bflit;
	 deburster.to_slave.m_bvalid (axis.b.bvalid,
				      0'b0,
				      unpack(pack(axis.b.bresp)),
				      axis.b.buser);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_bready;
	 axis.b.bready (deburster.to_slave.m_bready);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_arflit;
	 axis.ar.arflit (deburster.to_slave.m_arvalid,
			 deburster.to_slave.m_araddr,
			 deburster.to_slave.m_arprot,
			 deburster.to_slave.m_aruser);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_arready;
	 deburster.to_slave.m_arready (axis.ar.arready);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_rflit;
	 deburster.to_slave.m_rvalid (axis.r.rvalid,
				      0'b0,
				      axis.r.rdata,
				      unpack(pack(axis.r.rresp)),
				      True,
				      axis.r.ruser);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_rready;
	 axis.r.rready (deburster.to_slave.m_rready);
      endrule
   endmodule

endinstance
`else
import AXI4_Types :: *;
import AXI4_Lite_Types :: *;

instance Connectable #(AXI4_Types::AXI4_Master_IFC    #(m_wd_id, wd_addr, wd_data, wd_user),
		       AWS_AXI4_Types::AXI4_Slave_IFC #(s_wd_id, wd_addr, wd_data, wd_user))
   provisos (Add #(a__, m_wd_id, s_wd_id));

   module mkConnection #(AXI4_Types::AXI4_Master_IFC     #(m_wd_id, wd_addr, wd_data, wd_user) axim,
			 AWS_AXI4_Types::AXI4_Slave_IFC  #(s_wd_id, wd_addr, wd_data, wd_user) axis)
		       (Empty);

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_wr_addr_channel;
	 axis.m_awvalid (axim.m_awvalid,
			 zeroExtend(axim.m_awid),
			 axim.m_awaddr,
			 axim.m_awlen,
			 axim.m_awsize,
			 axim.m_awburst,
			 axim.m_awlock,
			 axim.m_awcache,
			 axim.m_awprot,
			 axim.m_awqos,
			 axim.m_awregion,
			 axim.m_awuser);
	 axim.m_awready (axis.m_awready);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_wr_data_channel;
	 axis.m_wvalid (axim.m_wvalid,
			axim.m_wdata,
			axim.m_wstrb,
			axim.m_wlast,
			axim.m_wuser);
	 axim.m_wready (axis.m_wready);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_wr_response_channel;
	 axim.m_bvalid (axis.m_bvalid,
			truncate(axis.m_bid),
			axis.m_bresp,
			axis.m_buser);
	 axis.m_bready (axim.m_bready);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_rd_addr_channel;
	 axis.m_arvalid (axim.m_arvalid,
			 zeroExtend(axim.m_arid),
			 axim.m_araddr,
			 axim.m_arlen,
			 axim.m_arsize,
			 axim.m_arburst,
			 axim.m_arlock,
			 axim.m_arcache,
			 axim.m_arprot,
			 axim.m_arqos,
			 axim.m_arregion,
			 axim.m_aruser);
	 axim.m_arready (axis.m_arready);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_rd_data_channel;
	 axim.m_rvalid (axis.m_rvalid,
			truncate(axis.m_rid),
			axis.m_rdata,
			axis.m_rresp,
			axis.m_rlast,
			axis.m_ruser);
	 axis.m_rready (axim.m_rready);
      endrule
   endmodule
endinstance

instance Connectable #(AWS_AXI4_Types::AXI4_Master_IFC #(0, wd_addr, wd_data, wd_user),
		       AXI4_Lite_Types::AXI4_Lite_Slave_IFC #(wd_addr, wd_data, wd_user))
   provisos (Add #(a__, 8, wd_addr));

   module mkConnection #(AWS_AXI4_Types::AXI4_Master_IFC #(0, wd_addr, wd_data, wd_user) axim,
			 AXI4_Lite_Types::AXI4_Lite_Slave_IFC #(wd_addr, wd_data, wd_user) axis)
		       (Empty);

      AXI4_Deburster_IFC #(0, wd_addr, wd_data, wd_user) deburster <- mkAXI4_Deburster();
      mkConnection(axim, deburster.from_master);

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_wr_addr_channel;
	 axis.m_awvalid (deburster.to_slave.m_awvalid,
			 deburster.to_slave.m_awaddr,
			 deburster.to_slave.m_awprot,
			 deburster.to_slave.m_awuser);
	 deburster.to_slave.m_awready (axis.m_awready);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_wr_data_channel;
	 axis.m_wvalid (deburster.to_slave.m_wvalid,
			deburster.to_slave.m_wdata,
			deburster.to_slave.m_wstrb);
	 deburster.to_slave.m_wready (axis.m_wready);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_wr_response_channel;
	 deburster.to_slave.m_bvalid (axis.m_bvalid,
				      0'b0,
				      axis.m_bresp,
				      axis.m_buser);
	 axis.m_bready (deburster.to_slave.m_bready);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_rd_addr_channel;
	 axis.m_arvalid (deburster.to_slave.m_arvalid,
			 deburster.to_slave.m_araddr,
			 deburster.to_slave.m_arprot,
			 deburster.to_slave.m_aruser);
	 deburster.to_slave.m_arready (axis.m_arready);
      endrule

      (* fire_when_enabled, no_implicit_conditions *)
      rule rl_rd_data_channel;
	 deburster.to_slave.m_rvalid (axis.m_rvalid,
				      0'b0,
				      axis.m_rdata,
				      axis.m_rresp,
				      True,
				      axis.m_ruser);
	 axis.m_rready (deburster.to_slave.m_rready);
      endrule
   endmodule

endinstance
`endif
