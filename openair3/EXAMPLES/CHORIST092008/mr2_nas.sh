source params.sh
sysctl -w net.ipv6.conf.all.forwarding=1
./del_mpls.sh
./mr2_conf_nas.sh 
#MN1MN2
./mpls_nas.sh $MR2_CH1_LABEL_IN $CH2_IN6_ADDR $MR2_CH2_LABEL_OUT
#MN2MN1
./mpls_nas.sh $MR2_CH2_LABEL_IN $CH1_IN6_ADDR $MR2_CH1_LABEL_OUT

sudo sysctl -w net.ipv6.conf.eth0.proxy_ndp=1
sudo sysctl -w net.ipv6.conf.nasmesh0.proxy_ndp=1




