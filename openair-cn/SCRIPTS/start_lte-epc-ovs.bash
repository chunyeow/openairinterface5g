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
#  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.
#
################################################################################
# Author Lionel GAUTHIER 01/20/2014
#
# This script start MME+S/P-GW (all in one executable, on one host) with openvswitch setting
# eNB executable have to be launched on the same host by your own (start_lte-enb-ovs.bash).
#
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
BRIDGE="vswitch"

###########################################################
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
source $THIS_SCRIPT_PATH/utils.bash
###########################################################

test_command_install_package "gccxml"   "gccxml" "--force-yes"
test_command_install_package "vconfig"  "vlan"
test_command_install_package "iptables" "iptables"
test_command_install_package "iperf"    "iperf"
test_command_install_package "ip"       "iproute"
test_command_install_script   "ovs-vsctl" "$OPENAIRCN_DIR/SCRIPTS/install_openvswitch1.9.0.bash"
test_command_install_package  "tunctl"  "uml-utilities"
if [ ! -d /usr/local/etc/freeDiameter ]
    then
        cd $OPENAIRCN_DIR/S6A/freediameter && ./install_freediameter.sh
    else
        echo_success "freediameter is installed"
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

# May we have booted on a new kernel, not the one when we build vswitch kernel module
if [ ! -f /lib/modules/`uname -r`/extra/openvswitch.ko ]; then
    $OPENAIRCN_DIR/SCRIPTS/install_openvswitch1.9.0.bash
fi

IPTABLES=`which iptables`

##################################
# Get or set OBJ DIR and compile #
##################################
# TEST IF EXIST
cd $OPENAIRCN_DIR
OBJ_DIR=`find . -maxdepth 1 -type d -iname obj*`
if [ -n "$OBJ_DIR" ]
then
    OBJ_DIR=`basename $OBJ_DIR`
    if [ ! -f $OBJ_DIR/Makefile ]
    then
        cd ./$OBJ_DIR
        echo_success "Invoking configure"
        rm -f Makefile
        ../configure --enable-standalone-epc --enable-raw-socket-for-sgi  LDFLAGS=-L/usr/local/lib
    else
        cd ./$OBJ_DIR
    fi
else
    OBJ_DIR="objs"
    bash_exec "mkdir -m 777 ./$OBJ_DIR"
    echo_success "Created $OBJ_DIR directory"
    echo_success "Invoking autogen"
    bash_exec "./autogen.sh"
    cd ./$OBJ_DIR
    echo_success "Invoking configure"
    ../configure --enable-standalone-epc --enable-raw-socket-for-sgi  LDFLAGS=-L/usr/local/lib
fi

pkill oai_epc
if [ -f Makefile ]
then
    echo_success "Compiling..."
    bash_exec "make"
else
    echo_error "Configure failed, exiting"
    exit 1
fi
cd $OPENAIRCN_DIR


#######################################################
# SOURCE $OPENAIRCN_DIR/UTILS/CONF/epc_$HOSTNAME.conf
#######################################################
rm -f /tmp/source.txt
if [ -f $OPENAIRCN_DIR/UTILS/CONF/epc_$HOSTNAME.conf ]
then
    cat $OPENAIRCN_DIR/UTILS/CONF/epc_$HOSTNAME.conf | tr -d " " > /tmp/source.txt
    source /tmp/source.txt
else
    echo_error "Missing config file $OPENAIRCN_DIR/UTILS/CONF/epc_$HOSTNAME.conf (Please write your own config file), exiting"
    exit 1
fi

#######################################################
# SOURCE $OPENAIRCN_DIR/UTILS/CONF/enb_$HOSTNAME.conf
#######################################################
rm -f /tmp/source.txt
if [ -f $OPENAIRCN_DIR/UTILS/CONF/enb_$HOSTNAME.conf ]
then
    cat $OPENAIRCN_DIR/UTILS/CONF/enb_$HOSTNAME.conf | tr -d " " > /tmp/source.txt
    source /tmp/source.txt
else
    echo_error "Missing config file $OPENAIRCN_DIR/UTILS/CONF/enb_$HOSTNAME.conf (Please write your own config file), exiting"
    exit 1
fi

clean_epc_network
build_epc_network
test_epc_network

##################################################..

# LAUNCH MME + S+P-GW executable
##################################################
MME_CONFIG_FILE=$OPENAIRCN_DIR/UTILS/CONF/mme_default.conf
if [ -f $OPENAIRCN_DIR/UTILS/CONF/mme_$HOSTNAME.conf ]; then
    MME_CONFIG_FILE=$OPENAIRCN_DIR/UTILS/CONF/mme_$HOSTNAME.conf
    echo_warning "MME config file found is now $MME_CONFIG_FILE"
else
    echo_warning "MME config file for host $HOSTNAME not found, trying default: $MME_CONFIG_FILE"
    if [ -f $MME_CONFIG_FILE ]; then
        echo_success "Default MME config file found: $MME_CONFIG_FILE"
    else
        echo_error "Default MME config file not found, exiting"
        exit 1
    fi
fi
cd $OPENAIRCN_DIR/$OBJ_DIR

ITTI_LOG_FILE=/tmp/itti_mme.log
rotate_log_file $ITTI_LOG_FILE

$OPENAIRCN_DIR/$OBJ_DIR/OAI_EPC/oai_epc -K $ITTI_LOG_FILE -c $MME_CONFIG_FILE
