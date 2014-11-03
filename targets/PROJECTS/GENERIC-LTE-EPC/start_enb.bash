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

###########################################################
# Parameters
###########################################################
declare EMULATION_DEV_INTERFACE="eth1"
declare EMULATION_MULTICAST_GROUP=1

declare MAKE_LTE_ACCESS_STRATUM_TARGET="oaisim DEBUG=1 ENABLE_ITTI=1 USE_MME=R10 LINK_PDCP_TO_GTPV1U=1 NAS=1 SECU=1 RRC_MSG_PRINT=1"
declare MAKE_LTE_ACCESS_STRATUM_TARGET_RTAI="lte-softmodem DEBUG=1  RTAI=1 HARD_RT=1 EXMIMO=1  ENABLE_ITTI=1 USE_MME=R10 LINK_PDCP_TO_GTPV1U=1 DISABLE_XER_PRINT=1 SECU=1 RRC_MSG_PRINT=1 "
declare MAKE_LTE_ACCESS_STRATUM_TARGET_LINUX_KERNEL="lte-softmodem DEBUG=1 RTAI=0 EXMIMO=1 ENABLE_ITTI=1 USE_MME=R10 LINK_PDCP_TO_GTPV1U=1 DISABLE_XER_PRINT=1 SECU=1 RRC_MSG_PRINT=1 "


