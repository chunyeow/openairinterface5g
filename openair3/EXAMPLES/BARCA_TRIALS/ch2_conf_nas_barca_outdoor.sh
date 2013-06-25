#!/bin/bash

source params.sh
sleep 1
echo ClusterHead address is $CH2_IN6_ADDR
echo Launching AS and RF
./start_openair.sh 8 1

# Installing NASMESH driver
sudo rmmod -f nasmesh
sudo insmod $OPENAIR2_DIR/NAS/DRIVER/MESH/nasmesh.ko nas_IMEI=0x12345678,0x01000000
echo Classification rules for CH2

#CH2 Broadcast (transmission of Router ADV)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c0 -i0 -z0 -x $CH2_IN6_ADDR -y ff02::1 -r 3


#CH2<-> MR2 IP Signaling
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -x $CH2_IN6_ADDR -y $MR2_IN6_ADDR2 -r 20
#CH<-> MR1 MPLS user-plane bearer
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -l $MR2_CH2_LABEL_IN -m $MR2_CH2_LABEL_OUT -r 21
#CH2<-> MR2 MPLS PMIP Signaling
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -l $CH2_MR2_CH1_LABEL_OUT -m $CH2_MR2_CH1_LABEL_IN -r 21
#CH2<-> MR2 MPLS Signaling - For com between MN3 and MN2
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c4 -i0 -z0 -l $MN2_CH2_MR2_MN3 -m $MN3_MR2_CH2_MN2 -r 21


#CH2<-> MR2 Imulticast
echo  EADS Multicast for MR2
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c5 -i0 -z0 -s $CH2_IN_ADDR -t 226.50.10.10 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c6 -i0 -z0 -s $MR2_IN_ADDR -t 226.50.10.10 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c7 -i0 -z0 -s $CH2_IN_ADDR -t 226.50.10.11 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c8 -i0 -z0 -s $MR2_IN_ADDR -t 226.50.10.11 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c9 -i0 -z0 -s $CH2_IN_ADDR -t 226.50.10.12 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c10 -i0 -z0 -s $MR2_IN_ADDR -t 226.50.10.12 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c11 -i0 -z0 -s $CH2_IN_ADDR -t 226.50.10.13 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c12 -i0 -z0 -s $MR2_IN_ADDR -t 226.50.10.13 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c13 -i0 -z0 -s $CH2_IN_ADDR -t 226.50.10.14 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c14 -i0 -z0 -s $MR2_IN_ADDR -t 226.50.10.14 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c15 -i0 -z0 -s $MR2_IN_ADDR -t 226.50.10.15 -r 20
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c16 -i0 -z0 -s $CH2_IN_ADDR -t 226.50.10.15 -r 20




#CH2<-> MR3 (IP Signaling)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c17 -i0 -z0 -x $CH2_IN6_ADDR -y $MR3_IN6_ADDR -r 28
#CH2<-> MR3 (MPLS user-plane bearer)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c18 -i0 -z0 -l $MR3_LABEL_IN -m $MR3_LABEL_OUT -r 29
#CH2<-> MR3 (MPLS for com between MN2 and MN3)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c19 -i0 -z0 -l $MN3_CH2_MR3_MN2 -m $MN2_MR3_CH2_MN3 -r 29


$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c20 -i0 -z0 -s $MR3_IN_ADDR -t 226.30.10.10 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c21 -i0 -z0 -s $CH2_IN_ADDR -t 226.30.10.10 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c22 -i0 -z0 -s $MR3_IN_ADDR -t 226.30.10.11 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c23 -i0 -z0 -s $CH2_IN_ADDR -t 226.30.10.11 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c24 -i0 -z0 -s $MR3_IN_ADDR -t 226.30.10.12 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c25 -i0 -z0 -s $CH2_IN_ADDR -t 226.30.10.12 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c26 -i0 -z0 -s $MR3_IN_ADDR -t 226.30.10.13 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c27 -i0 -z0 -s $CH2_IN_ADDR -t 226.30.10.13 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c28 -i0 -z0 -s $MR3_IN_ADDR -t 226.30.10.14 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c29 -i0 -z0 -s $CH2_IN_ADDR -t 226.30.10.14 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c30 -i0 -z0 -s $MR3_IN_ADDR -t 226.30.10.15 -r 28 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c31 -i0 -z0 -s $CH2_IN_ADDR -t 226.30.10.15 -r 28 



# Bring up openair NASMESH device and set IPv6 address
sudo ifconfig nasmesh0 up
sudo ip addr add 10.0.0.2/24 dev nasmesh0
sudo ip -6 addr add $CH2_IN6_ADDR/64 dev nasmesh0

echo No MPLS debug
echo "0" >/sys/mpls/debug

echo Multicast state launched with config_ch2
xterm -hold -e /usr/bin/perl $REFLECTOR_DIR/reflector_launch.pl $REFLECTOR_DIR $REFLECTOR_DIR/config_ch2  &
