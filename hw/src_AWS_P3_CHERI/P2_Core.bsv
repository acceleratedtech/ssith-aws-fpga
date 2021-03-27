// Rename P3 to P3 so AWSP2 can instantiate a P3

package P2_Core;

import P3_Core :: *;
export P3_Core :: *;

export P2_Core_IFC, mkP2_Core;

typedef P3_Core_IFC P2_Core_IFC;

module mkP2_Core (P2_Core_IFC);
   let core <- mkP3_Core;
   return core;
endmodule

endpackage
