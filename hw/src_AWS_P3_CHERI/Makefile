TOOOBA_DIR ?= ../Toooba_CHERI

############################################################
## copied from src_SSITH_P3 and Include_RISCY_Config.mk
############################################################

ARCH ?= RV64ACDFIMSUxCHERI

# core size
CORE_SIZE ?= SMALL
# cache size
CACHE_SIZE ?= LARGE
# default 1 core
CORE_NUM ?= 1

ifeq (,$(filter $(CORE_SIZE),TINY SMALL BOOM MEDIUM SMALL_WIDE LARGE LARGE_WIDE))
$(error unsupported CORE_SIZE)
endif

ifeq (,$(filter $(CACHE_SIZE),TEST SMALL LARGE MC_1MB MC_2MB))
$(error unsupported CACHE_SIZE)
endif

XILINX_FP_FMA_LATENCY = 3
XILINX_INT_MUL_LATENCY = 2

BSCFLAGS += \
	-keep-fires -aggressive-conditions \
	-no-warn-action-shadowing \
	-suppress-warnings G0020    \
	+RTS -K128M -RTS  -show-range-conflict \
	-unspecified-to X -opt-undetermined-vals \
	-steps-max-intervals 10000000 \
	-steps-warn-interval 1000000

CONNECTALFLAGS += --bscflags="$(BSCFLAGS)"

CONNECTALFLAGS += -DCORE_$(CORE_SIZE)
CONNECTALFLAGS += -DNUM_CORES=$(CORE_NUM)
CONNECTALFLAGS += -DCACHE_$(CACHE_SIZE)
CONNECTALFLAGS += -DXILINX_FP_FMA_LATENCY=$(XILINX_FP_FMA_LATENCY)
CONNECTALFLAGS += -DXILINX_INT_MUL_LATENCY=$(XILINX_INT_MUL_LATENCY)
CONNECTALFLAGS += -DUSE_BSV_BRAM_SYNC_FIFO

CONNECTALFLAGS += -DRV64
CONNECTALFLAGS += -DISA_PRIV_M
CONNECTALFLAGS += -DISA_PRIV_S
CONNECTALFLAGS += -DISA_PRIV_U
CONNECTALFLAGS += -DSV39
CONNECTALFLAGS += -DISA_I
CONNECTALFLAGS += -DISA_M
CONNECTALFLAGS += -DISA_A
CONNECTALFLAGS += -DISA_F
CONNECTALFLAGS += -DISA_D
CONNECTALFLAGS += -DISA_FD_DIV
CONNECTALFLAGS += -DISA_C
CONNECTALFLAGS += -DCheriBusBytes=8
CONNECTALFLAGS += -DCheriMasterIDWidth=1
CONNECTALFLAGS += -DCheriTransactionIDWidth=5
CONNECTALFLAGS += -DPERFORMANCE_MONITORING
CONNECTALFLAGS += -DSHIFT_BARREL
CONNECTALFLAGS += -DMULT_SERIAL
CONNECTALFLAGS += -DNear_Mem_Caches
CONNECTALFLAGS += -DFABRIC64
CONNECTALFLAGS += -DCAP128
CONNECTALFLAGS += -DMEM64
CONNECTALFLAGS += -DRISCV
CONNECTALFLAGS += -DINCLUDE_GDB_CONTROL
CONNECTALFLAGS += -DBRVF_TRACE
CONNECTALFLAGS += -DXILINX_BSCAN
CONNECTALFLAGS += -DINCLUDE_DMEM_SLAVE
CONNECTALFLAGS += -DVERILATOR_PROJECT_ARGS=--trace

CONNECTALFLAGS += -DHAVE_BLUESTUFF_AXI
CONNECTALFLAGS += -DHAVE_BLUESTUFF_ROUTABLE

# Divide 100 MHz clock by 4 for 25 MHz
CONNECTALFLAGS += -DCORE_CLOCK_DIVISOR=4

## this design uses the DMI interface directly
#CONNECTALFLAGS += -DJTAG_TAP

CONNECTALFLAGS += --bsvpath=../src_BSV

# RISCY_DIRS
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/src_Core/RISCY_OOO/procs/RV64G_OOO
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/src_Core/RISCY_OOO/procs/lib
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/src_Core/RISCY_OOO/coherence/src
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/src_Core/RISCY_OOO/fpgautils/lib