###########################################################
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
. $THIS_SCRIPT_PATH/utils.bash
. $THIS_SCRIPT_PATH/interfaces.bash
. $THIS_SCRIPT_PATH/networks.bash
###########################################################
if [ $# -eq 1 ]; then
    declare -x CONFIG_FILE_DIR=$1
    if [ ! -d $CONFIG_FILE_DIR ]; then
        echo_error "ERROR while invoking this script, as first argument to this script you have to provide the path to a directory (./CONF/VLAN.VIRTUAL.$HOSTNAME for example) containing valid epc and enb config files"
        exit 1
    fi
else
    echo_error "ERROR while invoking this script, as first argument to this script you have to provide the path to a directory  (./CONF/VLAN.VIRTUAL.$HOSTNAME for example) containing valid epc and enb config files"
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
           real_time"

VARIABLES=$(echo $VARIABLES | sed -e 's/\\r//g')
VARIABLES=$(echo $VARIABLES | tr -d ' ')
cat $CONFIG_FILE_ENB | grep -w "$VARIABLES"| tr -d " " | tr -d ";" > /tmp/source.txt
source /tmp/source.txt

declare ENB_IPV4_NETMASK_FOR_S1_MME=$(       echo $ENB_IPV4_ADDRESS_FOR_S1_MME        | cut -f2 -d '/')
declare ENB_IPV4_NETMASK_FOR_S1U=$(          echo $ENB_IPV4_ADDRESS_FOR_S1U        | cut -f2 -d '/')

ENB_IPV4_ADDRESS_FOR_S1_MME=$(               echo $ENB_IPV4_ADDRESS_FOR_S1_MME        | cut -f1 -d '/')
ENB_IPV4_ADDRESS_FOR_S1U=$(                  echo $ENB_IPV4_ADDRESS_FOR_S1U           | cut -f1 -d '/')


##################################################
# LAUNCH eNB executable
##################################################
pkill oaisim
pkill tshark

if [ ! -d $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME ]; then 
    mkdir -m 777 -p $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME
fi

if [ x$real_time == "xemulation" ]; then
    ITTI_LOG_FILE=$THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/itti_enb_ue.$HOSTNAME.log
    touch $ITTI_LOG_FILE
    #rotate_log_file $ITTI_LOG_FILE
    
    STDOUT_LOG_FILE=$THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/stdout_enb_ue.$HOSTNAME.log
    #rotate_log_file $STDOUT_LOG_FILE
    #rotate_log_file $STDOUT_LOG_FILE.filtered
    touch $STDOUT_LOG_FILE
    
    PCAP_LOG_FILE=$THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/tshark_enb_ue.$HOSTNAME.pcap
    #rotate_log_file $PCAP_LOG_FILE
    touch $PCAP_LOG_FILE
else 
    ITTI_LOG_FILE=$THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/itti_enb_rf.$HOSTNAME.log
    #rotate_log_file $ITTI_LOG_FILE
    touch $ITTI_LOG_FILE
    
    STDOUT_LOG_FILE=$THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/stdout_enb_rf.$HOSTNAME.log
    #rotate_log_file $STDOUT_LOG_FILE
    #rotate_log_file $STDOUT_LOG_FILE.filtered
    touch $STDOUT_LOG_FILE
    
    PCAP_LOG_FILE=$THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/tshark_enb_rf.$HOSTNAME.pcap
    #rotate_log_file $PCAP_LOG_FILE
    touch $PCAP_LOG_FILE
fi


cd $THIS_SCRIPT_PATH

bash_exec "ethtool -A $ENB_INTERFACE_NAME_FOR_S1_MME autoneg off rx off tx off"
bash_exec "ethtool -G $ENB_INTERFACE_NAME_FOR_S1_MME rx 4096 tx 4096"
bash_exec "ethtool -C $ENB_INTERFACE_NAME_FOR_S1_MME rx-usecs 3"
bash_exec "ifconfig $ENB_INTERFACE_NAME_FOR_S1_MME txqueuelen 1000"

if [ x$ENB_INTERFACE_NAME_FOR_S1_MME != x$ENB_INTERFACE_NAME_FOR_S1U ]; then 
    nohup tshark -i $ENB_INTERFACE_NAME_FOR_S1_MME -w $PCAP_LOG_FILE &
else
    bash_exec "ethtool -A $ENB_INTERFACE_NAME_FOR_S1U autoneg off rx off tx off"
    bash_exec "ethtool -G $ENB_INTERFACE_NAME_FOR_S1U rx 4096 tx 4096"
    bash_exec "ethtool -C $ENB_INTERFACE_NAME_FOR_S1U rx-usecs 3"
    bash_exec "ifconfig $ENB_INTERFACE_NAME_FOR_S1U txqueuelen 1000"

    nohup tshark -i $ENB_INTERFACE_NAME_FOR_S1_MME -i $ENB_INTERFACE_NAME_FOR_S1U -w $PCAP_LOG_FILE &
fi


if [ x$real_time == "xemulation" ]; then
    echo_warning "USER MODE"
    make --directory=$OPENAIR_TARGETS/SIMU/USER $MAKE_LTE_ACCESS_STRATUM_TARGET -j`grep -c ^processor /proc/cpuinfo ` || exit 1
    bash_exec "ip route add 239.0.0.160/28 dev $EMULATION_DEV_INTERFACE"
    gdb --args $OPENAIR_TARGETS/SIMU/USER/oaisim -a  -l9 -u0 -b1 -M0 -p2  -g$EMULATION_MULTICAST_GROUP -D $EMULATION_DEV_ADDRESS -K $ITTI_LOG_FILE --enb-conf $CONFIG_FILE_ENB 2>&1 | tee $STDOUT_LOG_FILE 
else
    if [ x$real_time == "xrtai" ]; then
        echo_warning "HARD REAL TIME MODE"
        PATH=$PATH:/usr/realtime/bin

        #make --directory=$OPENAIR_TARGETS/RT/USER drivers  || exit 1
        # 2 lines below replace the line above
        cd $OPENAIR_TARGETS/ARCH/EXMIMO/DRIVER/eurecom && make clean && make   || exit 1
        cd $OPENAIR_TARGETS/ARCH/EXMIMO/USERSPACE/OAI_FW_INIT && make clean && make   || exit 1
        cd $THIS_SCRIPT_PATH

        make --directory=$OPENAIR_TARGETS/RT/USER $MAKE_LTE_ACCESS_STRATUM_TARGET_RTAI -j`grep -c ^processor /proc/cpuinfo ` || exit 1

        if [ ! -f /tmp/init_rt_done.tmp ]; then
            echo_warning "STARTING REAL TIME (RTAI)"
            insmod /usr/realtime/modules/rtai_hal.ko     > /dev/null 2>&1
            insmod /usr/realtime/modules/rtai_sched.ko   > /dev/null 2>&1
            insmod /usr/realtime/modules/rtai_sem.ko     > /dev/null 2>&1
            insmod /usr/realtime/modules/rtai_fifos.ko   > /dev/null 2>&1
            insmod /usr/realtime/modules/rtai_mbx.ko     > /dev/null 2>&1
            touch /tmp/init_rt_done.tmp
            chmod 666 /tmp/init_rt_done.tmp
        else
            echo_warning "REAL TIME FOUND STARTED (RTAI)"
        fi

        cd $OPENAIR_TARGETS/RT/USER
        bash ./init_exmimo2.sh
        echo_warning "STARTING SOFTMODEM..."
        #cat /dev/rtf62 > $STDOUT_LOG_FILE &
        gdb --args ./lte-softmodem -K $ITTI_LOG_FILE -V  -O $CONFIG_FILE_ENB  2>&1
        cd $THIS_SCRIPT_PATH
    else
        if [ x$real_time == "xlinux-kernel" ]; then
            echo_warning "LINUX_KERNEL MODE"

            #make --directory=$OPENAIR_TARGETS/RT/USER drivers  || exit 1
            # 2 lines below replace the line above
            cd $OPENAIR_TARGETS/ARCH/EXMIMO/DRIVER/eurecom && make clean && make   || exit 1
            cd $OPENAIR_TARGETS/ARCH/EXMIMO/USERSPACE/OAI_FW_INIT && make clean && make   || exit 1
            cd $THIS_SCRIPT_PATH

            make --directory=$OPENAIR_TARGETS/RT/USER $MAKE_LTE_ACCESS_STRATUM_TARGET_LINUX_KERNEL -j`grep -c ^processor /proc/cpuinfo ` || exit 1

            cd $OPENAIR_TARGETS/RT/USER
            bash ./init_exmimo2.sh
            echo_warning "STARTING SOFTMODEM..."
            touch .gdbinit_enb
            echo "file $OPENAIR_TARGETS/RT/USER/lte-softmodem" > ~/.gdbinit_enb
            echo "set args -K $ITTI_LOG_FILE -V  -O $CONFIG_FILE_ENB" >> ~/.gdbinit_enb
            echo "run" >> ~/.gdbinit_enb
            gdb -nh -x ~/.gdbinit_enb 2>&1 
            #> $STDOUT_LOG_FILE
            
            cd $THIS_SCRIPT_PATH
        fi
    fi
fi

pkill tshark

#cat $STDOUT_LOG_FILE |  grep -v '[PHY]' | grep -v '[MAC]' | grep -v '[EMU]' | \
#                        grep -v '[OCM]' | grep -v '[OMG]' | \
#                        grep -v 'RLC not configured' | grep -v 'check if serving becomes' | \
#                        grep -v 'mac_rrc_data_req'   | grep -v 'BCCH request =>' > $STDOUT_LOG_FILE.filtered
