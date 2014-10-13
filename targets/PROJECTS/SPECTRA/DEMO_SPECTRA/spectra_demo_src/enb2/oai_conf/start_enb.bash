#!/bin/bash
################################################################################
#   OpenAirInterface
#   Copyright(c) 1999 - 2014 Eurecom
#
#    OpenAirInterface is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#
#    OpenAirInterface is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with OpenAirInterface.The full GNU General Public License is
#    included in this distribution in the file called "COPYING". If not,
#    see <http://www.gnu.org/licenses/>.
#
#  Contact Information
#  OpenAirInterface Admin: openair_admin@eurecom.fr
#  OpenAirInterface Tech : openair_tech@eurecom.fr
#  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
#
#  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE
#
################################################################################
# file start_enb.bash
# brief
# author Lionel Gauthier
# company Eurecom
# email: lionel.gauthier@eurecom.fr
#
#------------------------------------------------
# ENB CONFIG FILE
#------------------------------------------------
#declare -x ENB_CONFIG_FILE="CONF/enb.sfr.yang.conf"
declare -x ENB_CONFIG_FILE="enb.conf"

#------------------------------------------------
# OAI NETWORKING
#------------------------------------------------
declare -x EMULATION_DEV_INTERFACE="eth0"
declare -x EMULATION_DEV_ADDRESS="192.168.15.6"
declare -x IP_DRIVER_NAME="oai_nw_drv"
declare -x LTEIF="oai0"
declare -x ENB_IPv4="10.0.2.4"
declare -x ENB_IPv6="2001:2::4"
declare -x ENB_IPv6_CIDR=$ENB_IPv6"/64"
declare -x ENB_IPv4_CIDR=$ENB_IPv4"/24"
declare -a NAS_IMEI=( 3 9 1 8 3 6 6 2 0 0 0 0 0 0 )
declare -x IP_DEFAULT_MARK="1" # originally 3
#------------------------------------------------
# OAI MIH
#------------------------------------------------
declare -x ENB_RAL_IP_ADDRESS="127.0.0.1"
declare -x ENB_MIHF_IP_ADDRESS=127.0.0.1
MIH_LOG_FILE="mih-f_enb.log"

#------------------------------------------------
LOG_FILE="/tmp/oai_sim_enb.log"

# EXE options 
EXE_MODE="DEBUG" # PROD or DEBUG

###########################################################
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
source $THIS_SCRIPT_PATH/env_802dot21.bash
###########################################################
bash_exec "service network-manager stop"
bash_exec "ifconfig $EMULATION_DEV_INTERFACE up $EMULATION_DEV_ADDRESS netmask 255.255.255.0"
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
pkill $MIH_F             > /dev/null 2>&1
pkill $ENB_MIH_USER      > /dev/null 2>&1
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
bash_exec "sysctl -w net.ipv4.ip_forward=1"
assert "  `sysctl -n net.ipv4.ip_forward` -eq 1" $LINENO

# Check table 200 lte in /etc/iproute2/rt_tables
fgrep lte /etc/iproute2/rt_tables  > /dev/null 
if [ $? -ne 0 ]; then
    echo '200 lte ' >> /etc/iproute2/rt_tables
fi
ip rule add fwmark $IP_DEFAULT_MARK  table lte
ip route add default dev $LTEIF table lte
ip route add 239.0.0.160/28 dev $EMULATION_DEV_INTERFACE

/sbin/ebtables -t nat -A POSTROUTING -p arp  -j mark --mark-set 3

/sbin/ip6tables -A OUTPUT -t mangle -o oai0 -m pkttype --pkt-type multicast -j MARK --set-mark $IP_DEFAULT_MARK
/sbin/iptables  -A OUTPUT -t mangle -o oai0 -m pkttype --pkt-type broadcast -j MARK --set-mark $IP_DEFAULT_MARK
/sbin/iptables  -A OUTPUT -t mangle -o oai0 -m pkttype --pkt-type multicast -j MARK --set-mark $IP_DEFAULT_MARK

