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
# file networks.bash
# brief
# author Lionel Gauthier
# company Eurecom
# email: lionel.gauthier@eurecom.fr
#

###########################################################
THIS_NETWORKS_SCRIPT_PATH=$(dirname $(readlink -f $0))
source $THIS_NETWORKS_SCRIPT_PATH/interfaces.bash
###########################################################

# example: netcalc 192.168.12.100 255.255.255.0
netcalc(){
    local IFS='.' ip i
    local -a oct msk
    
    read -ra oct <<<"$1"
    read -ra msk <<<"$2"

    for i in ${!oct[@]}; do
        ip+=( "$(( oct[i] & msk[i] ))" )
    done
    
    echo "${ip[*]}"
}

build_network() {
  echo "TO DO"
}


build_openvswitch_network() {
    start_openswitch_daemon
    # REMINDER:
    #                                                                           hss.eur
    #                                                                             |
    #        +-----------+          +------+              +-----------+           v   +----------+
    #        |  eNB      +------+   |  ovs | VLAN 1+------+    MME    +----+      +---+   HSS    |
    #        |           |cpenb0+------------------+cpmme0|           |    +------+   |          |
    #        |           +------+   |bridge|       +------+           +----+      +---+          |
    #        |           |upenb0+-------+  |              |           |               +----------+
    #        |           +------+   |   |  |              +-+-------+-+
    #        |           |          |   |  +----------------| s11mme|---+
    #        |           |          |   |                   +---+---+   |
    #        |           |          |   |             (optional)|       |ovs bridge is optional
    #        +-----------+          |   |                   +---+---+   |
    #                               +---|------------------ | s11sgw|---+        router.eur
    #                                   |                 +-+-------+-+              |   +--------------+
    #                                   |                 |  S+P-GW   |              v   |   ROUTER     |
    #                                   |  VLAN2   +------+           +-------+     +----+              +----+
    #                                   +----------+upsgw0|           |sgi    +-...-+    |              |    +---...Internet
    #                                              +------+           +-------+     +----+              +----+
    #                                                     |           |      11 VLANS    |              |
    #                                                     +-----------+   ids=[5..15]    +--------------+
    #
    ##################################################
    # build bridge between eNB and MME/SPGW
    ##################################################
    #create_openvswitch_interface $ENB_INTERFACE_NAME_FOR_S1_MME
    #create_openvswitch_interface $ENB_INTERFACE_NAME_FOR_S1U
    #create_openvswitch_interface $MME_INTERFACE_NAME_FOR_S1_MME
    #create_openvswitch_interface $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP
    #create_openvswitch_interface $MME_INTERFACE_NAME_FOR_S11_MME
    #create_openvswitch_interface $SGW_INTERFACE_NAME_FOR_S11
    
    bash_exec "ovs-vsctl add-br       $BRIDGE"
    bash_exec "ovs-vsctl add-port     $BRIDGE $ENB_INTERFACE_NAME_FOR_S1_MME        tag=1"
    bash_exec "ovs-vsctl add-port     $BRIDGE $MME_INTERFACE_NAME_FOR_S1_MME        tag=1"
    bash_exec "ovs-vsctl add-port     $BRIDGE $ENB_INTERFACE_NAME_FOR_S1U           tag=2"
    bash_exec "ovs-vsctl add-port     $BRIDGE $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP tag=2"
    if [ "a${MME_INTERFACE_NAME_FOR_S11_MME:0:4}" != "anone" ]; then
        bash_exec "ovs-vsctl add-port     $BRIDGE $MME_INTERFACE_NAME_FOR_S11_MME       tag=3"
    fi
    if [ "a${SGW_INTERFACE_NAME_FOR_S11:0:4}" != "anone" ]; then
        bash_exec "ovs-vsctl add-port     $BRIDGE $SGW_INTERFACE_NAME_FOR_S11           tag=3"
    fi

    set_interface_up $MME_INTERFACE_NAME_FOR_S1_MME        $MME_IPV4_ADDRESS_FOR_S1_MME         $MME_IPV4_NETMASK_FOR_S1_MME
    set_interface_up $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP $SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP  $SGW_IPV4_NETMASK_FOR_S1U_S12_S4_UP

    set_interface_up $ENB_INTERFACE_NAME_FOR_S1_MME        $ENB_IPV4_ADDRESS_FOR_S1_MME         $ENB_IPV4_NETMASK_FOR_S1_MME
    set_interface_up $ENB_INTERFACE_NAME_FOR_S1U           $ENB_IPV4_ADDRESS_FOR_S1U            $ENB_IPV4_NETMASK_FOR_S1U

    set_interface_up $MME_INTERFACE_NAME_FOR_S11_MME       $MME_IPV4_ADDRESS_FOR_S11_MME        $MME_IPV4_NETMASK_FOR_S11_MME
    set_interface_up $SGW_INTERFACE_NAME_FOR_S11           $SGW_IPV4_ADDRESS_FOR_S11            $SGW_IPV4_NETMASK_FOR_S11
    
    get_mac_router
}

