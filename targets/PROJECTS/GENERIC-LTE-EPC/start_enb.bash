#!/bin/bash
################################################################################
# Eurecom OpenAirInterface core network
# Copyright(c) 1999 - 2014 Eurecom
#
# This program is free software; you can redistribute it and/or modify it
# under the terms and conditions of the GNU General Public License,
# version 2, as published by the Free Software Foundation.
#
# This program is distributed in the hope it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
#
# The full GNU General Public License is included in this distribution in
# the file called "COPYING".
#
# Contact Information
# Openair Admin: openair_admin@eurecom.fr
# Openair Tech : openair_tech@eurecom.fr
# Forums       : http://forums.eurecom.fsr/openairinterface
# Address      : EURECOM,
#                Campus SophiaTech,
#                450 Route des Chappes,
#                CS 50193
#                06904 Biot Sophia Antipolis cedex,
#                FRANCE
################################################################################
# file start_enb.bash
# brief
# author Lionel Gauthier
# company Eurecom
# email: lionel.gauthier@eurecom.fr
###########################################
# INPUT OF THIS SCRIPT:
# THE DIRECTORY WHERE ARE LOCATED THE CONFIGURATION FILES
#########################################
# This script start  ENB  
# Depending on configuration files, it can be instanciated a virtual switch 
# setting or a VLAN setting for the networking between eNB and MME.
# MME+SP-GW executable have to be launched on the same host by your own (start_epc.bash) before this script is invoked.
# UE executable have to be launched on another host by your own (start_ue.bash) after this script is invoked.
#
###########################################################################################################################
#                                    VIRTUAL SWITCH SETTING (PROBLEM WHITH SCTP, USE VLAN, TBC)
###########################################################################################################################
#
#                                                                           hss.eur
#                                                                             |
#        +-----------+          +------+              +-----------+           v   +----------+
#        |  eNB      +------+   |  ovs | VLAN 1+------+    MME    +----+      +---+   HSS    |
#        |           |cpenb0+------------------+cpmme0|           |    +------+   |          |
#        |           +------+   |bridge|       +------+           +----+      +---+          |
#        |           |upenb0+-------+  |              |           |               +----------+
#        +-----------+------+   |   |  |              +-----------+
#                               +---|--+                    |                   router.eur
#                                   |                 +-----------+              |   +--------------+
#                                   |                 |  S+P-GW   |              v   |   ROUTER     |
#                                   |  VLAN2   +------+           +-------+     +----+              +----+
#                                   +----------+upsgw0|           |sgi    +-...-+    |              |    +---...Internet
#                                              +------+           +-------+     +----+              +----+
#                                                     |           |      11 VLANS    |              |
#                                                     +-----------+   ids=[5..15]    +--------------+
#
#
###########################################################################################################################
#                                    VLAN SETTING
###########################################################################################################################
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

###########################################################
# Parameters
###########################################################
declare EMULATION_DEV_INTERFACE="eth2"

declare MAKE_LTE_ACCESS_STRATUM_TARGET="oaisim DEBUG=1 ENABLE_ITTI=1 USE_MME=R10 LINK_PDCP_TO_GTPV1U=1 NAS=1 Rel10=1 SECU=1 RRC_MSG_PRINT=1"
declare MAKE_LTE_ACCESS_STRATUM_TARGET_RT="lte-softmodem HARD_RT=1 ENABLE_ITTI=1 USE_MME=R10 LINK_PDCP_TO_GTPV1U=1 DISABLE_XER_PRINT=1 SECU=1 RRC_MSG_PRINT=1 "


