package DMI_Converter;

import FIFOF::*;
typedef Bit #(7)  DM_Addr;
DM_Addr max_DM_Addr = 'h5F;
typedef Bit #(32) DM_Word;

interface DMI;
   method Action                 read_addr  (DM_Addr dm_addr);
   method ActionValue #(DM_Word) read_data;
   method Action                 write    (DM_Addr dm_addr, DM_Word dm_word);
endinterface

(*always_ready, always_enabled*)
interface DMI_Blasted;
   method Action   req_ready (Bit#(1) x);
   method Bit#(1)  req_valid;
   method Bit#(7)  req_bits_addr;
   method Bit#(32) req_bits_data;
   method Bit#(2)  req_bits_op;
   method Bit#(1)  resp_ready;
   method Action   resp_valid (Bit#(1) x);
   method Action   resp_bits_data (Bit#(32) x);
   method Action   resp_bits_resp (Bit#(2) x);
endinterface

interface DMI_Converter;
   interface DMI_Blasted blasted;
   interface DMI         dmi;
endinterface

(*synthesize*)
module mkDMI_Converter(DMI_Converter);
   Wire#(Bit#(1))  w_req_valid     <- mkDWire(0);
   Wire#(Bit#(7))  w_req_bits_addr <- mkDWire(0);
   Wire#(Bit#(32)) w_req_bits_data <- mkDWire(0);
   Wire#(Bit#(2))  w_req_bits_op   <- mkDWire(0);
   Wire#(Bit#(1))  w_req_ready     <- mkWire;

   Wire#(Bit#(1)) w_resp_valid      <- mkWire;
   Wire#(Bit#(32)) w_resp_bits_data <- mkWire;
   Wire#(Bit#(2)) w_resp_bits_resp  <- mkWire;

   FIFOF#(Bool)   ff_reading        <- mkFIFOF;

   rule clear_wr_resp_rl if (!ff_reading.first && unpack(w_resp_valid));
	 ff_reading.deq;
   endrule

   interface DMI_Blasted blasted;
      method req_ready      = w_req_ready._write;
      method req_valid      = w_req_valid;
      method req_bits_addr  = w_req_bits_addr;
      method req_bits_data  = w_req_bits_data;
      method req_bits_op    = w_req_bits_op;

      method resp_ready     = pack(ff_reading.notEmpty());
      method resp_valid     = w_resp_valid._write;
      method resp_bits_data = w_resp_bits_data._write;
      method resp_bits_resp = w_resp_bits_resp._write;
   endinterface

   interface DMI dmi;
      method Action                 read_addr  (a) if (unpack(w_req_ready));
	 w_req_bits_addr <= a;
	 w_req_valid     <= 1;
         w_req_bits_op   <= 1;
	 ff_reading.enq(True);
      endmethod

      method ActionValue #(DM_Word) read_data() if (ff_reading.first && unpack(w_resp_valid));
	 ff_reading.deq;
	 let x = (w_resp_bits_resp == 0 ? w_resp_bits_data : 'h_AAAA_AAAA);
	 return x;
      endmethod

      method Action                 write    (a, d) if (unpack(w_req_ready));
	 w_req_bits_addr <= a;
	 w_req_bits_data <= d;
	 w_req_valid     <= 1;
         w_req_bits_op   <= 2;
	 ff_reading.enq(False);
      endmethod
   endinterface
endmodule

endpackage