clean_openvswitch_network() {
    start_openswitch_daemon
    ##################################################
    # del bridge between eNB and MME/SPGW
    ##################################################
    delete_openvswitch_interface $ENB_INTERFACE_NAME_FOR_S1_MME
    delete_openvswitch_interface $ENB_INTERFACE_NAME_FOR_S1U
    delete_openvswitch_interface $MME_INTERFACE_NAME_FOR_S1_MME
    delete_openvswitch_interface $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP
    delete_openvswitch_interface $MME_INTERFACE_NAME_FOR_S11_MME
    delete_openvswitch_interface $SGW_INTERFACE_NAME_FOR_S11
    if  is_process_started ovs-vswitchd  ; then
        ovs-vsctl del-br $BRIDGE > /dev/null 2>&1
    fi
    stop_openswitch_daemon
}

build_enb_vlan_network() {
    # create vlan interface
    is_vlan_interface $ENB_INTERFACE_NAME_FOR_S1_MME
    if [ $? -eq 1 ]; then
        interface_name=`echo $ENB_INTERFACE_NAME_FOR_S1_MME | cut -f1 -d '.'`
        vlan=`echo $ENB_INTERFACE_NAME_FOR_S1_MME | cut -f2 -d '.'`
        ifconfig    $ENB_INTERFACE_NAME_FOR_S1_MME down > /dev/null 2>&1
        vconfig rem $ENB_INTERFACE_NAME_FOR_S1_MME      > /dev/null 2>&1
        sync
        bash_exec "vconfig add $interface_name $vlan"
        sync
        #bash_exec "ifconfig  $ENB_INTERFACE_NAME_FOR_S1_MME up"
        #sync
        #bash_exec "ip -4 addr add  $ENB_IPV4_ADDRESS_FOR_S1_MME/$ENB_IPV4_NETMASK_FOR_S1_MME dev $ENB_INTERFACE_NAME_FOR_S1_MME"
        set_interface_up $ENB_INTERFACE_NAME_FOR_S1_MME $ENB_IPV4_ADDRESS_FOR_S1_MME $ENB_IPV4_NETMASK_FOR_S1_MME
    else
        echo_fatal "BAD INTERFACE NAME FOR ENB S1-MME $ENB_INTERFACE_NAME_FOR_S1_MME"' (waiting for ethx.y, wlanx.y or wifix.y)'
    fi;
    
    is_vlan_interface $ENB_INTERFACE_NAME_FOR_S1U
    if [ $? -eq 1 ]; then
        interface_name=`echo $ENB_INTERFACE_NAME_FOR_S1U | cut -f1 -d '.'`
        vlan=`echo $ENB_INTERFACE_NAME_FOR_S1U | cut -f2 -d '.'`
        ifconfig    $ENB_INTERFACE_NAME_FOR_S1U down > /dev/null 2>&1
        vconfig rem $ENB_INTERFACE_NAME_FOR_S1U      > /dev/null 2>&1
        sync
        bash_exec "vconfig add $interface_name $vlan"
        sync
        #bash_exec "ifconfig $ENB_INTERFACE_NAME_FOR_S1U up"
        #sync
        #bash_exec "ip -4 addr add  $ENB_IPV4_ADDRESS_FOR_S1U/$ENB_IPV4_NETMASK_FOR_S1U dev $ENB_INTERFACE_NAME_FOR_S1U"
        set_interface_up $ENB_INTERFACE_NAME_FOR_S1U $ENB_IPV4_ADDRESS_FOR_S1U $ENB_IPV4_NETMASK_FOR_S1U
        sync
    else
        echo_fatal "BAD INTERFACE NAME FOR ENB S1U $ENB_INTERFACE_NAME_FOR_S1U"' (waiting for ethx.y, wlanx.y or wifix.y)'
    fi;
}

