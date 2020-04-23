CONNECTALDIR?= ../connectal

FLUTE_DIR?=../Flute_CHERI

############################################################
## Connectal topgen configuration 
############################################################

S2H_INTERFACES = AWSP2_Request:AWSP2.request
H2S_INTERFACES = AWSP2:AWSP2_Response
MEM_READ_INTERFACES = lAWSP2.readClients
MEM_WRITE_INTERFACES = lAWSP2.writeClients

ifeq ($(BOARD),awsf1)
PIN_TYPE=AWSP2_Pin_IFC
PIN_TYPE_INCLUDE=AWSP2_IFC
AUTOTOP = --interface pins:AWSP2.pins
endif

BSVFILES = AWSP2_IFC.bsv

############################################################
## the software source code and compiler flags
############################################################

CPPFILES = ../src_AWS_P2/loadelf.cpp ../src_AWS_P2/testawsp2.cpp
CONNECTALFLAGS += -lelf


############################################################
## Configuring CTSRD-CHERI Flute
############################################################

CONNECTALFLAGS += -D ISA_CHERI
CONNECTALFLAGS += -D USECAP
CONNECTALFLAGS += -D CAP128
CONNECTALFLAGS += -D MEM64
CONNECTALFLAGS += -D RISCV

# CHERI paramters
CAPSIZE = 128
TAGS_STRUCT = 0 64
TAGS_ALIGN = 32

CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Verifier
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/libs/cheri-cap-lib
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/libs/TagController/TagController
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/libs/TagController/TagController/CacheCore
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/libs/BlueStuff/BlueUtils

CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/libs/BlueStuff/AXI
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/libs/BlueStuff/BlueBasics
CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/libs/BlueStuff

# ================================================================
# Generate Bluespec CHERI tag controller source file

prebuild::
	(cd $(FLUTE_DIR)/src_SSITH_P2; make tagsparams)

############################################################
## Configuring connectal platform hardware
############################################################

CONNECTALFLAGS += -D SIM_DMA_READ_LATENCY=1
CONNECTALFLAGS += -D SIM_DMA_WRITE_LATENCY=1

CONNECTALFLAGS += -D BYTE_ENABLES_MEM_DATA
CONNECTALFLAGS += -D DataBusWidth=512
CONNECTALFLAGS += -D AWSF1_CL_DEBUG_BRIDGE

CONNECTALFLAGS += -D AWSF1_DDR_A

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
CONNECTALFLAGS += -DMULT_SERIAL   
CONNECTALFLAGS += -DNear_Mem_Caches   
CONNECTALFLAGS += -DFABRIC64   
CONNECTALFLAGS += -DINCLUDE_GDB_CONTROL
# CONNECTALFLAGS += -DINCLUDE_TANDEM_VERIF
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

# CONNECTALFLAGS += --bsvpath=$(FLUTE_DIR)/src_Testbench/Fabrics/AXI4

## for TLM.defines
CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/Axi && echo --bsvpath=$(BLUESPECDIR)/Libraries/Axi)
CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/Axi4 && echo --bsvpath=$(BLUESPECDIR)/Libraries/Axi4)
CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/Bus && echo --bsvpath=$(BLUESPECDIR)/Libraries/Bus)
# CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/TLM3 && echo --bsvpath=$(BLUESPECDIR)/Libraries/TLM3)
# CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/AMBA_TLM3/Axi && echo --bsvpath=$(BLUESPECDIR)/Libraries/AMBA_TLM3/Axi)
# CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/AMBA_TLM3/Axi4 && echo --bsvpath=$(BLUESPECDIR)/Libraries/AMBA_TLM3/Axi4)
# CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/AMBA_TLM3/TLM3 && echo --bsvpath=$(BLUESPECDIR)/Libraries/AMBA_TLM3/TLM3)

include $(CONNECTALDIR)/Makefile.connectal
