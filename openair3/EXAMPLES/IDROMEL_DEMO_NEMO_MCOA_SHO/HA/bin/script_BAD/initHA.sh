#!/bin/sh
# transfered from HA
OUTGOINGIF=eth1
LOGFILE=logHAtcpdump.log
tcpdump -i $OUTGOINGIF -w $LOGFILE -s 1500 &

