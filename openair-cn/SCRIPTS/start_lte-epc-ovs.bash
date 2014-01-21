#!/bin/bash
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
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
source $THIS_SCRIPT_PATH/utils.bash
###########################################################

test_command_install_package "gccxml" "gccxml" "--force-yes"
test_command_install_package "vconfig" "vlan"
test_command_install_package "iptables" "iptables"
test_command_install_package "ip" "iproute"
test_command_install_script   "ovs-vsctl" "$OPENAIRCN_DIR/SCRIPTS/install_openvswitch1.9.0.bash"
test_command_install_script   "tunctl" "uml-utilities"
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


IPTABLES=`which iptables`

cd $OPENAIRCN_DIR
##################################
# Get or set OBJ DIR and compile #
##################################
# TEST IF EXIST
OBJ_DIR=`find . -maxdepth 1 -type d -iname obj*`
if [ -n "$OBJ_DIR" ]
then
    OBJ_DIR=`basename $OBJ_DIR`
    if [ ! -f $OBJ_DIR/Makefile ]
    then
        cd ./$OBJ_DIR
        echo_success "Invoking configure"
        rm -f Makefile
        ../configure --enable-standalone-epc --disable-nas LDFLAGS=-L/usr/local/lib
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
    ../configure --enable-standalone-epc LDFLAGS=-L/usr/local/lib
fi
if [ -f Makefile ]
then
    echo_success "Compiling..."
    bash_exec "make"
else
    echo_error "Configure failed, exiting"
    exit 1
fi
cd $OPENAIRCN_DIR


cat $OPENAIRCN_DIR/$OBJ_DIR/Makefile | grep CFLAGS\ \=\  | grep DENABLE_USE_NETFILTER_FOR_SGI
if [ $? -ne 0 ]
then
    export ENABLE_USE_NETFILTER_FOR_SGI=0
else
    export ENABLE_USE_NETFILTER_FOR_SGI=1
fi

cat $OPENAIRCN_DIR/$OBJ_DIR/Makefile | grep CFLAGS\ \=\  | grep DENABLE_USE_RAW_FOR_SGI
if [ $? -ne 0 ]
then
    export ENABLE_USE_RAW_FOR_SGI=0
else
    export ENABLE_USE_RAW_FOR_SGI=1
fi

pkill oai_epc

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


ping -c 1 hss.eur || { echo "hss.eur does not respond to ping" >&2 ; exit ; }
ping -c 1 router.eur || { echo "router.eur does not respond to ping" >&2 ; exit ; }
IP_ROUTER=`python -c 'import socket; print socket.gethostbyname("router.eur")'`
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

#echo_warning "LG FORCED EXIT"
#exit

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
gdb --args $OPENAIRCN_DIR/$OBJ_DIR/OAI_EPC/oai_epc -c $MME_CONFIG_FILE
wait_process_started "oai_epc"
