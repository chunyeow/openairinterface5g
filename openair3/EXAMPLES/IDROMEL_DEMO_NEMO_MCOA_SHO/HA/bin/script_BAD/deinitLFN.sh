#!/bin/sh
# transfered from HA
PID_TCPDUMP=`ps -A|grep tcpdump|cut -c1-5`
PID_IPERF=`ps -A|grep iperf|cut -c1-5`
kill -9 $PID_TCPDUMP
kill -9 $PID_IPERF