clean_enb_vlan_network() {
    is_vlan_interface $ENB_INTERFACE_NAME_FOR_S1_MME
    if [ $? -eq 1 ]; then
        echo_success "Found VLAN interface $ENB_INTERFACE_NAME_FOR_S1_MME ... deleting"
        ifconfig    $ENB_INTERFACE_NAME_FOR_S1_MME down > /dev/null 2>&1
        vconfig rem $ENB_INTERFACE_NAME_FOR_S1_MME      > /dev/null 2>&1
    fi;
    
    is_vlan_interface $ENB_INTERFACE_NAME_FOR_S1U
    if [ $? -eq 1 ]; then
        echo_success "Found VLAN interface $ENB_INTERFACE_NAME_FOR_S1U ... deleting"
        ifconfig    $ENB_INTERFACE_NAME_FOR_S1U down > /dev/null 2>&1
        vconfig rem $ENB_INTERFACE_NAME_FOR_S1U > /dev/null 2>&1
    fi;
    sync;
    clean_network
}



build_mme_spgw_vlan_network() {
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
    
    bash_exec "modprobe 8021q"
    
    # create vlan interface
    is_vlan_interface $MME_INTERFACE_NAME_FOR_S1_MME
    if [ $? -eq 1 ]; then
        interface_name=`echo $MME_INTERFACE_NAME_FOR_S1_MME | cut -f1 -d '.'`
        vlan=`echo $MME_INTERFACE_NAME_FOR_S1_MME | cut -f2 -d '.'`
        ifconfig    $MME_INTERFACE_NAME_FOR_S1_MME down > /dev/null 2>&1
        vconfig rem $MME_INTERFACE_NAME_FOR_S1_MME      > /dev/null 2>&1
        sync
        bash_exec "vconfig add $interface_name $vlan"
        sync
        #bash_exec "ifconfig  $MME_INTERFACE_NAME_FOR_S1_MME up"
        #sync
        #"bash_exec "ip -4 addr add  $MME_IPV4_ADDRESS_FOR_S1_MME/$MME_IPV4_NETMASK_FOR_S1_MME dev $MME_INTERFACE_NAME_FOR_S1_MME"
        set_interface_up $MME_INTERFACE_NAME_FOR_S1_MME $MME_IPV4_ADDRESS_FOR_S1_MME $MME_IPV4_NETMASK_FOR_S1_MME
    else
        echo_fatal "BAD INTERFACE NAME FOR SGW S1-MME $MME_INTERFACE_NAME_FOR_S1_MME"' (waiting for ethx.y, wlanx.y or wifix.y)'
    fi;
    
    is_vlan_interface $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP
    if [ $? -eq 1 ]; then
        interface_name=`echo $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP | cut -f1 -d '.'`
        vlan=`echo $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP | cut -f2 -d '.'`
        ifconfig    $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP down > /dev/null 2>&1
        vconfig rem $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP      > /dev/null 2>&1
        sync
        bash_exec "vconfig add $interface_name $vlan"
        sync
        #bash_exec "ifconfig  $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP up"
        #sync
        #bash_exec "ip -4 addr add  $SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP/$SGW_IPV4_NETMASK_FOR_S1U_S12_S4_UP dev $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP"
        #sync
        set_interface_up $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP $SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP $SGW_IPV4_NETMASK_FOR_S1U_S12_S4_UP
    else
        echo_fatal "BAD INTERFACE NAME FOR SGW S1U $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP"' (waiting for ethx.y, wlanx.y or wifix.y)'
    fi;
    
    #
    is_real_interface $PGW_INTERFACE_NAME_FOR_SGI
    if [ $? -eq 1 ]; then
        create_sgi_vlans
    else
        echo_warning "SGI interface disabled by config file"
    fi
}

