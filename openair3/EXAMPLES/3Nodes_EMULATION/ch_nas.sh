#!/bin/bash

source params.sh
sudo ./ch_conf_nas.sh

# MN1 -> MN2
sudo ./mpls_nas.sh $MR1_LABEL_OUT $MR2_IN6_ADDR1 $MR2_CH1_LABEL_IN
# MN2 -> MN1
sudo ./mpls_nas.sh $MR2_CH1_LABEL_OUT $MR1_IN6_ADDR $MR1_LABEL_IN

# MN1 -> MN3
sudo ./mpls_nas.sh $MN1_MR1_CH1_MN3 $MR2_IN6_ADDR1 $MN1_CH1_MR2_MN3
# MN3 -> MN1
sudo ./mpls_nas.sh $MN3_MR2_CH1_MN1 $MR1_IN6_ADDR $MN3_CH1_MR1_MN1

# CH1 -> CH2
#echo 'CH2->MR2->CH1'
mpls labelspace set dev nasmesh0 labelspace 0
mpls ilm add label gen $CH1_MR2_CH2_LABEL_IN labelspace 0 proto ipv6

#echo 'CH1->MR2->CH2'
var=`mpls nhlfe add key 0 instructions push gen $CH1_MR2_CH2_LABEL_OUT nexthop nasmesh0 ipv6 $MR2_IN6_ADDR1 | grep key |cut -c 17-26`

echo "Creating routes"
sudo ip -6 route add $CH2_IN6_ADDR/128 via $MR2_IN6_ADDR1 mpls $var

echo 0 > /sys/mpls/debug

# PMIP
echo /openair3/pmip6d/pmip6d -c -L $CH1_IN6_ADDR -A $CH2_IN6_ADDR
sudo xterm -hold -e "$OPENAIR3_PMIP6D_PATH/pmip6d -c -L $CH1_IN6_ADDR -A $CH2_IN6_ADDR" & 

xterm -hold -e "cd /homes/foubert/Openair/openair2/SIMULATION/USER_TOOLS/LAYER2_SIM ; ./mac_sim -m0 -t $OPENAIR2_DIR/SIMULATION/TOPOLOGIES/eth_3nodes.top"

#watch -n .1 "cat /proc/openair2/lchan_stats ; cat /proc/openair1/bch_stats ; cat /proc/openair1/openair1_state"
#./stop_rf.sh
sleep 2
./del_mpls.sh
#sleep 2
#./del_mpls.sh

