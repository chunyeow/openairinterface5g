#Phil, Lamia 10 Oct 08

source params.sh
sudo sysctl -w net.ipv6.conf.all.forwarding=1
./mr1_conf_nas.sh 
./mr1_mpls_nas.sh

sudo sysctl -w net.ipv6.conf.eth0.proxy_ndp=1
sudo sysctl -w net.ipv6.conf.nasmesh0.proxy_ndp=1

sudo ifconfig eth0 promisc
sudo ip -6 addr add $MR1_EG_ADDR/64 dev eth0
sudo xterm -hold -e "$OPENAIR3_PMIP6D_PATH/pmip6d -m -s -L $CH_ADDR -N $MR1_EG_ADDR -E $MR1_IN_ADDR" &
#$OPENAIR3_SCRIPTS_PATH/mr1_del_mpls.sh

watch -n.1 "cat /proc/openair2/lchan_stats ; cat /proc/openair1/bch_stats ; cat /proc/openair1/openair1_state"

./stop_rf.sh
./ch_del_mpls.sh
