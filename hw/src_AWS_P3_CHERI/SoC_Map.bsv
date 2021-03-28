// Copyright (c) 2013-2019 Bluespec, Inc. All Rights Reserved

package SoC_Map;

// ================================================================
// This module defines the overall 'address map' of the SoC, showing
// the addresses serviced by each slave IP, and which addresses are
// memory vs. I/O.

// ***** WARNING! WARNING! WARNING! *****

// During system integration, this address map should be identical to
// the system interconnect settings (e.g., routing of requests between
// masters and slaves).  This map is also needed by software so that
// it knows how to address various IPs.

// This module contains no state; it just has constants, and so can be
// freely instantiated at multiple places in the SoC module hierarchy
// at no hardware cost.  It allows this map to be defined in one
// place and shared across the SoC.

// ================================================================
// This version of SoC_Map is for the DARPA SSITH GFE

// Our "Near_Mem_IO" corresponds to "CLINT" in Rocket

// ================================================================
// Exports

export  SoC_Map_Struct (..), soc_map_struct;

export  SoC_Map_IFC (..), mkSoC_Map;

export  N_External_Interrupt_Sources;
export  n_external_interrupt_sources;
export  irq_num_uart16550_0;

// ================================================================
// Bluespec library imports

import Routable :: *; // For Range

// ================================================================
// Project imports

import Fabric_Defs :: *;    // Only for type Fabric_Addr

// ================================================================
// Top-level-struct version of the SoC Map for RISCY-OOO

typedef struct {
   Bit #(64)  near_mem_io_addr_base;
   Bit #(64)  near_mem_io_addr_size;

   Bit #(64)  boot_rom_addr_base;
   Bit #(64)  boot_rom_addr_size;

   Bit #(64)  main_mem_addr_base;
   Bit #(64)  main_mem_addr_size;

   Bit #(64)  pc_reset_value;
   } SoC_Map_Struct
deriving (FShow);

SoC_Map_Struct soc_map_struct =
SoC_Map_Struct {
   near_mem_io_addr_base:  'h_1000_0000,
   near_mem_io_addr_size:  'h_0001_0000,

   boot_rom_addr_base:     'h_7000_0000,
   boot_rom_addr_size:     'h_0000_1000,

   main_mem_addr_base:     'h_8000_0000,
   main_mem_addr_size:     'h_4_0000_0000,

   pc_reset_value:         'h_7000_0000    // = boot_rom_addr_base
   };

// ================================================================

function Bool addr_function(Fabric_Addr base, Fabric_Addr size, Fabric_Addr addr);
   return (base <= addr) && (addr < (base + size));
endfunction

// ================================================================
// Interface and module for the address map

interface SoC_Map_IFC;
   (* always_ready *)   method  Range#(Wd_Addr)  m_plic_addr_range;
   (* always_ready *)   method  Range#(Wd_Addr)  m_near_mem_io_addr_range;
   (* always_ready *)   method  Range#(Wd_Addr)  m_flash_mem_addr_range;
   (* always_ready *)   method  Range#(Wd_Addr)  m_ethernet_0_addr_range;
   (* always_ready *)   method  Range#(Wd_Addr)  m_dma_0_addr_range;
   (* always_ready *)   method  Range#(Wd_Addr)  m_uart16550_0_addr_range;
   (* always_ready *)   method  Range#(Wd_Addr)  m_gpio_0_addr_range;
   (* always_ready *)   method  Range#(Wd_Addr)  m_boot_rom_addr_range;
   (* always_ready *)   method  Range#(Wd_Addr)  m_ddr4_0_cached_addr_range;
   (* always_ready *)   method  Range#(Wd_Addr)  m_mem0_controller_addr_range;

   (* always_ready *)
   method  Bool  m_is_mem_addr (Fabric_Addr addr);

   (* always_ready *)
   method  Bool  m_is_IO_addr (Fabric_Addr addr, Bool imem_not_dmem);

   (* always_ready *)
   method  Bool  m_is_near_mem_IO_addr (Fabric_Addr addr);

   (* always_ready *)   method  Bit #(64)  m_pc_reset_value;
   (* always_ready *)   method  Bit #(64)  m_mtvec_reset_value;
   (* always_ready *)   method  Bit #(64)  m_nmivec_reset_value;
endinterface

// ================================================================

