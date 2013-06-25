#!/bin/bash
# Script by Lamia Romdhani, Nghia, Philippe, Hicham  18 Feb 2009	 

source params.sh
sudo modprobe mpls6
echo Setting MPLS for mr3

echo "0" > /sys/mpls/debug


echo 'MN1->MN2'
sudo mpls labelspace set dev nasmesh0 labelspace 0
sudo mpls ilm add label gen $MR3_LABEL_IN  labelspace 0 proto ipv6 


echo 'MN2->MN1'
var=`mpls nhlfe add key 0 instructions push gen $MR3_LABEL_OUT nexthop nasmesh0 ipv6 $CH2_IN6_ADDR |grep key | cut -c 17-26`

echo "Creating routes"
sudo ip -6 route add $MN1_IN6_ADDR/128 via $CH2_IN6_ADDR mpls $var
sudo ip -6 route add $MN2_IN6_ADDR/128 dev $ETH_MR3



echo 'MN3->MN2'
sudo mpls labelspace set dev nasmesh0 labelspace 0
sudo mpls ilm add label gen $MN3_CH2_MR3_MN2  labelspace 0 proto ipv6 


echo 'MN2->MN3'
var=`mpls nhlfe add key 0 instructions push gen $MN2_MR3_CH2_MN3 nexthop nasmesh0 ipv6 $CH2_IN6_ADDR |grep key | cut -c 17-26`

echo "Creating routes"
sudo ip -6 route add $MN3_IN6_ADDR/128 via $CH2_IN6_ADDR mpls $var
