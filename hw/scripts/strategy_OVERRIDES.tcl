set synth_options   "-no_lc -shreg_min_size 10 -control_set_opt_threshold 16 $synth_uram_option"
set synth_directive "AlternateRoutability"

set place_directive "AltSpreadLogic_low"
set place_preHookTcl "$CL_DIR/core_clock.tcl"

set route_directive "AggressiveExplore"
