source params.sh
export OPENAIR2_DIR
#echo "0" >/proc/sys/net/ipv6/conf/all/forwarding
echo Installing NASMESH Driver
sudo rmmod -f nasmesh
sudo insmod $OPENAIR2_DIR/NAS/DRIVER/MESH/nasmesh.ko nas_IMEI=0x12345678,0x03000000
echo Classifcation rule for DTCH-Broadcast -reception of Router ADV
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c0 -i0 -z0 -x ff02::1 -y ff02::1 -r 3

#Multicast
#ifconfig eth1 192.168.8.1
sudo route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0
echo Configuring interfaces on mr1
#sudo ifconfig nasmesh0 up
#sudo ifconfig nasmesh0 up
sudo ip link set nasmesh0 up
sudo ip addr add $MR1_IN_ADDR/32 dev nasmesh0 
sudo ip -6 addr add $MR1_IN6_ADDR/64 dev nasmesh0
sudo ip addr add $MR1_EG_ADDR/32 dev $ETH_MR1
sudo ip -6 addr add $MR1_EG6_ADDR/64 dev $ETH_MR1



echo Classification rules for MR1 - Default DTCH UL for L3 signaling
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -x $MR1_IN6_ADDR -y $CH1_IN6_ADDR -r 4
echo Classification rules for MR1 - MPLS User-plane Bearer
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -l $MR1_LABEL_OUT -m $MR1_LABEL_IN -r 5

#MR1<-> CH1 multicast (rajout THC)
echo  EADS Multicast for MR1
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -s CH1_IN_ADDR -t 226.30.10.10 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -s MR1_IN_ADDR -t 226.30.10.10 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -s CH1_IN_ADDR -t 226.30.10.11 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c3 -i0 -z0 -s MR1_IN_ADDR -t 226.30.10.11 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c4 -i0 -z0 -s CH1_IN_ADDR -t 226.30.10.12 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c4 -i0 -z0 -s MR1_IN_ADDR -t 226.30.10.12 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c5 -i0 -z0 -s CH1_IN_ADDR -t 226.30.10.13 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c5 -i0 -z0 -s MR1_IN_ADDR -t 226.30.10.13 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c6 -i0 -z0 -s CH1_IN_ADDR -t 226.30.10.14 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c6 -i0 -z0 -s MR1_IN_ADDR -t 226.30.10.14 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c7 -i0 -z0 -s MR1_IN_ADDR -t 226.30.10.15 -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c7 -i0 -z0 -s CH1_IN_ADDR -t 226.30.10.15 -r 4




sleep 1
#rajout THC pour MULTICAST
echo Multicast state launched with config_ch2
xterm -hold -e /usr/bin/perl $REFLECTOR_DIR/reflector_launch.pl $REFLECTOR_DIR $REFLECTOR_DIR/config_mr1  &

sleep 1

echo Launching AS simulator
xterm -T MR1 -hold -e sh start_openair2_mr1.sh &

