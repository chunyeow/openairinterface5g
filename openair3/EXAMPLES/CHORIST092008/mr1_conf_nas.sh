source params.sh
export OPENAIR2_DIR
#echo "0" >/proc/sys/net/ipv6/conf/all/forwarding
echo Installing NASMESH Driver
sudo rmmod -f nasmesh
sudo insmod $OPENAIR2_DIR/NAS/DRIVER/MESH/nasmesh.ko nas_IMEI=0x12345678,0x03000000
echo Classifcation rule for DTCH-Broadcast -reception of Router ADV
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c0 -i0 -z0 -x ff02::1 -y ff02::1 -r 3

echo Configuring interfaces on mr1
sudo ifconfig nasmesh0 up
sudo ifconfig nasmesh0 $MR1_IN_ADDR
sudo ip -6 addr add $MR1_IN6_ADDR/64 dev nasmesh0
sudo ip -6 addr add $MR1_EG6_ADDR/64 dev eth1

echo Launching AS simulator
xterm -hold -e sh start_openair2_mr1.sh &

echo Classification rules for MR1 - Default DTCH UL for L3 signaling
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -x $MR1_IN6_ADDR -y $CH1_IN6_ADDR -r 4
echo Classification rules for MR1 - MPLS User-plane Bearer
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -l $MR1_LABEL_OUT -m $MR1_LABEL_IN -r 5





