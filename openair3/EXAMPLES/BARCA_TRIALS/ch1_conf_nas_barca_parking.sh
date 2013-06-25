#!/bin/bash

source params.sh
sleep 1

echo Clusterhead address is $CH1_IN6_ADDR


echo Launching AS and RF
./start_openair.sh 6 0


# Installing NASMESH driver
sudo rmmod -f nasmesh
sudo insmod $OPENAIR2_DIR/NAS/DRIVER/MESH/nasmesh.ko nas_IMEI=0x12345678,0x01000000
echo Classification rules for CH

#CH1<-> MR1 IP Signaling
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c0 -i0 -z0 -x $CH1_IN6_ADDR -y $MR1_IN6_ADDR -r 12
#CH1<-> MR1 MPLS user-plane bearer
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -l $MR1_LABEL_IN -m $MR1_LABEL_OUT -r 13

## Est ce bon ?
#CH1<->MR1 - For com between MN1 and MN3 - MPLS user-plane bearer
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -l $MN3_CH1_MR1_MN1 -m $MN1_MR1_CH1_MN3 -r 13


#CH1<-> MR1 multicast (rajout THC)
echo  EADS Multicast for CH1
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -s $CH1_IN_ADDR -t 226.30.10.10 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c4 -i0 -z0 -s $MR1_IN_ADDR -t 226.30.10.10 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c5 -i0 -z0 -s $CH1_IN_ADDR -t 226.30.10.11 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c6 -i0 -z0 -s $MR1_IN_ADDR -t 226.30.10.11 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c7 -i0 -z0 -s $CH1_IN_ADDR -t 226.30.10.12 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c8 -i0 -z0 -s $MR1_IN_ADDR -t 226.30.10.12 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c9 -i0 -z0 -s $CH1_IN_ADDR -t 226.30.10.13 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c10 -i0 -z0 -s $MR1_IN_ADDR -t 226.30.10.13 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c11 -i0 -z0 -s $CH1_IN_ADDR -t 226.30.10.14 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c12 -i0 -z0 -s $MR1_IN_ADDR -t 226.30.10.14 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c13 -i0 -z0 -s $MR1_IN_ADDR -t 226.30.10.15 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c14 -i0 -z0 -s $CH1_IN_ADDR -t 226.30.10.15 -r 12



#CH1<-> MR2 (IP Signaling)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c15 -i0 -z0 -x $CH1_IN6_ADDR -y $MR2_IN6_ADDR1 -r 20
#CH1<-> MR2 (MPLS user-plane bearer)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c16 -i0 -z0 -l $MR2_CH1_LABEL_IN -m $MR2_CH1_LABEL_OUT -r 21
#CH1<-> MR2 (MPLS PMIP signaling bearer)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c17 -i0 -z0 -l $CH1_MR2_CH2_LABEL_OUT -m $CH1_MR2_CH2_LABEL_IN -r 21
#CH1<-> MR2 - For com between MN1 and MN3 - (MPLS signaling bearer)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c18 -i0 -z0 -l $MN1_CH1_MR2_MN3 -m $MN3_MR2_CH1_MN1 -r 21


#CH1<-> MR2 multicast EADS (rajout THC)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c19 -i0 -z0 -s $MR2_IN_ADDR -t 226.40.10.10 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c20 -i0 -z0 -s $CH1_IN_ADDR -t 226.40.10.10 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c21 -i0 -z0 -s $MR2_IN_ADDR -t 226.40.10.11 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c22 -i0 -z0 -s $CH1_IN_ADDR -t 226.40.10.11 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c23 -i0 -z0 -s $MR2_IN_ADDR -t 226.40.10.12 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c24 -i0 -z0 -s $CH1_IN_ADDR -t 226.40.10.12 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c25 -i0 -z0 -s $MR2_IN_ADDR -t 226.40.10.13 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c26 -i0 -z0 -s $CH1_IN_ADDR -t 226.40.10.13 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c27 -i0 -z0 -s $MR2_IN_ADDR -t 226.40.10.14 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c28 -i0 -z0 -s $CH1_IN_ADDR -t 226.40.10.14 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c29 -i0 -z0 -s $MR2_IN_ADDR -t 226.40.10.15 -r 20 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c30 -i0 -z0 -s $CH1_IN_ADDR -t 226.40.10.15 -r 20 



#CH Broadcast (transmission of Router ADV)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c31 -i0 -z0 -x $CH1_IN6_ADDR -y ff02::1 -r 3


#CH<-> MR1 Imulticast
#$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -x ff3e:30:2001:660:5502:ffff:e000:3737 -y $MR1_IN_ADDR -r 14
#$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -s 10.0.0.2 -t 224.0.55.55 -r 12
#$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -s 10.0.0.1 -t 224.0.55.55 -r 12
#CH<-> MR2 (IP Signaling)
#$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -x $CH_ADDR -y $MR2_IN_ADDR -r 20
#$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -s 10.0.0.1 -t 224.0.66.66 -r 20
#$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -s 10.0.0.3 -t 224.0.66.66 -r 20
#CH<-> MR2 (MPLS user-plane bearer)


# Bring up openair NASMESH device and set IPv6 address
sudo ifconfig nasmesh0 up
sudo ip addr add 10.0.0.1/24 dev nasmesh0
sudo ip -6 addr add $CH1_IN6_ADDR/64 dev nasmesh0

echo No MPLS debug
echo "0" > /sys/mpls/debug

echo Multicast state launched with config_ch1
xterm -hold -e /usr/bin/perl $REFLECTOR_DIR/reflector_launch.pl $REFLECTOR_DIR $REFLECTOR_DIR/config_ch1  &


