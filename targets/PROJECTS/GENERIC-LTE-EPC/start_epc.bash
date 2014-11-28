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
# THE DIRECTORY WHERE ARE LOCATED THE CONFIGURATION FILES
#########################################
# This script start MME+S/P-GW (all in one executable, on one host) 

###########################################################
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
. $THIS_SCRIPT_PATH/utils.bash
. $THIS_SCRIPT_PATH/interfaces.bash
. $THIS_SCRIPT_PATH/networks.bash
###########################################################

control_c()
# run if user hits control-c
{
  echo -en "\n*** Exiting... ***\n"
  pkill -9 tshark
  pkill -9 oai_epc
  echo -en "\n*** Exit ***\n"
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
    autoreconf -i
    echo_success "Invoking autogen"
    bash_exec "./autogen.sh"
    cd ./$OBJ_DIR
    echo_success "Invoking configure"
    #../configure HAVE_CHECK=true --enable-debug --enable-standalone-epc --enable-raw-socket-for-sgi  LDFLAGS=-L/usr/local/lib
    ../configure HAVE_CHECK=true --enable-debug --enable-standalone-epc --enable-gtp1u-in-kernel LDFLAGS=-L/usr/local/lib
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
           GNU_DEBUGGER\|\
           REALM\|\
           ENB_INTERFACE_NAME_FOR_S1_MME\|\
           ENB_IPV4_ADDRESS_FOR_S1_MME\|\
           ENB_INTERFACE_NAME_FOR_S1U\|\
           ENB_IPV4_ADDRESS_FOR_S1U\|\
           MME_INTERFACE_NAME_FOR_S6A\|\
           MME_IPV4_ADDRESS_FOR_S6A\|\
           MME_INTERFACE_NAME_FOR_S1_MME\|\
           MME_IPV4_ADDRESS_FOR_S1_MME\|\
           MME_INTERFACE_NAME_FOR_S11_MME\|\
           MME_IPV4_ADDRESS_FOR_S11_MME\|\
           HSS_INTERFACE_NAME_FOR_S6A\|\
           HSS_IPV4_ADDRESS_FOR_S6A\|\
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
declare MME_IPV4_NETMASK_FOR_S6A=$(          echo $MME_IPV4_ADDRESS_FOR_S6A           | cut -f2 -d '/')
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
SGW_IPV4_ADDRESS_FOR_S6A=$(                  echo $SGW_IPV4_ADDRESS_FOR_S6A           | cut -f1 -d '/')
SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP=$(        echo $SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP | cut -f1 -d '/')
SGW_IPV4_ADDRESS_FOR_S5_S8_UP=$(             echo $SGW_IPV4_ADDRESS_FOR_S5_S8_UP      | cut -f1 -d '/')
PGW_IPV4_ADDRESS_FOR_S5_S8=$(                echo $PGW_IPV4_ADDRESS_FOR_S5_S8         | cut -f1 -d '/')
PGW_IPV4_ADDR_FOR_SGI=$(                     echo $PGW_IPV4_ADDR_FOR_SGI              | cut -f1 -d '/')

#is_vlan_interface $MME_INTERFACE_NAME_FOR_S1_MME  \
#                  $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP
                  


#delete_sgi_vlans
#get_mac_router

echo "   Clearing any existing rules and setting default policy.."
rmmod iptable_raw > /dev/null 2>&1
rmmod iptable_mangle > /dev/null 2>&1
rmmod iptable_nat > /dev/null 2>&1
rmmod iptable_filter > /dev/null 2>&1
rmmod ip_tables > /dev/null 2>&1
rmmod xt_state xt_mark xt_GTPUAH xt_GTPURH xt_tcpudp xt_connmark ipt_LOG ipt_MASQUERADE > /dev/null 2>&1
rmmod x_tables > /dev/null 2>&1
rmmod nf_conntrack_netlink nfnetlink nf_nat nf_conntrack_ipv4 nf_conntrack  > /dev/null 2>&1

modprobe ip_tables
modprobe x_tables

iptables -P INPUT ACCEPT
iptables -F INPUT
iptables -P OUTPUT ACCEPT
iptables -F OUTPUT
iptables -P FORWARD ACCEPT
iptables -F FORWARD
iptables -t nat -F
iptables -t mangle -F
iptables -t filter -F
iptables -t raw -F

ip route flush cache

(cd $OPENAIRCN_DIR/GTPV1-U/GTPUAH;make;cp -f ./Bin/libxt_*.so /lib/xtables;insmod $OPENAIRCN_DIR/GTPV1-U/GTPUAH/Bin/xt_GTPUAH.ko)
(cd $OPENAIRCN_DIR/GTPV1-U/GTPURH;make;cp -f ./Bin/libxt_*.so /lib/xtables;insmod $OPENAIRCN_DIR/GTPV1-U/GTPURH/Bin/xt_GTPURH.ko)

echo "   Enabling forwarding"
bash_exec "sysctl -w net.ipv4.ip_forward=1"
assert "  `sysctl -n net.ipv4.ip_forward` -eq 1" $LINENO

# Dynamic IP users:
#
#   If you get your IP address dynamically from SLIP, PPP, or DHCP,
#   enable this following option.  This enables dynamic-address hacking
#   which makes the life with Diald and similar programs much easier.
#
echo "   Disabling DynamicAddr.."
bash_exec "sysctl -w net.ipv4.ip_dynaddr=0"
assert "  `sysctl -n net.ipv4.ip_dynaddr` -eq 0" $LINENO

bash_exec "sysctl -w net.ipv4.conf.all.log_martians=0"
assert "  `sysctl -n net.ipv4.conf.all.log_martians` -eq 0" $LINENO


echo "   Disabling reverse path filtering"
bash_exec "sysctl -w net.ipv4.conf.all.rp_filter=1"
assert "  `sysctl -n net.ipv4.conf.all.rp_filter` -eq 1" $LINENO
bash_exec "sysctl -w net.ipv4.conf.$PGW_INTERFACE_NAME_FOR_SGI.rp_filter=1"
assert "  `sysctl -n net.ipv4.conf.$PGW_INTERFACE_NAME_FOR_SGI.rp_filter` -eq 1" $LINENO
bash_exec "sysctl -w net.ipv4.conf.$SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP.rp_filter=1"
assert "  `sysctl -n net.ipv4.conf.$SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP.rp_filter` -eq 1" $LINENO

echo "0" > /proc/sys/net/ipv4/conf/all/proxy_arp
echo "0" > /proc/sys/net/ipv4/conf/$PGW_INTERFACE_NAME_FOR_SGI/proxy_arp
echo "0" > /proc/sys/net/ipv4/conf/$SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP/proxy_arp

# REMINDER for tuning
bash_exec "sysctl -w net.ipv4.netfilter.ip_conntrack_max=10000"

iptables -A FORWARD -m state --state INVALID -j LOG

#iptables -I PREROUTING  -t raw  -i $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP  -j LOG --log-level crit --log-ip-options --log-prefix "PREROUTING raw:"

#iptables -A OUTPUT       -t filter  -j LOG --log-level crit --log-ip-options --log-prefix "OUTPUT filter:"
#iptables -A OUTPUT       -t mangle  -j LOG --log-level crit --log-ip-options --log-prefix "OUTPUT mangle:"
#iptables -A OUTPUT       -t nat     -j LOG --log-level crit --log-ip-options --log-prefix "OUTPUT nat:"
#iptables -A OUTPUT       -t raw     -j LOG --log-level crit --log-ip-options --log-prefix "OUTPUT raw:"
#iptables -A FORWARD      -t filter  -j LOG --log-level crit --log-ip-options --log-prefix "FORWARD filter:"
#iptables -A FORWARD      -t mangle  -j LOG --log-level crit --log-ip-options --log-prefix "FORWARD mangle:"
#iptables -I POSTROUTING  -t mangle  -j LOG --log-level crit --log-ip-options --log-prefix " ---> POSTROUTING mangle:"
#iptables -I POSTROUTING  -t nat     -j LOG --log-level crit --log-ip-options --log-prefix " ---> POSTROUTING nat:"
#iptables -A PREROUTING   -t nat     -j LOG --log-level crit --log-ip-options --log-prefix " <--- PREROUTING nat:"
#iptables -I PREROUTING   -t mangle  -j LOG --log-level crit --log-ip-options --log-prefix " <--- PREROUTING mangle:"
##################################################
# LAUNCH MME + S+P-GW executable
##################################################

cd $THIS_SCRIPT_PATH
if [ ! -d "OUTPUT/"$HOSTNAME ]
then
    bash_exec "mkdir -p -m 777 ./OUTPUT/$HOSTNAME"
    echo_success "Created OUTPUT/$HOSTNAME directory"
fi


ITTI_LOG_FILE=itti_mme.$HOSTNAME.log
#rotate_log_file $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$ITTI_LOG_FILE

STDOUT_LOG_FILE=stdout_mme.$HOSTNAME.log
#rotate_log_file $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$STDOUT_LOG_FILE

PCAP_S1C_LOG_FILE=tshark_mme_s1c.$HOSTNAME.pcap
#rotate_log_file $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$PCAP_S1C_LOG_FILE

PCAP_S6A_LOG_FILE=tshark_mme_s6a.$HOSTNAME.pcap
#rotate_log_file $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$PCAP_S6A_LOG_FILE

PCAP_S6A_S1C_LOG_FILE=tshark_mme_s6a_s1c.$HOSTNAME.pcap

PCAP_SGI_LOG_FILE=tshark_mme_sgi.$HOSTNAME.pcap


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


trap control_c SIGINT

echo_success "Resolving hss.$REALM"
HSS_IP=$(get_ip hss.$REALM)
echo_success "HSS_IP: $HSS_IP"

HSS_ROUTE=$(ip route get $HSS_IP | grep $HSS_IP )
HSS_ROUTE_IS_LOCAL=`echo $HSS_ROUTE | cut -d ' ' -f 1`
if [ "x$HSS_ROUTE_IS_LOCAL" == "xlocal" ]; then 
    MME_INTERFACE_NAME_FOR_S6A=`ip route get $HSS_IP | grep $HSS_IP | cut -d ' ' -f 4`
else
    MME_INTERFACE_NAME_FOR_S6A=`ip route get $HSS_IP | grep $HSS_IP | cut -d ' ' -f 3`
fi

echo_success "MME_INTERFACE_NAME_FOR_S1_MME : $MME_INTERFACE_NAME_FOR_S1_MME"
echo_success "MME_INTERFACE_NAME_FOR_S6A    : $MME_INTERFACE_NAME_FOR_S6A"

# see http://www.coverfire.com/articles/queueing-in-the-linux-network-stack/
bash_exec "ethtool -A $MME_INTERFACE_NAME_FOR_S1_MME autoneg off rx off tx off"
bash_exec "ethtool -G $MME_INTERFACE_NAME_FOR_S1_MME rx 4096 tx 4096"
bash_exec "ethtool -C $MME_INTERFACE_NAME_FOR_S1_MME rx-usecs 3"
bash_exec "ifconfig   $MME_INTERFACE_NAME_FOR_S1_MME txqueuelen 1000"

if [ x$MME_INTERFACE_NAME_FOR_S1_MME != x$SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP ]; then 
    bash_exec "ethtool -A $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP autoneg off rx off tx off"
    bash_exec "ethtool -G $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP rx 4096 tx 4096"
    bash_exec "ethtool -C $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP rx-usecs 3"
    bash_exec "ifconfig   $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP txqueuelen 1000"
fi

if [ x$MME_INTERFACE_NAME_FOR_S1_MME == x$MME_INTERFACE_NAME_FOR_S6A ]; then 
    nohup tshark -i $MME_INTERFACE_NAME_FOR_S1_MME -f "not port 22" -w $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$PCAP_S6A_S1C_LOG_FILE &
else
    nohup tshark -i $MME_INTERFACE_NAME_FOR_S1_MME -f "not port 22" -w $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$PCAP_S1C_LOG_FILE &
    nohup tshark -i $MME_INTERFACE_NAME_FOR_S6A    -f "not port 22" -w $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$PCAP_S6A_LOG_FILE &
fi

if [ x$PGW_INTERFACE_NAME_FOR_SGI != xnone ]; then 
    nohup tshark -i $PGW_INTERFACE_NAME_FOR_SGI -f "not port 22" -w $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$PCAP_SGI_LOG_FILE &
fi

wait_process_started tshark


cd $OPENAIRCN_DIR/$OBJ_DIR
echo "GNU_DEBUGGER:"$GNU_DEBUGGER

if [ "x$GNU_DEBUGGER" == "xyes" ]; then
    echo_success "Running with GDB"
    touch .gdbinit
    echo "file $OPENAIRCN_DIR/$OBJ_DIR/OAI_EPC/oai_epc" > ~/.gdbinit
    echo "set args -K $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$ITTI_LOG_FILE -c $THIS_SCRIPT_PATH/$CONFIG_FILE_EPC" >> ~/.gdbinit
#    echo "b encode_eps_mobile_identity" >> ~/.gdbinit
    echo "run" >> ~/.gdbinit
    gdb 2>&1 | tee $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$STDOUT_LOG_FILE
else 
    echo_success "Running without GDB"
    valgrind --tool=memcheck $OPENAIRCN_DIR/$OBJ_DIR/OAI_EPC/oai_epc -K $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$ITTI_LOG_FILE -c $THIS_SCRIPT_PATH/$CONFIG_FILE_EPC  2>&1 | tee $THIS_SCRIPT_PATH/OUTPUT/$HOSTNAME/$STDOUT_LOG_FILE
fi     

