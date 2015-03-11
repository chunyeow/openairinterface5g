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
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
source $THIS_SCRIPT_PATH/build_helper.bash

#EMULATION_DEV_INTERFACE="eth0"
#EMULATION_MULTICAST_GROUP=1
#EMULATION_DEV_ADDRESS=`ifconfig $EMULATION_DEV_INTERFACE | grep 'inet addr:'| grep -v '127.0.0.1' | cut -d: -f2 | awk '{ print $1}'`

TARGET="ALL"
XFORMS="False"
VCD_TIMING="False"
REL="Rel10"
HW="EXMIMO"

until [ -z "$1" ]
  do
  case "$1" in
       -c | --clean)
            export CLEAN=1
            echo "Will re-compile ALL"
            shift;
            ;;
       --clean-iptables)
            CLEAN_IPTABLES=1;
            shift;
            ;;
       -C | --config-file)
            CONFIG_FILE=$2
            # may be relative path 
            if [ -f $(dirname $(readlink -f $0))/$CONFIG_FILE ]; then
                CONFIG_FILE=$(dirname $(readlink -f $0))/$CONFIG_FILE
                echo "setting config file to: $CONFIG_FILE"
                CONFIG_FILE_ACCESS_OK=1
            else
                # may be absolute path 
                if [ -f $CONFIG_FILE ]; then
                    echo "setting config file to: $CONFIG_FILE"
                    CONFIG_FILE_ACCESS_OK=1
                else
                    echo "config file not found"
                    exit 1
                fi
            fi
            shift 2;
            ;;
       -D | --disable-check-installed-software)
            DISABLE_CHECK_INSTALLED_SOFTWARE=1
            echo "disable check installed software"
            shift;
            ;;
       -g | --run-with-gdb)
            GDB=1
            echo "Running with gdb"
            shift;
            ;;
       -K | --itti-dump-file)
            ITTI_ANALYZER=1
            ITTI_DUMP_FILE=$2
            echo "setting ITTI dump file to: $ITTI_DUMP_FILE"
            EXE_ARGUMENTS="$EXE_ARGUMENTS -K $ITTI_DUMP_FILE"
            shift 2;
            ;;
       -l | --build-target) 
            TARGET=$2
            echo "setting top-level build target to: $2"
            shift 2;
            ;;
       -h | --help)
            print_help
            exit 1
            ;;
       -r | --3gpp-release)
            REL=$2 
            echo "setting release to: $REL"
            shift 2 ;
            ;;
       -s | --check)
            OAI_TEST=1
            echo "setting sanity check to: $OAI_TEST"
            shift;
            ;;
       -V | --vcd)
            echo "setting gtk-wave output"
            VCD_TIMING=1
            EXE_ARGUMENTS="$EXE_ARGUMENTS -V"
            shift ;
            ;;
       -w | --hardware)
            HW="$2" #"${i#*=}"
            echo "setting hardware to: $HW"
            shift 2 ;
            ;;
       -x | --xforms)
            XFORMS=1
            EXE_ARGUMENTS="$EXE_ARGUMENTS -d"
            echo "setting xforms to: $XFORMS"
            shift;
            ;;
       *)   
            echo "Unknown option $1"
            break ;
            ;;
   esac
done

################################
# cleanup first 
################################
echo_info "1. Cleaning ..."

if [ "$CLEAN_IPTABLES" == "1" ] ; then
    echo_info "Flushing iptables..."
    $SUDO modprobe ip_tables
    $SUDO modprobe x_tables
    $SUDO iptables -P INPUT ACCEPT
    $SUDO iptables -F INPUT
    $SUDO iptables -P OUTPUT ACCEPT
    $SUDO iptables -F OUTPUT
    $SUDO iptables -P FORWARD ACCEPT
    $SUDO iptables -F FORWARD
    $SUDO iptables -t nat -F
    $SUDO iptables -t mangle -F
    $SUDO iptables -t filter -F
    $SUDO iptables -t raw -F
    echo_info "Flushed iptables"
fi


############################################
# setting and printing OAI envs, we should check here
############################################
    
echo_info "2. Setting the OAI PATHS ..."

set_openair_env 
cecho "OPENAIR_HOME    = $OPENAIR_HOME" $green
cecho "OPENAIR1_DIR    = $OPENAIR1_DIR" $green
cecho "OPENAIR2_DIR    = $OPENAIR2_DIR" $green
cecho "OPENAIR3_DIR    = $OPENAIR3_DIR" $green
cecho "OPENAIRCN_DIR   = $OPENAIRCN_DIR" $green
cecho "OPENAIR_TARGETS = $OPENAIR_TARGETS" $green

dbin=$OPENAIR_HOME/cmake_targets/bin
dlog=$OPENAIR_HOME/cmake_targets/log
mkdir -p $dbin $dlog

if [ "$DISABLE_CHECK_INSTALLED_SOFTWARE" != "1" ] ; then
   check_install_oai_software
   make_certs
   check_epc_s6a_certificate
   check_hss_s6a_certificate
   if [ "$HW" == "USRP" ] ; then
     check_install_usrp_uhd_driver
   fi
fi

