#!/bin/bash
#Phil, 25 july 08
#Cluster Head (1) – with NAS_MPLS_PMIP working

source params.sh
./ch1_conf_nas_eads.sh 
#MN1->MN2
./mpls_nas.sh $MR1_LABEL_OUT $MR2_IN6_ADDR1 $MR2_CH1_LABEL_IN
#MN2->MN1
./mpls_nas.sh $MR2_CH1_LABEL_OUT $MR1_IN6_ADDR $MR1_LABEL_IN

echo 'CH2->MR2->CH1'
mpls labelspace set dev nasmesh0 labelspace 0
mpls ilm add label gen $CH1_MR2_CH2_LABEL_IN labelspace 0 proto ipv6

echo 'CH1->MR2->CH2'
var=`mpls nhlfe add key 0 instructions push gen $CH1_MR2_CH2_LABEL_OUT nexthop nasmesh0 ipv6 $MR2_IN6_ADDR1 | grep key |cut -c 17-26`

echo "Creating routes"
sudo ip -6 route add $CH2_IN6_ADDR/128 via $MR2_IN6_ADDR1 mpls $var

echo $OPENAIR3_HOME/PMIP6D/pmip6d -c -L $CH1_IN6_ADDR -A $CH2_IN6_ADDR -P 7778
sudo xterm -hold -e $OPENAIR3_HOME/PMIP6D/pmip6d -c -L $CH1_IN6_ADDR -A $CH2_IN6_ADDR -P 7778 &
sleep 1
echo $OPENAIR3_HOME/PMIP6D/pmip6d -m -s -L $CH1_IN6_ADDR -N $CH1_EG6_ADDR -E $CH1_IN6_ADDR
sudo $OPENAIR3_HOME/PMIP6D/pmip6d -m -s -L $CH1_IN6_ADDR -N $CH1_EG6_ADDR -E $CH1_IN6_ADDR
sleep 2
./del_mpls.sh
