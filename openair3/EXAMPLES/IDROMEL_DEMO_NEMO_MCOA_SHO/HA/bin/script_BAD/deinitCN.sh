#!/bin/sh
# transfered from HA
PID_TCPDUMP=`ps -A|grep tcpdump|cut -c1-5`
kill -9 $PID_TCPDUMP