###########################################################
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
source $THIS_SCRIPT_PATH/utils.bash
###########################################################
if [ $# -eq 1 ]; then
    declare -x CONFIG_FILE_DIR=$1
    if [ ! -d $CONFIG_FILE_DIR ]; then
        echo_error "ERROR while invoking this script, as first argument to this script you have to provide the path to a directory (./CONF/VLAN.VIRTUAL.$HOSTNAME for example) containing valid epc and enb config files"
        exit 1
    fi
else
    echo_error "ERROR while invoking this script, as first argument to this script  you have to provide the path to a directory  (./CONF/VLAN.VIRTUAL.$HOSTNAME for example) containing valid epc and enb config files"
    exit 1
fi


#check_install_epc_software



cd $THIS_SCRIPT_PATH

EMULATION_DEV_ADDRESS=`ifconfig $EMULATION_DEV_INTERFACE | grep 'inet addr:'| grep -v '127.0.0.1' | cut -d: -f2 | awk '{ print $1}'`

#######################################################
# FIND CONFIG FILE
#######################################################
SEARCHED_CONFIG_FILE_ENB="enb*.conf"
CONFIG_FILE_ENB=$THIS_SCRIPT_PATH/`find $CONFIG_FILE_DIR -iname $SEARCHED_CONFIG_FILE_ENB`
if [ -f $CONFIG_FILE_ENB ]; then
    echo_warning "eNB config file found is now $CONFIG_FILE_ENB"
else
    echo_error "eNB config file not found, exiting"
    exit 1
fi

#######################################################
# SOURCE CONFIG FILE
#######################################################
rm -f /tmp/source.txt
VARIABLES="
           ENB_INTERFACE_NAME_FOR_S1_MME\|\
           ENB_IPV4_ADDRESS_FOR_S1_MME\|\
           ENB_INTERFACE_NAME_FOR_S1U\|\
           ENB_IPV4_ADDRESS_FOR_S1U\|\
           hard_real_time"

VARIABLES=$(echo $VARIABLES | sed -e 's/\\r//g')
VARIABLES=$(echo $VARIABLES | tr -d ' ')
cat $CONFIG_FILE_ENB | grep -w "$VARIABLES"| tr -d " " | tr -d ";" > /tmp/source.txt
source /tmp/source.txt

declare ENB_IPV4_NETMASK_FOR_S1_MME=$(       echo $ENB_IPV4_ADDRESS_FOR_S1_MME        | cut -f2 -d '/')
declare ENB_IPV4_NETMASK_FOR_S1U=$(          echo $ENB_IPV4_ADDRESS_FOR_S1U        | cut -f2 -d '/')

ENB_IPV4_ADDRESS_FOR_S1_MME=$(               echo $ENB_IPV4_ADDRESS_FOR_S1_MME        | cut -f1 -d '/')
ENB_IPV4_ADDRESS_FOR_S1U=$(                  echo $ENB_IPV4_ADDRESS_FOR_S1U           | cut -f1 -d '/')

is_openvswitch_interface $ENB_INTERFACE_NAME_FOR_S1_MME  \
                               $ENB_INTERFACE_NAME_FOR_S1U

if [ $? -eq 1 ]; then
   echo_success "Found open-vswitch network configuration"
else
    is_vlan_interface $ENB_INTERFACE_NAME_FOR_S1_MME \
                      $ENB_INTERFACE_NAME_FOR_S1U
    if [ $? -eq 1 ]; then
        echo_success "Found VLAN network configuration"
        clean_enb_vlan_network
        build_enb_vlan_network
        test_enb_vlan_network
    else
        is_real_interface  $ENB_INTERFACE_NAME_FOR_S1_MME \
                      $ENB_INTERFACE_NAME_FOR_S1U
        if [ $? -eq 1 ]; then
            echo_success "Found standart network configuration"
        else
            echo_error "Cannot find open-vswitch network configuration or VLAN network configuration or standard network configuration"
            exit 1
        fi
    fi 
fi


##################################################
# LAUNCH eNB + UE executable
##################################################
pkill oaisim
pkill tshark


if [ x$hard_real_time != "xyes" ]; then
    ITTI_LOG_FILE=$THIS_SCRIPT_PATH/OUTPUT/itti_enb_ue.$HOSTNAME.log
    rotate_log_file $ITTI_LOG_FILE
    
    STDOUT_LOG_FILE=$THIS_SCRIPT_PATH/OUTPUT/stdout_enb_ue.$HOSTNAME.log
    rotate_log_file $STDOUT_LOG_FILE
    rotate_log_file $STDOUT_LOG_FILE.filtered
    
    PCAP_LOG_FILE=$THIS_SCRIPT_PATH/OUTPUT/tshark_enb_ue.$HOSTNAME.pcap
    rotate_log_file $PCAP_LOG_FILE
else 
    ITTI_LOG_FILE=$THIS_SCRIPT_PATH/OUTPUT/itti_enb_rf.$HOSTNAME.log
    rotate_log_file $ITTI_LOG_FILE
    
    STDOUT_LOG_FILE=$THIS_SCRIPT_PATH/OUTPUT/stdout_enb_rf.$HOSTNAME.log
    rotate_log_file $STDOUT_LOG_FILE
    rotate_log_file $STDOUT_LOG_FILE.filtered
    
    PCAP_LOG_FILE=$THIS_SCRIPT_PATH/OUTPUT/tshark_enb_rf.$HOSTNAME.pcap
    rotate_log_file $PCAP_LOG_FILE
fi


cd $THIS_SCRIPT_PATH

nohup tshark -i $ENB_INTERFACE_NAME_FOR_S1_MME -i $ENB_INTERFACE_NAME_FOR_S1U -w $PCAP_LOG_FILE &

if [ x$hard_real_time != "xyes" ]; then
    echo_warning "USER MODE"
    make --directory=$OPENAIR_TARGETS/SIMU/USER $MAKE_LTE_ACCESS_STRATUM_TARGET -j`grep -c ^processor /proc/cpuinfo ` || exit 1
    bash_exec "ip route add 239.0.0.160/28 dev $EMULATION_DEV_INTERFACE"
    gdb --args $OPENAIR_TARGETS/SIMU/USER/oaisim -a  -l9 -u0 -b1 -M0 -p2  -g1 -D $EMULATION_DEV_ADDRESS -K $ITTI_LOG_FILE --enb-conf $CONFIG_FILE_ENB 2>&1 | tee $STDOUT_LOG_FILE 
else
    echo_warning "HARD REAL TIME MODE"
    PATH=$PATH:/usr/realtime/bin
    make --directory=$OPENAIR_TARGETS/RTAI/USER drivers  || exit 1
    make --directory=$OPENAIR_TARGETS/RTAI/USER $MAKE_LTE_ACCESS_STRATUM_TARGET_RT -j`grep -c ^processor /proc/cpuinfo ` || exit 1
    
    if [ ! -f /tmp/init_rt_done.tmp ]; then
        insmod /usr/realtime/modules/rtai_hal.ko     > /dev/null 2>&1
        insmod /usr/realtime/modules/rtai_sched.ko   > /dev/null 2>&1
        insmod /usr/realtime/modules/rtai_sem.ko     > /dev/null 2>&1
        insmod /usr/realtime/modules/rtai_fifos.ko   > /dev/null 2>&1
        insmod /usr/realtime/modules/rtai_mbx.ko     > /dev/null 2>&1
        echo "1" > /sys/bus/pci/rescan
        touch /tmp/init_rt_done.tmp
        chmod 666 /tmp/init_rt_done.tmp
    fi
    
    cd $OPENAIR_TARGETS/RTAI/USER
    ./init_exmimo2.sh
    ./lte-softmodem -K $ITTI_LOG_FILE -O $CONFIG_FILE_ENB 2>&1
    #cat /dev/rtf62 > $STDOUT_LOG_FILE
    cd $THIS_SCRIPT_PATH
fi

pkill tshark

cat $STDOUT_LOG_FILE |  grep -v '[PHY]' | grep -v '[MAC]' | grep -v '[EMU]' | \
                        grep -v '[OCM]' | grep -v '[OMG]' | \
                        grep -v 'RLC not configured' | grep -v 'check if serving becomes' | \
                        grep -v 'mac_rrc_data_req'   | grep -v 'BCCH request =>' > $STDOUT_LOG_FILE.filtered
