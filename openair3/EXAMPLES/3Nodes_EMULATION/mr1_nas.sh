#Phil, Lamia 10 Oct 08

source params.sh
sudo sysctl -w net.ipv6.conf.all.forwarding=1
./mr1_conf_nas.sh
./mr1_mpls_nas.sh

sudo sysctl -w net.ipv6.conf.eth0.proxy_ndp=1
sudo sysctl -w net.ipv6.conf.nasmesh0.proxy_ndp=1

sudo ifconfig eth0 promisc
sudo ip -6 addr add $MR1_EG6_ADDR/64 dev eth2
sudo xterm -hold -e "$OPENAIR3_PMIP6D_PATH/pmip6d -m -s -L $CH1_IN6_ADDR -N $MR1_EG6_ADDR -E $MR1_IN6_ADDR" &

#watch -n1 "cat /proc/openair2/lchan_stats ; cat /proc/openair1/bch_stats"

xterm -hold -e "cd /homes/foubert/Openair/openair2/SIMULATION/USER_TOOLS/LAYER2_SIM ; ./mac_sim -m1 -t $OPENAIR2_DIR/SIMULATION/TOPOLOGIES/eth_3nodes.top"

#./stop_rf.sh
sleep 2
./del_mpls.sh
#sleep 2
#./del_mpls.sh
