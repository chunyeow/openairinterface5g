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
source $THIS_SCRIPT_PATH/build_helper.bash

#EMULATION_DEV_INTERFACE="eth0"
#EMULATION_MULTICAST_GROUP=1
#EMULATION_DEV_ADDRESS=`ifconfig $EMULATION_DEV_INTERFACE | grep 'inet addr:'| grep -v '127.0.0.1' | cut -d: -f2 | awk '{ print $1}'`

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
	-C | --config-file)
            CONFIG_FILE=$2
	    echo_info "Will install the config file $CONFIG_FILE"
            shift 2;;
	-I | --install-external-packages)
            INSTALL_EXTERNAL
            echo_info "Will install external packages"
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

dbin=$OPENAIR_HOME/cmake_targets/bin
dlog=$OPENAIR_HOME/cmake_targets/log
mkdir -p $dbin $dlog

if [ "$INSTALL_EXTERNAL" = "1" ] ; then
   echo_info "Installing packages"
   check_install_oai_software
   echo_info "Making X.509 certificates"
   make_certs
   if [ "$HW" == "USRP" ] ; then
     echo_info "installing packages for USRP support"
     check_install_usrp_uhd_driver
   fi
fi

echo_info "3. building the compilation directives ..."

DIR=$OPENAIR_HOME/cmake_targets

# Create and configure the building directories
#####################################"

# For eNB, UE, ...
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
cmake_file=$DIR/epc_build_oai/CMakeLists.txt
cp $DIR/epc_build_oai/CMakeLists.template $cmake_file
echo "set(ENABLE_VCD_FIFO $VCD_TIMING )" >>  $cmake_file
echo 'include(${CMAKE_CURRENT_SOURCE_DIR}/../CMakeLists.txt)' >> $cmake_file
mkdir -p $DIR/epc_build_oai/build
cd $DIR/epc_build_oai/build
cmake ..

# For oaisim
cmake_file=$DIR/oaisim_build_oai/CMakeLists.txt
cp $DIR/oaisim_build_oai/CMakeLists.template $cmake_file
echo "set(XFORMS $XFORMS )" >>  $cmake_file
echo "set(RRC_ASN1_VERSION \"${REL}\")" >>  $cmake_file
echo "set(ENABLE_VCD_FIFO $VCD_TIMING )" >>  $cmake_file
echo 'include(${CMAKE_CURRENT_SOURCE_DIR}/../CMakeLists.txt)' >> $cmake_file
mkdir -p $DIR/oaisim_build_oai/build
cd $DIR/oaisim_build_oai/build
cmake ..

# For unitary test simulators
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
fi

if [ "$UE" = 1 ] ; then
    # ue_ip driver compilation
    echo_info "Compiling UE specific part"
    compilations \
	lte_build_oai ue_ip \
	CMakeFiles/ue_ip/ue_ip.ko $dbin/ue_ip.ko
    compilations \
	lte_build_oai usim_data \
	usim_data $dbin/usim_data
    compilations \
	lte_build_oai ue_data \
	ue_data $dbin/ue_data
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
if [ "$SIMUS_CORE" = "1" ] ; then
   # lte unitary simulators compilation
   echo_info "Compiling unitary tests simulators"
   simlist="secu_knas_encrypt_eia1 secu_kenb aes128_ctr_encrypt aes128_ctr_decrypt secu secu_knas_stream_int secu_knas_encrypt_eea2 secu_knas secu_knas_encrypt_eea1 kdf aes128_cmac_encrypt secu_knas_encrypt_eia2"
   for f in $simlist ; do
       compilations \
	   lte-simulators test_$f \
	   test_$f $dbin/test_$f.$REL
   done
fi

# EXMIMO drivers & firmware loader
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
	oaisim_build_oai oaisim_mme \
	oaisim_mme $dbin/oaisim_mme.$REL
    #oai_nw_drv
    compilations \
	oaisim_build_oai oai_nw_drv \
	CMakeFiles/oai_nw_drv/oai_nw_drv.ko $dbin/oai_nw_drv.ko
    # nasmesh driver compilation
    compilations \
	oaisim_build_oai nasmesh \
	CMakeFiles/nasmesh/nasmesh.ko $dbin/nasmesh.ko
fi

# EPC compilation
##################
if [ "$EPC" = "1" ] ; then
    echo_info "Compiling EPC"

    compilations \
	epc_build_oai oai_epc \
	oai_epc $dbin/oai_epc.$REL
    compilations \
	epc_build_oai oai_sgw \
	oai_sgw $dbin/oai_sgw.$REL
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

    compile_hss
fi

if [ "$INSTALL_SYSTEM_FILES" = "1" ] ;then
    echo_info "Copying iptables libraries into system directory: /lib/xtables"
    $SUDO ln -s $dbin/libxt_GTPURH_lib.so /lib/xtables/libxt_GTPURH.so
    $SUDO ln -s $dbin/libxt_GTPUAH_lib.so /lib/xtables/libxt_GTPUAH.so

fi

# Install config file
###################
if [ "$CONFIG_FILE" != "" ] ; then

    dconf=$DIR/conf
    mkdir -p $dconf
    cp $ORIGIN_PATH/$CONFIG_FILE $dconf || echo_fatal "config file $ORIGIN_PATH/$CONFIG_FILE not found"

    # generate USIM data
    install_nas_tools $dbin $dconf

    # Do HSS 
    # bash doesn't like space char around = char
    sed -e 's/ *= */=/' $OPENAIRCN_DIR/OPENAIRHSS/conf/hss.local.conf > $dconf/hss.local.conf
    source $dconf/hss.local.conf
    create_hss_database root linux "$MYSQL_user" "$MYSQL_pass" "$MYSQL_db"

fi 

