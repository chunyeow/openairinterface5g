Cluster Head (1) – with NAS_MPLS_PMIP working
CH1_NAS.SH
#!/bin/bash
#Phil, 25 july 08

source params.sh
./ch1_conf_nas.sh 
#MN1MN2
./ch1_mpls_nas.sh $MR1_LABEL_OUT $MR2_IN_ADDR $MR2_LABEL_IN
#MN2MN1
./ch1_mpls_nas.sh $MR2_LABEL_OUT $MR1_IN_ADDR $MR1_LABEL_IN
echo /openair3/pmip6d/pmip6d -c -L $CH1_ADDR
/openair3/pmip6d/pmip6d -c -L $CH1_ADDR
./ch1_del_mpls.sh
CH1_CONF_NAS.SH
#!/bin/bash

source params.sh
echo Clusterhead address is $CH_ADDR

# Installing NASMESH driver
rmmod -f nasmesh
insmod $OPENAIR2_DIR/NAS/DRIVER/MESH/nasmesh.ko nas_IMEI=0x12345678,0x01000000
echo Classification rules for CH

#CH1<-> MR1 IP Signaling
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -x $CH1_ADDR -y $MR1_IN_ADDR -r 12
#CH1<-> MR1 MPLS user-plane bearer
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -l $MR1_LABEL_IN -m $MR1_LABEL_OUT -r 13

#CH1<-> MR2 (IP Signaling)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -x $CH_ADDR -y $MR2_IN_ADDR -r 20
#CH1<-> MR2 (MPLS user-plane bearer)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c2 -i0 -z0 -l $MR2_LABEL_IN -m $MR2_LABEL_OUT -r 21

#CH1 Broadcast (transmission of Router ADV)
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c0 -i0 -z0 -x $CH1_ADDR -y ff02::1 -r 3

echo Configuring interfaces on CH
# Setup IPv4 multicast route for openair emulation
route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0

# Bring up openair NASMESH device and set IPv6 address
ifconfig nasmesh0 up
ip addr add 10.0.0.1/24 dev nasmesh0
ip -6 addr add $CH1_ADDR/64 dev nasmesh0

echo nasmesh0 is $CH1_ADDR
echo No MPLS debug
echo "0" >/sys/mpls/debug
sleep 1
echo Launching AS simulator
export OPENAIR2_DIR
xterm -hold -e sh start_openair2_ch1.sh &
sleep 1


CH1_MPLS_NAS.SH
#!/bin/bash
#Script by Lamia Romdhani
#June 2008

echo Setting MPLS for CH 
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
	echo usage: ch1_mpls_dyntest_nas.sh LABEL_IN IP_NEXT_HOP LABEL_OUT
fi
