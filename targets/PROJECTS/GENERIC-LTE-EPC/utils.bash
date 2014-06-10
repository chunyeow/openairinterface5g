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
# file utils.bash
# brief
# author Lionel Gauthier
# company Eurecom
# email: lionel.gauthier@eurecom.fr
#
cidr2mask() {
  local i mask=""
  local full_octets=$(($1/8))
  local partial_octet=$(($1%8))

  for ((i=0;i<4;i+=1)); do
    if [ $i -lt $full_octets ]; then
      mask+=255
    elif [ $i -eq $full_octets ]; then
      mask+=$((256 - 2**(8-$partial_octet)))
    else
      mask+=0
    fi
    test $i -lt 3 && mask+=.
  done

  echo $mask
}

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

# example: s
bcastcalc(){

    local IFS='.' ip i
    local -a oct msk
    
    read -ra oct <<<"$1"
    read -ra msk <<<"$2"

    for i in ${!oct[@]}; do
        ip+=( "$(( oct[i] + ( 255 - ( oct[i] | msk[i] ) ) ))" )
    done

    echo "${ip[*]}"
}

black='\E[30m'
red='\E[31m'
green='\E[32m'
yellow='\E[33m'
blue='\E[34m'
magenta='\E[35m'
cyan='\E[36m'
white='\E[37m'
reset_color='\E[00m'

ROOT_UID=0
E_NOTROOT=67

HOSTNAME=$(hostname -f)
IPTABLES=`which iptables`

trim ()
{
    echo "$1" | sed -n '1h;1!H;${;g;s/^[ \t]*//g;s/[ \t]*$//g;p;}'
}

trim2()
{
    local var=$@
    var="${var#"${var%%[![:space:]]*}"}"   # remove leading whitespace characters
    var="${var%"${var##*[![:space:]]}"}"   # remove trailing whitespace characters
    echo -n "$var"
}

cecho()   # Color-echo
# arg1 = message
# arg2 = color
{
    local default_msg="No Message."
    message=${1:-$default_msg}
    color=${2:-$green}
    echo -e -n "$color$message$reset_color"
    echo
    return
}

echo_error() {
    local my_string=""
    until [ -z "$1" ]
    do
        my_string="$my_string$1"
        shift
        done
        cecho "$my_string" $red
}

echo_fatal() {
    local my_string=""
    until [ -z "$1" ]
    do
        my_string="$my_string$1"
        shift
        done
        echo_error "$my_string"
    exit -1
}

echo_warning() {
    local my_string=""
    until [ -z "$1" ]
    do
        my_string="$my_string$1"
        shift
    done
    cecho "$my_string" $yellow
}

echo_success() {
    local my_string=""
    until [ -z "$1" ]
    do
        my_string="$my_string$1"
        shift
    done
    cecho "$my_string" $green
}

bash_exec() {
    output=$($1 2>&1)
    result=$?
    if [ $result -eq 0 ]
    then
        echo_success "$1"
    else
        echo_error "$1: $output"
    fi
}

extract() {
    if [ -f $1 ] ; then
        case $1 in
            *.tar.bz2)    tar xvjf $1        ;;
            *.tar.gz)     tar xvzf $1        ;;
            *.bz2)        bunzip2  $1        ;;
            *.rar)        unrar    $1        ;;
            *.gz)         gunzip   $1        ;;
            *.tar)        tar xvf  $1        ;;
            *.tbz2)       tar xvjf $1        ;;
            *.tgz)        tar xvzf $1        ;;
            *.zip)        unzip    $1        ;;
            *.Z)          uncompress $1      ;;
            *.7z)         7z x     $1        ;;
            *)            echo_error "'$1' cannot be extracted via >extract<" ; return 1;;
        esac
    else
        echo_error "'$1' is not a valid file"
        return 1
    fi
    return 0
}

