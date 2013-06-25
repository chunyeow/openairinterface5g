#!/bin/bash


source params.sh
sleep 1

export OPENAIR1_DIR
export OPENAIR2_DIR
#echo "0" >/proc/sys/net/ipv6/conf/all/forwarding

echo Launching AS and RF
./start_openair.sh 6 8


echo Installing NASMESH Driver
sudo rmmod -f nasmesh
sudo insmod $OPENAIR2_DIR/NAS/DRIVER/MESH/nasmesh.ko nas_IMEI=0x12345678,0x02000000


sudo ifconfig eth0 10.0.1.3
sudo ifconfig nasmesh0 up
sudo ifconfig nasmesh0 10.0.0.3
sudo ip -6 addr add $MR1_IN6_ADDR/64 dev nasmesh0


echo Classification rules for MR1 - Default DTCH UL for L3 signaling
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c0 -i0 -z0 -x $MR1_IN6_ADDR -y $CH1_IN6_ADDR -r 4
echo Classification rules for MR1 - MPLS User-plane Bearer
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -l $MN1_MR1_CH1_MN3 -m $MN3_CH1_MR1_MN1 -r 5

$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -l $MR1_LABEL_OUT -m $MR1_LABEL_IN -r 5

$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -s $CH1_IN_ADDR -t 226.30.10.10 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c4 -i0 -z0 -s $MR1_IN_ADDR -t 226.30.10.10 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c5 -i0 -z0 -s $CH1_IN_ADDR -t 226.30.10.11 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c6 -i0 -z0 -s $MR1_IN_ADDR -t 226.30.10.11 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c7 -i0 -z0 -s $CH1_IN_ADDR -t 226.30.10.12 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c8 -i0 -z0 -s $MR1_IN_ADDR -t 226.30.10.12 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c9 -i0 -z0 -s $CH1_IN_ADDR -t 226.30.10.13 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c10 -i0 -z0 -s $MR1_IN_ADDR -t 226.30.10.13 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c11 -i0 -z0 -s $CH1_IN_ADDR -t 226.30.10.14 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c12 -i0 -z0 -s $MR1_IN_ADDR -t 226.30.10.14 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c13 -i0 -z0 -s $MR1_IN_ADDR -t 226.30.10.15 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c14 -i0 -z0 -s $CH1_IN_ADDR -t 226.30.10.15 -r 4


echo Classifcation rule for DTCH-Broadcast - reception of Router ADV
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c15 -i0 -z0 -x ff02::1 -y ff02::1 -r 3

echo Multicast state launched with config_mr1
xterm -hold -e /usr/bin/perl $REFLECTOR_DIR/reflector_launch.pl $REFLECTOR_DIR $REFLECTOR_DIR/config_mr1  &






