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
# file start_epc.bash
# brief
# author Lionel Gauthier
# company Eurecom
# email: lionel.gauthier@eurecom.fr
#

#########################################
# INPUT OF THIS SCRIPT:
# THE DIRECTORY WHERE ARE LOCATED THE CONFIGURATION FILES
#########################################
# This script start MME+S/P-GW (all in one executable, on one host) 
# Depending on configuration files, it can be instanciated a virtual switch 
# setting or a VLAN setting.
#
###########################################################################################################################
#                                    VIRTUAL BOX SETTING
###########################################################################################################################
#   NETWORK SETTING AT EURECOM IN EXPERIMENTAL NETWORK (192.168.12.X)
#
#
#             INTERNET GW 192.168.12.100      
#                                  |
#                                  |
#                 192.168.12.X/24  | 
# +-----------+----------------+---+--+---------------------------------------+
# | COMPUTER 1|                | eth0 |                                       |
# +-----------+                +---+--+                                       |
# |                                |                                          |
# |                                |                                          |
# |    ROUTER                +-----+------+                                   |
# |                          |MASQUERADING|                                   |
# |                          |FORWARDING  |                                   |
# |                          +-----+------+                                   |
# |                                |                                          |
# |                                |                                          |
# |                                |                                          |
# |                                |                                          |
# |                                |                                          |
# |                            +---+--+                                       |
# |                            | eth1 |                                       |
# +----------------------------+---+--+---------------------------------------+
#                                  |router.eur
#                   11 VLANS       |                 INTERNET GW 192.168.12.100
#                  ids=[5..15]     |                            |
#                  192.168.13.X/24 |                            |
# +-----------+----------------+---+--+---------------------+---+--+----------+
# | COMPUTER 2|          SGI   | eth1 | Physical            | eth0 |          |
# +-----------+                +-+--+-+ Interface           +------+          |
# |                              |  |  'HOST_BRIDGED_IF_NAME'                 |
# |                              |  |                                         |
# |                              |  |                 +-----------+           |   
# |                              |  |          +------+    HSS    |           |   
# |                              |  +----------+ eth0 |   (VM)    |           |   
# |                              |             +------+           |           |
# |                              |                    |           |           |
# |                              |                    +-+------+--+           |
# |                              |                      |eth1  | hss.eur      |
# |                virtual box   |                      +--+---+              |
# |           +------------------+                         | virtual box      |
# |           |    bridged network                         | host-only        |
# |           |                                            | network          |
# |           |                                            | 192.168.57/24    |
# |           |                                            |                  |
# |        +--+---+                                     +--+-----+            |
# |        |eth0  |                                     |vboxnet1|            |
# |      +-+------+--+192.168.56.101      192.168.56.1+-+--------++           |
# |      |  eNB 0    +------+   virtual box  +--------+  MME      |           |
# |      |  (VM)     |eth1  +----------------+vboxnet0|  S+P/GW   |           |
# |      |           +------+   host-only    +--------+(execu. on |           |
# |      |           |          network               |COMPUTER2) |           |
# |      |           |       192.168.56/24            |           |           |
# |      | LTE eNB 1 |                                |           |           |
# |      | LTE UEs   |                                |           |           |
# |      +-----------+                                +-----------+           |
# |                                                                           |
# |                                                                           |
# +---------------------------------------------------------------------------+
#
###########################################################################################################################
#                                    VLAN SETTING
###########################################################################################################################
#   each box is a host                                                        hss.eur
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
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
. $THIS_SCRIPT_PATH/utils.bash
. $THIS_SCRIPT_PATH/interfaces.bash
. $THIS_SCRIPT_PATH/networks.bash
###########################################################

control_c()
# run if user hits control-c
{
  pkill -9 tshark
  pkill -9 oai_epc
  rotate_log_file $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$ITTI_LOG_FILE
  rotate_log_file $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$STDOUT_LOG_FILE
  rotate_log_file $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$PCAP_S1C_LOG_FILE
  rotate_log_file $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$PCAP_S6A_LOG_FILE  echo -en "\n*** Exiting ***\n"

  exit $?
}



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



check_install_epc_software


