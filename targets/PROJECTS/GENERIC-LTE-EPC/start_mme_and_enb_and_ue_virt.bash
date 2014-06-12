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
# file start_mme_and_enb_and_ue.bash
# brief
# author Lionel Gauthier
# company Eurecom
# email: lionel.gauthier@eurecom.fr
###########################################
# INPUT OF THIS SCRIPT:
# THE DIRECTORY WHERE ARE LOCATED THE CONFIGURATION FILES
#########################################
# This script start  MME+ENB+UE (ENB+UE in one executable, MME in one executable, all on one host)
#
###########################################################################################################################
#                                    TUN SETTING
###########################################################################################################################
#                                                                           hss.eur
#                                                                             |
#        +-----------+                                +-----------+           v     +----------+
#        |    UE     +------+                  +------+    MME    +-----+     +-----+   HSS    |
#        |    +      | tun10+------------------+tun11 |           |tun61+-----+tun60|          |
#        |   eNB     +------+                  +------+           +-----+     +-----+          |
#        |           | tun20+-------+                 |           |                 +----------+
#        |           +------+       |                 +-+-------+-+
#        |           |              |                 | | tun111| |
#        |           |              |                 | +---+---+ |
#        |           |              |             (optional)|     |
#        +-----------+              |                 | +---+---+ |
#                                   |                 | | tun110| |          router.eur
#                                   |                 +-+-------+-+              |   +--------------+
#                                   |                 |  S+P-GW   |              v   |   ROUTER     |
#                                   |          +------+           +-------+     +----+              +----+
#                                   +----------+tun21 |           |sgi    +-...-+    |              |    +---...Internet
#                                              +------+           +-------+     +----+              +----+
#                                                     |           |      11 VLANS    |              |
#                                                     +-----------+   ids=[5..15]    +--------------+

###########################################################
# Parameters
###########################################################
declare MAKE_LTE_ACCESS_STRATUM_TARGET="oaisim DEBUG=1 ENABLE_ITTI=1 USE_MME=R10 LINK_PDCP_TO_GTPV1U=1 NAS=1 SECU=1 Rel10=1"
declare MAKE_IP_DRIVER_TARGET="ue_ip.ko"
declare IP_DRIVER_NAME="ue_ip"
declare LTEIF="oip1"
declare UE_IPv4="10.0.0.8"
declare UE_IPv6="2001:1::8"
declare UE_IPv6_CIDR=$UE_IPv6"/64"
declare UE_IPv4_CIDR=$UE_IPv4"/24"
declare MME_ITTI_LOG_FILE=./itti_mme.$HOSTNAME.log
declare MME_STDOUT_LOG_FILE=./stdout_mme.$HOSTNAME.log
declare MME_PCAP_LOG_FILE=./tshark_s1_mme.$HOSTNAME.pcap
declare ITTI_LOG_FILE=./itti_enb_ue.$HOSTNAME.log
declare STDOUT_LOG_FILE=./stdout_enb_ue.$HOSTNAME.log
declare PCAP_LOG_FILE=./tshark_enb.$HOSTNAME.pcap
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


#######################################################
# CHECK MISC SOFTWARES AND LIBS
#######################################################
#check_install_epc_software

######################################
# compile EPC #
######################################
compile_epc

######################################
# compile UE + eNB #
######################################
#------------------------------------
# USIM, NVRAM files
#------------------------------------
export NVRAM_DIR=$THIS_SCRIPT_PATH

if [ ! -f $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/ue_data ]; then
    make --directory=$OPENAIRCN_DIR/NAS/EURECOM-NAS veryveryclean
    make --directory=$OPENAIRCN_DIR/NAS/EURECOM-NAS PROCESS=UE
    rm .ue.nvram
fi
if [ ! -f $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/usim_data ]; then
    make --directory=$OPENAIRCN_DIR/NAS/EURECOM-NAS veryveryclean
    make --directory=$OPENAIRCN_DIR/NAS/EURECOM-NAS PROCESS=UE
    rm .usim.nvram
