#!/bin/bash
# Author Lionel GAUTHIER 02/03/2014
#
# This script start ENB+UE (all in one executable, on one host) with vlan setting
# MME+SP-GW executable have to be launched on an other host by your own (start_lte-epc-vlan.bash).
#
#                                                                           hss.eur
#                                                                             |
#        +-----------+                                +-----------+           v   +----------+
#        |  eNB      +------+            VLAN 1+------+    MME    +----+      +---+   HSS    |
#        |           |ethx.1+------------------+ethy.1|           |    +------+   |          |
#        |           +------+                  +------+           +----+      +---+          |
#        |           |ethx.2+-------+                 |           |               +----------+
#        |           +------+       |                 +-+-------+-+
#        |           |              |                   | s11mme|    
#        |           |              |                   +---+---+    
#        |           |              |             (optional)|   VLAN 3
#        +-----------+              |                   +---+---+    
#                                   |                   | s11sgw|            router.eur
#                                   |                 +-+-------+-+              |   +--------------+
#                                   |                 |  S+P-GW   |              v   |   ROUTER     |
#                                   |  VLAN2   +------+           +-------+     +----+              +----+
#                                   +----------+ethy.2|           |sgi    +-...-+    |              |    +---...Internet
#                                              +------+           +-------+     +----+              +----+
#                                                     |           |      11 VLANS    |              |
#                                                     +-----------+   ids=[5..15]    +--------------+
#
###########################################################
# Parameters
###########################################################
declare MAKE_LTE_ACCESS_STRATUM_TARGET="oaisim ENABLE_ITTI=1 USE_MME=R10 NAS=1 Rel10=1"
declare MAKE_IP_DRIVER_TARGET="ue_ip.ko"
declare IP_DRIVER_NAME="ue_ip"
declare LTEIF="oip1"
declare UE_IPv4="10.0.0.8"
declare UE_IPv6="2001:1::8"
declare UE_IPv6_CIDR=$UE_IPv6"/64"
declare UE_IPv4_CIDR=$UE_IPv4"/24"


###########################################################
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
source $THIS_SCRIPT_PATH/utils.bash
###########################################################
test_command_install_package "gccxml"   "gccxml" "--force-yes"
test_command_install_package "vconfig"  "vlan"
test_command_install_package "iptables" "iptables"
test_command_install_package "iperf"    "iperf"
test_command_install_package "ip"       "iproute"
test_command_install_package "tunctl"  "uml-utilities"
test_command_install_lib     "/usr/lib/libconfig.so"  "libconfig-dev"


#######################################################
# USIM, NVRAM files
#######################################################
if [ ! -f $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/ue_data ]; then
    make --directory=$OPENAIRCN_DIR/NAS/EURECOM-NAS veryveryclean
    make --directory=$OPENAIRCN_DIR/NAS/EURECOM-NAS PROCESS=UE
fi
if [ ! -f $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/usim_data ]; then
    make --directory=$OPENAIRCN_DIR/NAS/EURECOM-NAS veryveryclean
    make --directory=$OPENAIRCN_DIR/NAS/EURECOM-NAS PROCESS=UE
fi
if [ ! -f .ue.nvram ]; then
    # generate .ue_emm.nvram .ue.nvram
    $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/ue_data -g
fi

if [ ! -f .usim.nvram ]; then
    # generate .usim.nvram
    $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/usim_data -g
fi

##################################################
# NETWORK TOPOLOGY
##################################################
clean_enb_vlan_network
build_enb_vlan_network
test_enb_vlan_network

##################################################
# LAUNCH eNB + UE executable
##################################################
echo "Bringup UE interface"
pkill oaisim
bash_exec "rmmod $IP_DRIVER_NAME" > /dev/null 2>&1

cecho "make $MAKE_IP_DRIVER_TARGET $MAKE_LTE_ACCESS_STRATUM_TARGET ....." $green
#bash_exec "make --directory=$OPENAIR2_DIR $MAKE_IP_DRIVER_TARGET "
make --directory=$OPENAIR2_DIR $MAKE_IP_DRIVER_TARGET || exit 1
#bash_exec "make --directory=$OPENAIR_TARGETS/SIMU/USER $MAKE_LTE_ACCESS_STRATUM_TARGET "
make --directory=$OPENAIR_TARGETS/SIMU/USER $MAKE_LTE_ACCESS_STRATUM_TARGET || exit 1

bash_exec "insmod  $OPENAIR2_DIR/NETWORK_DRIVER/UE_IP/$IP_DRIVER_NAME.ko"

bash_exec "ip route flush cache"

bash_exec "ip link set $LTEIF up"
sleep 1
#bash_exec "ip addr add dev $LTEIF $UE_IPv4_CIDR"
#bash_exec "ip addr add dev $LTEIF $UE_IPv6_CIDR"

sleep 1

bash_exec "sysctl -w net.ipv4.conf.all.log_martians=1"
assert "  `sysctl -n net.ipv4.conf.all.log_martians` -eq 1" $LINENO

echo "   Disabling reverse path filtering"
bash_exec "sysctl -w net.ipv4.conf.all.rp_filter=0"
assert "  `sysctl -n net.ipv4.conf.all.rp_filter` -eq 0" $LINENO


bash_exec "ip route flush cache"

# Check table 200 lte in /etc/iproute2/rt_tables
fgrep lte /etc/iproute2/rt_tables
if [ $? -ne 0 ]; then
    echo "200 lte " >> /etc/iproute2/rt_tables
fi
ip rule add fwmark 5 table lte
ip route add default dev $LTEIF table lte

ITTI_LOG_FILE=/tmp/itti_enb.$HOSTNAME.log
rotate_log_file $ITTI_LOG_FILE


#######################################################
# FIND CONFIG FILE
#######################################################
CONFIG_FILE=$THIS_SCRIPT_PATH/CONF/enb.sfr.default_vlan.conf
SEARCHED_CONFIG_FILE=$THIS_SCRIPT_PATH/CONF/enb.sfr."$HOSTNAME"_vlan.conf
if [ -f $SEARCHED_CONFIG_FILE ]; then
    CONFIG_FILE=$SEARCHED_CONFIG_FILE
    echo_warning "config file found is now $CONFIG_FILE"
else
    echo_warning "config file $SEARCHED_CONFIG_FILE for host $HOSTNAME not found, trying default: $CONFIG_FILE"
    if [ -f $CONFIG_FILE ]; then
        echo_success "Default config file found: $CONFIG_FILE"
    else
        echo_error "Default config file not found, exiting"
        exit 1
    fi
fi

gdb --args $OPENAIR_TARGETS/SIMU/USER/oaisim -a -u1 -l7 -K $ITTI_LOG_FILE --enb-conf $CONFIG_FILE


