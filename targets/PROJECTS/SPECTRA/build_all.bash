#! /bin/bash

###########################################################
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
source $THIS_SCRIPT_PATH/env_802dot21.bash
###########################################################

cd $OPENAIR_TARGETS/SIMU/USER


###############################
# COMPILE oaisim
###############################

test_command_install_package "gccxml" "gccxml" "--force-yes"
test_command_install_package "iptables" "iptables"
test_command_install_package "ip" "iproute"
test_install_package "openssl"
test_install_package "libblas-dev"
test_install_package "libxml2"
test_install_package "libxml2-dev"
test_install_package "libforms-bin" "--force-yes"
test_install_package "libforms-dev"
test_install_package "libatlas-dev"
test_install_package "libatlas-base-dev"
test_install_package "libpgm-5.1-0" "--force-yes"
test_install_package "libpgm-dev"   "--force-yes"
test_install_package linux-headers-`uname -r`
    
test_install_asn1c_4_rrc_cellular
    
#make clean > /dev/null 
#make cleancell >/dev/null

echo_success "Executing: make oaisim NAS=1 OAI_NW_DRIVER_TYPE_ETHERNET=1 ENABLE_ITTI=1 Rel10=1 -j`grep -c ^processor /proc/cpuinfo `"
cd $OPENAIR_TARGETS/SIMU/USER
make oaisim NAS=1 OAI_NW_DRIVER_TYPE_ETHERNET=1 ENABLE_ITTI=1 Rel10=1 -j`grep -c ^processor /proc/cpuinfo `
if [[ $? -eq 2 ]] ; then
    exit 1
fi


###############################
# COMPILE LTE_RAL_UE
###############################
echo_success "Compiling Radio Access Layer UE (RAL)"
cd $OPENAIR3_DIR/RAL-LTE/LTE_RAL_UE
make all


###############################
# COMPILE LTE_RAL_ENB
###############################
echo_success "Compiling Radio Access Layer eNB (RAL)"
cd $OPENAIR3_DIR/RAL-LTE/LTE_RAL_ENB
make all


###############################
# COMPILE rrm_std_cellular
###############################
echo_success "Compiling RRM"
cd $OPENAIR_TARGETS/SIMU/USER
make rrm_std_cellular
if [[ $? -eq 2 ]] ; then
    exit 1
fi


###############################
# COMPILE IP kernel drivers
###############################
echo_success "Compiling IP Drivers"
cd $OPENAIR2_DIR
make naslite_netlink_ether.ko
cd $OPENAIR2_DIR/NAS/DRIVER/LITE/RB_TOOL/
make

###############################
# COMPILE MIH-F
###############################
cd $ODTONE_ROOT
b2 --boost-root=$BOOST_ROOT


###############################
# COMPILE MIH-USER
###############################
cd  $ODTONE_ROOT/app/lte_test_user/
b2 --boost-root=$BOOST_ROOT linkflags=-lrt





