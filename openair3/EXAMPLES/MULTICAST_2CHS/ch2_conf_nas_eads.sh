#!/bin/bash

source params.sh
echo Clusterhead address is $CH2_IN6_ADDR

# Installing NASMESH driver
sudo rmmod -f nasmesh
sudo insmod $OPENAIR2_DIR/NAS/DRIVER/MESH/nasmesh.ko nas_IMEI=0x12345678,0x01000000
echo Classification rules for CH2

#CH2<-> MR2 IP Signaling
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -x $CH2_IN6_ADDR -y $MR2_IN6_ADDR2 -r 20
#CH<-> MR1 MPLS user-plane bearer
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -l $MR2_CH2_LABEL_IN -m $MR2_CH2_LABEL_OUT -r 21
#CH2<-> MR2 MPLS PMIP Signaling
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -l $CH2_MR2_CH1_LABEL_OUT -m $CH2_MR2_CH1_LABEL_IN -r 20

#CH2<-> MR2 Imulticast
echo  EADS Multicast for MR2
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -s CH2_IN_ADDR -t 226.50.10.10 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -s MR2_IN_ADDR -t 226.50.10.10 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -s CH2_IN_ADDR -t 226.50.10.11 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -s MR2_IN_ADDR -t 226.50.10.11 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c4 -i0 -z0 -s CH2_IN_ADDR -t 226.50.10.12 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c4 -i0 -z0 -s MR2_IN_ADDR -t 226.50.10.12 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c5 -i0 -z0 -s CH2_IN_ADDR -t 226.50.10.13 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c5 -i0 -z0 -s MR2_IN_ADDR -t 226.50.10.13 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c6 -i0 -z0 -s CH2_IN_ADDR -t 226.50.10.14 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c6 -i0 -z0 -s MR2_IN_ADDR -t 226.50.10.14 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c7 -i0 -z0 -s MR2_IN_ADDR -t 226.50.10.15 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c7 -i0 -z0 -s CH2_IN_ADDR -t 226.50.10.15 -r 20

#CH2<-> MR3 (IP Signaling)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -x $CH2_IN6_ADDR -y $MR3_IN6_ADDR -r 28
#CH2<-> MR3 (MPLS user-plane bearer)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -l $MR3_LABEL_IN -m $MR3_LABEL_OUT -r 29

#CH2 Broadcast (transmission of Router ADV)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c0 -i0 -z0 -x $CH2_IN6_ADDR -y ff02::1 -r 3

#CH2<-> MR3 multicast EADS (rajout THC)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -s MR3_IN_ADDR -t 226.30.10.10 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -s CH2_IN_ADDR -t 226.30.10.10 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -s MR3_IN_ADDR -t 226.30.10.11 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -s CH2_IN_ADDR -t 226.30.10.11 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c4 -i0 -z0 -s MR3_IN_ADDR -t 226.30.10.12 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c4 -i0 -z0 -s CH2_IN_ADDR -t 226.30.10.12 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c5 -i0 -z0 -s MR3_IN_ADDR -t 226.30.10.13 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c5 -i0 -z0 -s CH2_IN_ADDR -t 226.30.10.13 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c6 -i0 -z0 -s MR3_IN_ADDR -t 226.30.10.14 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c6 -i0 -z0 -s CH2_IN_ADDR -t 226.30.10.14 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c7 -i0 -z0 -s MR3_IN_ADDR -t 226.30.10.15 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c7 -i0 -z0 -s CH2_IN_ADDR -t 226.30.10.15 -r 28 
echo Configuring interfaces on CH2
##Multicast
#ifconfig eth0 192.168.8.4
sudo route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0

# Bring up openair NASMESH device and set IPv6 address
sudo ifconfig nasmesh0 up
sudo ip addr add $CH2_IN_ADDR/24 dev nasmesh0
sudo ip -6 addr add $CH2_IN6_ADDR/64 dev nasmesh0

echo nasmesh0 is $CH2_IN6_ADDR
echo No MPLS debug
sudo sh -c 'echo "0" >/sys/mpls/debug'
sleep 1
#rajout THC pour MULTICAST
echo Multicast state launched with config_ch2
xterm -hold -e /usr/bin/perl $REFLECTOR_DIR/reflector_launch.pl $REFLECTOR_DIR $REFLECTOR_DIR/config_ch2  &



sleep 1
echo Launching AS simulator
export OPENAIR2_DIR
xterm -T CH2 -hold -e sh start_openair2_ch2.sh &
sleep 1


