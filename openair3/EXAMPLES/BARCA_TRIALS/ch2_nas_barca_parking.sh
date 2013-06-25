#!/bin/bash
#CH2_NAS.SH
#Cluster Head (2)– with NAS_MPLS_PMIP working
#Phil, 25 july 08

source params.sh
./ch2_conf_nas_barca_parking.sh 

echo "0" > /sys/mpls/debug
#MN1->MN2
./mpls_nas.sh $MR2_CH2_LABEL_OUT $MR3_IN6_ADDR $MR3_LABEL_IN
#MN2->MN1
./mpls_nas.sh $MR3_LABEL_OUT $MR2_IN6_ADDR2 $MR2_CH2_LABEL_IN

echo "0" > /sys/mpls/debug
#CH1->MR2->CH2
echo 'CH1->MR2->CH2'
mpls labelspace set dev nasmesh0 labelspace 0
mpls ilm add label gen $CH2_MR2_CH1_LABEL_IN labelspace 0 proto ipv6
echo 'CH1->MR2->CH2'
var=`mpls nhlfe add key 0 instructions push gen $CH2_MR2_CH1_LABEL_OUT nexthop nasmesh0 ipv6 $MR2_IN6_ADDR2 | grep key |cut -c 17-26`
echo "Creating routes from CH2 to CH1"
sudo ip -6 route add $CH1_IN6_ADDR/128 via $MR2_IN6_ADDR2 mpls $var

#MN3->MN2
./mpls_nas.sh $MN3_MR2_CH2_MN1 $MR3_IN6_ADDR $MN3_CH2_MR3_MN2
#MN2->MN3
./mpls_nas.sh $MN2_MR3_CH2_MN3 $MR2_IN6_ADDR2 $MN1_CH2_MR2_MN3


echo $OPENAIR3_HOME/pmip6d/pmip6d -c -L $CH2_IN6_ADDR -A $CH1_IN6_ADDR
#sudo xterm -hold -e "$OPENAIR3_HOME/pmip6d/pmip6d -c -L $CH2_IN6_ADDR -A $CH1_IN6_ADDR" &

watch -n .5 "cat /proc/openair2/lchan_stats ; cat /proc/openair1/bch_stats ; cat /proc/openair1/openair1_state"

./stop_rf.sh
./del_mpls.sh
sleep 2
./del_mpls.sh
