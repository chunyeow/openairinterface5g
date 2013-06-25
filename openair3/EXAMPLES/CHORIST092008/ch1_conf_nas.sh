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

#CH1<-> MR2 (IP Signaling)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -x $CH1_IN6_ADDR -y $MR2_IN6_ADDR1 -r 20
#CH1<-> MR2 (MPLS user-plane bearer)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -l $MR2_CH1_LABEL_IN -m $MR2_CH1_LABEL_OUT -r 21

#CH1 Broadcast (transmission of Router ADV)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c0 -i0 -z0 -x $CH1_IN6_ADDR -y ff02::1 -r 3

echo Configuring interfaces on CH

# Bring up openair NASMESH device and set IPv6 address
sudo ifconfig nasmesh0 up
sudo ip addr add $CH1_IN_ADDR/24 dev nasmesh0
sudo ip -6 addr add $CH1_IN6_ADDR/64 dev nasmesh0

echo nasmesh0 is $CH1_IN6_ADDR
echo No MPLS debug
sudo sh -c 'echo "0" >/sys/mpls/debug'
sleep 1
echo Launching AS simulator
export OPENAIR2_DIR
xterm -hold -e sh start_openair2_ch1.sh &
sleep 1

