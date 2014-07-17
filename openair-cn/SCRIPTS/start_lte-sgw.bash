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
# start MME+S/P-GW with openvswitch setting

                                                                              hss.eur
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


BRIDGE="vswitch"

###########################################################
IPTABLES=/sbin/iptables

###########################################################

THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
source $THIS_SCRIPT_PATH/utils.bash

###########################################################
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

cat ./objs/Makefile | grep CFLAGS\ \=\  | grep DENABLE_USE_NETFILTER_FOR_SGI
if [ $? -ne 0 ]
then
    export ENABLE_USE_NETFILTER_FOR_SGI=0
else
    export ENABLE_USE_NETFILTER_FOR_SGI=1
fi

cat ./objs/Makefile | grep CFLAGS\ \=\  | grep DENABLE_USE_RAW_FOR_SGI
if [ $? -ne 0 ]
then
    export ENABLE_USE_RAW_FOR_SGI=0
else
    export ENABLE_USE_RAW_FOR_SGI=1
fi

pkill oaisim_mme


rm -f /tmp/source.txt
cat $OPENAIR3_DIR/OPENAIRMME/UTILS/CONF/mme_default.conf | tr -d " " > /tmp/source.txt
source /tmp/source.txt

rm -f /tmp/source.txt
cat $OPENAIR3_DIR/OPENAIRMME/UTILS/CONF/enb_default.conf | tr -d " " > /tmp/source.txt
source /tmp/source.txt

ping -c 1 hss.eur || { echo "hss.eur does not respond to ping" >&2 ; exit ; }
ping -c 1 router.eur || { echo "router.eur does not respond to ping" >&2 ; exit ; }
export IP_ROUTER=`python -c 'import socket; print socket.gethostbyname("router.eur")'`
export MAC_ROUTER=`ip neigh show | grep $IP_ROUTER | cut -d ' '  -f5 | tr -d ':'`
echo_success "ROUTER MAC ADDRESS= $MAC_ROUTER"
bash_exec "modprobe tun"
bash_exec "modprobe ip_tables"
bash_exec "modprobe iptable_nat"
bash_exec "modprobe x_tables"

bash_exec "$IPTABLES -P INPUT ACCEPT"
bash_exec "$IPTABLES -F INPUT"
bash_exec "$IPTABLES -P OUTPUT ACCEPT"
bash_exec "$IPTABLES -F OUTPUT"
bash_exec "$IPTABLES -P FORWARD ACCEPT"
bash_exec "$IPTABLES -F FORWARD"
bash_exec "$IPTABLES -t raw    -F"
bash_exec "$IPTABLES -t nat    -F"
bash_exec "$IPTABLES -t mangle -F"
bash_exec "$IPTABLES -t filter -F"



bash_exec "ip route flush cache"


echo "   Disabling forwarding"
bash_exec "sysctl -w net.ipv4.ip_forward=0"
assert "  `sysctl -n net.ipv4.ip_forward` -eq 0" $LINENO

echo "   Enabling DynamicAddr.."
bash_exec "sysctl -w net.ipv4.ip_dynaddr=1"
assert "  `sysctl -n net.ipv4.ip_dynaddr` -eq 1" $LINENO

bash_exec "sysctl -w net.ipv4.conf.all.log_martians=1"
assert "  `sysctl -n net.ipv4.conf.all.log_martians` -eq 1" $LINENO


echo "   Disabling reverse path filtering"
bash_exec "sysctl -w net.ipv4.conf.all.rp_filter=0"
assert "  `sysctl -n net.ipv4.conf.all.rp_filter` -eq 0" $LINENO


start_openswitch_daemon
# REMINDER:
#        +-----------+          +------+              +-----------+
#        |  eNB      +------+   |  ovs | VLAN 1+------+    MME    |
#        |           |cpenb0+------------------+cpmme0|           |
#        |           +------+   |bridge|       +------+           |
#        |           |upenb0+-------+  |              |           |
#        +-----------+------+   |   |  |              +-----------+
#                               +---|--+                    |
#                                   |                 +-----------+
#                                   |                 |  S+P-GW   |
#                                   |  VLAN2   +------+           +-------+   +----+    +----+
#                                   +----------+upsgw0|           |pgwsgi0+---+br2 +----+eth0|
#                                              +------+           +-------+   +----+    +----+
#                                                     |           |
#                                                     +-----------+
#
##################################################
# del bridge between eNB and MME/SPGW
##################################################
bash_exec "tunctl -d $ENB_INTERFACE_NAME_FOR_S1_MME"
bash_exec "tunctl -d $ENB_INTERFACE_NAME_FOR_S1U"
bash_exec "tunctl -d $MME_INTERFACE_NAME_FOR_S1_MME"
bash_exec "tunctl -d $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP"
bash_exec "ovs-vsctl del-br       $BRIDGE"

