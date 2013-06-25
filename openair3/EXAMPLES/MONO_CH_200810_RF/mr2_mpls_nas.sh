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
# add label 2000 and forward the packets to mr2 , for destination MN1 

#Changed by Huu-Nghia, Philippe
var=`mpls nhlfe add key 0 instructions push gen $MR2_LABEL_OUT nexthop nasmesh0 ipv6 $CH_ADDR |grep key | cut -c 17-26`
sudo ip -6 route add $MN1_ADDR/128 via $CH_ADDR mpls $var
sudo ip -6 route add $MN2_ADDR/128 dev eth0

echo 'MN1->MN2'
#pop label 1001 and do Ip lookup.
mpls labelspace set dev nasmesh0 labelspace 0
mpls ilm add label gen 1001 labelspace 0 proto ipv6 

