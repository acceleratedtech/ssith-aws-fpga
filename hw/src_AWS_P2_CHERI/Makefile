CONNECTALDIR ?= ../connectal

FLUTE_DIR ?= ../Flute_CHERI

############################################################
## Connectal topgen configuration
############################################################

S2H_INTERFACES = AWSP2_Request:AWSP2.request
H2S_INTERFACES = AWSP2:AWSP2_Response:host.derivedClock,host.derivedReset
MEM_READ_INTERFACES = lAWSP2.readClients
MEM_WRITE_INTERFACES = lAWSP2.writeClients

ifeq ($(BOARD),awsf1)
PIN_TYPE=AWSP2_Pin_IFC
PIN_TYPE_INCLUDE=AWSP2_IFC
AUTOTOP = --interface pins:AWSP2.pins
endif

BSVFILES = ../src_BSV/AWSP2_IFC.bsv

############################################################
## the software source code and compiler flags
############################################################

CPPFILES = ../src_Software/loadelf.cpp ../src_Software/testawsp2.cpp
CONNECTALFLAGS += -lelf


############################################################
## Configuring connectal platform hardware
############################################################

CONNECTALFLAGS += -D SIM_DMA_READ_LATENCY=1
CONNECTALFLAGS += -D SIM_DMA_WRITE_LATENCY=1

CONNECTALFLAGS += -D BYTE_ENABLES_MEM_DATA
CONNECTALFLAGS += -D DataBusWidth=512
CONNECTALFLAGS += -D AWSF1_CL_DEBUG_BRIDGE

CONNECTALFLAGS += -D AWSF1_DDR_A
CONNECTALFLAGS += -D AWSF1_DMA_PCIS

CONNECTALFLAGS += --awsflags="-strategy TIMING"

CONNECTALFLAGS += --awsflags="-clock_recipe_a A1"
CONNECTALFLAGS += --mainclockperiod=4
CONNECTALFLAGS += --pcieclockperiod=4
CONNECTALFLAGS += --awsflags="-clock_recipe_b B5"
CONNECTALFLAGS += --derivedclockperiod=10
CONNECTALFLAGS += -D AWSF1_DERIVED_CLOCK=clk_extra_b1
CONNECTALFLAGS += -D IMPORT_HOST_CLOCKS

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
CONNECTALFLAGS += -DCAP128
CONNECTALFLAGS += -DMEM64 -DRISCV
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
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/Core
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/CPU
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/Debug_Module
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/ISA
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/Near_Mem_IO
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Core/Near_Mem_VM
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

ifeq ($(BOARD),awsf1)

prebuild:: $(CONNECTALDIR)/out/awsf1/ila_connectal_1/ila_connectal_1.xci

$(CONNECTALDIR)/out/awsf1/ila_connectal_1/ila_connectal_1.xci:
	cd awsf1; vivado -mode batch -source $(CONNECTALDIR)/scripts/connectal-synth-ila.tcl
endif

include $(CONNECTALDIR)/Makefile.connectal