##################################################
# build bridge between eNB and MME/SPGW
##################################################
bash_exec "tunctl -t $ENB_INTERFACE_NAME_FOR_S1_MME"
bash_exec "tunctl -t $ENB_INTERFACE_NAME_FOR_S1U"
bash_exec "tunctl -t $MME_INTERFACE_NAME_FOR_S1_MME"
bash_exec "tunctl -t $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP"

bash_exec "ovs-vsctl add-br       $BRIDGE"
bash_exec "ovs-vsctl add-port     $BRIDGE $ENB_INTERFACE_NAME_FOR_S1_MME        tag=1"
bash_exec "ovs-vsctl add-port     $BRIDGE $MME_INTERFACE_NAME_FOR_S1_MME        tag=1"
bash_exec "ovs-vsctl add-port     $BRIDGE $ENB_INTERFACE_NAME_FOR_S1U           tag=2"
bash_exec "ovs-vsctl add-port     $BRIDGE $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP tag=2"

bash_exec "ifconfig $MME_INTERFACE_NAME_FOR_S1_MME promisc up"
bash_exec "ifconfig $MME_INTERFACE_NAME_FOR_S1_MME $MME_IP_ADDRESS_FOR_S1_MME netmask `cidr2mask $MME_IP_NETMASK_FOR_S1_MME` promisc up"
bash_exec "ifconfig $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP promisc up"
bash_exec "ifconfig $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP $SGW_IP_ADDRESS_FOR_S1U_S12_S4_UP netmask `cidr2mask $SGW_IP_NETMASK_FOR_S1U_S12_S4_UP` promisc up"

bash_exec "ifconfig $ENB_INTERFACE_NAME_FOR_S1_MME promisc up"
bash_exec "ifconfig $ENB_INTERFACE_NAME_FOR_S1_MME $ENB_IP_ADDRESS_FOR_S1_MME netmask `cidr2mask $ENB_IP_NETMASK_FOR_S1_MME` promisc up"
bash_exec "ifconfig $ENB_INTERFACE_NAME_FOR_S1U promisc up"
bash_exec "ifconfig $ENB_INTERFACE_NAME_FOR_S1U $ENB_IP_ADDRESS_FOR_S1U netmask `cidr2mask $ENB_IP_NETMASK_FOR_S1U` promisc up"

##################################################
# del bridge between SPGW and Internet
##################################################
#bash_exec "tunctl -d $PGW_INTERFACE_NAME_FOR_SGI"
#bash_exec "ovs-vsctl del-br       $SGI_BRIDGE"

##################################################
# build bridge between SPGW and Internet
##################################################

# # get ipv4 address from PGW_INTERFACE_NAME_FOR_SGI
# IP_ADDR=`ifconfig $PGW_INTERFACE_NAME_FOR_SGI | awk '/inet addr/ {split ($2,A,":"); print A[2]}' | tr '\n' ' ' | sed -n '1h;1!H;${;g;s/^[ \t]*//g;s/[ \t]*$//g;p;}'`
# if [ $IP_ADDR ]; then
#   bash_exec "ip -4 addr del $IP_ADDR dev $PGW_INTERFACE_NAME_FOR_SGI"
# fi
# 
# # remove all ipv6 address from PGW_INTERFACE_NAME_FOR_SGI
# IP_ADDR="not empty"
# until [ "$IP_ADDR"x == "x" ]; do
#   IP_ADDR=`ifconfig $PGW_INTERFACE_NAME_FOR_SGI | grep 'inet6' | head -1 | tr '\n' ' ' | sed -n '1h;1!H;${;g;s/^[ \t]*//g;s/[ \t]*$//g;p;}' | cut -d ' ' -f3`
#   if [ $IP_ADDR ]; then
#     bash_exec "ip -6 addr del $IP_ADDR dev $PGW_INTERFACE_NAME_FOR_SGI"
#   fi
# done