(* synthesize *)
module mkSoC_Map (SoC_Map_IFC);

   // ----------------------------------------------------------------
   // PLIC

   let plic_addr_range = Range {
      base: 'h_0C00_0000,
      size: 'h_0040_0000    // 4M
   };

   // ----------------------------------------------------------------
   // Near_Mem_IO (CLINT)

   let near_mem_io_addr_range = Range {
      base: 'h_1000_0000,
      size: 'h_0001_0000    // 64K
   };

   // ----------------------------------------------------------------
   // Flash Mem

   let flash_mem_addr_range = Range {
      base: 'h_4000_0000,
      size: 'h_0800_0000    // 128M
   };

   // ----------------------------------------------------------------
   // Ethernet 0

   let ethernet_0_addr_range = Range {
      base: 'h_6210_0000,
      size: 'h_0004_0000    // 256K
   };

   // ----------------------------------------------------------------
   // DMA 0

   let dma_0_addr_range = Range {
      base: 'h_6220_0000,
      size: 'h_0001_0000    // 64K
   };

   // ----------------------------------------------------------------
   // UART 0

   let uart16550_0_addr_range = Range {
      base: 'h_6230_0000,
      size: 'h_0000_1000    // 4K
   };

   // ----------------------------------------------------------------
   // GPIO 0

   let gpio_0_addr_range = Range {
      base: 'h_6FFF_0000,
      size: 'h_0001_0000    // 64K
   };

   // ----------------------------------------------------------------
   // Boot ROM

   let boot_rom_addr_range = Range {
      base: 'h_7000_0000,
      size: 'h_0000_1000    // 4K
   };

   // ----------------------------------------------------------------
   // DDR memory 0 cached

   let ddr4_0_cached_addr_range = Range {
      base: 'h_8000_0000,
      size: 'h_4_0000_0000    // 16G
   };

   // ----------------------------------------------------------------

   function fn_is_flash_regs_addr = addr_function('h6240_0000, 'h1000);
   function fn_is_uart1_addr = addr_function('h6230_0000, 'h1000);
   function fn_is_i2c_addr = addr_function('h6231_0000, 'h1000);
   function fn_is_spi_addr = addr_function('h6232_0000, 'h1000);
   function fn_is_uart2_addr = addr_function('h6236_0000, 'h1000);
   function fn_is_gpio1_addr = addr_function('h6233_0000, 'h1000);
   function fn_is_gpio2_addr = addr_function('h6237_0000, 'h1000);

   function fn_is_xdma_control = addr_function('h2000_0000, 'h1000_0000);
   function fn_is_xdma_ecam = addr_function('h3000_0000, 'h1000_0000);

   // ----------------------------------------------------------------
   // Memory address predicate
   // Identifies memory addresses in the Fabric.
   // (Caches needs this information to cache these addresses.)

   function Bool fn_is_mem_addr (Fabric_Addr addr);
      return (   inRange(ddr4_0_cached_addr_range, addr)
	      );
   endfunction

   // ----------------------------------------------------------------
   // I/O address predicate
   // Identifies I/O addresses in the Fabric.
   // (Caches needs this information to avoid cacheing these addresses.)

   function Bool fn_is_IO_addr (Fabric_Addr addr, Bool imem_not_dmem);
      return (   inRange(boot_rom_addr_range, addr)
	      || inRange(flash_mem_addr_range, addr)
	      || (   (! imem_not_dmem)
		  && (   inRange(plic_addr_range, addr)
		      || inRange(near_mem_io_addr_range, addr)
		      || inRange(ethernet_0_addr_range, addr)
		      || inRange(dma_0_addr_range, addr)
		      || inRange(uart16550_0_addr_range, addr)
		      || inRange(gpio_0_addr_range, addr)
		      || fn_is_flash_regs_addr (addr)
		      || fn_is_uart1_addr (addr)
		      || fn_is_i2c_addr (addr)
		      || fn_is_spi_addr (addr)
		      || fn_is_uart2_addr (addr)
		      || fn_is_gpio1_addr (addr)
		      || fn_is_gpio2_addr (addr)
		      || fn_is_xdma_control (addr)
		      || fn_is_xdma_ecam (addr)
		      )
		  )
	      );
   endfunction

   // ----------------------------------------------------------------
   // PC, MTVEC and NMIVEC reset values

   Bit #(64) pc_reset_value     = boot_rom_addr_range.base;
   Bit #(64) mtvec_reset_value  = 'h1000;    // TODO
   Bit #(64) nmivec_reset_value = ?;         // TODO

   // ================================================================
   // INTERFACE

   method  Range#(Wd_Addr)  m_plic_addr_range = plic_addr_range;
   method  Range#(Wd_Addr)  m_near_mem_io_addr_range = near_mem_io_addr_range;
   method  Range#(Wd_Addr)  m_flash_mem_addr_range = flash_mem_addr_range;
   method  Range#(Wd_Addr)  m_ethernet_0_addr_range = ethernet_0_addr_range;
   method  Range#(Wd_Addr)  m_dma_0_addr_range = dma_0_addr_range;
   method  Range#(Wd_Addr)  m_uart16550_0_addr_range = uart16550_0_addr_range;
   method  Range#(Wd_Addr)  m_gpio_0_addr_range = gpio_0_addr_range;
   method  Range#(Wd_Addr)  m_boot_rom_addr_range = boot_rom_addr_range;
   method  Range#(Wd_Addr)  m_ddr4_0_cached_addr_range = ddr4_0_cached_addr_range;
   method  Range#(Wd_Addr)  m_mem0_controller_addr_range = ddr4_0_cached_addr_range;

   method  Bool  m_is_mem_addr (Fabric_Addr addr) = fn_is_mem_addr (addr);

   method  Bool  m_is_IO_addr (Fabric_Addr addr, Bool imem_not_dmem) = fn_is_IO_addr (addr, imem_not_dmem);

   method  Bool  m_is_near_mem_IO_addr (Fabric_Addr addr) = inRange (near_mem_io_addr_range, addr);

   method  Bit #(64)  m_pc_reset_value     = pc_reset_value;
   method  Bit #(64)  m_mtvec_reset_value  = mtvec_reset_value;
   method  Bit #(64)  m_nmivec_reset_value = nmivec_reset_value;
endmodule

// ================================================================
// Interrupt request numbers (== index in to vector of
// interrupt-request lines in Core)

typedef  16  N_External_Interrupt_Sources;
Integer  n_external_interrupt_sources = valueOf (N_External_Interrupt_Sources);

Integer irq_num_uart16550_0 = 0;

// ================================================================

endpackage