fi
if [ ! -f .ue.nvram ]; then
    # generate .ue_emm.nvram .ue.nvram
    $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/ue_data --gen
fi

if [ ! -f .usim.nvram ]; then
    # generate .usim.nvram
    $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/usim_data --gen
fi
$OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/ue_data --print
$OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/usim_data --print

cecho "make $MAKE_IP_DRIVER_TARGET $MAKE_LTE_ACCESS_STRATUM_TARGET ....." $green
make --directory=$OPENAIR2_DIR $MAKE_IP_DRIVER_TARGET || exit 1
#make --directory=$OPENAIR_TARGETS/SIMU/USER $MAKE_LTE_ACCESS_STRATUM_TARGET -j`grep -c ^processor /proc/cpuinfo ` || exit 1
make --debug=b --directory=$OPENAIR_TARGETS/SIMU/USER $MAKE_LTE_ACCESS_STRATUM_TARGET || exit 1

cd $THIS_SCRIPT_PATH
#######################################################
# FIND CONFIG FILES
#######################################################
SEARCHED_CONFIG_FILE_ENB="enb*_$HOSTNAME.conf"
CONFIG_FILE_ENB=`find $CONFIG_FILE_DIR -iname $SEARCHED_CONFIG_FILE_ENB`
if [ a$CONFIG_FILE_ENB != a ]; then
    if [ -f $CONFIG_FILE_ENB ]; then
        echo_warning "eNB config file found is now $CONFIG_FILE_ENB"
    else
        echo_error "eNB config file not found, exiting (searched for $SEARCHED_CONFIG_FILE_ENB in $CONFIG_FILE_DIR)"
        exit 1
    fi
else
    echo_error "eNB config file not found, exiting (searched for $SEARCHED_CONFIG_FILE_ENB in $CONFIG_FILE_DIR)"
    exit 1
fi

SEARCHED_CONFIG_FILE_EPC="epc*_$HOSTNAME.conf"
CONFIG_FILE_EPC=`find $CONFIG_FILE_DIR -iname $SEARCHED_CONFIG_FILE_EPC`
if [ a$CONFIG_FILE_EPC != a ]; then
    if [ -f $CONFIG_FILE_EPC ]; then
        echo_warning "EPC config file found is now $CONFIG_FILE_EPC"
    else
        echo_error "EPC config file not found, exiting (searched for $SEARCHED_CONFIG_FILE_EPC in $CONFIG_FILE_DIR)"
        exit 1
    fi
else
    echo_error "EPC config file not found, exiting (searched for $SEARCHED_CONFIG_FILE_EPC in $CONFIG_FILE_DIR)"
    exit 1
fi
#######################################################
# SOURCE CONFIG FILES for MME AND eNB
#######################################################
echo_success "PARSING CONFIG FILES..."

rm -f /tmp/source.txt
VARIABLES="
           ENB_INTERFACE_NAME_FOR_S1_MME\|\
           ENB_IPV4_ADDRESS_FOR_S1_MME\|\
           ENB_INTERFACE_NAME_FOR_S1U\|\
           ENB_IPV4_ADDRESS_FOR_S1U\|\
           MME_INTERFACE_NAME_FOR_S1_MME\|\
           MME_IPV4_ADDRESS_FOR_S1_MME\|\
           MME_INTERFACE_NAME_FOR_S11_MME\|\
           MME_IPV4_ADDRESS_FOR_S11_MME\|\
           MME_INTERFACE_NAME_FOR_S6A\|\
           MME_IPV4_ADDRESS_FOR_S6A\|\
           SGW_INTERFACE_NAME_FOR_S11\|\
           SGW_IPV4_ADDRESS_FOR_S11\|\
           SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP\|\
           SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP\|\
           SGW_INTERFACE_NAME_FOR_S5_S8_UP\|\
           SGW_IPV4_ADDRESS_FOR_S5_S8_UP\|\
           PGW_INTERFACE_NAME_FOR_S5_S8\|\
           PGW_IPV4_ADDRESS_FOR_S5_S8\|\
           PGW_INTERFACE_NAME_FOR_SGI\|\
           PGW_IPV4_ADDRESS_FOR_SGI\|\
           HSS_INTERFACE_NAME_FOR_S6A\|\
           HSS_IPV4_ADDRESS_FOR_S6A"