echo_info "3. building the compilation directives ..."

if [ "$TARGET" = "ALL" -o "$TARGET" = "SOFTMODEM" ] ; then
   DIR=$OPENAIR_HOME/cmake_targets

   # LTE softmodem compilation
   mkdir -p $DIR/lte_build_oai
   cmake_file=$DIR/lte_build_oai/CMakeLists.txt
   echo "cmake_minimum_required(VERSION 2.8)" > $cmake_file
   echo "set(XFORMS $XFORMS )" >>  $cmake_file
   echo "set(RRC_ASN1_VERSION \"${REL}\")" >>  $cmake_file
   echo "set(ENABLE_VCD_FIFO $VCD_TIMING )" >>  $cmake_file
   echo "set(RF_BOARD \"${HW}\")" >>  $cmake_file
   echo 'set(PACKAGE_NAME "lte-softmodem")' >>  $cmake_file
   echo 'include(${CMAKE_CURRENT_SOURCE_DIR}/../CMakeLists.txt)' >> $cmake_file
   compilations \
       lte_build_oai lte-softmodem \
       lte-softmodem $dbin/lte-softmodem.$REL \
       $dlog/lte-softmodem.$REL.txt \
       "lte-softmodem compiled" \
       "lte-softmodem compilation failed"

   # nasmesh driver compilation
   compilations \
       lte_build_oai nasmesh \
       CMakeFiles/nasmesh/nasmesh.ko $dbin/nasmesh.ko \
       $dlog/nasmesh.txt \
       "nasmesh driver compiled" \
       "nasmesh driver compilation failed"

   # lte unitary simulators compilation
   simlist="dlsim ulsim pucchsim prachsim pdcchsim pbchsim mbmssim"
   log=$dlog/lte-simulators.log
   cd $OPENAIR_DIR/cmake_targets/lte-simulators
   [ "$CLEAN" = "1" ] && rm -rf build
   mkdir -p build
   cd build
   rm -f *sim
   cmake ..  > $log 2>&1
   for f in $simlist ; do
       make -j4 $f > $log 2>&1
       if [ -s $f ] ; then
	   echo_success "$f compiled"
           cp $f $OPENAIR_DIR/cmake_targets/tests/bin 
       else
	   echo_error "$f compilation failed"
       fi
   done

   # EXMIMO drivers & firmware loader
   if [ "$HW" = "EXMIMO" ] ; then
     compilations \
        lte_build_oai openair_rf \
        CMakeFiles/openair_rf/openair_rf.ko $dbin/openair_rf.ko \
        $dlog/openair_rf.txt 2>&1 \
	"EXMIMO driver compiled" \
	"EXMIMO driver compilation failed"
     compilations \
       lte_build_oai updatefw \
       updatefw $dbin/updatefw \
       $dlog/updatefw.txt 2>&1 \
       "EXMIMO firmware loader compiled" \
       "EXMIMO firmware loader compilation failed"
   fi

   # oasim compilation
   cmake_file=$DIR/oaisim_build_oai/CMakeLists.txt
   cp $DIR/oaisim_build_oai/CMakeLists.template $cmake_file
   echo "set(XFORMS $XFORMS )" >>  $cmake_file
   echo "set(RRC_ASN1_VERSION \"${REL}\")" >>  $cmake_file
   echo "set(ENABLE_VCD_FIFO $VCD_TIMING )" >>  $cmake_file
   echo 'include(${CMAKE_CURRENT_SOURCE_DIR}/../CMakeLists.txt)' >> $cmake_file
   compilations \
       oaisim_build_oai oaisim \
       oaisim $dbin/oaisim.$REL \
       $dlog/oaisim.$REL.txt 2>&1 \
       "oaisim compiled" \
       "oaisim compilation failed"

fi

# EPC compilation
##################

# Auto-tests 
#####################
if [ "$OAI_TEST" = "1" ]; then 
    echo_info "10. Running OAI pre commit tests (pre-ci) ..."
    updated=$(svn st -q $OPENAIR_DIR)
    if [ "$updated" != "" ] ; then
	echo_warning "some files are not in svn: $updated"
    fi
    mkdir -p $dbin.test
    compilations \
        test.0101 oaisim \
        oaisim  $dbin.test/oaisim.r8 \
        $dlog/oaisim.r8.test0101.txt \
	"test 0101:oaisim passed" \
        "test 0101:oaisim failed"
    compilations test.0102 nasmesh \
        CMakeFiles/nasmesh/nasmesh.ko $dbin/test/nasmesh.ko \
        $dlog/nasmesh.test0102.txt \
	"test 0102: nasmesh.ko  passed" \
        "test 0102: nasmesk.ko failed"
else 
    echo_info "10. Bypassing the Tests ..."
fi 
exit 0




