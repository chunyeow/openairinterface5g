#!/bin/bash
# Author: Lamia Romdhani, Raymond Knopp

source params.sh
export OPENAIR2_DIR
#echo "0" >/proc/sys/net/ipv6/conf/all/forwarding

echo Launching AS and RF
./start_openair.sh 1 9

#Sleep to allow L2 modules to load
sleep 1
echo Installing NASMESH Driver
sudo rmmod -f nasmesh
sudo insmod $OPENAIR2_DIR/NAS/DRIVER/MESH/nasmesh.ko nas_IMEI=0x12345678,0x03000000

echo Classifcation rule for DTCH-Broadcast - reception of Router ADV
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c0 -i0 -z0 -x ff02::1 -y ff02::1 -r 3

echo Configuring interfaces on mr2
sudo route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0

sudo ifconfig nasmesh0 up
sudo ifconfig nasmesh0 10.0.1.3
sudo ip -6 addr add 2001:10:0:1:7856:3412:0:3/64 dev nasmesh0


#echo "Waiting for Router ADV from CH (DTCH Broadcast test)"
#IPv6ADR=`ip addr show dev nasmesh0 | grep -e inet6 | egrep -v fe80 | cut -d " " -f6 | cut -d "/" -f1`
#while [ -z $IPv6ADR ] ; do 
#  sleep 1 
#  IPv6ADR=`ip addr show dev nasmesh0 | grep -e inet6 | egrep -v fe80 | cut -d " " -f6 | cut -d "/" -f1`
#done
#echo Got Router ADV : IPv6 address is $IPv6ADR

echo Classification rules for MR1 - Default DTCH UL for L3 signaling
#$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -x $IPv6ADR -y $CH_ADDR -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -x 2001:10:0:1:7856:3412:0:3 -y $CH_ADDR -r 4
echo Classification rules for MR1 - MPLS User-plane Bearer
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -l $MR2_LABEL_OUT -m $MR1_LABEL_IN -r 5

echo eth0 is $MR2_EG_ADDR

#echo Starting routing ...
#echo No MPLS debug
#echo "1" >/sys/mpls/debug