VARIABLES=$(echo $VARIABLES | sed -e 's/\\r//g')
VARIABLES=$(echo $VARIABLES | tr -d ' ')
cat $CONFIG_FILE_EPC | grep -w "$VARIABLES"| tr -d " " | tr -d ";" > /tmp/source.txt
cat $CONFIG_FILE_ENB | grep -w "$VARIABLES"| tr -d " " | tr -d ";" >> /tmp/source.txt
source /tmp/source.txt

declare ENB_IPV4_NETMASK_FOR_S1_MME=$(       echo $ENB_IPV4_ADDRESS_FOR_S1_MME        | cut -f2 -d '/')
declare ENB_IPV4_NETMASK_FOR_S1U=$(          echo $ENB_IPV4_ADDRESS_FOR_S1U           | cut -f2 -d '/')
declare MME_IPV4_NETMASK_FOR_S1_MME=$(       echo $MME_IPV4_ADDRESS_FOR_S1_MME        | cut -f2 -d '/')
declare MME_IPV4_NETMASK_FOR_S11_MME=$(      echo $MME_IPV4_ADDRESS_FOR_S11_MME       | cut -f2 -d '/')
declare MME_IPV4_NETMASK_FOR_S6A=$(          echo $MME_IPV4_ADDRESS_FOR_S6A           | cut -f2 -d '/')
declare SGW_IPV4_NETMASK_FOR_S11=$(          echo $SGW_IPV4_ADDRESS_FOR_S11           | cut -f2 -d '/')
declare SGW_IPV4_NETMASK_FOR_S1U_S12_S4_UP=$(echo $SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP | cut -f2 -d '/')
declare SGW_IPV4_NETMASK_FOR_S5_S8_UP=$(     echo $SGW_IPV4_ADDRESS_FOR_S5_S8_UP      | cut -f2 -d '/')
declare PGW_IPV4_NETMASK_FOR_S5_S8=$(        echo $PGW_IPV4_ADDRESS_FOR_S5_S8         | cut -f2 -d '/')
declare PGW_IPV4_NETMASK_FOR_SGI=$(          echo $PGW_IPV4_ADDRESS_FOR_SGI           | cut -f2 -d '/')
declare HSS_IPV4_NETMASK_FOR_S6A=$(          echo $HSS_IPV4_ADDRESS_FOR_S6A           | cut -f2 -d '/')

ENB_IPV4_ADDRESS_FOR_S1_MME=$(               echo $ENB_IPV4_ADDRESS_FOR_S1_MME        | cut -f1 -d '/')
ENB_IPV4_ADDRESS_FOR_S1U=$(                  echo $ENB_IPV4_ADDRESS_FOR_S1U           | cut -f1 -d '/')
MME_IPV4_ADDRESS_FOR_S1_MME=$(               echo $MME_IPV4_ADDRESS_FOR_S1_MME        | cut -f1 -d '/')
MME_IPV4_ADDRESS_FOR_S11_MME=$(              echo $MME_IPV4_ADDRESS_FOR_S11_MME       | cut -f1 -d '/')
MME_IPV4_ADDRESS_FOR_S6A=$(                  echo $MME_IPV4_ADDRESS_FOR_S6A           | cut -f1 -d '/')
SGW_IPV4_ADDRESS_FOR_S11=$(                  echo $SGW_IPV4_ADDRESS_FOR_S11           | cut -f1 -d '/')
SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP=$(        echo $SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP | cut -f1 -d '/')
SGW_IPV4_ADDRESS_FOR_S5_S8_UP=$(             echo $SGW_IPV4_ADDRESS_FOR_S5_S8_UP      | cut -f1 -d '/')
PGW_IPV4_ADDRESS_FOR_S5_S8=$(                echo $PGW_IPV4_ADDRESS_FOR_S5_S8         | cut -f1 -d '/')
PGW_IPV4_ADDRESS_FOR_SGI=$(                  echo $PGW_IPV4_ADDRESS_FOR_SGI           | cut -f1 -d '/')
HSS_IPV4_ADDRESS_FOR_S6A=$(                  echo $HSS_IPV4_ADDRESS_FOR_S6A           | cut -f1 -d '/')


