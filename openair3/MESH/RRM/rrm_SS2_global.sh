#!/bin/bash
killall emul_int
killall graph
killall rrm

xterm -T COMMAND_CONSOLE -bg '#000000' -fg '#00ff00' -hold -e ./emul_int  &

sleep 2

#xterm -T RRM1 -hold -e 
./rrm -i 7 #2> log.txt

#sleep 1
#./emul_int  &


