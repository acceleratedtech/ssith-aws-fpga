source "board.tcl"

if {[llength $argv] != 1} {
	error "Usage: $argv0 <ip>"
}

set ip_name [lindex $argv 0]

create_project -in_memory -part $partname

read_ip "ip/$ip_name/$ip_name.xci"

set ip [get_ips $ip_name]

upgrade_ip -quiet $ip

set_property GENERATE_SYNTH_CHECKPOINT FALSE [get_files [get_property IP_FILE $ip]]

generate_target -force all $ip
