Cluster Head (2)– with NAS_MPLS_PMIP working
CH2_NAS.SH
#!/bin/bash
#Phil, 25 july 08

source params.sh
./ch2_conf_nas.sh 
#MN1MN2
./ch2_mpls_nas.sh $MR2_CH2_LABEL_OUT $MR3_IN_ADDR $MR2_LABEL_IN
#MN2MN1
./ch_mpls_nas.sh $MR2_LABEL_OUT $MR2_IN_ADDR $MR2_CH2_LABEL_IN
echo /openair3/pmip6d/pmip6d -c -L $CH2_ADDR
/openair3/pmip6d/pmip6d -c -L $CH2_ADDR
./ch2_del_mpls.sh
CH2_CONF_NAS.SH
#!/bin/bash

source params.sh
echo Clusterhead address is $CH2_ADDR

# Installing NASMESH driver
rmmod -f nasmesh
insmod $OPENAIR2_DIR/NAS/DRIVER/MESH/nasmesh.ko nas_IMEI=0x12345678,0x01000000
echo Classification rules for CH2

#CH2<-> MR2 IP Signaling
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -x $CH2_ADDR -y $MR2_IN_ADDR -r 12
#CH<-> MR1 MPLS user-plane bearer
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -l $MR2_CH2_LABEL_IN -m $MR2_CH2_LABEL_OUT -r 13

#CH2<-> MR3 (IP Signaling)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -x $CH2_ADDR -y $MR3_IN_ADDR -r 20
#CH2<-> MR3 (MPLS user-plane bearer)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -l $MR3_LABEL_IN -m $MR3_LABEL_OUT -r 21

#CH2 Broadcast (transmission of Router ADV)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c0 -i0 -z0 -x $CH2_ADDR -y ff02::1 -r 3

echo Configuring interfaces on CH2
# Setup IPv4 multicast route for openair emulation
route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0

# Bring up openair NASMESH device and set IPv6 address
ifconfig nasmesh0 up
ip addr add 10.0.0.1/24 dev nasmesh0
ip -6 addr add $CH2_ADDR/64 dev nasmesh0

echo nasmesh0 is $CH2_ADDR
echo No MPLS debug
echo "0" >/sys/mpls/debug
sleep 1
echo Launching AS simulator
export OPENAIR2_DIR
xterm -hold -e sh start_openair2_ch2.sh &
sleep 1


CH2_MPLS_NAS.SH
#!/bin/bash
#Script by Lamia Romdhani
#June 2008

echo Setting MPLS for CH2 
modprobe mpls6

echo Number of params = $#
if [ $# -eq 3 ]; then
	LABEL_IN=$1
	IP_NEXT_HOP=$2
	LABEL_OUT=$3

	echo "IP Next Hop is $IP_NEXT_HOP"

	echo 'Starting mpls'
	mpls labelspace set dev nasmesh0 labelspace 0
	mpls ilm add label gen $LABEL_IN labelspace 0 proto ipv6
var=`mpls nhlfe add key 0 instructions push gen $LABEL_OUT nexthop nasmesh0 ipv6     $IP_NEXT_HOP |grep key | cut -c 17-26`
	mpls xc add ilm_label gen $LABEL_IN ilm_labelspace 0 nhlfe_key $var
else
	echo usage: ch_mpls_dyntest_nas.sh LABEL_IN IP_NEXT_HOP LABEL_OUT
fi
