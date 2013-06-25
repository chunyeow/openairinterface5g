#!/bin/bash

/etc/init.d/radvd stop

sysctl -w net.ipv6.conf.all.accept_ra=0
sysctl -w net.ipv6.conf.all.forwarding=1
sysctl -w net.ipv6.conf.eth0.accept_ra=0
sysctl -w net.ipv6.conf.eth0.forwarding=1
sysctl -w net.ipv6.conf.eth1.accept_ra=0
sysctl -w net.ipv6.conf.eth1.forwarding=1

ifconfig eth0 down
ifconfig eth0 up

ifconfig eth1 down
ifconfig eth1 up

#ifconfig eth0 inet6 add 2001:1::6/64
#ifconfig eth1 inet6 add 2001:2f0:110:6000::1/64

route -A inet6 add 2001:4::/32 gw 2001:1::1
route -A inet6 add 2001:2f0:110:7000::/64 gw 2001:1::7

cp -f ../etc/radvd.conf /etc

#/etc/init.d/radvd stop
#/etc/init.d/radvd start

tc qdisc add dev eth0 root netem loss 0%
tc qdisc add dev eth1 root netem loss 0%

cd $OPENAIR2_DIR/EXAMPLES/ETH_EMUL_2CHS_1NODE && ./start_ch1_user