#######################################################
# BUILD NETWORK
#######################################################
clean_tun_network
build_epc_tun_network
test_tun_network

##################################################
# LAUNCH HSS
##################################################
# TO DO

##################################################
# LAUNCH MME executable
##################################################
cd $THIS_SCRIPT_PATH
if [ ! -d "OUTPUT/"$HOSTNAME ]
then
    bash_exec "mkdir -p -m 777 ./OUTPUT/$HOSTNAME"
    echo_success "Created OUTPUT/$HOSTNAME directory"
fi

cd OUTPUT/$HOSTNAME
rotate_log_file $MME_ITTI_LOG_FILE
rotate_log_file $MME_STDOUT_LOG_FILE
rotate_log_file $MME_PCAP_LOG_FILE

cd $OPENAIRCN_DIR/$OBJ_DIR

nohup tshark -i MME_INTERFACE_NAME_FOR_S1_MME -w $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$MME_PCAP_LOG_FILE &

#gdb --args $OPENAIRCN_DIR/$OBJ_DIR/OAI_EPC/oai_epc -K $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$MME_ITTI_LOG_FILE -c $THIS_SCRIPT_PATH/$CONFIG_FILE_EPC  2>&1 | tee $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$MME_STDOUT_LOG_FILE 
nohup xterm -hold -e gdb --args $OPENAIRCN_DIR/$OBJ_DIR/OAI_EPC/oai_epc -K $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$MME_ITTI_LOG_FILE -c $THIS_SCRIPT_PATH/$CONFIG_FILE_EPC  2>&1 | tee $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$MME_STDOUT_LOG_FILE & 


##################################################
# LAUNCH eNB + UE executable
##################################################
echo "Bringup UE interface"
pkill oaisim
bash_exec "rmmod $IP_DRIVER_NAME" > /dev/null 2>&1

bash_exec "insmod  $OPENAIR2_DIR/NETWORK_DRIVER/UE_IP/$IP_DRIVER_NAME.ko"

bash_exec "ip route flush cache"
#bash_exec "ip link set $LTEIF up"
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
fgrep lte /etc/iproute2/rt_tables  > /dev/null 
if [ $? -ne 0 ]; then
    echo "200 lte " >> /etc/iproute2/rt_tables
fi
ip rule add fwmark 5 table lte
ip route add default dev $LTEIF table lte


cd OUTPUT/$HOSTNAME
rotate_log_file $ITTI_LOG_FILE
rotate_log_file $STDOUT_LOG_FILE
rotate_log_file $STDOUT_LOG_FILE.filtered
rotate_log_file $PCAP_LOG_FILE

cd $THIS_SCRIPT_PATH

nohup tshark -i $ENB_INTERFACE_NAME_FOR_S1_MME -i $ENB_INTERFACE_NAME_FOR_S1U -w OUTPUT/$HOSTNAME/$PCAP_LOG_FILE &

nohup xterm -e $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/UserProcess &

gdb --args $OPENAIR_TARGETS/SIMU/USER/oaisim -a -u1 -l9 -K $ITTI_LOG_FILE --enb-conf $CONFIG_FILE_ENB 2>&1 | tee OUTPUT/$HOSTNAME/$STDOUT_LOG_FILE 

pkill tshark

cat OUTPUT/$HOSTNAME/$STDOUT_LOG_FILE |  grep -v '[PHY]' | grep -v '[MAC]' | grep -v '[EMU]' | \
                        grep -v '[OCM]' | grep -v '[OMG]' | \
                        grep -v 'RLC not configured' | grep -v 'check if serving becomes' | \
                        grep -v 'mac_rrc_data_req'   | grep -v 'BCCH request =>' > OUTPUT/$HOSTNAME/$STDOUT_LOG_FILE.filtered
