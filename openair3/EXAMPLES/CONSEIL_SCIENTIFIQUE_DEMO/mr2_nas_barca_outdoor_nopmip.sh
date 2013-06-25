#!/bin/bash
#Lamia, Philippe huu nghia, 10 Oct 08
source params.sh

sudo sysctl -w net.ipv6.conf.all.forwarding=1
./mr2_conf_nas_barca_outdoor.sh 
./mr2_mpls_nas_barca.sh
sudo sysctl -w net.ipv6.conf.eth0.proxy_ndp=1
sudo sysctl -w net.ipv6.conf.nasmesh0.proxy_ndp=1

sudo ifconfig eth0 promisc
sudo ip -6 addr add $MR2_EG6_ADDR/64 dev eth0
#sudo xterm -hold -e "$OPENAIR3_PMIP6D_PATH/pmip6d -m -s -L $CH1_IN6_ADDR -N $MR2_EG6_ADDR -E $MR2_IN6_ADDR1" &

watch -n1 "cat /proc/openair2/lchan_stats ; cat /proc/openair1/bch_stats"

./stop_rf.sh
./del_mpls.sh
sleep 2
./del_mpls.sh
