#!/bin/bash
# NEMO-SHO configuration for HA

WORKING_DIRECTORY=$OPENAIR3_DIR"/EXAMPLES/IDROMEL_DEMO_NEMO_MCOA_SHO/HA"
DATE=`date +"%Y_%b_%d.%HH%MMin"`
LOG_FILE=$WORKING_DIRECTORY"/log/ha_sho_"$DATE".log"

echo "enable forwarding:"
#Enable Forwarding
echo "1" >/proc/sys/net/ipv6/conf/all/forwarding
echo "1" >/proc/sys/net/ipv6/conf/eth0/forwarding
echo "1" >/proc/sys/net/ipv6/conf/eth1/forwarding
echo "0" >/proc/sys/net/ipv6/conf/all/accept_ra
echo "0" >/proc/sys/net/ipv6/conf/all/autoconf
echo "0" >/proc/sys/net/ipv6/conf/all/accept_redirects
echo "1" >/proc/sys/net/ipv6/conf/all/proxy_ndp

#MRLLADDR=fe80::20a:5eff:fe45:f14d
killall mip6d tee radvd
ip6tables -F
ip tunnel del ip6tnl1
ip tunnel del ip6tnl2
echo "set ipv6 @ on interfaces:"
#Set IPv6 addresses on two IFs
#ifconfig eth0 inet add 192.168.2.1 netmask 255.255.255.0
ip -6 addr add 2001:4::1/64 scope global dev eth0
ip -6 addr add 2001:1::1/64 scope global dev eth1

echo "adding routes:"
# UMTS-AR
ip -6 route add 2001:2f0:110:6000::/64 via 2001:1::6
# WLAN-AR
ip -6 route add 2001:2f0:110:7000::/64 via 2001:1::7


echo "restarting radvd:"
/etc/init.d/radvd stop 
/etc/init.d/radvd start 
echo "sleep 3 seconds"
sleep 3
echo "starting mip6d:"
mip6d -c $WORKING_DIRECTORY/etc/ha_mcoa_sho.conf 2>&1 | tee $LOG_FILE &


tcpdump -s 256 -i eth0 -v -w $WORKING_DIRECTORY"/log/ha_sho_tcpdump_eth0_"$DATE".log" &
tcpdump -s 256 -i eth1 -v -w $WORKING_DIRECTORY"/log/ha_sho_tcpdump_eth1_"$DATE".log" &

echo "sleep 1 hour (type Ctrl+C to end)"
sleep 3600


 