##################################
# Get or set OBJ DIR and compile #
##################################
# TEST IF EXIST
cd $OPENAIRCN_DIR
OBJ_DIR=`find . -maxdepth 1 -type d -iname obj*`
if [ ! -n "$OBJ_DIR" ]
then
    OBJ_DIR="objs"
    bash_exec "mkdir -m 777 ./$OBJ_DIR"
    echo_success "Created $OBJ_DIR directory"
else
    OBJ_DIR=`basename $OBJ_DIR`
fi
if [ ! -f $OBJ_DIR/Makefile ]
then
    if [ ! -n "m4" ]
    then
        mkdir -m 777 m4
    fi
    echo_success "Invoking autogen"
    bash_exec "./autogen.sh"
    cd ./$OBJ_DIR
    echo_success "Invoking configure"
    ../configure --enable-standalone-epc --enable-raw-socket-for-sgi  LDFLAGS=-L/usr/local/lib
else
    cd ./$OBJ_DIR
fi

pkill oai_epc
pkill tshark

if [ -f Makefile ]
then
    echo_success "Compiling..."
    make -j `cat /proc/cpuinfo | grep processor | wc -l`
    if [ $? -ne 0 ]; then
        echo_error "Build failed, exiting"
        exit 1
    fi
else
    echo_error "Configure failed, exiting"
    exit 1
fi


cd $THIS_SCRIPT_PATH
#######################################################
# FIND CONFIG FILE
#######################################################
SEARCHED_CONFIG_FILE_EPC="epc*.conf"
CONFIG_FILE_EPC=`find $CONFIG_FILE_DIR -iname $SEARCHED_CONFIG_FILE_EPC`
if [ -f $CONFIG_FILE_EPC ]; then
    echo_warning "EPC config file found is now $CONFIG_FILE_EPC"
else
    echo_error "EPC config file not found, exiting (searched for $SEARCHED_CONFIG_FILE_EPC)"
    exit 1
fi

SEARCHED_CONFIG_FILE_ENB="enb*.conf"
CONFIG_FILE_ENB=`find $CONFIG_FILE_DIR -iname $SEARCHED_CONFIG_FILE_ENB`
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
           MME_INTERFACE_NAME_FOR_S1_MME\|\
           MME_IPV4_ADDRESS_FOR_S1_MME\|\
           MME_INTERFACE_NAME_FOR_S11_MME\|\
           MME_IPV4_ADDRESS_FOR_S11_MME\|\
           SGW_INTERFACE_NAME_FOR_S11\|\
           SGW_IPV4_ADDRESS_FOR_S11\|\
           SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP\|\
           SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP\|\
           SGW_INTERFACE_NAME_FOR_S5_S8_UP\|\
           SGW_IPV4_ADDRESS_FOR_S5_S8_UP\|\
           PGW_INTERFACE_NAME_FOR_S5_S8\|\
           PGW_IPV4_ADDRESS_FOR_S5_S8\|\
           PGW_INTERFACE_NAME_FOR_SGI\|\
           PGW_IPV4_ADDR_FOR_SGI"

VARIABLES=$(echo $VARIABLES | sed -e 's/\\r//g')
VARIABLES=$(echo $VARIABLES | tr -d ' ')
cat $CONFIG_FILE_EPC | grep -w "$VARIABLES"| tr -d " " | tr -d ";" > /tmp/source.txt
cat $CONFIG_FILE_ENB | grep -w "$VARIABLES"| tr -d " " | tr -d ";" >> /tmp/source.txt
source /tmp/source.txt

declare ENB_IPV4_NETMASK_FOR_S1_MME=$(       echo $ENB_IPV4_ADDRESS_FOR_S1_MME        | cut -f2 -d '/')
declare ENB_IPV4_NETMASK_FOR_S1U=$(          echo $ENB_IPV4_ADDRESS_FOR_S1U           | cut -f2 -d '/')
declare MME_IPV4_NETMASK_FOR_S1_MME=$(       echo $MME_IPV4_ADDRESS_FOR_S1_MME        | cut -f2 -d '/')
declare MME_IPV4_NETMASK_FOR_S11_MME=$(      echo $MME_IPV4_ADDRESS_FOR_S11_MME       | cut -f2 -d '/')
declare SGW_IPV4_NETMASK_FOR_S11=$(          echo $SGW_IPV4_ADDRESS_FOR_S11           | cut -f2 -d '/')
declare SGW_IPV4_NETMASK_FOR_S1U_S12_S4_UP=$(echo $SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP | cut -f2 -d '/')
declare SGW_IPV4_NETMASK_FOR_S5_S8_UP=$(     echo $SGW_IPV4_ADDRESS_FOR_S5_S8_UP      | cut -f2 -d '/')
declare PGW_IPV4_NETMASK_FOR_S5_S8=$(        echo $PGW_IPV4_ADDRESS_FOR_S5_S8         | cut -f2 -d '/')
declare PGW_IPV4_NETMASK_FOR_SGI=$(          echo $PGW_IPV4_ADDR_FOR_SGI              | cut -f2 -d '/')

