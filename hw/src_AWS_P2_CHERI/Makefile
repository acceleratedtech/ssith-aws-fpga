FLUTE_DIR ?= ../Flute_CHERI

############################################################
## copied from src_SSITH_P2 Makefile
############################################################

ARCH ?= RV64ACDFIMSU

BSCFLAGS += \
	-keep-fires -aggressive-conditions -no-warn-action-shadowing -no-show-timestamps \
	-suppress-warnings G0020    \
	+RTS -K128M -RTS  -show-range-conflict \
	-unspecified-to X -opt-undetermined-vals

CONNECTALFLAGS += --bscflags="$(BSCFLAGS)"
CONNECTALFLAGS += -DRV64
CONNECTALFLAGS += -DSV39
CONNECTALFLAGS += -DRISCV
CONNECTALFLAGS += -DCapWidth=128 -DCheriBusBytes=8
CONNECTALFLAGS += -DCheriMasterIDWidth=1 -DCheriTransactionIDWidth=5
CONNECTALFLAGS += -DISA_CHERI
CONNECTALFLAGS += -DISA_PRIV_M  -DISA_PRIV_S  -DISA_PRIV_U 
CONNECTALFLAGS += -DISA_I  -DISA_M  -DISA_A  -DISA_C 
CONNECTALFLAGS += -DISA_F -DISA_D -DINCLUDE_FDIV -DINCLUDE_FSQRT
CONNECTALFLAGS += -DSHIFT_BARREL   
CONNECTALFLAGS += -DMULT_SYNTH   
CONNECTALFLAGS += -DNear_Mem_Caches   
CONNECTALFLAGS += -DFABRIC64   
CONNECTALFLAGS += -DINCLUDE_GDB_CONTROL
CONNECTALFLAGS += -DBRVF_TRACE
CONNECTALFLAGS += -DXILINX_BSCAN  -DXILINX_XCVU9P
CONNECTALFLAGS += -DINCLUDE_DMEM_SLAVE
CONNECTALFLAGS += -DVERILATOR_PROJECT_ARGS=--trace

CONNECTALFLAGS += -DHAVE_BLUESTUFF_AXI
CONNECTALFLAGS += -DHAVE_BLUESTUFF_ROUTABLE

## this design uses the DMI interface directly
#CONNECTALFLAGS += -DJTAG_TAP

CONNECTALFLAGS += --bsvpath=../src_BSV
## CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_SSITH_P2/src_BSV/
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/BSV_Additional_Libs
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/Cache_Config
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/Core
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/CPU
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/Debug_Module
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/ISA
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/Near_Mem_IO
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/Near_Mem_VM_WT_L1
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/PLIC
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/RegFiles

# AXI_DIRS
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/libs/BlueStuff/AXI
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/libs/BlueStuff/BlueBasics
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/libs/BlueStuff
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/libs/cheri-cap-lib

## CHERI_DIRS
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/libs/TagController/TagController
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/libs/TagController/TagController/CacheCore
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/libs/BlueStuff/BlueUtils

## for TLM.defines
CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/Axi && echo --bsvpath=$(BLUESPECDIR)/Libraries/Axi)
CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/Axi4 && echo --bsvpath=$(BLUESPECDIR)/Libraries/Axi4)
CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/Bus && echo --bsvpath=$(BLUESPECDIR)/Libraries/Bus)

############################################################
# Generate Bluespec CHERI tag controller source file
############################################################

prebuild::
	$(MAKE) -C $(FLUTE_DIR)/src_SSITH_P2 src_BSV/TagTableStructure.bsv
	cp -uv $(FLUTE_DIR)/src_SSITH_P2/src_BSV/TagTableStructure.bsv .

include ../scripts/Include_Common.mk
