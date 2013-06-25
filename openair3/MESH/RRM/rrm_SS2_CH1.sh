#!/bin/bash
killall emul_int
killall graph
killall rrm

xterm -T COMMAND_CONSOLE -bg '#000000' -fg '#00ff00' -hold -e ./emul_int  &
sleep 2
./rrm -i 4  #2> log.txt
#sleep 1
#./emul_int

