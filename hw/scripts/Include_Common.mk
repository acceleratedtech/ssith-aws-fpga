SCRIPTSDIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

CONNECTALDIR ?= ../connectal

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
CONNECTALFLAGS += --derivedclockperiod=4

ifeq ($(BOARD),awsf1)

prebuild:: $(CONNECTALDIR)/out/awsf1/ila_connectal_1/ila_connectal_1.xci

$(CONNECTALDIR)/out/awsf1/ila_connectal_1/ila_connectal_1.xci:
	cd awsf1; vivado -mode batch -source $(CONNECTALDIR)/scripts/connectal-synth-ila.tcl

CORE_FREQ_MHZ ?= 100

awsf1/ip:
	mkdir -p $@

prebuild:: awsf1/ip/clk_wiz_core/clk_wiz_core.xci

awsf1/ip/clk_wiz_core/clk_wiz_core.xci: $(SCRIPTSDIR)/create_clk_wiz_core_ip.tcl | awsf1/ip
	cd awsf1; vivado -mode batch -source $(SCRIPTSDIR)/create_clk_wiz_core_ip.tcl -tclargs $(CORE_FREQ_MHZ)

READ_EXTRA_FILES_FRAGS += $(SCRIPTSDIR)/read_extra_files.tcl

prebuild:: awsf1/read_extra_files.tcl

awsf1/read_extra_files.tcl: $(READ_EXTRA_FILES_FRAGS)
	cat $^ > $@

prebuild:: awsf1/core_clock.tcl

awsf1/core_clock.tcl: $(SCRIPTSDIR)/core_clock.tcl
	cp $^ $@

prebuild:: awsf1/core_clock.xdc

awsf1/core_clock.xdc: $(SCRIPTSDIR)/core_clock.xdc
	cp $^ $@

prebuild:: awsf1/strategy_OVERRIDES.tcl

awsf1/strategy_OVERRIDES.tcl: $(SCRIPTSDIR)/strategy_OVERRIDES.tcl
	cp $^ $@

endif

include $(CONNECTALDIR)/Makefile.connectal
