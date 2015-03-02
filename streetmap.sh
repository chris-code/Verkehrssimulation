#!/bin/bash

binary='bin/roundabout'
plot_script="bin/street_map_plot_densities.txt"

if [ -x "$binary" ]; then
	echo "Launching $binary"
	$($binary > /dev/null)
	if [ -f "$plot_script" ]; then
		echo "Running plot script $plot_script"
		$(gnuplot $plot_script)
	else
		echo "$plot_script not found!"
	fi
else
	echo "$binary not found or not executable!"
	exit 1
fi
