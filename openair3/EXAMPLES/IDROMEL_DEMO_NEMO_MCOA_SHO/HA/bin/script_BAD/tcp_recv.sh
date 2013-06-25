#!/bin/sh
# transfered from HA
BASELOG=logLFN.log
ntpdate -b 2001:4::1 > /dev/null
iperf -V -s -i 1 >$BASELOG
exit 0




