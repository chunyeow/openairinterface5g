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
# file start_epc.bash
# brief
# author Lionel Gauthier
# company Eurecom
# email: lionel.gauthier@eurecom.fr
#

#########################################
# INPUT OF THIS SCRIPT:
# THE EPC CONFIGURATION FILE
#########################################
# This script start MME+S/P-GW (all in one executable, on one host) 

###########################################################
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
. $THIS_SCRIPT_PATH/utils.bash
. $THIS_SCRIPT_PATH/interfaces.bash
. $THIS_SCRIPT_PATH/networks.bash
###########################################################

#check_install_epc_software

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
if [ ! -n "m4" ]
then
    mkdir -m 777 m4
fi
if [ ! -f $OBJ_DIR/Makefile ]
then
    autoreconf -i -f
    echo_success "Invoking autogen"
    bash_exec "./autogen.sh"
    cd ./$OBJ_DIR
    echo_success "Invoking configure"
    #../configure HAVE_CHECK=true --enable-debug --enable-standalone-epc --enable-raw-socket-for-sgi  LDFLAGS=-L/usr/local/lib
    ../configure HAVE_CHECK=true --enable-debug --enable-standalone-epc --enable-gtp1u-in-kernel LDFLAGS=-L/usr/local/lib
else
    cd ./$OBJ_DIR
fi


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
# COMPILE GTP kernel modules
#######################################################
(cd $OPENAIRCN_DIR/GTPV1-U/GTPUAH;make;cp -f ./Bin/libxt_*.so /lib/xtables;)
(cd $OPENAIRCN_DIR/GTPV1-U/GTPURH;make;cp -f ./Bin/libxt_*.so /lib/xtables;)

#######################################################
# FIND CONFIG FILE
#######################################################
DEFAULT_CONFIG_FILE="epc.`hostname -s`.conf"
CONFIG_FILE_EPC=${1:-DEFAULT_CONFIG_FILE}

echo "CONFIG_FILE_EPC=$CONFIG_FILE_EPC"
if [ -f $CONFIG_FILE_EPC ]; then
    echo_warning "EPC config file found is now $CONFIG_FILE_EPC"
else
    echo_error "EPC config file not found, exiting (searched for $CONFIG_FILE_EPC)"
    exit 1
fi


#######################################################
# SOURCE CONFIG FILE
#######################################################
rm -f /tmp/source.txt
VARIABLES="
           GNU_DEBUGGER\|\
           REALM"

VARIABLES=$(echo $VARIABLES | sed -e 's/\\r//g')
VARIABLES=$(echo $VARIABLES | tr -d ' ')
cat $CONFIG_FILE_EPC | grep -w "$VARIABLES"| tr -d " " | tr -d ";" > /tmp/source.txt
source /tmp/source.txt


check_epc_s6a_certificate $REALM

iptables -F FORWARD
iptables -A FORWARD -m state --state INVALID -j LOG --log-prefix "FW: INVALID STATE:"
#iptables -I PREROUTING  -t raw  -i $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP  -j LOG --log-level crit --log-ip-options --log-prefix "PREROUTING raw:"


touch $THIS_SCRIPT_PATH/kill_epc.bash
echo '#!/bin/bash' >                 $THIS_SCRIPT_PATH/kill_epc.bash
echo "pkill -P $$" >>                $THIS_SCRIPT_PATH/kill_epc.bash
echo "iptables -P INPUT ACCEPT" >>   $THIS_SCRIPT_PATH/kill_epc.bash
echo "iptables -F INPUT" >>          $THIS_SCRIPT_PATH/kill_epc.bash
echo "iptables -P OUTPUT ACCEPT" >>  $THIS_SCRIPT_PATH/kill_epc.bash
echo "iptables -F OUTPUT" >>         $THIS_SCRIPT_PATH/kill_epc.bash
echo "iptables -P FORWARD ACCEPT" >> $THIS_SCRIPT_PATH/kill_epc.bash
echo "iptables -F FORWARD" >>        $THIS_SCRIPT_PATH/kill_epc.bash
echo "iptables -t nat -F" >>         $THIS_SCRIPT_PATH/kill_epc.bash
echo "iptables -t mangle -F" >>      $THIS_SCRIPT_PATH/kill_epc.bash
echo "iptables -t filter -F" >>      $THIS_SCRIPT_PATH/kill_epc.bash
echo "iptables -t raw -F" >>         $THIS_SCRIPT_PATH/kill_epc.bash
echo "ip route flush cache" >>       $THIS_SCRIPT_PATH/kill_epc.bash
echo "rmmod iptable_raw" >>          $THIS_SCRIPT_PATH/kill_epc.bash
echo "rmmod iptable_mangle" >>       $THIS_SCRIPT_PATH/kill_epc.bash
echo "rmmod iptable_nat" >>          $THIS_SCRIPT_PATH/kill_epc.bash
echo "rmmod iptable_filter" >>       $THIS_SCRIPT_PATH/kill_epc.bash
echo "rmmod ip_tables" >>            $THIS_SCRIPT_PATH/kill_epc.bash
echo "rmmod xt_state xt_mark xt_GTPUAH xt_GTPURH  xt_tcpudp xt_connmark ipt_LOG ipt_MASQUERADE" >>            $THIS_SCRIPT_PATH/kill_epc.bash
echo "rmmod x_tables" >>             $THIS_SCRIPT_PATH/kill_epc.bash
echo "rmmod nf_conntrack_netlink nfnetlink nf_nat nf_conntrack_ipv4 nf_conntrack" >>             $THIS_SCRIPT_PATH/kill_epc.bash
chmod 777 $THIS_SCRIPT_PATH/kill_epc.bash



# see http://www.coverfire.com/articles/queueing-in-the-linux-network-stack/
#ethtool -A $MME_INTERFACE_NAME_FOR_S1_MME autoneg off rx off tx off"
#ethtool -G $MME_INTERFACE_NAME_FOR_S1_MME rx 4096 tx 4096"
#ethtool -C $MME_INTERFACE_NAME_FOR_S1_MME rx-usecs 3"
#ifconfig   $MME_INTERFACE_NAME_FOR_S1_MME txqueuelen 1000"



cd $OPENAIRCN_DIR/$OBJ_DIR
echo "GNU_DEBUGGER:"$GNU_DEBUGGER

if [ "x$GNU_DEBUGGER" == "xyes" ]; then
    echo_success "Running with GDB"
    touch .gdbinit
    echo "file $OPENAIRCN_DIR/$OBJ_DIR/OAI_EPC/oai_epc" > ~/.gdbinit
    echo "set args -K /tmp/itti_epc.log -c $THIS_SCRIPT_PATH/$CONFIG_FILE_EPC" >> ~/.gdbinit
#    echo "b encode_eps_mobile_identity" >> ~/.gdbinit
    echo "run" >> ~/.gdbinit
    gdb 2>&1 | tee /tmp/stdout_epc.log
else 
    echo_success "Running without GDB"
    valgrind --tool=memcheck --track-origins=yes $OPENAIRCN_DIR/$OBJ_DIR/OAI_EPC/oai_epc -K /tmp/itti_epc.log -c $THIS_SCRIPT_PATH/$CONFIG_FILE_EPC  2>&1 | tee /tmp/stdout_epc.log
fi     

