#!/bin/bash
killall emul_int
killall rrm

xterm -T CMM -hold -e ./emul_int  &
sleep 2
./rrm -i 6  #2> log.txt
#sleep 1
#./emul_int

