#!/bin/bash

source params.sh
echo Clusterhead address is $CH_ADDR

# Installing NASMESH driver
sudo rmmod -f nasmesh
sudo insmod $OPENAIR2_DIR/NAS/DRIVER/MESH/nasmesh.ko nas_IMEI=0x12345678,0x01000000
echo Classification rules for CH

#CH1<-> MR1 IP Signaling
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -x $CH1_IN6_ADDR -y $MR1_IN6_ADDR -r 12
#CH1<-> MR1 MPLS user-plane bearer
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -l $MR1_LABEL_IN -m $MR1_LABEL_OUT -r 13

#CH1<-> MR1 multicast (rajout THC)
echo  EADS Multicast for CH1
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -s CH1_IN_ADDR -t 226.30.10.10 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -s MR1_IN_ADDR -t 226.30.10.10 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -s CH1_IN_ADDR -t 226.30.10.11 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -s MR1_IN_ADDR -t 226.30.10.11 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c4 -i0 -z0 -s CH1_IN_ADDR -t 226.30.10.12 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c4 -i0 -z0 -s MR1_IN_ADDR -t 226.30.10.12 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c5 -i0 -z0 -s CH1_IN_ADDR -t 226.30.10.13 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c5 -i0 -z0 -s MR1_IN_ADDR -t 226.30.10.13 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c6 -i0 -z0 -s CH1_IN_ADDR -t 226.30.10.14 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c6 -i0 -z0 -s MR1_IN_ADDR -t 226.30.10.14 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c7 -i0 -z0 -s MR1_IN_ADDR -t 226.30.10.15 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c7 -i0 -z0 -s CH1_IN_ADDR -t 226.30.10.15 -r 12
#CH1<-> MR2 (IP Signaling)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -x $CH1_IN6_ADDR -y $MR2_IN6_ADDR1 -r 20
#CH1<-> MR2 (MPLS user-plane bearer)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -l $MR2_CH1_LABEL_IN -m $MR2_CH1_LABEL_OUT -r 21
#CH1<-> MR2 (MPLS PMIP signaling bearer)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -l $CH1_MR2_CH2_LABEL_OUT -m $CH1_MR2_CH2_LABEL_IN -r 20

#CH1<-> MR2 multicast EADS (rajout THC)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -s MR2_IN_ADDR -t 226.40.10.10 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -s CH1_IN_ADDR -t 226.40.10.10 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -s MR2_IN_ADDR -t 226.40.10.11 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -s CH1_IN_ADDR -t 226.40.10.11 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c4 -i0 -z0 -s MR2_IN_ADDR -t 226.40.10.12 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c4 -i0 -z0 -s CH1_IN_ADDR -t 226.40.10.12 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c5 -i0 -z0 -s MR2_IN_ADDR -t 226.40.10.13 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c5 -i0 -z0 -s CH1_IN_ADDR -t 226.40.10.13 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c6 -i0 -z0 -s MR2_IN_ADDR -t 226.40.10.14 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c6 -i0 -z0 -s CH1_IN_ADDR -t 226.40.10.14 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c7 -i0 -z0 -s MR2_IN_ADDR -t 226.40.10.15 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c7 -i0 -z0 -s CH1_IN_ADDR -t 226.40.10.15 -r 20 

#CH1 Broadcast (transmission of Router ADV)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c0 -i0 -z0 -x $CH1_IN6_ADDR -y ff02::1 -r 3

echo Configuring interfaces on CH
#Multicast
#sudo ifconfig eth0 192.168.8.2
sudo route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0

# Bring up openair NASMESH device and set IPv6 address
sudo ifconfig nasmesh0 up
sudo ip addr add $CH1_IN_ADDR/24 dev nasmesh0
sudo ip -6 addr add $CH1_IN6_ADDR/64 dev nasmesh0

echo nasmesh0 is $CH1_IN6_ADDR
echo No MPLS debug
sudo sh -c 'echo "0" >/sys/mpls/debug'
sleep 1
#rajout THC pour MULTICAST
echo Multicast state launched with config_ch2
xterm -hold -e /usr/bin/perl $REFLECTOR_DIR/reflector_launch.pl $REFLECTOR_DIR $REFLECTOR_DIR/config_ch2  &



sleep 1
echo Launching AS simulator
export OPENAIR2_DIR
xterm -T CH1 -hold -e sh start_openair2_ch1.sh &
sleep 1