clean_epc_vlan_network() {
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

    bash_exec "modprobe 8021q"

    delete_vlan_interface  $MME_INTERFACE_NAME_FOR_S1_MME  
    delete_vlan_interface  $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP  


    for i in 5 6 7 8 9 10 11 12 13 14 15
    do
        # delete vlan interface
        delete_vlan_interface   $PGW_INTERFACE_NAME_FOR_SGI.$i down 
    done
    clean_network
}

build_tun_network() {
    # REMINDER:
    #                                                                           hss.eur
    #                                                                             |
    #        +-----------+                                +-----------+           v   +----------+
    #        |  eNB      +------+                  +------+    MME    +----+      +---+   HSS    |
    #        |           |cpenb0+------------------+cpmme0|           |s6am+------+s6a|          |
    #        |           +------+                  +------+           +----+      +---+          |
    #        |           |upenb0+-------+                 |           |               +----------+
    #        |           +------+       |                 +-+-------+-+
    #        |           |              |                   | s11mme| 
    #        |           |              |                   +---+---+ 
    #        |           |              |             (optional)| 
    #        +-----------+              |                   +---+---+ 
    #                                   |                   | s11sgw|             router.eur
    #                                   |                 +-+-------+-+              |   +--------------+
    #                                   |                 |  S+P-GW   |              v   |   ROUTER     |
    #                                   |          +------+           +-------+     +----+              +----+
    #                                   +----------+upsgw0|           |sgi    +-...-+    |              |    +---...Internet
    #                                              +------+           +-------+     +----+              +----+
    #                                                     |           |      11 VLANS    |              |
    #                                                     +-----------+   ids=[5..15]    +--------------+
    #
    bash_exec "modprobe tun"
    ##################################################
    # build network between eNB and MME/SPGW and HSS
    ##################################################
    echo_success  "ENB_INTERFACE_NAME_FOR_S1_MME is "$ENB_INTERFACE_NAME_FOR_S1_MME" addr is "$ENB_IPV4_ADDRESS_FOR_S1_MME
    echo_success  "ENB_INTERFACE_NAME_FOR_S1U is "$ENB_INTERFACE_NAME_FOR_S1U" addr is "$ENB_IPV4_ADDRESS_FOR_S1U
    echo_success  "MME_INTERFACE_NAME_FOR_S1_MME is "$MME_INTERFACE_NAME_FOR_S1_MME" addr is "$MME_IPV4_ADDRESS_FOR_S1_MME
    echo_success  "SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP is "$SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP" addr is "$SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP
    echo_success  "MME_INTERFACE_NAME_FOR_S11_MME is "$MME_INTERFACE_NAME_FOR_S11_MME" addr is "$MME_IPV4_ADDRESS_FOR_S11_MME
    echo_success  "SGW_INTERFACE_NAME_FOR_S11 is "$SGW_INTERFACE_NAME_FOR_S11" addr is "$SGW_IPV4_ADDRESS_FOR_S11
    echo_success  "MME_INTERFACE_NAME_FOR_S6A is "$MME_INTERFACE_NAME_FOR_S6A" addr is "$MME_IPV4_ADDRESS_FOR_S6A
    echo_success  "HSS_INTERFACE_NAME_FOR_S6A is "$HSS_INTERFACE_NAME_FOR_S6A" addr is "$HSS_IPV4_ADDRESS_FOR_S6A
    
    create_tun_interface $ENB_INTERFACE_NAME_FOR_S1_MME
    create_tun_interface $ENB_INTERFACE_NAME_FOR_S1U
    create_tun_interface $MME_INTERFACE_NAME_FOR_S1_MME
    create_tun_interface $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP
    create_tun_interface $MME_INTERFACE_NAME_FOR_S11_MME
    create_tun_interface $SGW_INTERFACE_NAME_FOR_S11
    create_tun_interface $MME_INTERFACE_NAME_FOR_S6A
    create_tun_interface $HSS_INTERFACE_NAME_FOR_S6A
    
    set_interface_up $MME_INTERFACE_NAME_FOR_S1_MME        $MME_IPV4_ADDRESS_FOR_S1_MME        $MME_IPV4_NETMASK_FOR_S1_MME
    set_interface_up $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP $SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP $SGW_IPV4_NETMASK_FOR_S1U_S12_S4_UP

    set_interface_up $ENB_INTERFACE_NAME_FOR_S1_MME        $ENB_IPV4_ADDRESS_FOR_S1_MME        $ENB_IPV4_NETMASK_FOR_S1_MME
    set_interface_up $ENB_INTERFACE_NAME_FOR_S1U           $ENB_IPV4_ADDRESS_FOR_S1U           $ENB_IPV4_NETMASK_FOR_S1U

    set_interface_up $MME_INTERFACE_NAME_FOR_S11_MME       $MME_IPV4_ADDRESS_FOR_S11_MME       $MME_IPV4_NETMASK_FOR_S11_MME
    set_interface_up $SGW_INTERFACE_NAME_FOR_S11           $SGW_IPV4_ADDRESS_FOR_S11           $SGW_IPV4_NETMASK_FOR_S11

    set_interface_up $MME_INTERFACE_NAME_FOR_S6A           $MME_IPV4_ADDRESS_FOR_S6A           $MME_IPV4_NETMASK_FOR_S6A
    set_interface_up $HSS_INTERFACE_NAME_FOR_S6A           $HSS_IPV4_ADDRESS_FOR_S6A           $HSS_IPV4_NETMASK_FOR_S6A
}

