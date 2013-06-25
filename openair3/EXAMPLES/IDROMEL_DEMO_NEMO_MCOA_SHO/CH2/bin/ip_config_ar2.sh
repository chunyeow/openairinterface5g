#!/bin/bash
ifconfig eth0 down
ifconfig eth1 down
ifconfig eth0 up
ifconfig eth1 up
ifconfig eth1 add 2001:1::7/64
ifconfig eth0 add 2001:2f0:110:7000::1/64
ethtool -s eth0 autoneg on
ethtool -s eth0 speed 100
route -A inet6 add 2001:4::/32 gw 2001:1::1
route -A inet6 add 2001:2f0:110:6000::/64 gw 2001:1::6
# radvd service is not installed on this host
radvd -m logfile -C /opt/bin/radvd_ar2.conf

tc qdisc add dev eth0 root netem loss 0%
tc qdisc add dev eth1 root netem loss 0%

