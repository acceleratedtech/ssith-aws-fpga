set coreClockGen [get_cells -hier -filter {NAME=~*/lAWSP2_coreClock}]

set coreClockIn [get_pins -leaf -of $coreClockGen -filter {NAME=~*/clk_in1}]
set coreClockOut [get_pins -leaf -of $coreClockGen -filter {NAME=~*/clk_out1}]

set mainClock [get_clocks -of $coreClockIn]
set coreClock [get_clocks -of $coreClockOut]

set mainClockDriver [get_pins -of [get_nets -segments -of $coreClockIn] -filter {IS_LEAF && DIRECTION==OUT}]

set coreClockSource [get_pins [get_property SOURCE_PINS $coreClock]]

set_property CLOCK_DEDICATED_ROUTE ANY_CMT_COLUMN [get_nets -of $mainClockDriver]

create_generated_clock -name clk_awsp2_core $coreClockSource
set coreClock [get_clocks clk_awsp2_core]

set_clock_group -asynchronous -group $mainClock -group $coreClock
