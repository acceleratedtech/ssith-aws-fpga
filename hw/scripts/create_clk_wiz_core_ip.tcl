source "board.tcl"

if {[llength $argv] != 1} {
	error "Usage: $argv0 <MHz>"
}

set freq [lindex $argv 0]

create_project -in_memory -part $partname

create_ip -name clk_wiz -vendor xilinx.com -library ip -version 6.0 -dir ip -module_name clk_wiz_core -force

set ip [get_ips clk_wiz_core]

set_property -dict [list \
	CONFIG.PRIM_IN_FREQ {250} \
	CONFIG.PRIM_SOURCE {No_buffer} \
	CONFIG.CLKOUT1_REQUESTED_OUT_FREQ $freq \
	CONFIG.USE_SAFE_CLOCK_STARTUP {true} \
	CONFIG.USE_LOCKED {false} \
	CONFIG.USE_RESET {false} \
] $ip

set_property GENERATE_SYNTH_CHECKPOINT FALSE [get_files [get_property IP_FILE $ip]]

generate_target -force all $ip
