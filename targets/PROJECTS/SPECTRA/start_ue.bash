#!/bin/bash
# Author Lionel GAUTHIER

# OAI NETWORKING--------------------------------
declare -x EMULATION_DEV_INTERFACE="eth2"
declare -x IP_DRIVER_NAME="oai_nw_drv"
declare -x LTEIF="oai0"
declare -x UE_IPv4="10.0.0.2"
declare -x UE_IPv6="2001:1::2"
declare -x UE_IPv6_CIDR=$UE_IPv6"/64"
declare -x UE_IPv4_CIDR=$UE_IPv4"/24"
declare -a NAS_IMEI=( 3 9 1 8 3 6 7 3 0 2 0 0 0 0 )
# OAI MIH----------------------------------------
declare -x UE_MIHF_IP_ADDRESS="127.0.0.1"
declare -x UE_RAL_IP_ADDRESS="127.0.0.1"
#------------------------------------------------
LOG_FILE="/tmp/oai_sim_ue.log"


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


##################################################
# LAUNCH UE
##################################################

echo "Bringup UE interface"
pkill oaisim             > /dev/null 2>&1
pkill oaisim             > /dev/null 2>&1
rmmod -f $IP_DRIVER_NAME > /dev/null 2>&1

bash_exec "insmod  $OPENAIR2_DIR/NAS/DRIVER/LITE/$IP_DRIVER_NAME.ko oai_nw_drv_IMEI=${NAS_IMEI[0]},${NAS_IMEI[1]},${NAS_IMEI[2]},${NAS_IMEI[3]},${NAS_IMEI[4]},${NAS_IMEI[5]},${NAS_IMEI[6]},${NAS_IMEI[7]},${NAS_IMEI[8]},${NAS_IMEI[9]},${NAS_IMEI[10]},${NAS_IMEI[11]},${NAS_IMEI[12]},${NAS_IMEI[13]}"
bash_exec "ip route flush cache"
bash_exec "ip link set $LTEIF up"
sleep 1
bash_exec "ip addr add dev $LTEIF $UE_IPv4_CIDR"
bash_exec "ip addr add dev $LTEIF $UE_IPv6_CIDR"
sleep 1
bash_exec "sysctl -w net.ipv4.conf.all.log_martians=1"
assert "  `sysctl -n net.ipv4.conf.all.log_martians` -eq 1" $LINENO
bash_exec "sysctl -w net.ipv4.conf.all.rp_filter=0"
assert "  `sysctl -n net.ipv4.conf.all.rp_filter` -eq 0" $LINENO
bash_exec "ip route flush cache"

# please add table 200 lte in file /etc/iproute2/rt_tables
ip rule add fwmark 5  table lte
ip -4 route add default dev $LTEIF table lte
ip -6 route add default dev $LTEIF table lte
ip route add 239.0.0.160/28 dev $EMULATION_DEV_INTERFACE

# start MIH-F
xterm -hold -e $ODTONE_MIH_EXE_DIR/$MIH_F --log 4 --conf.file $ODTONE_MIH_EXE_DIR/$UE_MIH_F_CONF_FILE &
wait_process_started $MIH_F
sleep 3

NOW=$(date +"%Y-%m-%d.%Hh_%Mm_%Ss")
rm -f $LOG_FILE

UE_RAL_LINK_ID=`cat $ODTONE_MIH_EXE_DIR/$UE_MIH_F_CONF_FILE | grep links | grep \= | grep -v \# | cut -d"=" -f2`
UE_RAL_LINK_ID=`trim2 $UE_RAL_LINK_ID`
UE_RAL_LINK_ID=`echo $UE_RAL_LINK_ID | cut -d" " -f1`
UE_RAL_LINK_ID_STRIPPED=${UE_RAL_LINK_ID%%??}

UE_RAL_LISTENING_PORT=`cat $ODTONE_MIH_EXE_DIR/$UE_MIH_F_CONF_FILE | grep links | grep \= | grep -v \# | cut -d"=" -f2`
UE_RAL_LISTENING_PORT=`trim2 $UE_RAL_LISTENING_PORT`
UE_RAL_LISTENING_PORT=`echo $UE_RAL_LISTENING_PORT | cut -d" " -f2`

UE_MIHF_REMOTE_PORT=`cat $ODTONE_MIH_EXE_DIR/$UE_MIH_F_CONF_FILE | grep local_port | grep \= | grep -v \# | tr -d " "  | cut -d'=' -f2`

UE_MIHF_ID=`cat $ODTONE_MIH_EXE_DIR/$UE_MIH_F_CONF_FILE | grep id | grep \= | grep -v \# | tr -d " "  | cut -d'=' -f2`

#xterm -hold -e gdb --args 
$OPENAIR_TARGETS/SIMU/USER/oaisim -a -K $LOG_FILE -l9 -u1 -b0 -M1 -p2 -g1 -D $EMULATION_DEV_INTERFACE  \
             --ue-ral-listening-port   $UE_RAL_LISTENING_PORT \
             --ue-ral-link-id          $UE_RAL_LINK_ID_STRIPPED \
             --ue-ral-ip-address       $UE_RAL_IP_ADDRESS \
             --ue-mihf-remote-port     $UE_MIHF_REMOTE_PORT \
             --ue-mihf-ip-address      $UE_MIHF_IP_ADDRESS \
             --ue-mihf-id              $UE_MIHF_ID &
             
wait_process_started oaisim


# start MIH-USER

#  wait for emulation start
tshark -c 500 -i $EMULATION_DEV_INTERFACE > /dev/null 2>&1
sleep 5

xterm -hold -e $ODTONE_MIH_EXE_DIR/$UE_MIH_USER --conf.file $ODTONE_MIH_EXE_DIR/$UE_MIH_USER_CONF_FILE &
wait_process_started $UE_MIH_USER

sleep 100000