ENB_IPV4_ADDRESS_FOR_S1_MME=$(               echo $ENB_IPV4_ADDRESS_FOR_S1_MME        | cut -f1 -d '/')
ENB_IPV4_ADDRESS_FOR_S1U=$(                  echo $ENB_IPV4_ADDRESS_FOR_S1U           | cut -f1 -d '/')
MME_IPV4_ADDRESS_FOR_S1_MME=$(               echo $MME_IPV4_ADDRESS_FOR_S1_MME        | cut -f1 -d '/')
MME_IPV4_ADDRESS_FOR_S11_MME=$(              echo $MME_IPV4_ADDRESS_FOR_S11_MME       | cut -f1 -d '/')
SGW_IPV4_ADDRESS_FOR_S11=$(                  echo $SGW_IPV4_ADDRESS_FOR_S11           | cut -f1 -d '/')
SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP=$(        echo $SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP | cut -f1 -d '/')
SGW_IPV4_ADDRESS_FOR_S5_S8_UP=$(             echo $SGW_IPV4_ADDRESS_FOR_S5_S8_UP      | cut -f1 -d '/')
PGW_IPV4_ADDRESS_FOR_S5_S8=$(                echo $PGW_IPV4_ADDRESS_FOR_S5_S8         | cut -f1 -d '/')
PGW_IPV4_ADDR_FOR_SGI=$(                     echo $PGW_IPV4_ADDR_FOR_SGI              | cut -f1 -d '/')

#is_vlan_interface $MME_INTERFACE_NAME_FOR_S1_MME  \
#                  $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP
                  

#if [ $? -eq 1 ]; then
#        echo_success "Found open VLAN network configuration"
#        clean_epc_vlan_network
#        build_mme_spgw_vlan_network
#else
#   clean_epc_vlan_network
#   create_sgi_vlans
#fi
get_mac_router

##################################################
# LAUNCH MME + S+P-GW executable
##################################################

cd $THIS_SCRIPT_PATH
if [ ! -d "OUTPUT/"$HOSTNAME ]
then
    bash_exec "mkdir -m 777 ./OUTPUT/$HOSTNAME"
    echo_success "Created OUTPUT/$HOSTNAME directory"
fi


ITTI_LOG_FILE=itti_mme.$HOSTNAME.log
rotate_log_file $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$ITTI_LOG_FILE

STDOUT_LOG_FILE=stdout_mme.$HOSTNAME.log
rotate_log_file $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$STDOUT_LOG_FILE

PCAP_S1C_LOG_FILE=tshark_mme_s1c.$HOSTNAME.pcap
rotate_log_file $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$PCAP_S1C_LOG_FILE

PCAP_S6A_LOG_FILE=tshark_mme_s6a.$HOSTNAME.pcap
rotate_log_file $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$PCAP_S6A_LOG_FILE

cd $OPENAIRCN_DIR/$OBJ_DIR

trap control_c SIGINT

#nohup tshark -i $MME_INTERFACE_NAME_FOR_S1_MME -w $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$PCAP_S1C_LOG_FILE &
#nohup tshark -i $MME_INTERFACE_NAME_FOR_S6A -w $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$PCAP_S6A_LOG_FILE &


$OPENAIRCN_DIR/$OBJ_DIR/OAI_EPC/oai_epc -K $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$ITTI_LOG_FILE -c $THIS_SCRIPT_PATH/$CONFIG_FILE_EPC  2>&1 | tee $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$STDOUT_LOG_FILE 

