#!/bin/bash
################################################################################
#   OpenAirInterface
#   Copyright(c) 1999 - 2014 Eurecom
#
#    OpenAirInterface is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) anylater version.
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
# file build_oai.bash
# brief OAI automated build tool that can be used to install, compile, run OAI.
# author  Navid Nikaein, Lionel GAUTHIER, Laurent Thomas


################################
# include helper functions
################################
ORIGIN_PATH=$PWD
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
source $THIS_SCRIPT_PATH/tools/build_helper.bash

XFORMS="False"
VCD_TIMING="False"
REL="Rel10"
HW="EXMIMO"

until [ -z "$1" ]
do
    case "$1" in
	-c | --clean)
	    CLEAN=1
	    clean_all_files
	    echo_info "Erased all previously producted files"
            shift;;
	--clean-kernel)
	    clean_kernel
	    echo_info "Erased iptables config and removed modules from kernel"
            shift;;
	-I | --install-external-packages)
            INSTALL_EXTERNAL=1
            echo_info "Will install external packages"
            shift;;
	--install-optional-packages)
            INSTALL_OPTIONAL=1
            echo_info "Will install optional packages"
            shift;;
	-g | --run-with-gdb)
            GDB=1
            echo_info "Will Compile with gdb symbols"
            shift;;
	--eNB)
	    eNB=1
	    echo_info "Will compile eNB"
	    shift;;
	--UE)
	    UE=1
	    echo_info "Will compile UE"
	    shift;;
	--EPC)
	    EPC=1
	    echo_info "Will compile EPC"
	    shift;;
       -r | --3gpp-release)
            REL=$2 
            echo_info "setting release to: $REL"
            shift 2;;
       -w | --hardware)
            HW="$2" #"${i#*=}"
            echo_info "setting hardware to: $HW"
            shift 2;;
	--oaisim)
	    oaisim=1
	    echo_info "Will compile oaisim and drivers nasmesh, ..."
	    shift;;
	--phy_simulators)
            SIMUS_PHY=1
	    echo_info "Will compile dlsim, ulsim, ..."
	    shift;;
	--core_simulators)
	    SIMUS_CORE=1
	    echo_info "Will compile security unitary tests"
	    shift;;
       -s | --check)
            OAI_TEST=1
            echo_info "Will run auto-tests"
            shift;;
       -V | --vcd)
            echo_info "setting gtk-wave output"
            VCD_TIMING=1
            EXE_ARGUMENTS="$EXE_ARGUMENTS -V"
            shift;;
       -x | --xforms)
            XFORMS=1
            EXE_ARGUMENTS="$EXE_ARGUMENTS -d"
            echo_info "Will generate the software oscilloscope features"
            shift;;
	--install-system-files)
	    INSTALL_SYSTEM_FILES=1
	    echo_info "Will copy OpenAirInterface files in Linux directories"
	    shift;;
        -h | --help)
            print_help
            exit 1;;
	*)   
	    print_help
            echo_fatal "Unknown option $1"
            break;;
   esac
done

############################################
# setting and printing OAI envs, we should check here
############################################
    
echo_info "2. Setting the OAI PATHS ..."

set_openair_env 
cecho "OPENAIR_HOME    = $OPENAIR_HOME" $green

dbin=$OPENAIR_HOME/targets/bin
dlog=$OPENAIR_HOME/cmake_targets/log
mkdir -p $dbin $dlog

if [ "$INSTALL_EXTERNAL" = "1" ] ; then
   echo_info "Installing packages"
   check_install_oai_software
   echo_info "Making X.509 certificates"
   make_certs eur
   if [ "$HW" == "USRP" ] ; then
     echo_info "installing packages for USRP support"
     check_install_usrp_uhd_driver
   fi
fi

if [ "$INSTALL_OPTIONAL" = "1" ] ; then
   echo_info "Installing optional packages"
   check_install_additional_tools
fi

echo_info "3. building the compilation directives ..."

DIR=$OPENAIR_HOME/cmake_targets

# Create and configure the building directories
#####################################"

# For eNB, UE, ...
############
mkdir -p $DIR/lte_build_oai/build
cmake_file=$DIR/lte_build_oai/CMakeLists.txt
echo "cmake_minimum_required(VERSION 2.8)" > $cmake_file
echo "set(XFORMS $XFORMS )" >>  $cmake_file
echo "set(RRC_ASN1_VERSION \"${REL}\")" >>  $cmake_file
echo "set(ENABLE_VCD_FIFO $VCD_TIMING )" >>  $cmake_file
echo "set(RF_BOARD \"${HW}\")" >>  $cmake_file
echo 'set(PACKAGE_NAME "\"lte-softmodem\"")' >>  $cmake_file
echo 'include(${CMAKE_CURRENT_SOURCE_DIR}/../CMakeLists.txt)' >> $cmake_file
cd  $DIR/lte_build_oai/build
cmake ..