/sbin/ip6tables -A POSTROUTING -t mangle -o oai0 -m pkttype --pkt-type multicast -j MARK --set-mark $IP_DEFAULT_MARK
/sbin/iptables  -A POSTROUTING -t mangle -o oai0 -m pkttype --pkt-type broadcast -j MARK --set-mark $IP_DEFAULT_MARK
/sbin/iptables  -A POSTROUTING -t mangle -o oai0 -m pkttype --pkt-type multicast -j MARK --set-mark $IP_DEFAULT_MARK

#All other traffic is sent on the RAB you want (mark = RAB ID)
/sbin/ip6tables -A POSTROUTING -t mangle -o oai0 -m pkttype --pkt-type unicast -j MARK --set-mark $IP_DEFAULT_MARK
/sbin/ip6tables -A OUTPUT      -t mangle -o oai0 -m pkttype --pkt-type unicast -j MARK --set-mark $IP_DEFAULT_MARK
/sbin/iptables  -A POSTROUTING -t mangle -o oai0 -m pkttype --pkt-type unicast -j MARK --set-mark $IP_DEFAULT_MARK
/sbin/iptables  -A OUTPUT      -t mangle -o oai0 -m pkttype --pkt-type unicast -j MARK --set-mark $IP_DEFAULT_MARK

rotate_log_file $MIH_LOG_FILE


# start MIH-F
xterm -hold -title "[eNB][eNB2] MIHF" -e $ODTONE_MIH_EXE_DIR/$MIH_F --log 4 --conf.file $ODTONE_MIH_EXE_DIR/$ENB_MIH_F_CONF_FILE > $MIH_LOG_FILE 2>&1 &
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

if [ $EXE_MODE = "DEBUG" ] ; then
#xterm -hold -e gdb --args
$OPENAIR_TARGETS/SIMU/USER/oaisim -a  -K $LOG_FILE -l9 -u0 -b1 -M0 -p2  -g3 -D $EMULATION_DEV_INTERFACE  \
             --enb-ral-listening-port   $ENB_RAL_LISTENING_PORT \
             --enb-ral-link-id          $ENB_RAL_LINK_ID_STRIPPED \
             --enb-ral-ip-address       $ENB_RAL_IP_ADDRESS \
             --enb-mihf-remote-port     $ENB_MIHF_REMOTE_PORT \
             --enb-mihf-ip-address      $ENB_MIHF_IP_ADDRESS \
             --enb-mihf-id              $ENB_MIHF_ID \
             -O $ENB_CONFIG_FILE   > log_enb.txt 2> /dev/null &
#             -O $ENB_CONFIG_FILE  | grep  "RAL\|PDCP" &
else
#xterm -hold -e gdb --args
$OPENAIR_TARGETS/SIMU/USER/oaisim -a  -K $LOG_FILE -l3 -u0 -b1 -M0 -p2  -g3 -D $EMULATION_DEV_INTERFACE  \
             --enb-ral-listening-port   $ENB_RAL_LISTENING_PORT \
             --enb-ral-link-id          $ENB_RAL_LINK_ID_STRIPPED \
             --enb-ral-ip-address       $ENB_RAL_IP_ADDRESS \
             --enb-mihf-remote-port     $ENB_MIHF_REMOTE_PORT \
             --enb-mihf-ip-address      $ENB_MIHF_IP_ADDRESS \
             --enb-mihf-id              $ENB_MIHF_ID \
             -O $ENB_CONFIG_FILE   > /dev/null &
#             -O $ENB_CONFIG_FILE  | grep  "RAL\|PDCP" &
fi

wait_process_started oaisim

# start MIH-USER
#  wait for emulation start
tshark -c 150 -i $EMULATION_DEV_INTERFACE > /dev/null 2>&1
xterm -hold -title "[eNB][eNB2] MIH_USER" -e $ODTONE_MIH_EXE_DIR/$ENB_MIH_USER    --conf.file $ODTONE_MIH_EXE_DIR/$ENB_MIH_USER_CONF_FILE &
wait_process_started $ENB_MIH_USER

xterm -hold -title "[eNB][eNB2] TVWS Sensing" -e ./server &
#xterm -hold -title "[eNB][eNB2] output" -e tail -f log_enb.txt | grep -i recon &

sleep 100000


