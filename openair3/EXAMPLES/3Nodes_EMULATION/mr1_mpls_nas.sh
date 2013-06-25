#!/bin/bash
# Script by Lamia Romdhani, Nghia, Philippe, Hicham  18 Feb 2009	 

source params.sh
sudo modprobe mpls6
echo Setting MPLS for mr1

echo "0" > /sys/mpls/debug

# MN1 <-> MN3
echo 'MN3->MN1'
mpls labelspace set dev nasmesh0 labelspace 0
mpls ilm add label gen $MN3_CH1_MR1_MN1 labelspace 0 proto ipv6
echo 'MN1->MN3'
var=`mpls nhlfe add key 0 instructions push gen $MN1_MR1_CH1_MN3 nexthop nasmesh0 ipv6 $CH1_IN6_ADDR | grep key |cut -c 17-26`
echo "Creating routes from MN1 to MN3"
sudo ip -6 route add $MN3_IN6_ADDR/128 via $CH1_IN6_ADDR mpls $var
sudo ip -6 route add $MN1_IN6_ADDR/128 dev eth2

# MN1 <-> MN2
echo 'MN2->MN1'
mpls labelspace set dev nasmesh0 labelspace 0
mpls ilm add label gen $MR1_LABEL_IN labelspace 0 proto ipv6
echo 'MN1->MN2'
var=`mpls nhlfe add key 0 instructions push gen $MR1_LABEL_OUT nexthop nasmesh0 ipv6 $CH1_IN6_ADDR | grep key |cut -c 17-26`
echo "Creating routes from MN1 to MN2" 
sudo ip -6 route add $MN2_IN6_ADDR/128 via $CH1_IN6_ADDR mpls $var

