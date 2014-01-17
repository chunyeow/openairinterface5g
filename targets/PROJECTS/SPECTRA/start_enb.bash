#!/bin/bash
# Author Lionel GAUTHIER

# OAI NETWORKING--------------------------------
declare -x EMULATION_DEV_INTERFACE="eth2"
declare -x IP_DRIVER_NAME="oai_nw_drv"
declare -x LTEIF="oai0"
declare -x ENB_IPv4="10.0.0.1"
declare -x ENB_IPv6="2001:1::1"
declare -x ENB_IPv6_CIDR=$ENB_IPv6"/64"
declare -x ENB_IPv4_CIDR=$ENB_IPv4"/24"
declare -a NAS_IMEI=( 3 9 1 8 3 6 6 2 0 0 0 0 0 0 )
# OAI MIH----------------------------------------
declare -x ENB_RAL_IP_ADDRESS="127.0.0.1"
declare -x ENB_MIHF_IP_ADDRESS=127.0.0.1
#------------------------------------------------
LOG_FILE="/tmp/oai_sim_enb.log"

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

echo "Bringup eNB interface"
pkill oaisim             > /dev/null 2>&1
pkill oaisim             > /dev/null 2>&1
rmmod -f $IP_DRIVER_NAME > /dev/null 2>&1

bash_exec "insmod  $OPENAIR2_DIR/NAS/DRIVER/LITE/$IP_DRIVER_NAME.ko oai_nw_drv_IMEI=${NAS_IMEI[0]},${NAS_IMEI[1]},${NAS_IMEI[2]},${NAS_IMEI[3]},${NAS_IMEI[4]},${NAS_IMEI[5]},${NAS_IMEI[6]},${NAS_IMEI[7]},${NAS_IMEI[8]},${NAS_IMEI[9]},${NAS_IMEI[10]},${NAS_IMEI[11]},${NAS_IMEI[12]},${NAS_IMEI[13]}"
bash_exec "ip route flush cache"
bash_exec "ip link set $LTEIF up"
sleep 1
bash_exec "ip addr add dev $LTEIF $ENB_IPv4_CIDR"
bash_exec "ip addr add dev $LTEIF $ENB_IPv6_CIDR"
sleep 1
bash_exec "sysctl -w net.ipv4.conf.all.log_martians=1"
assert "  `sysctl -n net.ipv4.conf.all.log_martians` -eq 1" $LINENO
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
xterm -hold -e $ODTONE_MIH_EXE_DIR/$MIH_F --log 4 --conf.file $ODTONE_MIH_EXE_DIR/$ENB_MIH_F_CONF_FILE &
wait_process_started $MIH_F

NOW=$(date +"%Y-%m-%d.%Hh_%Mm_%Ss")
rm -f $LOG_FILE

ENB_RAL_LINK_ID=`cat $ODTONE_MIH_EXE_DIR/$ENB_MIH_F_CONF_FILE | grep links | grep \= | grep -v \# | cut -d"=" -f2`
ENB_RAL_LINK_ID=`trim2 $ENB_RAL_LINK_ID`
ENB_RAL_LINK_ID=`echo $ENB_RAL_LINK_ID | cut -d" " -f1`

ENB_RAL_LISTENING_PORT=`cat $ODTONE_MIH_EXE_DIR/$ENB_MIH_F_CONF_FILE | grep links | grep \= | grep -v \# | cut -d"=" -f2`
ENB_RAL_LISTENING_PORT=`trim2 $ENB_RAL_LISTENING_PORT`
ENB_RAL_LISTENING_PORT=`echo $ENB_RAL_LISTENING_PORT | cut -d" " -f2`

ENB_MIHF_REMOTE_PORT=`cat $ODTONE_MIH_EXE_DIR/$ENB_MIH_F_CONF_FILE | grep local_port | grep \= | grep -v \# | tr -d " "  | cut -d'=' -f2`

ENB_MIHF_ID=`cat $ODTONE_MIH_EXE_DIR/$ENB_MIH_F_CONF_FILE | grep id | grep \= | grep -v \# | tr -d " "  | cut -d'=' -f2`
#remove 2 last digits (vitualization, index on 2 digits)
ENB_RAL_LINK_ID_STRIPPED=${ENB_RAL_LINK_ID%%??}

#xterm -hold -e gdb --args 
$OPENAIR_TARGETS/SIMU/USER/oaisim -a  -K $LOG_FILE -l9 -u0 -b1 -M0 -p2  -g1 -D $EMULATION_DEV_INTERFACE  \
             --enb-ral-listening-port   $ENB_RAL_LISTENING_PORT \
             --enb-ral-link-id          $ENB_RAL_LINK_ID_STRIPPED \
             --enb-ral-ip-address       $ENB_RAL_IP_ADDRESS \
             --enb-mihf-remote-port     $ENB_MIHF_REMOTE_PORT \
             --enb-mihf-ip-address      $ENB_MIHF_IP_ADDRESS \
             --enb-mihf-id              $ENB_MIHF_ID &

wait_process_started oaisim

# start MIH-USER
#  wait for emulation start
tshark -c 500 -i $EMULATION_DEV_INTERFACE > /dev/null 2>&1 
#xterm -hold -e $ODTONE_MIH_EXE_DIR/$ENB_MIH_USER    --conf.file $ODTONE_MIH_EXE_DIR/$ENB_MIH_USER_CONF_FILE &
#wait_process_started $ENB_MIH_USER

sleep 100000


