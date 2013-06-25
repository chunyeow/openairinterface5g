#/bin/bash
# File : ip_config_mr_sho.sh
# Authors
# Lionel Gauthier
# Philippe Foubert
# V1 2009 Fev 06
DATE=`date +"%Y_%b_%d.%HH%MMin"`
LOG_FILE="/opt/TESTBED_NEMO_MCOA_SHO/log/mr_sho_"$DATE".log"


IF0=eth0
IF1=eth1
IF2=eth2
NEMOD=`which mip6d`

echo "Stopping radvd service"
/etc/init.d/radvd stop
echo "Shutting down all ethx interfaces"
ifconfig $IF0 down
ifconfig $IF1 down
ifconfig $IF2 down

echo "Flushing ip6tables"
ip6tables -F
sysctl -w net.ipv6.conf.all.accept_ra=0
sysctl -w net.ipv6.conf.all.forwarding=1

echo "Bringing up eth0, eth1"
ifconfig $IF0 up
ifconfig $IF1 up
#ifconfig $IF2 up

echo "Configuring address on NEMO link"
ip -6 addr add 2001:4:0:2::1/64 dev $IF1
echo "Starting radvd service"
/etc/init.d/radvd start
echo "Sleep 3 seconds"
sleep 3

echo "Lauching tcpdump s on all interfaces logs are in /opt/TESTBED_NEMO_MCOA_SHO/log folder"
tcpdump -s 256 -i eth1 -v -w "/opt/TESTBED_NEMO_MCOA_SHO/log/mr_tcpdump_eth1_"$DATE".log" &
tcpdump -s 256 -i eth2 -v -w "/opt/TESTBED_NEMO_MCOA_SHO/log/mr_tcpdump_eth2_"$DATE".log" &
tcpdump -s 256 -i eth0 -v -w "/opt/TESTBED_NEMO_MCOA_SHO/log/mr_tcpdump_eth0_"$DATE".log" &

echo "Starting NEMO daemon logfile is $LOG_FILE"
$NEMOD -c /opt/TESTBED_NEMO_MCOA_SHO/etc/mr_mcoa_sho.conf 2>&1 | tee $LOG_FILE &

sleep 20
#ip6tables -A PREROUTING -t mangle -p icmpv6 --destination 2001:2f0:110:6000::1 -j MARK --set-mark 100
#ip6tables -A PREROUTING -t mangle -p icmpv6 --destination 2001:2f0:110:7000::1 -j MARK --set-mark 200
#ip6tables -A PREROUTING -t mangle  --destination 2001:2f0:110:6000::1 -j MARK --set-mark 100
#ip6tables -A PREROUTING -t mangle  --destination 2001:2f0:110:7000::1 -j MARK --set-mark 200
#ip6tables -A PREROUTING -t mangle  --destination 2001:2f0:110:6000::1 -j MARK --set-mark 100
#ip6tables -A PREROUTING -t mangle  --destination 2001:2f0:110:7000::1 -j MARK --set-mark 200
echo '\n\n####################################################'
echo '#   PACKETS ARE MARKED NOW YOU CAN START TRAFFIC   #\n'
echo '####################################################\n\n'
echo "sleep 1 hour (type ctrl+C to end)"
sleep 3600
#killall tcpdump


#tc qdisc add dev eth0 root netem loss 0%
#tc qdisc add dev eth1 root netem loss 0%