# arg1 is IF1 IP addr
# arg2 is IF2 IP addr
test_local_iperf() {
    
    TRY_NO=3

    until [  $TRY_NO -lt 1 ]; do
        iperf  --bind $1 -u -s 2>&1  > /dev/null &
        iperf  --bind $2 -u --num 1K -c $1 2>&1 | grep -i WARNING > /dev/null
        if [ $? -eq 0 ]; then
            pkill iperf 2>&1 > /dev/null
            echo_warning "NETWORK TEST FAILED between $1 and $2"
        else
            echo_success "NETWORK TEST SUCCESS between $1 and $2"
            pkill iperf 2>&1 > /dev/null
            return
        fi 
        let TRY_NO-=1
        pkill iperf 2>&1 > /dev/null
    done
    echo_fatal "FATAL: NETWORK TEST FAILED between $1 and $2"
}

test_network() {

    # TEST INTERFACES
    if [ "a${MME_INTERFACE_NAME_FOR_S1_MME:0:4}" != "anone" ]; then
        ping -q -c 1 $MME_IPV4_ADDRESS_FOR_S1_MME > /dev/null 2>&1
        if [ $? -ne 0 ]; then echo_fatal "PING INTERFACE $MME_INTERFACE_NAME_FOR_S1_MME ERROR, ADDRESS IS $MME_IPV4_ADDRESS_FOR_S1_MME $LINENO"; fi;
    fi
    
    if [ "a${SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP:0:4}" != "anone" ]; then
        ping -q -c 1 $SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP > /dev/null 2>&1
        if [ $? -ne 0 ]; then echo_fatal "PING INTERFACE $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP ERROR, ADDRESS IS $SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP $LINENO"; fi;
    fi
    
    if [ "a${ENB_INTERFACE_NAME_FOR_S1_MME:0:4}" != "anone" ]; then
        ping -q -c 1 $ENB_IPV4_ADDRESS_FOR_S1_MME > /dev/null 2>&1
        if [ $? -ne 0 ]; then echo_fatal "PING INTERFACE $ENB_INTERFACE_NAME_FOR_S1_MME ADDR ERROR, ADDRESS IS $ENB_IPV4_ADDRESS_FOR_S1_MME $LINENO"; fi;
    fi
    
    if [ "a${ENB_INTERFACE_NAME_FOR_S1U:0:4}" != "anone" ]; then
        ping -q -c 1 $ENB_IPV4_ADDRESS_FOR_S1U > /dev/null 2>&1
        if [ $? -ne 0 ]; then echo_fatal "PING INTERFACE $ENB_INTERFACE_NAME_FOR_S1U ERROR, ADDRESS IS $ENB_IPV4_ADDRESS_FOR_S1U $LINENO"; fi;
    fi
    
    if [ "a${MME_INTERFACE_NAME_FOR_S11_MME:0:4}" != "anone" ]; then
        ping -q -c 1 $MME_IPV4_ADDRESS_FOR_S11_MME > /dev/null 2>&1
        if [ $? -ne 0 ]; then echo_fatal "PING INTERFACE $MME_INTERFACE_NAME_FOR_S11_MME ERROR, ADDRESS IS $MME_IPV4_ADDRESS_FOR_S11_MME $LINENO"; fi;
    fi
    
    if [ "a${SGW_INTERFACE_NAME_FOR_S11:0:4}" != "anone" ]; then
        ping -q -c 1 $SGW_IPV4_ADDRESS_FOR_S11 > /dev/null 2>&1
        if [ $? -ne 0 ]; then echo_fatal "PING INTERFACE $SGW_INTERFACE_NAME_FOR_S11 ERROR, ADDRESS IS $SGW_IPV4_ADDRESS_FOR_S11 $LINENO"; fi;
    fi
    
    if [ "a${MME_INTERFACE_NAME_FOR_S6A:0:4}" != "anone" ]; then
        ping -q -c 1 $MME_IPV4_ADDRESS_FOR_S6A > /dev/null 2>&1
        if [ $? -ne 0 ]; then echo_fatal "PING INTERFACE $MME_INTERFACE_NAME_FOR_S6A ERROR, ADDRESS IS $MME_IPV4_ADDRESS_FOR_S6A $LINENO"; fi;
    fi
    
    if [ "a${HSS_INTERFACE_NAME_FOR_S6A:0:4}" != "anone" ]; then
        ping -q -c 1 $HSS_IPV4_ADDRESS_FOR_S6A > /dev/null 2>&1
        if [ $? -ne 0 ]; then echo_fatal "PING INTERFACE $HSS_INTERFACE_NAME_FOR_S6A ERROR, ADDRESS IS $HSS_IPV4_ADDRESS_FOR_S6A $LINENO"; fi;
    fi
    
    ## TEST NETWORK BETWEEN ENB-MME-SP-GW
    
    if [ "a${MME_INTERFACE_NAME_FOR_S1_MME:0:4}" != "anone" ] && [ "a${ENB_INTERFACE_NAME_FOR_S1_MME:0:4}" != "anone" ]; then
        test_local_iperf $ENB_IPV4_ADDRESS_FOR_S1_MME  $MME_IPV4_ADDRESS_FOR_S1_MME 
    fi
    
    if [ "a${ENB_INTERFACE_NAME_FOR_S1U:0:4}" != "anone" ] && [ "a${SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP:0:4}" != "anone" ]; then
        test_local_iperf $ENB_IPV4_ADDRESS_FOR_S1U     $SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP
    fi
    
    if [ "a${MME_INTERFACE_NAME_FOR_S11_MME:0:4}" != "anone" ] && [ "a${SGW_INTERFACE_NAME_FOR_S11:0:4}" != "anone" ]; then
        test_local_iperf $MME_IPV4_ADDRESS_FOR_S11_MME $SGW_IPV4_ADDRESS_FOR_S11
    fi
    
    if [ "a${MME_INTERFACE_NAME_FOR_S6A:0:4}" != "anone" ]  &&  [ "a${HSS_INTERFACE_NAME_FOR_S6A:0:4}" != "anone" ]; then
        test_local_iperf $MME_IPV4_ADDRESS_FOR_S6A     $HSS_IPV4_ADDRESS_FOR_S6A
    fi

    # Get MAC address of router.eur
    ping -c 1 hss.eur > /dev/null || { echo_fatal "hss.eur does not respond to ping" >&2 ; }
    ping -c 1 router.eur > /dev/null || { echo_fatal "router.eur does not respond to ping" >&2 ; }
    return 0
}

