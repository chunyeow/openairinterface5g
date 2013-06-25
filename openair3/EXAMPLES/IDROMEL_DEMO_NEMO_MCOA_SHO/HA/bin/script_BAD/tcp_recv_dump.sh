#!/bin/sh
# transfered from HA
BASELOG=logLFNtcpdump.log
tcpdump -i eth0 -w $BASELOG -s 1500 > /dev/null
exit 0