# For EPC
############
cmake_file=$DIR/epc_build_oai/CMakeLists.txt
cp $DIR/epc_build_oai/CMakeLists.template $cmake_file
echo "set(ENABLE_VCD_FIFO $VCD_TIMING )" >>  $cmake_file
echo 'include(${CMAKE_CURRENT_SOURCE_DIR}/../CMakeLists.txt)' >> $cmake_file
mkdir -p $DIR/epc_build_oai/build
cd $DIR/epc_build_oai/build
cmake ..

# For oaisim
############
cmake_file=$DIR/oaisim_build_oai/CMakeLists.txt
cp $DIR/oaisim_build_oai/CMakeLists.template $cmake_file
echo "set(XFORMS $XFORMS )" >>  $cmake_file
echo "set(RRC_ASN1_VERSION \"${REL}\")" >>  $cmake_file
echo "set(ENABLE_VCD_FIFO $VCD_TIMING )" >>  $cmake_file
echo 'include(${CMAKE_CURRENT_SOURCE_DIR}/../CMakeLists.txt)' >> $cmake_file
mkdir -p $DIR/oaisim_build_oai/build
cd $DIR/oaisim_build_oai/build
cmake ..

# For oaisim_mme
############
cmake_file=$DIR/oaisim_mme_build_oai/CMakeLists.txt
cp $DIR/oaisim_mme_build_oai/CMakeLists.template $cmake_file
echo "set(XFORMS $XFORMS )" >>  $cmake_file
echo "set(RRC_ASN1_VERSION \"${REL}\")" >>  $cmake_file
echo "set(ENABLE_VCD_FIFO $VCD_TIMING )" >>  $cmake_file
echo 'include(${CMAKE_CURRENT_SOURCE_DIR}/../CMakeLists.txt)' >> $cmake_file
mkdir -p $DIR/oaisim_mme_build_oai/build
cd $DIR/oaisim_mme_build_oai/build
cmake ..

# For unitary test simulators
###################
cd $OPENAIR_DIR/cmake_targets/lte-simulators
[ "$CLEAN" = "1" ] && rm -rf build
mkdir -p build
cd build
rm -f *sim
cmake ..

if [ "$eNB" = "1" -o "UE" = "1" ] ; then
    # LTE softmodem compilation
    echo_info "Compiling LTE softmodem"
    compilations \
	lte_build_oai lte-softmodem \
	lte-softmodem lte-softmodem.$REL 
    # nasmesh driver compilation
    compilations \
	oaisim_build_oai nasmesh \
	CMakeFiles/nasmesh/nasmesh.ko $dbin/nasmesh.ko
fi

if [ "$UE" = 1 ] ; then
    # ue_ip driver compilation
    echo_info "Compiling UE specific part"
    compilations \
	lte_build_oai ue_ip \
	CMakeFiles/ue_ip/ue_ip.ko $dbin/ue_ip.ko
    compilations \
	lte_build_oai usim \
	usim $dbin/usim
    compilations \
	lte_build_oai nvram \
	nvram $dbin/nvram
fi

if [ "$SIMUS_PHY" = "1" ] ; then
   # lte unitary simulators compilation
   echo_info "Compiling unitary tests simulators"
   simlist="dlsim ulsim pucchsim prachsim pdcchsim pbchsim mbmssim"
   for f in $simlist ; do
       compilations \
	   lte-simulators $f \
	   $f $dbin/$f.$REL
   done
fi

# Core simulators 
#############
if [ "$SIMUS_CORE" = "1" ] ; then
   # lte unitary simulators compilation
   echo_info "Compiling unitary tests simulators"
   simlist="secu_knas_encrypt_eia1 secu_kenb aes128_ctr_encrypt aes128_ctr_decrypt secu_knas_encrypt_eea2 secu_knas secu_knas_encrypt_eea1 kdf aes128_cmac_encrypt secu_knas_encrypt_eia2"
   for f in $simlist ; do
       compilations \
	   lte-simulators test_$f \
	   test_$f $dbin/test_$f.$REL
   done
fi

# EXMIMO drivers & firmware loader
###############
if [ "$HW" = "EXMIMO" ] ; then
    echo_info "Compiling Express MIMO 2 board drivers"
    compilations \
        lte_build_oai openair_rf \
        CMakeFiles/openair_rf/openair_rf.ko $dbin/openair_rf.ko
    compilations \
	lte_build_oai updatefw \
	updatefw $dbin/updatefw 