###########################################
# configure and compile
##########################################

    echo_info "5. configure and compile epc"

    output=$(compile_epc $OAI_CLEAN  >> $OPENAIR_TARGETS/bin/install_log.txt  2>&1 )
    epc_compiled=$?
    if [ $epc_compiled -ne 0 ]; then
        echo_error "EPC compilation failed : check the installation log file bin/install_log.txt" 
        exit 1
    fi
    check_for_epc_executable
    echo_info "finished epc target: check the installation log file bin/install_log.txt" 

    if [ $CONFIG_FILE_ACCESS_OK -eq 0 ]; then
        echo_error "You have to provide a EPC config file"
        exit 1
    fi
    
    TEMP_FILE=`tempfile`
    VARIABLES=" S6A_CONF\|\
           HSS_HOSTNAME\|\
           REALM"

    VARIABLES=$(echo $VARIABLES | sed -e 's/\\r//g')
    VARIABLES=$(echo $VARIABLES | tr -d ' ')
    cat $CONFIG_FILE | grep -w "$VARIABLES"| tr -d " " | tr -d ";" > $TEMP_FILE
    source $TEMP_FILE
    rm -f $TEMP_FILE

    if [ x"$REALM" == "x" ]; then
        echo_error "Your config file do not contain a REALM for S6A configuration"
        exit 1
    fi
    if [ x"$S6A_CONF" != "x./epc_s6a.conf" ]; then
        echo_error "Your config file do not contain the good path for the S6A config file,"
        echo_error "accordingly to what is done in this script, it should be set to epc_s6a.conf"
        exit 1
    fi

    check_epc_s6a_certificate $REALM

###########################################
# install the binary in bin
##########################################

    echo_info "6. install the binary file"

    if [ $epc_compiled -eq 0 ]; then 
        echo_success "target epc built and installed in the bin directory"
        echo "target epc built and installed in the bin directory"  >>  bin/${oai_build_date}
        cp -f $CONFIG_FILE  $OPENAIR_TARGETS/bin
        cp -f $OPENAIRCN_DIR/objs/UTILS/CONF/s6a.conf  $OPENAIR_TARGETS/bin/epc_s6a.conf
    fi
}

build_hss(){
    echo_info "Note: this script tested only for Ubuntu 12.04 x64 -> 14.04 x64"

######################################
# CHECK MISC SOFTWARES AND LIBS      #
######################################
    if [ $DISABLE_CHECK_INSTALLED_SOFTWARE -eq 0 ]; then 
        echo_info "4. check the required packages for HSS"
        check_install_hss_software
        if [ $OAI_CLEAN -eq 1 ]; then
            check_install_freediamter
        else 
            if [ ! -d /usr/local/etc/freeDiameter ]; then
                check_install_freediamter
            fi
        fi
    else
        echo_info "4. Not checking the required packages for HSS"
    fi
    
  
######################################
# compile HSS                        #
######################################
    echo_info "5. compile HSS"

     # Bad behaviour of $OAI_CLEAN with ./.lock_oaibuild ...
     compile_hss $CLEAN_HSS
     hss_compiled=$?
     check_for_hss_executable
     echo_info "finished hss target" 
 
######################################
# Check certificates                 #
######################################
  
    TEMP_FILE=`tempfile`
    cat $OPENAIRCN_DIR/OPENAIRHSS/conf/hss_fd.conf | grep -w "Identity" | tr -d " " | tr -d ";" > $TEMP_FILE
    cat $OPENAIRCN_DIR/OPENAIRHSS/conf/hss.conf    | grep -w "MYSQL_user" | tr -d " " | tr -d ";" >> $TEMP_FILE
    cat $OPENAIRCN_DIR/OPENAIRHSS/conf/hss.conf    | grep -w "MYSQL_pass" | tr -d " " | tr -d ";" >> $TEMP_FILE
    cat $OPENAIRCN_DIR/OPENAIRHSS/conf/hss.conf    | grep -w "MYSQL_db" | tr -d " " | tr -d ";" >> $TEMP_FILE
    source $TEMP_FILE
    rm -f  $TEMP_FILE

    if [ x"$Identity" == "x" ]; then
        echo_error "Your config file do not contain a host identity for S6A configuration"
        exit 1
    fi
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


 

echo_info "3. set the top-level build target"
case "$BUILD_LTE" in
    'ENB')
         echo_success "build LTE eNB"
         build_enb
         ;;
    'EPC')
         echo_success "build EPC(MME and xGW)"
         build_epc
         ;;
    'HSS')
        echo_success "build HSS"
        build_hss 
        ;;
    'NONE')
        ;;
    *)
        ;;
esac

# Additional operation 

############################################
# Generate doxygen documentation
############################################

if [ $DOXYGEN = 1 ]; then 
    echo_info "9. Generate doxygen documentation ..."
    doxygen $OPENAIR_TARGETS/DOCS/Doxyfile
    echo_info "9.1 use your navigator to open $OPENAIR_TARGETS/DOCS/html/index.html "
else 
    echo_info "9. Bypassing doxygen documentation ..."
fi 


############################################
# testing
############################################
    
if [ $OAI_TEST -eq 1 ]; then 
    echo_info "10. Running OAI pre commit tests (pre-ci) ..."
    python $OPENAIR_TARGETS/TEST/OAI/test01.py -l 
else 
    echo_info "10. Bypassing the Tests ..."
fi 
    
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
                    $SUDO chmod 777 $OPENAIR_TARGETS/RT/USER/init_exmimo2.sh
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