if [ $ENABLE_USE_NETFILTER_FOR_SGI -eq 1 ]; then 

    bash_exec "modprobe nf_conntrack"
    bash_exec "modprobe nf_conntrack_ftp"

    ######################################################
    # PREROUTING 
    ######################################################
    # We restore the mark following the CONNMARK mark. In fact, it does a simple MARK=CONNMARK 
    # where MARK is the standard mark (usable by tc)
    # In French: Cette option de cible restaure le paquet marqué dans la marque de connexion 
    # comme défini par CONNMARK. Un masque peut aussi être défini par l'option --mask. 
    # Si une option mask est placée, seules les options masquées seront placées. 
    # Notez que cette option de cible n'est valide que dans la table mangle.
    bash_exec "$IPTABLES -t mangle -A PREROUTING -j CONNMARK --restore-mark"
    
    # TEST bash_exec "$IPTABLES -t mangle -A PREROUTING -m mark --mark 0 -i $PGW_INTERFACE_NAME_FOR_SGI -j MARK --set-mark 15"
    # We set the mark of the initial packet as value of the conntrack mark for all the packets of the connection. 
    # This mark will be restore for the other packets by the first rule of POSTROUTING --restore-mark).
    bash_exec "$IPTABLES -t mangle -A PREROUTING -j CONNMARK --save-mark"


    ######################################################
    # POSTROUTING 
    ######################################################
    
    # MARK=CONNMARK
    bash_exec "iptables -A POSTROUTING -t mangle -o tap0 -j CONNMARK --restore-mark"
    # If we’ve got a mark no need to get further[
    bash_exec "iptables -A POSTROUTING -t mangle -o tap0 -m mark ! --mark 0 -j ACCEPT"
    
    #bash_exec "iptables -A POSTROUTING -p tcp --dport 21 -t mangle -j MARK --set-mark 1"
    #bash_exec "iptables -A POSTROUTING -p tcp --dport 80 -t mangle -j MARK --set-mark 2"
    
    # We set the mark of the initial packet as value of the conntrack mark for all the packets 
    # of the connection. This mark will be restore for the other packets by the first rule 
    # of POSTROUTING (–restore-mark).
    bash_exec "iptables -A POSTROUTING -t mangle -j CONNMARK --save-mark"
    
    bash_exec "iptables -A PREROUTING  -t mangle -j CONNMARK --restore-mark"

    # We restore the mark following the CONNMARK mark. 
    # In fact, it does a simple MARK=CONNMARK where MARK is the standard mark (usable by tc)
    #bash_exec "$IPTABLES -A OUTPUT -t mangle -m mark ! --mark 0 -j CONNMARK --restore-mark"
    
    # If we’ve got a mark no need to get further[1]
    #TEST bash_exec "$IPTABLES -A OUTPUT -t mangle -p icmp -j MARK --set-mark 14"
    #bash_exec "$IPTABLES -A OUTPUT -t mangle -m mark ! --mark 0 -j ACCEPT"
    
    
    # We set the mark of the initial packet as value of the conntrack mark for all the packets of the connection. 
    # This mark will be restore for the other packets by the first rule of OUTPUT (–restore-mark).
    #bash_exec "$IPTABLES -A OUTPUT -t mangle -j CONNMARK --save-mark"
    


    
    ######################################################
    # NETFILTER QUEUE 
    ######################################################
    bash_exec "$IPTABLES -t mangle -A PREROUTING -i $PGW_INTERFACE_NAME_FOR_SGI -m connmark  --mark 5 -j NFQUEUE --queue-num 1"
    bash_exec "$IPTABLES -t mangle -A PREROUTING -i $PGW_INTERFACE_NAME_FOR_SGI -m connmark  --mark 6 -j NFQUEUE --queue-num 1"
    bash_exec "$IPTABLES -t mangle -A PREROUTING -i $PGW_INTERFACE_NAME_FOR_SGI -m connmark  --mark 7 -j NFQUEUE --queue-num 1"
    bash_exec "$IPTABLES -t mangle -A PREROUTING -i $PGW_INTERFACE_NAME_FOR_SGI -m connmark  --mark 8 -j NFQUEUE --queue-num 1"
    bash_exec "$IPTABLES -t mangle -A PREROUTING -i $PGW_INTERFACE_NAME_FOR_SGI -m connmark  --mark 9 -j NFQUEUE --queue-num 1"
    bash_exec "$IPTABLES -t mangle -A PREROUTING -i $PGW_INTERFACE_NAME_FOR_SGI -m connmark  --mark 10 -j NFQUEUE --queue-num 1"
    bash_exec "$IPTABLES -t mangle -A PREROUTING -i $PGW_INTERFACE_NAME_FOR_SGI -m connmark  --mark 11 -j NFQUEUE --queue-num 1"
    bash_exec "$IPTABLES -t mangle -A PREROUTING -i $PGW_INTERFACE_NAME_FOR_SGI -m connmark  --mark 12 -j NFQUEUE --queue-num 1"
    bash_exec "$IPTABLES -t mangle -A PREROUTING -i $PGW_INTERFACE_NAME_FOR_SGI -m connmark  --mark 13 -j NFQUEUE --queue-num 1"
    bash_exec "$IPTABLES -t mangle -A PREROUTING -i $PGW_INTERFACE_NAME_FOR_SGI -m connmark  --mark 14 -j NFQUEUE --queue-num 1"
    bash_exec "$IPTABLES -t mangle -A PREROUTING -i $PGW_INTERFACE_NAME_FOR_SGI -m connmark  --mark 15 -j NFQUEUE --queue-num 1"
    
    #echo 0 > /proc/sys/net/bridge/bridge-nf-call-iptables #To disable Iptables in the bridge.
    #Raw table: Some years ago appeared a new tables in Iptables. 
    #This table can be used to avoid packets (connection really) to enter the NAT table: 
    # iptables -t raw -I PREROUTING -i BRIDGE -s x.x.x.x -j NOTRACK.
    
    
    

    #bash_exec "$IPTABLES -t nat -A POSTROUTING -o $PGW_INTERFACE_NAME_FOR_SGI -j SNAT --to-source $PGW_IP_ADDR_FOR_SGI"
