#!/bin/bash


source params.sh
sleep 1

export OPENAIR1_DIR
export OPENAIR2_DIR
#echo "0" >/proc/sys/net/ipv6/conf/all/forwarding

echo Launching AS and RF
./start_openair.sh 1 10


echo Installing NASMESH Driver
sudo rmmod -f nasmesh
sudo insmod $OPENAIR2_DIR/NAS/DRIVER/MESH/nasmesh.ko nas_IMEI=0x12345678,0x02000000



sudo ifconfig nasmesh0 up
sudo ifconfig nasmesh0 10.0.0.5
sudo ip -6 addr add $MR3_IN6_ADDR/64 dev nasmesh0

echo Classification rules for MR3 - Default DTCH UL for L3 signaling
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c0 -i0 -z0 -x $MR3_IN6_ADDR -y $CH2_IN6_ADDR -r 12
echo Classification rules for MR3 - MPLS User-plane Bearer
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -l $MN2_MR3_CH2_MN3 -m $MN3_CH2_MR3_MN2 -r 13

$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -l $MR3_LABEL_OUT -m $MR3_LABEL_IN -r 13


echo  EADS Multicast for MR3
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -s $CH2_IN_ADDR -t 226.30.10.10 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c4 -i0 -z0 -s $MR3_IN_ADDR -t 226.30.10.10 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c5 -i0 -z0 -s $CH2_IN_ADDR -t 226.30.10.11 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c6 -i0 -z0 -s $MR3_IN_ADDR -t 226.30.10.11 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c7 -i0 -z0 -s $CH2_IN_ADDR -t 226.30.10.12 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c8 -i0 -z0 -s $MR3_IN_ADDR -t 226.30.10.12 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c9 -i0 -z0 -s $CH2_IN_ADDR -t 226.30.10.13 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c10 -i0 -z0 -s $MR3_IN_ADDR -t 226.30.10.13 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c11 -i0 -z0 -s $CH2_IN_ADDR -t 226.30.10.14 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c12 -i0 -z0 -s $MR3_IN_ADDR -t 226.30.10.14 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c13 -i0 -z0 -s $MR3_IN_ADDR -t 226.30.10.15 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c14 -i0 -z0 -s $CH2_IN_ADDR -t 226.30.10.15 -r 12


echo Classifcation rule for DTCH-Broadcast - reception of Router ADV
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c15 -i0 -z0 -x ff02::1 -y ff02::1 -r 3

