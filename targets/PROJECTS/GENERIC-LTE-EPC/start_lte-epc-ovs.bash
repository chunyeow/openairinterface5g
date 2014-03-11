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
        autoreconf -i -f 
        cd ./$OBJ_DIR
        echo_success "Invoking configure"
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
    make -j `cat /proc/cpuinfo | grep processor | wc -l`
    if [ $? -ne 0 ]; then
        echo_error "Build failed, exiting"
        exit 1
    fi
else
    echo_error "Configure failed, exiting"
    exit 1
fi
cd $OPENAIRCN_DIR


#######################################################
# FIND CONFIG FILE
#######################################################
CONFIG_FILE=$THIS_SCRIPT_PATH/CONF/epc.sfr.default.vswitch.conf
SEARCHED_CONFIG_FILE=$THIS_SCRIPT_PATH/CONF/epc.sfr."$HOSTNAME".vswitch.conf
if [ -f $SEARCHED_CONFIG_FILE ]; then
    CONFIG_FILE=$SEARCHED_CONFIG_FILE
    echo_warning "config file found is now $CONFIG_FILE"
else
    echo_warning "config file $SEARCHED_CONFIG_FILE for host $HOSTNAME not found, trying default: $CONFIG_FILE"
    if [ -f $CONFIG_FILE ]; then
        echo_success "Default config file found: $CONFIG_FILE"
    else
        echo_error "Default config file not found, exiting"
        exit 1
    fi
fi

CONFIG_FILE_ENB=$THIS_SCRIPT_PATH/CONF/enb.sfr.default.vswitch.conf
SEARCHED_CONFIG_FILE_ENB=$THIS_SCRIPT_PATH/CONF/enb.sfr."$HOSTNAME".vswitch.conf
if [ -f $SEARCHED_CONFIG_FILE_ENB ]; then
    CONFIG_FILE_ENB=$SEARCHED_CONFIG_FILE_ENB
    echo_warning "config file eNB found is now $CONFIG_FILE_ENB"
else
    echo_warning "config file eNB $SEARCHED_CONFIG_FILE_ENB for host $HOSTNAME not found, trying default: $CONFIG_FILE_ENB"
    if [ -f $CONFIG_FILE_ENB ]; then
        echo_success "Default config file eNB found: $CONFIG_FILE_ENB"
    else
        echo_error "Default config file eNB not found, exiting"
        exit 1
    fi
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
cat $CONFIG_FILE | grep -w "$VARIABLES"| tr -d " " | tr -d ";" > /tmp/source.txt
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

if [ `is_openvswitch_interface $ENB_INTERFACE_NAME_FOR_S1_MME` -eq 1 ] && \
   [ `is_openvswitch_interface $ENB_INTERFACE_NAME_FOR_S1U` -eq 1 ] && \
   [ `is_openvswitch_interface $MME_INTERFACE_NAME_FOR_S1_MME` -eq 1 ] && \
   [ `is_openvswitch_interface $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP` -eq 1 ]; then 
   ovs_setting=1
   vlan_setting=0
   echo_success "Found open-vswitch network configuration"
   clean_epc_ovs_network
   build_epc_ovs_network
   test_epc_ovs_network
else
    if [ `is_vlan_interface $MME_INTERFACE_NAME_FOR_S1_MME` -eq 1 ] || \
       [ `is_vlan_interface $SGW_INTERFACE_NAME_FOR_S1U_S12_S4_UP` -eq 1 ]; then
        echo_success "Found open vswitch network configuration"
        ovs_setting=0
        vlan_setting=1
        clean_epc_vlan_network
        build_mme_spgw_vlan_network
    else
        echo_error "Cannot find open-vswitch network configuration or VLAN network configuration"
        exit 1
    fi 
fi

##################################################..

# LAUNCH MME + S+P-GW executable
##################################################

cd $OPENAIRCN_DIR/$OBJ_DIR

ITTI_LOG_FILE=./itti_mme.log
rotate_log_file $ITTI_LOG_FILE
STDOUT_LOG_FILE=./stdout_mme.log
rotate_log_file $STDOUT_LOG_FILE

$OPENAIRCN_DIR/$OBJ_DIR/OAI_EPC/oai_epc -K $ITTI_LOG_FILE -c $CONFIG_FILE | tee $STDOUT_LOG_FILE 2>&1
