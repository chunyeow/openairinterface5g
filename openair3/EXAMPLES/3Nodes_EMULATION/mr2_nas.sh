#!/bin/bash
#Lamia, Philippe huu nghia, 10 Oct 08

source params.sh
sudo sysctl -w net.ipv6.conf.all.forwarding=1

./mr2_conf_nas.sh 
./mr2_mpls_nas.sh
sudo sysctl -w net.ipv6.conf.eth3.proxy_ndp=1
sudo sysctl -w net.ipv6.conf.nasmesh0.proxy_ndp=1

sudo ifconfig eth3 promisc
sudo ip -6 addr add $MR2_EG6_ADDR/64 dev eth3
sudo xterm -hold -e "$OPENAIR3_PMIP6D_PATH/pmip6d -m -s -L $CH1_IN6_ADDR -N $MR2_EG6_ADDR -E $MR2_IN6_ADDR1" &
#watch -n1 "cat /proc/openair2/lchan_stats ; cat /proc/openair1/bch_stats"

xterm -hold -e "cd /homes/foubert/Openair/openair2/SIMULATION/USER_TOOLS/LAYER2_SIM ; ./mac_sim -m2 -t $OPENAIR2_DIR/SIMULATION/TOPOLOGIES/eth_3nodes.top"

#./stop_rf.sh
sleep 2
./del_mpls.sh
#sleep 2
#./del_mpls.sh
