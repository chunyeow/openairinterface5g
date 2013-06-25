source params.sh
export OPENAIR2_DIR
#echo "0" >/proc/sys/net/ipv6/conf/all/forwarding
echo Installing NASMESH Driver
sudo rmmod -f nasmesh
sudo insmod $OPENAIR2_DIR/NAS/DRIVER/MESH/nasmesh.ko nas_IMEI=0x12345678,0x03000000
echo Classifcation rule for DTCH-Broadcast -reception of Router ADV
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c0 -i0 -z0 -x ff02::1 -y ff02::1 -r 11
echo Configuring interfaces on mr3

#Multicast
#ifconfig eth1 192.168.8.5
sudo route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0
#sudo ifconfig nasmesh0 up
#sudo ifconfig nasmesh0 $MR3_IN_ADDR
sudo ip link set nasmesh0 up
sudo ip addr add $MR3_IN_ADDR/32 dev nasmesh0
sudo ip -6 addr add $MR3_IN6_ADDR/64 dev nasmesh0
sudo ip addr add $MR3_EG_ADDR/32 dev $ETH_MR3
sudo ip -6 addr add $MR3_EG6_ADDR/64 dev $ETH_MR3


echo Classification rules for MR3 - Default DTCH UL for L3 signaling
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -x $MR3_IN6_ADDR -y $CH2_IN6_ADDR -r 12
echo Classification rules for MR3 - MPLS User-plane Bearer
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -l $MR3_LABEL_OUT -m $MR3_LABEL_IN -r 13

#MR3<-> CH2 multicast (rajout THC)
echo  EADS Multicast for MR3
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -s CH2_IN_ADDR -t 226.30.10.10 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -s MR3_IN_ADDR -t 226.30.10.10 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -s CH2_IN_ADDR -t 226.30.10.11 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -s MR3_IN_ADDR -t 226.30.10.11 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c4 -i0 -z0 -s CH2_IN_ADDR -t 226.30.10.12 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c4 -i0 -z0 -s MR3_IN_ADDR -t 226.30.10.12 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c5 -i0 -z0 -s CH2_IN_ADDR -t 226.30.10.13 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c5 -i0 -z0 -s MR3_IN_ADDR -t 226.30.10.13 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c6 -i0 -z0 -s CH2_IN_ADDR -t 226.30.10.14 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c6 -i0 -z0 -s MR3_IN_ADDR -t 226.30.10.14 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c7 -i0 -z0 -s MR3_IN_ADDR -t 226.30.10.15 -r 12
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c7 -i0 -z0 -s CH2_IN_ADDR -t 226.30.10.15 -r 12

#rajout THC pour MULTICAST
echo Multicast state launched with config_ch2
xterm -hold -e /usr/bin/perl $REFLECTOR_DIR/reflector_launch.pl $REFLECTOR_DIR $REFLECTOR_DIR/config_mr3  &

sleep 1


echo Launching AS simulator
sudo xterm -T MR3 -hold -e sh start_openair2_mr3.sh &
