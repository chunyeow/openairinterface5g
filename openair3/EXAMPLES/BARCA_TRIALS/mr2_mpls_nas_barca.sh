#!/bin/bash
# Author: Lamia Romdhani
#June 2008

source params.sh
modprobe mpls6

echo Starting routing ...
echo No MPLS debug
echo "0" >/sys/mpls/debug


#MN1->MN2
./mpls_nas.sh $MR2_CH1_LABEL_IN $CH2_IN6_ADDR $MR2_CH2_LABEL_OUT
#MN2->MN1
./mpls_nas.sh $MR2_CH2_LABEL_IN $CH1_IN6_ADDR $MR2_CH1_LABEL_OUT

#CH1->CH2
./mpls_nas.sh $CH1_MR2_CH2_LABEL_OUT $CH2_IN6_ADDR $CH2_MR2_CH1_LABEL_IN
#CH2->CH1
./mpls_nas.sh $CH2_MR2_CH1_LABEL_OUT $CH1_IN6_ADDR $CH1_MR2_CH2_LABEL_IN

#MN3->MN1
var=`mpls nhlfe add key 0 instructions push gen $MN3_MR2_CH1_MN1 nexthop nasmesh0 ipv6 $CH1_IN6_ADDR |grep key | cut -c 17-26`
sudo ip -6 route add $MN1_IN6_ADDR/128 via $CH1_IN6_ADDR mpls $var
sudo ip -6 route add $MN3_IN6_ADDR/128 dev eth0
#MN1->MN3
mpls labelspace set dev nasmesh0 labelspace 0
mpls ilm add label gen $MN1_CH1_MR2_MN3 labelspace 0 proto ipv6

#MN3->MN2
var=`mpls nhlfe add key 0 instructions push gen $MN3_MR2_CH2_MN2 nexthop nasmesh0 ipv6 $CH2_IN6_ADDR |grep key | cut -c 17-26`
sudo ip -6 route add $MN2_IN6_ADDR/128 via $CH2_IN6_ADDR mpls $var
#MN2->MN3
mpls labelspace set dev nasmesh0 labelspace 0
mpls ilm add label gen $MN2_CH2_MR2_MN3 labelspace 0 proto ipv6

