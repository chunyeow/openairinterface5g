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
#                                    VIRTUAL SWITCH SETTING
###########################################################################################################################
#                                                                           hss.eur
#                                                                             |
#        +-----------+          +------+              +-----------+           v   +----------+
#        |  eNB      +------+   |  ovs | VLAN 1+------+    MME    +----+      +---+   HSS    |
#        |           |cpenb0+------------------+cpmme0|           |    +------+   |          |
#        |           +------+   |bridge|       +------+           +----+      +---+          |
#        |           |upenb0+-------+  |              |           |               +----------+
#        +-----------+------+   |   |  |              +-+-------+-+
#                               |   |  +----------------| s11mme|---+
#                               |   |                   +---+---+   |
#                               |   |             (optional)|       |
#                               |   |                   +---+---+   |
#                               +---|------------------ | s11sgw|---+        router.eur
#                                   |                 +-+-------+-+              |   +--------------+
#                                   |                 |  S+P-GW   |              v   |   ROUTER     |
#                                   |  VLAN2   +------+           +-------+     +----+              +----+
#                                   +----------+upsgw0|           |sgi    +-...-+    |              |    +---...Internet
#                                              +------+           +-------+     +----+              +----+
#                                                     |           |      11 VLANS    |              |
#                                                     +-----------+   ids=[5..15]    +--------------+
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

BRIDGE="vswitch"

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


test_command_install_package "gccxml"   "gccxml" "--force-yes"
test_command_install_package "gcc"      "gcc"      "--force-yes"
test_command_install_package "g++"      "g++"      "--force-yes"
test_command_install_package "automake" "automake" "--force-yes"
test_command_install_package "vconfig"  "vlan" "--force-yes"
test_command_install_package "iptables" "iptables"
test_command_install_package "iperf"    "iperf" "--force-yes"
test_command_install_package "ip"       "iproute"
test_command_install_script   "ovs-vsctl" "$OPENAIRCN_DIR/SCRIPTS/install_openvswitch1.9.0.bash"
test_command_install_package  "tunctl"  "uml-utilities"
test_command_install_package "bison"    "bison"     "--force-yes"
test_command_install_package "flex"     "flex"      "--force-yes"
test_command_install_package "libtool"  "libtool"   "--force-yes"
test_command_install_lib     "/usr/lib/libconfig.so"      "libconfig-dev"  "--force-yes"
#test_command_install_lib     "/usr/lib/libsctp-dev.so"    "libsctp-dev"    "--force-yes"
test_command_install_lib     "/usr/lib/libsctp.so"        "libsctp1"       "--force-yes"
#test_command_install_lib     "/usr/lib/libpthread-stubs0-dev.so" "libpthread-stubs0-dev"    "--force-yes"
if [ ! -d /usr/local/etc/freeDiameter ]
    then
        # This script make certificates also
        cd $OPENAIRCN_DIR/S6A/freediameter && ./install_freediameter.sh
    else
        echo_success "freediameter is installed"
        check_s6a_certificate
fi

test_command_install_script   "asn1c" "$OPENAIRCN_DIR/SCRIPTS/install_asn1c_0.9.24.modified.bash"

# One mor check about version of asn1c
ASN1C_COMPILER_REQUIRED_VERSION_MESSAGE="ASN.1 Compiler, v0.9.24"
ASN1C_COMPILER_VERSION_MESSAGE=`asn1c -h 2>&1 | grep -i ASN\.1\ Compiler`
##ASN1C_COMPILER_VERSION_MESSAGE=`trim $ASN1C_COMPILER_VERSION_MESSAGE`
if [ "$ASN1C_COMPILER_VERSION_MESSAGE" != "$ASN1C_COMPILER_REQUIRED_VERSION_MESSAGE" ]
then
    diff <(echo -n "$ASN1C_COMPILER_VERSION_MESSAGE") <(echo -n "$ASN1C_COMPILER_REQUIRED_VERSION_MESSAGE")
    echo_error "Version of asn1c is not the required one, do you want to install the required one (overwrite installation) ? (Y/n)"
    echo_error "$ASN1C_COMPILER_VERSION_MESSAGE"
    while read -r -n 1 -s answer; do
        if [[ $answer = [YyNn] ]]; then
            [[ $answer = [Yy] ]] && $OPENAIRCN_DIR/SCRIPTS/install_asn1c_0.9.24.modified.bash
            [[ $answer = [Nn] ]] && echo_error "Version of asn1c is not the required one, exiting." && exit 1
            break
        fi
    done
fi


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
    echo_success "Invoking autogen"
    bash_exec "./autogen.sh"
    cd ./$OBJ_DIR
    echo_success "Invoking configure"
    ../configure --enable-standalone-epc --enable-raw-socket-for-sgi  LDFLAGS=-L/usr/local/lib
else
    cd ./$OBJ_DIR
fi

pkill oai_epc

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

is_openvswitch_interface $ENB_INTERFACE_NAME_FOR_S1_MME  \
                               $ENB_INTERFACE_NAME_FOR_S1U     \
                               $MME_INTERFACE_NAME_FOR_S1_MME  \
                               $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP
if [ $? -eq 1 ]; then
   ovs_setting=1
   vlan_setting=0
   echo_success "Found open-vswitch network configuration"
   # May we have booted on a new kernel, not the one when we build vswitch kernel module
   if [ ! -f /lib/modules/`uname -r`/extra/openvswitch.ko ]; then
      $OPENAIRCN_DIR/SCRIPTS/install_openvswitch1.9.0.bash
   fi
   clean_epc_ovs_network
   build_epc_ovs_network
   test_epc_ovs_network
else
    is_vlan_interface $MME_INTERFACE_NAME_FOR_S1_MME \
                      $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP
    if [ $? -eq 1 ]; then
        echo_success "Found open VLAN network configuration"
        ovs_setting=0
        vlan_setting=1
        
        # may be openvswitch is needed for S11
        is_openvswitch_interface $MME_IPV4_ADDRESS_FOR_S11_MME  \
                               $SGW_IPV4_ADDRESS_FOR_S11
        if [ $? -eq 1 ]; then
            # May we have booted on a new kernel, not the one when we build vswitch kernel module
            if [ ! -f /lib/modules/`uname -r`/extra/openvswitch.ko ]; then
                $OPENAIRCN_DIR/SCRIPTS/install_openvswitch1.9.0.bash
            fi
        fi
        clean_epc_vlan_network
        build_mme_spgw_vlan_network
    else
        echo_error "Cannot find open-vswitch network configuration or VLAN network configuration"
        exit 1
    fi 
fi

##################################################
# LAUNCH MME + S+P-GW executable
##################################################

cd $OPENAIRCN_DIR/$OBJ_DIR

ITTI_LOG_FILE=./itti_mme.log
rotate_log_file $ITTI_LOG_FILE
STDOUT_LOG_FILE=./stdout_mme.log
rotate_log_file $STDOUT_LOG_FILE

gdb --args $OPENAIRCN_DIR/$OBJ_DIR/OAI_EPC/oai_epc -K $ITTI_LOG_FILE -c $THIS_SCRIPT_PATH/$CONFIG_FILE_EPC  2>&1 | tee $STDOUT_LOG_FILE 
