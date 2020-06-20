set synth_options   "-no_lc -shreg_min_size 10 -control_set_opt_threshold 16 $synth_uram_option"
set synth_directive "AlternateRoutability"

set place_directive "AltSpreadLogic_low"

set route_directive "AggressiveExplore"
