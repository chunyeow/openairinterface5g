#!/bin/bash

source params.sh
sleep 1
export OPENAIR2_DIR
#echo "0" >/proc/sys/net/ipv6/conf/all/forwarding

echo Launching AS and RF
./start_openair.sh 0 9 


echo Installing NASMESH Driver
sudo rmmod -f nasmesh
sudo insmod $OPENAIR2_DIR/NAS/DRIVER/MESH/nasmesh.ko nas_IMEI=0x12345678,0x03000000

echo Classifcation rule for DTCH-Broadcast - reception of Router ADV
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c0 -i0 -z0 -x ff02::1 -y ff02::1 -r 3

sudo ifconfig nasmesh0 up
sudo ifconfig nasmesh0 10.0.0.4
#sudo ifconfig eth0 10.0.3.3
sudo ip -6 addr add $MR2_IN6_ADDR1/64 dev nasmesh0
sudo ip -6 addr add $MR2_IN6_ADDR2/64 dev nasmesh0

# Com between MR2 <-> CH1
echo Classification rules for MR2- Cluster 1 - Default DTCH UL for L3 signaling
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -x $MR2_IN6_ADDR1 -y $CH1_IN6_ADDR -r 4
echo Classification rules for MR2- Cluster 1 - MPLS User-plane Bearer
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -l $MR2_CH1_LABEL_OUT -m $MR2_CH1_LABEL_IN -r 5
echo Classification rules for MR2- Cluster 1 - MPLS PMIP Signaling
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -l $CH1_MR2_CH2_LABEL_IN -m $CH1_MR2_CH2_LABEL_OUT -r 5
echo Classification rules for MR2- Cluster 1 - MPLS for com between MN3 and MN1 
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c4 -i0 -z0 -l $MN3_MR2_CH1_MN1 -m $MN1_CH1_MR2_MN3 -r 5


$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c5 -i0 -z0 -s $CH1_IN_ADDR -t 226.40.10.10 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c6 -i0 -z0 -s $MR2_IN_ADDR -t 226.40.10.10 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c7 -i0 -z0 -s $CH1_IN_ADDR -t 226.40.10.11 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c8 -i0 -z0 -s $MR2_IN_ADDR -t 226.40.10.11 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c9 -i0 -z0 -s $CH1_IN_ADDR -t 226.40.10.12 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c10 -i0 -z0 -s $MR2_IN_ADDR -t 226.40.10.12 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c11 -i0 -z0 -s $CH1_IN_ADDR -t 226.40.10.13 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c12 -i0 -z0 -s $MR2_IN_ADDR -t 226.40.10.13 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c13 -i0 -z0 -s $CH1_IN_ADDR -t 226.40.10.14 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c14 -i0 -z0 -s $MR2_IN_ADDR -t 226.40.10.14 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c15 -i0 -z0 -s $MR2_IN_ADDR -t 226.40.10.15 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c16 -i0 -z0 -s $CH1_IN_ADDR -t 226.40.10.15 -r 4



# Com between MR2 <-> CH2
#echo Classification rules for MR2 -Cluster 2 - Default DTCH UL for L3 signaling
#$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c17 -i0 -z0 -x ff02::1 -y ff02::1 -r 11
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c18 -i0 -z0 -x $MR2_IN6_ADDR2 -y $CH2_IN6_ADDR -r 12
#echo Classification rules for MR2 -Cluster 2 - MPLS User-plane Bearer
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c19 -i0 -z0 -l $MR2_CH2_LABEL_OUT -m $MR2_CH2_LABEL_IN -r 13
#echo Classification rules for MR2- Cluster 2 - MPLS PMIP Signaling
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c20 -i0 -z0 -l $CH2_MR2_CH1_LABEL_IN -m $CH2_MR2_CH1_LABEL_OUT -r 13

$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c21 -i0 -z0 -l $MN3_MR2_CH2_MN2 -m $MN2_CH2_MR2_MN3 -r 13


$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c22 -i0 -z0 -s $CH2_IN_ADDR -t 226.50.10.10 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c23 -i0 -z0 -s $MR2_IN_ADDR -t 226.50.10.10 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c24 -i0 -z0 -s $CH2_IN_ADDR -t 226.50.10.11 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c25 -i0 -z0 -s $MR2_IN_ADDR -t 226.50.10.11 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c26 -i0 -z0 -s $CH2_IN_ADDR -t 226.50.10.12 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c27 -i0 -z0 -s $MR2_IN_ADDR -t 226.50.10.12 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c28 -i0 -z0 -s $CH2_IN_ADDR -t 226.50.10.13 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c29 -i0 -z0 -s $MR2_IN_ADDR -t 226.50.10.13 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c30 -i0 -z0 -s $CH2_IN_ADDR -t 226.50.10.14 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c31 -i0 -z0 -s $MR2_IN_ADDR -t 226.50.10.14 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c32 -i0 -z0 -s $MR2_IN_ADDR -t 226.50.10.15 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c33 -i0 -z0 -s $CH2_IN_ADDR -t 226.50.10.15 -r 12