fi

if [ "$oaisim" = "1" ] ; then
    echo_info "Compiling oaisim"
    compilations \
	oaisim_build_oai oaisim \
	oaisim $dbin/oaisim.$REL
    compilations \
	oaisim_mme_build_oai oaisim_mme \
	oaisim_mme $dbin/oaisim_mme.$REL
    #oai_nw_drv
    compilations \
	oaisim_build_oai oai_nw_drv \
	CMakeFiles/oai_nw_drv/oai_nw_drv.ko $dbin/oai_nw_drv.ko
fi

# EPC compilation
##################
if [ "$EPC" = "1" ] ; then
    echo_info "Compiling EPC"

    compilations \
	epc_build_oai mme_gw \
	mme_gw $dbin/mme_gw.$REL
# Only integrated mme+sgw+pgw is operational today
#    compilations \
#	epc_build_oai oai_sgw \
#	oai_sgw $dbin/oai_sgw.$REL
    compilations \
	epc_build_oai xt_GTPUAH_lib \
	libxt_GTPUAH_lib.so $dbin
    compilations \
	epc_build_oai xt_GTPURH_lib \
	libxt_GTPURH_lib.so $dbin
    compilations \
	epc_build_oai xt_GTPURH \
	CMakeFiles/xt_GTPURH/xt_GTPURH.ko $dbin
    compilations \
	epc_build_oai xt_GTPUAH \
	CMakeFiles/xt_GTPUAH/xt_GTPUAH.ko $dbin

  if [ "$INSTALL_SYSTEM_FILES" = "1" ] ;then
    # Example HSS and EPC run on the same host
    $OPENAIR_HOME/cmake_targets/tools/build_hss --clean --debug --install-hss-files --transport-tcp-only --transport-prefer-tcp --fqdn `hostname --fqdn` --connect-to-mme `hostname --fqdn`
    # example HHS and EPC run on separate hosts (can use SCTP)
    # $OPENAIR_HOME/cmake_targets/tools/build_hss --clean --debug --install-hss-files
  else
    # Example HSS and EPC run on the same host
    $OPENAIR_HOME/cmake_targets/tools/build_hss --debug --transport-tcp-only --transport-prefer-tcp --fqdn `hostname --fqdn` --connect-to-mme `hostname --fqdn`
    # example HHS and EPC run on separate hosts (can use SCTP)
    # $OPENAIR_HOME/cmake_targets/tools/build_hss --debug 
  fi
fi

if [ "$INSTALL_SYSTEM_FILES" = "1" ] ;then

    echo_info "Copying iptables libraries into system directory: /lib/xtables"
    $SUDO ln -s $dbin/libxt_GTPURH_lib.so /lib/xtables/libxt_GTPURH.so
    $SUDO ln -s $dbin/libxt_GTPUAH_lib.so /lib/xtables/libxt_GTPUAH.so

    dconf=$OPENAIR_TARGETS/bin
    mkdir -p $dconf
    cp $ORIGIN_PATH/$CONFIG_FILE $dconf || echo_fatal "config file $ORIGIN_PATH/$CONFIG_FILE not found"

    # generate USIM data
    install_nas_tools $dbin $dconf

    # Do EPC
    cp $DIR/epc_build_oai/build/epc.*.conf $dconf
    $SUDO cp  $DIR/epc_build_oai/build/epc_s6a.conf  /usr/local/etc/freeDiameter

    # Do HSS 
    # bash doesn't like space char around = char
    cp $DIR/hss_build/hss.conf $dbin
    $SUDO cp  $DIR/hss_build/hss_fd.conf $DIR/hss_build/acl.conf /usr/local/etc/freeDiameter
    
    sed -e 's/ *= */=/' $dbin/hss.conf > $dconf/hss.conf.nospace
    source $dconf/hss.conf.nospace
    rm -f $dconf/hss.conf.nospace
    create_hss_database root linux "$MYSQL_user" "$MYSQL_pass" "$MYSQL_db"
fi 

# Auto-tests 
#####################
if [ "$OAI_TEST" = "1" ]; then 
    echo_info "10. Running OAI pre commit tests (pre-ci) ..."
    $OPENAIR_DIR/cmake_targets/autotests/run_compilation_autotests.bash
    $OPENAIR_DIR/cmake_targets/autotests/run_exec_autotests.bash
else 
    echo_info "10. Bypassing the Tests ..."
fi 