clean_tun_network() {
    bash_exec "modprobe tun"
    ##################################################
    # del interfaces eNB and MME/SPGW and HSS
    ##################################################
    delete_tun_interface $ENB_INTERFACE_NAME_FOR_S1_MME
    delete_tun_interface $ENB_INTERFACE_NAME_FOR_S1U
    delete_tun_interface $MME_INTERFACE_NAME_FOR_S1_MME
    delete_tun_interface $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP
    delete_tun_interface $MME_INTERFACE_NAME_FOR_S11_MME
    delete_tun_interface $SGW_INTERFACE_NAME_FOR_S11
    delete_tun_interface $MME_INTERFACE_NAME_FOR_S6A
    delete_tun_interface $HSS_INTERFACE_NAME_FOR_S6A
}

build_epc_tun_network() {

    build_tun_network
    create_sgi_vlans
}

create_sgi_vlans() {

    get_mac_router

    bash_exec "modprobe 8021q"

    for i in 5 6 7 8 9 10 11 12 13 14 15
    do
        # create vlan interface
        ifconfig    $PGW_INTERFACE_NAME_FOR_SGI.$i down > /dev/null 2>&1
        vconfig rem $PGW_INTERFACE_NAME_FOR_SGI.$i      > /dev/null 2>&1
        sync
        bash_exec "vconfig add $PGW_INTERFACE_NAME_FOR_SGI $i"
        sync
        bash_exec "ifconfig  $PGW_INTERFACE_NAME_FOR_SGI.$i up"
        sync
        # configure vlan interface
        #CIDR=$NETWORK'.'$i'/24'
        base=200
        NET=$(( $i + $base ))
        CIDR='10.0.'$NET'.2/24'
        bash_exec "ip -4 addr add  $CIDR dev $PGW_INTERFACE_NAME_FOR_SGI.$i"
    done

    bash_exec "ip link set $PGW_INTERFACE_NAME_FOR_SGI promisc on"

}


clean_epc_ovs_network() {
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
    
    
    for i in 5 6 7 8 9 10 11 12 13 14 15
    do
        ifconfig $PGW_INTERFACE_NAME_FOR_SGI.$i down > /dev/null 2>&1
        vconfig rem $PGW_INTERFACE_NAME_FOR_SGI.$i   > /dev/null 2>&1
    done
    
    clean_network
    clean_tun_network
}

clean_network() {
  interfaces=`ifconfig | grep HWaddr | cut -d " " -f1-2 | tr -d '\n'`
  for interface in $interfaces
  do
      is_openvswitch_interface $interface
      if [ $? -eq 1 ]; then
         echo_success "Found open-vswitch interface $interface ... deleting"
         delete_openvswitch_interface $interface
      fi
      
      is_vlan_interface $interface
      if [ $? -eq 1 ]; then
         echo_success "Found VLAN interface $interface ... deleting"
         ifconfig    $interface down > /dev/null 2>&1
         vconfig rem $interface      > /dev/null 2>&1
      fi
  done
}
