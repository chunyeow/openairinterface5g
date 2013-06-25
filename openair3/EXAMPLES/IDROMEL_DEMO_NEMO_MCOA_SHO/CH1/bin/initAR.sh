#!/bin/sh
# transfered from HA
ntpdate -b 2001:1::1
OUTBOUNDIF=eth0
INBOUNDIF=eth1
tc qdisc change dev $INBOUNDIF  root netem loss 0%
tc qdisc change dev $OUTBOUNDIF root netem loss 0%