# CONNECTAL_DIRS
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/src_Core/RISCY_OOO/connectal/bsv
## only present in Toooba for ConnectalProjectConfig.bsv, which we provide
#CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/src_Core/RISCY_OOO/connectal/tests/spi
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/src_Core/RISCY_OOO/connectal/lib/bsv

# CHERI_DIRS
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/libs/cheri-cap-lib

# CORE_DIRS
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/src_Core/BSV_Additional_Libs
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/src_Core/Core
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/src_Core/CPU
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/src_Core/Debug_Module
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/src_Core/ISA
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/src_Core/PLIC

# BLUESTUFF_DIRS
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/libs/BlueStuff/AXI
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/libs/BlueStuff/BlueBasics
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/libs/BlueStuff

# TAGCONTROLLEr_DIRS
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/libs/TagController/TagController
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/libs/TagController/TagController/CacheCore
CONNECTALFLAGS += --bsvpath=$(TOOOBA_DIR)/libs/BlueStuff/BlueUtils

## for TLM.defines
CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/Axi && echo --bsvpath=$(BLUESPECDIR)/Libraries/Axi)
CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/Axi4 && echo --bsvpath=$(BLUESPECDIR)/Libraries/Axi4)
CONNECTALFLAGS += $(shell test -d $(BLUESPECDIR)/Libraries/Bus && echo --bsvpath=$(BLUESPECDIR)/Libraries/Bus)

CORE_FREQ_MHZ ?= 25

############################################################
# Generate Bluespec CHERI tag controller source file
############################################################

CAPSIZE = 128
TAGS_STRUCT = 0 64
TAGS_ALIGN = 32
TagTableStructure.bsv: $(TOOOBA_DIR)/libs/TagController/tagsparams.py
	$^ -v -c $(CAPSIZE) -s $(TAGS_STRUCT:"%"=%) -a $(TAGS_ALIGN) --data-store-base-addr 0x80000000 -b $@ 0x3f7e00000 0x480000000

prebuild:: TagTableStructure.bsv

############################################################
# Additional Verilog and IP files needed by Toooba
############################################################

PowerOnReset.bsv: $(TOOOBA_DIR)/src_SSITH_P3/src_BSV/PowerOnReset.bsv
	cp $^ $@

prebuild:: PowerOnReset.bsv

ifeq ($(BOARD),awsf1)

awsf1/verilog:
	mkdir -p $@

define P3_VERILOG_LIB

prebuild:: awsf1/verilog/$(1).v

awsf1/verilog/$(1).v: $(TOOOBA_DIR)/src_SSITH_P3/xilinx_ip/hdl/$(1).v | awsf1/verilog
	cp $$^ $$@

endef

define LOCAL_VERILOG_LIB

prebuild:: awsf1/verilog/$(1).v

awsf1/verilog/$(1).v: $(1).v | awsf1/verilog
	cp $$^ $$@

endef

P3_VERILOG_LIBS = \
	reset_guard \
	ASSIGN1 \
	RegUNInit

$(foreach LIB,$(P3_VERILOG_LIBS),$(eval $(call P3_VERILOG_LIB,$(LIB))))

READ_EXTRA_FILES_FRAGS += read_extra_files.tcl

define IMPORT_IP

prebuild:: awsf1/ip/$(1)/.stamp

awsf1/ip/$(1):
	mkdir -p $$@

awsf1/ip/$(1)/$(1).xci awsf1/ip/$(1)/$(1).xml: awsf1/ip/$(1)/$(1).%: $(TOOOBA_DIR)/src_SSITH_P3/xilinx_ip/src/$(1)/$(1).% | awsf1/ip/$(1)
	cp $$^ $$@

awsf1/ip/$(1)/.stamp: awsf1/ip/$(1)/$(1).xci awsf1/ip/$(1)/$(1).xml import_ip.tcl
	cd awsf1; vivado -mode batch -source ../import_ip.tcl -tclargs $(1)
	@touch $$@

endef

IPS = \
	fp_div \
	fp_fma \
	fp_sqrt \
	int_div_unsigned \
	int_mul_signed \
	int_mul_signed_unsigned \
	int_mul_unsigned

$(foreach IP,$(IPS),$(eval $(call IMPORT_IP,$(IP))))

endif

include ../scripts/Include_Common.mk
