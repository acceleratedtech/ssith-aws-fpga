FLUTE_DIR?=../FluteEnclavesTagging

############################################################
## Configuring MIT SSITH P2
############################################################

CONNECTALFLAGS += -D PUBLIC_ACCESS_POLICY
CONNECTALFLAGS += -DUSE_MEM_FILTER

CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/Tagged
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/TaggingPolicies

############################################################
## copied from src_SSITH_P2 Makefile
############################################################

ARCH ?= RV64ACDFIMSU

CONNECTALFLAGS += --bscflags="-keep-fires -no-warn-action-shadowing -show-range-conflict"
CONNECTALFLAGS += -DRV64
CONNECTALFLAGS += -DSV39
CONNECTALFLAGS += -DISA_PRIV_M  -DISA_PRIV_S  -DISA_PRIV_U 
CONNECTALFLAGS += -DISA_I  -DISA_M  -DISA_A  -DISA_C 
CONNECTALFLAGS += -DISA_F -DISA_D -DISA_FD_DIV 
CONNECTALFLAGS += -DISA_FD -DISA_F_OR_D
CONNECTALFLAGS += -DSHIFT_BARREL   
CONNECTALFLAGS += -DMULT_SYNTH
CONNECTALFLAGS += -DNear_Mem_Caches   
CONNECTALFLAGS += -DFABRIC64   
CONNECTALFLAGS += -DINCLUDE_GDB_CONTROL
ifeq ($(BOARD), verilator)
 CONNECTALFLAGS += -DINCLUDE_TANDEM_VERIF
endif
CONNECTALFLAGS += -DBRVF_TRACE
CONNECTALFLAGS += -DXILINX_BSCAN  -DXILINX_XCVU9P

## this design uses the DMI interface directly
#CONNECTALFLAGS += -DJTAG_TAP

CONNECTALFLAGS += --bsvpath=.
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_SSITH_P2/src_BSV/
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/BSV_Additional_Libs
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/Core
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/CPU
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/Debug_Module
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/ISA
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/Near_Mem_IO
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/Near_Mem_VM
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/PLIC
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/RegFiles
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Testbench/Fabrics/AXI4
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Testbench/Fabrics/AXI4_Lite
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Testbench/SoC

## for TLM.defines
CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/Axi && echo --bsvpath=$(BLUESPECDIR)/Libraries/Axi)
CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/Axi4 && echo --bsvpath=$(BLUESPECDIR)/Libraries/Axi4)
CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/Bus && echo --bsvpath=$(BLUESPECDIR)/Libraries/Bus)
CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/TLM3 && echo --bsvpath=$(BLUESPECDIR)/Libraries/TLM3)
CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/AMBA_TLM3/Axi && echo --bsvpath=$(BLUESPECDIR)/Libraries/AMBA_TLM3/Axi)
CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/AMBA_TLM3/Axi4 && echo --bsvpath=$(BLUESPECDIR)/Libraries/AMBA_TLM3/Axi4)
CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/AMBA_TLM3/TLM3 && echo --bsvpath=$(BLUESPECDIR)/Libraries/AMBA_TLM3/TLM3)
CONNECTALFLAGS += --bsvpath=$(BLUESPECDIR)/Libraries/Bus
CONNECTALFLAGS += --bsvpath=$(BLUESPECDIR)/Libraries/AMBA_TLM3/Axi
CONNECTALFLAGS += --bsvpath=$(BLUESPECDIR)/Libraries/AMBA_TLM3/Axi4
CONNECTALFLAGS += --bsvpath=$(BLUESPECDIR)/Libraries/AMBA_TLM3/TLM3

include ../scripts/Include_Common.mk
