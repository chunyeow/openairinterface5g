Mobile Router 3 – with NAS_MPLS_PMIP working
MR3_NAS.SH
source params.sh
sysctl -w net.ipv6.conf.all.forwarding=1
$OPENAIR3_SCRIPTS_PATH/mr3_del_mpls.sh
$OPENAIR3_SCRIPTS_PATH/mr3_conf_nas.sh 
$OPENAIR3_SCRIPTS_PATH/mr3_mpls_nas.sh

#/etc/init.d/radvd status
#/etc/init.d/radvd start
#radvdump 

sysctl -w net.ipv6.conf.eth0.proxy_ndp=1
sysctl -w net.ipv6.conf.nasmesh0.proxy_ndp=1

ifconfig eth0 promisc
ip -6 addr add $MR3_EG_ADDR/64 dev eth0
$OPENAIR3_PMIP6D_PATH/pmip6d -m -s -L $CH2_ADDR -N $MR3_EG_ADDR -E $MR3_IN_ADDR
$OPENAIR3_SCRIPTS_PATH/mr3_del_mpls.sh
MR3_CONF_NAS.SH
source params.sh
export OPENAIR2_DIR
#echo "0" >/proc/sys/net/ipv6/conf/all/forwarding
echo Installing NASMESH Driver
rmmod -f nasmesh
insmod $OPENAIR2_DIR/NAS/DRIVER/MESH/nasmesh.ko nas_IMEI=0x12345678,0x03000000
echo Classifcation rule for DTCH-Broadcast -reception of Router ADV
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c0 -i0 -z0 -x ff02::1 -y ff02::1 -r 3
echo Configuring interfaces on mr3
route add -net 224.0.0.0 netmask 240.0.0.0 dev eth2

ifconfig nasmesh0 up
ifconfig nasmesh0 10.0.0.3
ip -6 addr add 2001:10:0:1:7856:3412:0:3/64 dev nasmesh0

echo Launching AS simulator
xterm -hold -e sh start_openair2_mr3.sh &

#echo Waiting for Router ADV from CH
#IPv6ADR=`ip addr show dev nasmesh0 | grep -e inet6 | egrep -v fe80 | cut -d " " -f6 | cut -d "/" -f1`
#while [ -z $IPv6ADR ] ; do 
#  sleep 1 
#  IPv6ADR=`ip addr show dev nasmesh0 | grep -e inet6 | egrep -v fe80 | cut -d " " -f6 | cut -d "/" -f1`
#done
#echo Got Router ADV : IPv6 address is $IPv6ADR

echo Classification rules for MR3 - Default DTCH UL for L3 signaling
#$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -x $IPv6ADR -y $CH2_ADDR -r 4
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -x 2001:10:0:1:7856:3412:0:3 -y $CH2_ADDR -r 4
#$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -x 2001:10:0:1::9a03 -y 2001:10:0:1::1 -r 4
echo Classification rules for MR3 - MPLS User-plane Bearer
$OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL/rb_tool -a -c1 -i0 -z0 -l $MR3_LABEL_OUT -m $MR3_LABEL_IN -r 5

echo eth0 is $MR1_EG_ADDR

MR3_MPLS_NAS.SH
#!/bin/bash
# Author: Lamia Romdhani
#June 2008

source params.sh
modprobe mpls6

echo Starting routing ...
echo No MPLS debug
echo "0" >/sys/mpls/debug

echo Setting MPLS for mr2
echo 'MN2->MN1'
# add label 3000 and forward the packets to mr2, for destination MN1 
var=`mpls nhlfe add key 0 instructions push gen $MR3_LABEL_OUT nexthop nasmesh0 ipv6 $CH2_ADDR |grep key | cut -c 17-26`
ip -6 route add $MN1_ADDR/128 via $CH2_ADDR mpls $var
ip -6 route add $MN2_ADDR/128 dev eth0

echo 'MN1->MN2'
#pop label 1003 and do Ip lookup.
mpls labelspace set dev nasmesh0 labelspace 0
# MR3_LABEL_IN  is 1003
mpls ilm add label gen $MR3_LABEL_IN  labelspace 0 proto ipv6 




