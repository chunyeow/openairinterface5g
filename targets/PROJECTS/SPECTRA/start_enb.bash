#!/bin/bash


###########################################################
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
source $THIS_SCRIPT_PATH/env_802dot21.bash
###########################################################

###########################################################
IPTABLES=/sbin/iptables
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
declare -x OPENAIR_DIR=""
declare -x OPENAIR1_DIR=""
declare -x OPENAIR2_DIR=""
declare -x OPENAIR3_DIR=""
declare -x OPENAIR_TARGETS=""
declare -x EMULATION_DEV_INTERFACE="eth2"
###########################################################

set_openair
cecho "OPENAIR_DIR     = $OPENAIR_DIR" $green
cecho "OPENAIR1_DIR    = $OPENAIR1_DIR" $green
cecho "OPENAIR2_DIR    = $OPENAIR2_DIR" $green
cecho "OPENAIR3_DIR    = $OPENAIR3_DIR" $green
cecho "OPENAIR_TARGETS = $OPENAIR_TARGETS" $green

bash_exec "/sbin/iptables  -t mangle -F"
bash_exec "/sbin/iptables  -t nat -F"
bash_exec "/sbin/iptables  -t raw -F"
bash_exec "/sbin/iptables  -t filter -F"
bash_exec "/sbin/ip6tables -t mangle -F"
bash_exec "/sbin/ip6tables -t filter -F"
bash_exec "/sbin/ip6tables -t raw -F"

##################################################
# LAUNCH eNB  executable
##################################################
declare MAKE_IP_DRIVER_TARGET="oai_nw_drv.ko"
declare MAKE_LTE_ACCESS_STRATUM_TARGET="oaisim USE_MME=R10"
declare IP_DRIVER_NAME="oai_nw_drv"
declare LTEIF="oai0"
ENB_IPv4="10.0.0.1"
ENB_IPv6="2001:1::1"
ENB_IPv6_CIDR=$ENB_IPv6"/64"
ENB_IPv4_CIDR=$ENB_IPv4"/24"
#------------------------------------------------
declare -a NAS_IMEI=( 3 9 1 8 3 6 6 2 0 0 0 0 0 0 )



echo "Bringup UE interface"
rmmod -f $IP_DRIVER_NAME > /dev/null 2>&1
cecho "make $MAKE_IP_DRIVER_TARGET $MAKE_LTE_ACCESS_STRATUM_TARGET ....." $green
# bash_exec "make --directory=$OPENAIR_TARGETS/SIMU/EXAMPLES/VIRT_EMUL_1eNB $MAKE_LTE_ACCESS_STRATUM_TARGET "
bash_exec "make --directory=$OPENAIR2_DIR $MAKE_IP_DRIVER_TARGET "
#bash_exec "make --directory=$OPENAIR2_DIR/NAS/DRIVER/LITE/RB_TOOL "

bash_exec "insmod  $OPENAIR2_DIR/NAS/DRIVER/LITE/$IP_DRIVER_NAME.ko oai_nw_drv_IMEI=${NAS_IMEI[0]},${NAS_IMEI[1]},${NAS_IMEI[2]},${NAS_IMEI[3]},${NAS_IMEI[4]},${NAS_IMEI[5]},${NAS_IMEI[6]},${NAS_IMEI[7]},${NAS_IMEI[8]},${NAS_IMEI[9]},${NAS_IMEI[10]},${NAS_IMEI[11]},${NAS_IMEI[12]},${NAS_IMEI[13]}"
#bash_exec "insmod  $OPENAIR2_DIR/NAS/DRIVER/UE_LTE/$IP_DRIVER_NAME.ko"

bash_exec "ip route flush cache"

#bash_exec "ip link set $LTEIF broadcast ff:ff:ff:ff:ff:ff"
bash_exec "ip link set $LTEIF up"
sleep 1
bash_exec "ip addr add dev $LTEIF $ENB_IPv4_CIDR"
bash_exec "ip addr add dev $LTEIF $ENB_IPv6_CIDR"


sleep 1

bash_exec "sysctl -w net.ipv4.conf.all.log_martians=1"
assert "  `sysctl -n net.ipv4.conf.all.log_martians` -eq 1" $LINENO

echo "   Disabling reverse path filtering"
bash_exec "sysctl -w net.ipv4.conf.all.rp_filter=0"
assert "  `sysctl -n net.ipv4.conf.all.rp_filter` -eq 0" $LINENO


bash_exec "ip route flush cache"

# please add table 200 lte in /etc/iproute2/rt_tables
ip rule add fwmark 3  table lte
ip route add default dev $LTEIF table lte

ip route add 239.0.0.160/28 dev $EMULATION_DEV_INTERFACE

/sbin/ebtables -t nat -A POSTROUTING -p arp  -j mark --mark-set 3

/sbin/ip6tables -A OUTPUT -t mangle -o oai0 -m pkttype --pkt-type multicast -j MARK --set-mark 3
/sbin/iptables  -A OUTPUT -t mangle -o oai0 -m pkttype --pkt-type broadcast -j MARK --set-mark 3
/sbin/iptables  -A OUTPUT -t mangle -o oai0 -m pkttype --pkt-type multicast -j MARK --set-mark 3

/sbin/ip6tables -A POSTROUTING -t mangle -o oai0 -m pkttype --pkt-type multicast -j MARK --set-mark 3
/sbin/iptables  -A POSTROUTING -t mangle -o oai0 -m pkttype --pkt-type broadcast -j MARK --set-mark 3
/sbin/iptables  -A POSTROUTING -t mangle -o oai0 -m pkttype --pkt-type multicast -j MARK --set-mark 3

#All other traffic is sent on the RAB you want (mark = RAB ID)
/sbin/ip6tables -A POSTROUTING -t mangle -o oai0 -m pkttype --pkt-type unicast -j MARK --set-mark 3
/sbin/ip6tables -A OUTPUT      -t mangle -o oai0 -m pkttype --pkt-type unicast -j MARK --set-mark 3
/sbin/iptables  -A POSTROUTING -t mangle -o oai0 -m pkttype --pkt-type unicast -j MARK --set-mark 3
/sbin/iptables  -A OUTPUT      -t mangle -o oai0 -m pkttype --pkt-type unicast -j MARK --set-mark 3

# start MIH-F
#xterm -hold -e $ODTONE_ROOT/dist/odtone-mihf --log 4 --conf.file $ODTONE_ROOT/dist/odtone_enb.conf &

#wait_process_started odtone-mihf


$OPENAIR_TARGETS/SIMU/USER/oaisim -a  -l9 -u0 -M0 -p2  -g1 -D $EMULATION_DEV_INTERFACE  \
             --enb-ral-listening-port   1234\
             --enb-ral-link-id          enb_lte_link\
             --enb-ral-ip-address       127.0.0.1\
             --enb-mihf-remote-port     1025\
             --enb-mihf-ip-address      127.0.0.1\
             --enb-mihf-id              mihf1_enb 



