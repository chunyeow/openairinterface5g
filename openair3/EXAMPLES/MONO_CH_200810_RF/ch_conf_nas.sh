#!/bin/bash
# Author: Lamia Romdhani, Raymond Knopp

source params.sh
echo Clusterhead address is $CH_ADDR

sleep 1
echo Launching AS and RF
./start_openair.sh 1 0

sleep 1
# Installing NASMESH driver
sudo rmmod -f nasmesh
sudo insmod $OPENAIR2_DIR/NAS/DRIVER/MESH/nasmesh.ko nas_IMEI=0x12345678,0x01000000
echo Classification rules for CH

#CH<-> MR1 IP Signaling
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -x $CH_ADDR -y $MR1_IN_ADDR -r 12
#CH<-> MR1 MPLS user-plane bearer
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -l $MR1_LABEL_IN -m $MR1_LABEL_OUT -r 13

#CH<-> MR2 (IP Signaling)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -x $CH_ADDR -y $MR2_IN_ADDR -r 20
#CH<-> MR2 (MPLS user-plane bearer)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -l $MR2_LABEL_IN -m $MR2_LABEL_OUT -r 21

#CH Broadcast (transmission of Router ADV)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c0 -i0 -z0 -x $CH_ADDR -y ff02::1 -r 3

echo Configuring interfaces on CH
# Setup IPv4 multicast route for openair emulation
sudo route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0

# Bring up openair NASMESH device and set IPv6 address
sudo ifconfig nasmesh0 up
sudo ip addr add 10.0.0.1/24 dev nasmesh0
sudo ip -6 addr add $CH_ADDR/64 dev nasmesh0

echo nasmesh0 is $CH_ADDR
echo No MPLS debug
echo "0" >/sys/mpls/debug



#/etc/init.d/radvd restart