else
    # # get ipv4 address from PGW_INTERFACE_NAME_FOR_SGI
    #IP_ADDR=`ifconfig $PGW_INTERFACE_NAME_FOR_SGI | awk '/inet addr/ {split ($2,A,":"); print A[2]}' | tr '\n' ' ' | sed -n '1h;1!H;${;g;s/^[ \t]*//g;s/[ \t]*$//g;p;}'`

    #NETWORK=`echo $IP_ADDR | cut -d . -f 1,2,3`

    bash_exec "modprobe 8021q"
    
    for i in 5 6 7 8 9 10 11 12 13 14 15
    do
        # create vlan interface
        bash_exec "vconfig rem $PGW_INTERFACE_NAME_FOR_SGI.$i"
        sync
        bash_exec "vconfig add $PGW_INTERFACE_NAME_FOR_SGI $i"
        sync
        # configure vlan interface
        #CIDR=$NETWORK'.'$i'/24'
        base=200
        NET=$(( $i + $base ))
        CIDR='10.0.'$NET'.2/8'
        bash_exec "ip -4 addr add  $CIDR dev $PGW_INTERFACE_NAME_FOR_SGI.$i"
    done
fi


bash_exec "ip link set $PGW_INTERFACE_NAME_FOR_SGI promisc on"
##################################################..

# LAUNCH MME + S+P-GW executable 
##################################################
#$OPENAIR3_DIR/OPENAIRMME/objs/OAISIM_MME/oaisim_mme -c $OPENAIR3_DIR/OPENAIRMME/UTILS/CONF/mme_default.conf 
#wait_process_started "oaisim_mme"

gdb --args $OPENAIR3_DIR/OPENAIRMME/objs/OAI_EPC/oai_epc -c $OPENAIR3_DIR/OPENAIRMME/UTILS/CONF/mme_default.conf
wait_process_started "oai_epc"

#gdb --args  $OPENAIR3_DIR/OPENAIRMME/objs/OAI_EPC/oai_epc -c $OPENAIR3_DIR/OPENAIRMME/UTILS/CONF/mme_default.conf 