rotate_log_file () {
    FULLPATH=$1
    if [ -f $FULLPATH ]; then
        FILENAME=${FULLPATH##*/}
        FILEEXTENSION=${FILENAME##*.}
        BASEDIRECTORY=${FULLPATH%$FILENAME}
        if [ "a$BASEDIRECTORY" == "a" ]; then
            BASEDIRECTORY='.'
        fi
        FILENAME_NO_EXT=$(echo "$FILENAME" | sed 's/\.[^\.]*$//')

        # use file last modification time
        TIMESTAMP=`date -r $FULLPATH +%Y-%m-%d.%Hh_%Mm_%Ss`
        
        NEWLOGFILE=$TIMESTAMP.$FILENAME_NO_EXT.$FILEEXTENSION
        mv $FULLPATH /tmp/$NEWLOGFILE
        cat /dev/null > $FULLPATH
        sync
        gzip -c --name -f -9 /tmp/$NEWLOGFILE > $BASEDIRECTORY/$NEWLOGFILE.gz &
    fi
}

set_openair() {
    path=`pwd`
    declare -i length_path
    declare -i index
    length_path=${#path}

    for i in 'openair1' 'openair2' 'openair3' 'openair-cn' 'targets'
    do
        index=`echo $path | grep -b -o $i | cut -d: -f1`
        #echo ${path%$token*}
        if [[ $index -lt $length_path  && index -gt 0 ]]
           then
               index=`expr $index - 1`
               openair_path=`echo $path | cut -c1-$index`
               #openair_path=`echo ${path:0:$index}`
               export OPENAIR_DIR=$openair_path
               export OPENAIR_HOME=$openair_path
               export OPENAIR1_DIR=$openair_path/openair1
               export OPENAIR2_DIR=$openair_path/openair2
               export OPENAIR3_DIR=$openair_path/openair3
               export OPENAIRCN_DIR=$openair_path/openair-cn
               export OPENAIR_TARGETS=$openair_path/targets
               return 0
           fi
    done
    return -1
}

wait_process_started () {
    if  [ -z "$1" ]
    then
        echo_error "WAITING FOR PROCESS START: NO PROCESS"
        return 1
    fi
    ps -C $1 > /dev/null 2>&1
    while [ $? -ne 0 ]; do
        echo_warning "WAITING FOR $1 START"
        sleep 2
        ps -C $1 > /dev/null 2>&1
    done
    echo_success "PROCESS $1 STARTED"
    return 0
}

is_process_started () {
    if  [ -z "$1" ]
    then
        echo_error "WAITING FOR PROCESS START: NO PROCESS"
        return 1
    fi
    ps -C $1 > /dev/null 2>&1
    if [ $? -ne 0 ]
    then
        echo_success "PROCESS $1 NOT STARTED"
        return 1
    fi
    echo_success "PROCESS $1 STARTED"
    return 0
}

assert() {
    # If condition false
    # exit from script with error message
    E_PARAM_ERR=98
    E_PARAM_FAILED=99

    if [ -z "$2" ] # Not enought parameters passed.
    then
        return $E_PARAM_ERR
    fi

    lineno=$2
    if [ ! $1 ]
    then
        echo_error "Assertion failed:  \"$1\""
        echo_fatal "File \"$0\", line $lineno"
    fi
}


test_install_package() {
  # usage: test_install_package package_name
  if [ $# -eq 1 ]; then
      dpkg -s "$1" > /dev/null 2>&1 && {
          echo "$1 is installed."
      } || {
          echo "$1 is not installed."
          apt-get install --assume-yes $1 
      }
  fi
}

test_command_install_script() {
  # usage: test_command_install_script searched_binary script_to_be_invoked_if_binary_not_found
  command -v $1 >/dev/null 2>&1 || { echo_warning "Program $1 is not installed. Trying installing it." >&2; bash $2; command -v $1 >/dev/null 2>&1 || { echo_fatal "Program $1 is not installed. Aborting." >&2; };}
  echo_success "$1 available"
}




check_for_epc_executable() {
    if [ ! -f $OPENAIR3_DIR/OPENAIRMME/objs/OAI_EPC/oai_epc ]
        then
        echo_error "Cannot find oai_epc executable object in directory $OPENAIR3_DIR/OPENAIRMME/objs/OAI_EPC/"
        echo_fatal "Please make sure you have compiled OAI EPC with --enable-standalone-epc option"
        fi
}

check_for_sgw_executable() {
    if [ ! -f $OPENAIR3_DIR/OPENAIRMME/objs/OAI_SGW/oai_sgw ]
    then
        echo_error "Cannot find oai_sgw executable object in directory $OPENAIR3_DIR/OPENAIRMME/objs/OAI_SGW/"
        echo_fatal "Please make sure you have compiled OAI EPC without --enable-standalone-epc option"
    fi
}

check_for_mme_executable() {
    if [ ! -f $OPENAIR3_DIR/OPENAIRMME/objs/OAISIM_MME/oaisim_mme ]
    then
        echo_error "Cannot find oai_sgw executable object in directory $OPENAIR3_DIR/OPENAIRMME/objs/OAISIM_MME/"
        echo_fatal "Please make sure you have compiled OAI EPC without --enable-standalone-epc option"
    fi
}

check_for_root_rights() {
    if [[ $EUID -ne 0 ]]; then
        echo_fatal "This script must be run as root" 1>&2
    fi
}


is_real_interface() {
    my_bool=1
    for var in "$@"
    do
        if [ "a$var" == "a" ]; then
           return 0
        fi
        if [ "a$var" == "anone" ]; then
           return 0
        fi
        IF=`cat /etc/udev/rules.d/70-persistent-net.rules | grep $var | sed 's/^.*NAME=//' | tr -d '"'`
        if [ "$IF" == "$var" ]; then
            if [ "a${var:0:3}" != "aeth" ]; then
                if [ "a${var:0:4}" != "awlan" ]; then
                    if [ "a${var:0:4}" != "awifi" ]; then
                        my_bool=0;
                    fi
                fi
            fi
        fi
    done
    return $my_bool
}

is_vlan_interface() {
    my_bool=1
    for var in "$@"
    do
        if [ "a$var" == "a" ]; then
            return 0
        fi
        if [[ $var == *.* ]]
        then
            interface_name=`echo $var | cut -f1 -d '.'`
            vlan=`echo $var | cut -f2 -d '.'`
            IF=`cat /etc/udev/rules.d/70-persistent-net.rules | grep $interface_name | sed 's/^.*NAME=//' | tr -d '"'`
            if [ "$IF" == "$interface_name" ]; then
                if [ "a${interface_name:0:3}" != "aeth" ]; then
                    if [ "a${interface_name:0:4}" != "awlan" ]; then
                        if [ "a${interface_name:0:4}" != "awifi" ]; then
                            return 0;
                        fi
                    fi
                fi
            fi
        else
            return 0;
        fi
    done
    return $my_bool
}

is_tun_interface() {
    my_bool=1
    for var in "$@"
    do
        if [ "a$var" == "a" ]; then
            return 0
        fi
        bus_info=`ethtool -i $var`
        if [[ "$IF" != *tun* ]]; then
            return 0;
        fi
    done
    return $my_bool
}


delete_tun_interface() {
  is_tun_interface $1 
  if [ $? -eq 1 ]; then
      ip link set $1 down  > /dev/null 2>&1
      openvpn --mktun --dev $1  > /dev/null 2>&1
  fi
}


create_tun_interface() {
  openvpn --mktun --dev $1
  #ip link set $1 up
}


# arg1 = interface name
# arg2 = ipv4 addr cidr
# arg3 = netmask cidr
set_interface_up() {
    interface=$1
    address=$2
    cidr_netmask=$3
    bash_exec "ifconfig  $interface up"
    sync
    netmask=`cidr2mask $cidr_netmask`
    broadcast=`bcastcalc $address $netmask`
    bash_exec "ip -4 addr add  $address/$cidr_netmask broadcast $broadcast dev $interface"
    sync
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


test_enb_vlan_network() {
    # TEST INTERFACES
    #ping -q -c 1 $MME_IPV4_ADDRESS_FOR_S1_MME > /dev/null 2>&1
    #if [ $? -ne 0 ]; then echo_fatal "PING MME S1_MME ERROR, ADDRESS IS $MME_IPV4_ADDRESS_FOR_S1_MME"; fi;
    #ping -q -c 1 $SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP > /dev/null 2>&1
    #if [ $? -ne 0 ]; then echo_fatal "PING SGW S1U ERROR, ADDRESS IS $SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP"; fi;
    return 0
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
        ping -c 1 router.eur > /dev/null || { echo_fatal "router.eur does not respond to ping" >&2 ; }
        IP_ROUTER=`python -c 'import socket; print socket.gethostbyname("router.eur")'`
        export MAC_ROUTER=`ip neigh show | grep $IP_ROUTER | cut -d ' '  -f5 | tr -d ':'`
        echo_success "ROUTER MAC ADDRESS= $MAC_ROUTER"


        # # get ipv4 address from PGW_INTERFACE_NAME_FOR_SGI
        #IP_ADDR=`ifconfig $PGW_INTERFACE_NAME_FOR_SGI | awk '/inet addr/ {split ($2,A,":"); print A[2]}' | tr '\n' ' ' | sed -n '1h;1!H;${;g;s/^[ \t]*//g;s/[ \t]*$//g;p;}'`

        #NETWORK=`echo $IP_ADDR | cut -d . -f 1,2,3`

        bash_exec "modprobe 8021q"

        for i in 5 6 7 8 9 10 11 12 13 14 15
        do
            # create vlan interface
            ifconfig    $PGW_INTERFACE_NAME_FOR_SGI.$i down > /dev/null 2>&1
            vconfig rem $PGW_INTERFACE_NAME_FOR_SGI.$i > /dev/null 2>&1
            sync
            bash_exec "vconfig add $PGW_INTERFACE_NAME_FOR_SGI $i"
            sync
            bash_exec "ifconfig  $PGW_INTERFACE_NAME_FOR_SGI.$i up"
            sync
            # configure vlan interface
            #CIDR=$NETWORK'.'$i'/24'
            base=200
            NET=$(( $i + $base ))
            CIDR='10.0.'$NET'.2/8'
            bash_exec "ip -4 addr add  $CIDR dev $PGW_INTERFACE_NAME_FOR_SGI.$i"
        done

        bash_exec "ip link set $PGW_INTERFACE_NAME_FOR_SGI promisc on"
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

    ifconfig    $MME_INTERFACE_NAME_FOR_S1_MME down > /dev/null 2>&1
    vconfig rem $MME_INTERFACE_NAME_FOR_S1_MME      > /dev/null 2>&1

    ifconfig    $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP down > /dev/null 2>&1
    vconfig rem $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP      > /dev/null 2>&1


    for i in 5 6 7 8 9 10 11 12 13 14 15
    do
        # delete vlan interface
        ifconfig    $PGW_INTERFACE_NAME_FOR_SGI.$i down > /dev/null 2>&1
        vconfig rem $PGW_INTERFACE_NAME_FOR_SGI.$i      > /dev/null 2>&1
    done
    #ip link set $PGW_INTERFACE_NAME_FOR_SGI down > /dev/null 2>&1
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

    set_interface_up $MME_INTERFACE_NAME_FOR_S11_MME       $MME_IPV4_ADDRESS_FOR_S11_MME       $MME_IPV4_NETMASK_FOR_S11_MME
    set_interface_up $SGW_INTERFACE_NAME_FOR_S11           $SGW_IPV4_ADDRESS_FOR_S11           $SGW_IPV4_NETMASK_FOR_S11

    set_interface_up $MME_INTERFACE_NAME_FOR_S6A           $MME_IPV4_ADDRESS_FOR_S6A           $MME_IPV4_NETMASK_FOR_S6A
    set_interface_up $HSS_INTERFACE_NAME_FOR_S6A           $HSS_IPV4_ADDRESS_FOR_S6A           $HSS_IPV4_NETMASK_FOR_S6A
}

test_tun_network() {

    # TEST INTERFACES
    ping -q -c 1 $MME_IPV4_ADDRESS_FOR_S1_MME > /dev/null 2>&1
    if [ $? -ne 0 ]; then echo_fatal "PING INTERFACE $MME_INTERFACE_NAME_FOR_S1_MME ERROR, ADDRESS IS $MME_IPV4_ADDRESS_FOR_S1_MME"; fi;
    ping -q -c 1 $SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP > /dev/null 2>&1
    if [ $? -ne 0 ]; then echo_fatal "PING INTERFACE $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP ERROR, ADDRESS IS $SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP"; fi;
    ping -q -c 1 $ENB_IPV4_ADDRESS_FOR_S1_MME > /dev/null 2>&1
    if [ $? -ne 0 ]; then echo_fatal "PING INTERFACE $ENB_INTERFACE_NAME_FOR_S1_MME ADDR ERROR, ADDRESS IS $ENB_IPV4_ADDRESS_FOR_S1_MME"; fi;
    ping -q -c 1 $ENB_IPV4_ADDRESS_FOR_S1U > /dev/null 2>&1
    if [ $? -ne 0 ]; then echo_fatal "PING INTERFACE $ENB_INTERFACE_NAME_FOR_S1U ERROR, ADDRESS IS $ENB_IPV4_ADDRESS_FOR_S1U"; fi;
    ping -q -c 1 $MME_IPV4_ADDRESS_FOR_S11_MME > /dev/null 2>&1
    if [ $? -ne 0 ]; then echo_fatal "PING INTERFACE $MME_INTERFACE_NAME_FOR_S11_MME ERROR, ADDRESS IS $MME_IPV4_ADDRESS_FOR_S11_MME"; fi;
    ping -q -c 1 $SGW_IPV4_ADDRESS_FOR_S11 > /dev/null 2>&1
    if [ $? -ne 0 ]; then echo_fatal "PING INTERFACE $SGW_INTERFACE_NAME_FOR_S11 ERROR, ADDRESS IS $SGW_IPV4_ADDRESS_FOR_S11"; fi;
    ping -q -c 1 $MME_IPV4_ADDRESS_FOR_S6A > /dev/null 2>&1
    if [ $? -ne 0 ]; then echo_fatal "PING INTERFACE $MME_INTERFACE_NAME_FOR_S6A ERROR, ADDRESS IS $MME_IPV4_ADDRESS_FOR_S6A"; fi;
    ping -q -c 1 $HSS_IPV4_ADDRESS_FOR_S6A > /dev/null 2>&1
    if [ $? -ne 0 ]; then echo_fatal "PING INTERFACE $HSS_INTERFACE_NAME_FOR_S6A ERROR, ADDRESS IS $HSS_IPV4_ADDRESS_FOR_S6A"; fi;
    
    
    ## TEST NETWORK BETWEEN ENB-MME-SP-GW
    iperf  --bind $MME_IPV4_ADDRESS_FOR_S1_MME -u -s 2>&1  > /dev/null &
    iperf  --bind $ENB_IPV4_ADDRESS_FOR_S1_MME -u --num 1K -c $MME_IPV4_ADDRESS_FOR_S1_MME 2>&1 | grep -i WARNING > /dev/null
    if [ $? -eq 0 ]; then
        pkill iperf 2>&1 > /dev/null
        echo_fatal 'NETWORK ERROR CONFIGURATION (tun) between ENB and MME S1'
    else
        echo_success 'NETWORK TEST SUCCESS (tun) between ENB and MME S1'

    fi
    pkill iperf 2>&1 > /dev/null

    iperf  --bind $SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP -u -s 2>&1  > /dev/null &
    iperf  --bind $ENB_IPV4_ADDRESS_FOR_S1U -u --num 1K -c $SGW_IPV4_ADDRESS_FOR_S1U_S12_S4_UP 2>&1 | grep -i WARNING > /dev/null
    if [ $? -eq 0 ]; then
        pkill iperf 2>&1 > /dev/null
        echo_fatal 'NETWORK ERROR CONFIGURATION (tun) between ENB and S-GW S1-U'
    else
        echo_success 'NETWORK TEST SUCCESS (tun) between ENB and S-GW S1-U'
    fi
    pkill iperf 2>&1 > /dev/null

    iperf  --bind $SGW_IPV4_ADDRESS_FOR_S11 -u -s 2>&1  > /dev/null &
    iperf  --bind $MME_IPV4_ADDRESS_FOR_S11_MME -u --num 1K -c $SGW_IPV4_ADDRESS_FOR_S11 2>&1 | grep -i WARNING > /dev/null
    if [ $? -eq 0 ]; then
        pkill iperf 2>&1 > /dev/null
        echo_fatal 'NETWORK ERROR CONFIGURATION (tun) between MME and S-GW S11'
    else
        echo_success 'NETWORK TEST SUCCESS (tun) between MME and S-GW S11'
    fi
    pkill iperf 2>&1 > /dev/null
    
    iperf  --bind $HSS_IPV4_ADDRESS_FOR_S6A -u -s 2>&1  > /dev/null &
    iperf  --bind $MME_IPV4_ADDRESS_FOR_S6A -u --num 1K -c $HSS_IPV4_ADDRESS_FOR_S6A 2>&1 | grep -i WARNING > /dev/null
    if [ $? -eq 0 ]; then
        pkill iperf 2>&1 > /dev/null
        echo_fatal 'NETWORK ERROR CONFIGURATION (tun) between MME and HSS S6A'
    else
        echo_success 'NETWORK TEST SUCCESS (openvswitch) between MME and S-GW S11'
    fi
    pkill iperf 2>&1 > /dev/null


    # Get MAC address of router.eur
    ping -c 1 hss.eur > /dev/null || { echo_fatal "hss.eur does not respond to ping" >&2 ; }
#TEMP    ping -c 1 router.eur > /dev/null || { echo_fatal "router.eur does not respond to ping" >&2 ; }
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

#TEMP    ping -c 1 router.eur > /dev/null || { echo_fatal "router.eur does not respond to ping" >&2 ; }
#TEMP    IP_ROUTER=`python -c 'import socket; print socket.gethostbyname("router.eur")'`
#TEMP    export MAC_ROUTER=`ip neigh show | grep $IP_ROUTER | cut -d ' '  -f5 | tr -d ':'`
#TEMP    echo_success "ROUTER MAC ADDRESS= $MAC_ROUTER"

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
        CIDR='10.0.'$NET'.2/8'
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
      is_vlan_interface $interface
      if [ $? -eq 1 ]; then
         echo_success "Found VLAN interface $interface ... deleting"
         ifconfig    $interface down > /dev/null 2>&1
         vconfig rem $interface      > /dev/null 2>&1
      fi
  done
}

check_s6a_certificate() {
    if [ -d /usr/local/etc/freeDiameter ]
    then
        if [ -f /usr/local/etc/freeDiameter/user.cert.pem ]
        then
            full_hostname=`cat /usr/local/etc/freeDiameter/user.cert.pem | grep "Subject" | grep "CN" | cut -d '=' -f6`
            if [ a$full_hostname == a`hostname`.eur ]
            then
                echo_success "S6A: Found valid certificate in /usr/local/etc/freeDiameter"
                return 1
            fi
        fi
    fi
    echo_error "S6A: Did not find valid certificate in /usr/local/etc/freeDiameter"
    echo_warning "S6A: generatting new certificate in /usr/local/etc/freeDiameter..."
    cd $OPENAIRCN_DIR/S6A/freediameter
    ./make_certs.sh
    check_s6a_certificate
    return 1
}

check_install_epc_software() {
    test_install_package autoconf
    test_install_package automake
    test_install_package bison
    test_install_package build-essential
    test_install_package cmake
    test_install_package cmake-curses-gui
    test_install_package ethtool
    test_install_package flex
    test_install_package g++
    test_install_package gawk
    test_install_package gcc
    test_install_package gccxml
    test_install_package gdb 
    test_install_package guile-2.0-dev
    test_install_package iperf
    test_install_package iproute
    test_install_package iptables
    test_install_package libatlas-base-dev
    test_install_package libatlas-dev
    test_install_package libblas
    test_install_package libblas-dev
    test_install_package libconfig-dev
    test_install_package libforms-bin
    test_install_package libforms-dev
    test_install_package libgcrypt11-dev
    test_install_package libgmp-dev
    test_install_package libgtk-3-dev
    test_install_package libidn11-dev
    test_install_package libidn2-0-dev
    test_install_package libmysqlclient-dev
    test_install_package libpgm-dev
    test_install_package libpthread-stubs0-dev
    test_install_package libsctp1
    test_install_package libsctp1
    test_install_package libsctp-dev
    test_install_package libsctp-dev
    test_install_package libtasn1-3-dev
    test_install_package libxml2
    test_install_package libxml2-dev
    test_install_package libxml2-dev
    test_install_package linux-headers-`uname -r`
    test_install_package make
    test_install_package openssl
    test_install_package openvpn
    test_install_package python-dev
    test_install_package subversion
    test_install_package swig
    test_install_package tshark
    test_install_package uml-utilities
    test_install_package unzip
    test_install_package valgrind
    test_install_package vlan

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
}

compile_epc() {
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
}

###########################################################
IPTABLES=/sbin/iptables
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
declare -x OPENAIR_DIR=""
declare -x OPENAIR1_DIR=""
declare -x OPENAIR2_DIR=""
declare -x OPENAIR3_DIR=""
declare -x OPENAIRCN_DIR=""
declare -x OPENAIR_TARGETS=""
###########################################################

set_openair
cecho "OPENAIR_DIR     = $OPENAIR_DIR" $green
cecho "OPENAIR_HOME    = $OPENAIR_HOME" $green
cecho "OPENAIR1_DIR    = $OPENAIR1_DIR" $green
cecho "OPENAIR2_DIR    = $OPENAIR2_DIR" $green
cecho "OPENAIR3_DIR    = $OPENAIR3_DIR" $green
cecho "OPENAIRCN_DIR   = $OPENAIRCN_DIR" $green
cecho "OPENAIR_TARGETS = $OPENAIR_TARGETS" $green