# Auto-tests 
#####################
if [ "$OAI_TEST" = "1" ]; then 
    echo_info "10. Running OAI pre commit tests (pre-ci) ..."
    $OPENAIR_DIR/cmake_targets/autotests/run_compilation_autotests.bash
else 
    echo_info "10. Bypassing the Tests ..."
fi 

exit 0



###########################################
# configure and compile
##########################################


    HSS_REALM=$(echo $Identity | sed 's/.*\.//')
    HSS_HOSTNAME=${Identity%.$HSS_REALM}
    NEW_HOSTNAME=`hostname -s`
    if [ "x$HSS_HOSTNAME" != "x$NEW_HOSTNAME" ]; then
       echo_warning "Changing identity of HSS from <$HSS_HOSTNAME.$HSS_REALM> to <$NEW_HOSTNAME.$HSS_REALM>"
       sed -ibak "s/$HSS_HOSTNAME/$NEW_HOSTNAME/"  $OPENAIRCN_DIR/OPENAIRHSS/conf/hss_fd.conf 
    fi
    check_hss_s6a_certificate $HSS_REALM
    
######################################
# fill the HSS DB
######################################
     echo_info "6. create HSS database (for EURECOM SIM CARDS)"
     hss_db_created=1
     create_hss_database $OAI_DB_ADMIN_USER_NAME $OAI_DB_ADMIN_USER_PASSWORD $MYSQL_user $MYSQL_pass $MYSQL_db
     if [ $? -eq 1 ]; then
         echo_fatal "hss DB not created"
     fi
}



############################################
# run 
############################################

if [ $RUN -ne 0 ]; then 
    echo_info "11. Running ..."
    cd $OPENAIR_TARGETS/bin
    case "$BUILD_LTE" in
        'ENB')
            if [ $TARGET == "SOFTMODEM" ]; then 
                if [ $HW == "EXMIMO" ]; then 
                    $SUDO $OPENAIR_TARGETS/RT/USER/init_exmimo2.sh
                fi
                echo "############# running ltesoftmodem #############"
                if [ $RUN_GDB -eq 0 ]; then 
                    $SUDO $OPENAIR_TARGETS/bin/lte-softmodem  `echo $EXE_ARGUMENTS`
                else
                    $SUDO setenv MALLOC_CHECK_ 2
                    $SUDO touch ~/.gdb_lte_softmodem
                    $SUDO echo "file $OPENAIR_TARGETS/bin/lte-softmodem" > ~/.gdb_lte_softmodem
                    $SUDO echo "set args $EXE_ARGUMENTS" >> ~/.gdb_lte_softmodem
                    $SUDO echo "run" >> ~/.gdb_lte_softmodem
                    $SUDO gdb -nh -x ~/.gdb_lte_softmodem 2>&1 
                fi
                
            elif [ $TARGET == "OAISIM" ]; then
            
                if [ $ENB_S1 -eq 0 ]; then
		    install_nasmesh
                else
                    # prepare NAS for UE
                    if [ ! -f .ue.nvram ]; then
                        echo_success "generate .ue_emm.nvram .ue.nvram"
                        $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/ue_data --gen
                    fi

                    if [ ! -f .usim.nvram ]; then
                        echo_success "generate .usim.nvram"
                        $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/usim_data --gen
                    fi
                    $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/ue_data --print
                    $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/usim_data --print

                    insmod  $OPENAIR2_DIR/NETWORK_DRIVER/UE_IP/ue_ip.ko
                    
                fi
                
                if [ $RUN_GDB -eq 0 ]; then 
                    $SUDO exec $OPENAIR_TARGETS/bin/oaisim  `echo $EXE_ARGUMENTS`
                else
                    $SUDO setenv MALLOC_CHECK_ 2
                    $SUDO touch ~/.gdb_oaisim
                    $SUDO echo "file $OPENAIR_TARGETS/bin/lte-oaisim" > ~/.gdb_oaisim
                    $SUDO echo "set args $EXE_ARGUMENTS" >> ~/.gdb_oaisim
                    $SUDO echo "run" >> ~/.gdb_oaisim
                    $SUDO gdb -nh -x ~/.gdb_oaisim 2>&1 
                fi
            fi
            ;;
        
        
        'EPC')
            echo "############# running EPC #############"
            test_is_host_reachable $HSS_HOSTNAME.$REALM HSS
            if [ $RUN_GDB -eq 0 ]; then
                $SUDO $OPENAIR_TARGETS/bin/oai_epc  `echo $EXE_ARGUMENTS`
            else
                touch ~/.gdb_epc
                chmod 777 ~/.gdb_epc
                echo "file $OPENAIR_TARGETS/bin/oai_epc" > ~/.gdb_epc
                echo "set args $EXE_ARGUMENTS" >> ~/.gdb_epc
                echo "run" >> ~/.gdb_epc
                $SUDO gdb -nh -x ~/.gdb_epc 2>&1 
            fi
            ;;
        
        
        'HSS')
            echo "############# running HSS #############"
            cd $OPENAIRCN_DIR/OPENAIRHSS/objs
            if [ $RUN_GDB -eq 0 ]; then
                $SUDO exec ./openair-hss -c ./conf/hss.conf
            else
                touch ~/.gdb_hss
                chmod 777 ~/.gdb_hss
                echo "file ./openair-hss" > ~/.gdb_hss
                echo "set args -c ./conf/hss.conf" >> ~/.gdb_hss
                echo "run" >> ~/.gdb_hss
                $SUDO gdb -nh -x ~/.gdb_hss 2>&1 
            fi
             ;;
         
         
        'NONE')
             ;;
         
         
        *)
             echo_error "Unknown option $BUILD_LTE: do not execute"
             ;;
    esac
else
    echo_info "11. No run requested, end of script"
    exit 0
fi
