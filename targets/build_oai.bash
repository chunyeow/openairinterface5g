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
# author  Navid Nikaein 
# company Eurecom
# email:  navid.nikaein@eurecom.fr 
#

#!/bin/bash
################################
# include helper functions
################################
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
. $THIS_SCRIPT_PATH/build_helper.bash

check_for_root_rights

#######################################
# Default PARAMETERS
######################################

#only one could be set at the time
BUILD_LTE="ENB" # ENB, EPC, HSS, NONE

HW="EXMIMO" # EXMIMO, USRP, ETHERNET, NONE
TARGET="ALL" # ALL, SOFTMODEM, OAISIM, UNISIM, NONE
ENB_S1=1
REL="REL8" # REL8, REL10
RT="NONE" # RTAI, RT_PREMPT or RT_DISABLED, NONE
DEBUG=0


OAI_TEST=0
XFORMS=0

# script is not currently handling these params
EPC=0 # flag to build EPC

ITTI_ANALYZER=0
VCD_TIMING=0
WIRESHARK=0
TIME_MEAS=0
DEV=0

EMULATION_DEV_INTERFACE="eth0"
EMULATION_MULTICAST_GROUP=1
EMULATION_DEV_ADDRESS=`ifconfig $EMULATION_DEV_INTERFACE | grep 'inet addr:'| grep -v '127.0.0.1' | cut -d: -f2 | awk '{ print $1}'`

############## script params #####################

if [ -f ./.lock_oaibuild ]; then 
    OAI_CLEAN=0
else 
    OAI_CLEAN=1
fi 
 
#for i in "$@"
#do 
#    echo "i is : $i"
#    case $i in

while getopts "bcdmsxzhe:l:w:r:t:" OPTION; do
   case "$OPTION" in
       b)
	   ENB_S1=0
	   echo "disable eNB S1 flag"
	   ;;
       c)
	   rm -rf ./.lock_oaibuild
	   OAI_CLEAN=1
	   echo "setting clean flag to: $OAI_CLEAN"
	   echo "check package installation, and recompile OAI"
	   ;;
       d)
	   DEBUG=1
	   echo "setting debug flag to: $DEBUG"
	   ;;
       e)
	   RT="$OPTARG"
	   echo "setting realtime flag to: $RT"
	   ;;
       l) 
	   BUILD_LTE="$OPTARG"
	   echo "setting top-level build target to: $OPTARG"
	   ;;
       h)
	   print_help
	   exit -1
	   ;;
       m)
	   BUILD_FROM_MAKEFILE=1
	   set_build_from_makefile $BUILD_FROM_MAKEFILE
	   echo "setting a flag to build from makefile to: $BUILD_FROM_MAKEFILE"
	   ;;
       r)
	   REL="$OPTARG" 
	   echo "setting release to: $REL"
	   ;;
       s)
	   OAI_TEST=1
	   echo "setting sanity check to: $OAI_TEST"
	   ;;
       t)
	   TARGET="$OPTARG" 
	   echo "setting target to: $TARGET"
	   ;;
       w)
	   HW="$OPTARG" #"${i#*=}"
	   echo "setting hardware to: $HW"
	   ;;
       x)
	   XFORMS=1
	   echo "setting xforms to: $XFORMS"
	   ;;
       z)
	   echo "setting all parameters to: default"
	   rm -rf ./.lock_oaibuild
	   OAI_CLEAN=1
	   HW="EXMIMO"
	   TARGET="ALL" 
	   ENB_S1=1
	   REL="REL8" 
	   RT="NONE"
	   DEBUG=0
	   ENB_CONFIG_FILE=$OPENAIR_TARGETS/"PROJECTS/GENERIC-LTE-EPC/CONF/enb.band7.conf"
	   OAI_TEST=0
  	   ;;
       *)
            # unknown option
	   ;;
   esac
done

#####################
# create a bin dir
#####################
echo_info "1. Creating the bin dir ..." 
rm -rf bin
mkdir -m 777 -p bin 

build_date=`date +%Y_%m_%d`
oai_build_date="oai_built_${build_date}"
touch bin/${oai_build_date} 
touch bin/install_log.txt
################################
# cleanup first 
################################
#echo_info "3. Cleaning ..."

#$SUDO kill -9 `ps -ef | grep oaisim | awk '{print $2}'` 2>&1
#$SUDO kill -9 `ps -ef | grep lte-softmodem | awk '{print $2}'`  2>&1
#$SUDO kill -9 `ps -ef | grep dlsim | awk '{print $2}'`  2>&1
#$SUDO kill -9 `ps -ef | grep ulsim | awk '{print $2}'`  2>&1


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


echo "OPENAIR_HOME    = $OPENAIR_HOME" >>  bin/${oai_build_date}
echo "OPENAIR1_DIR    = $OPENAIR1_DIR"  >>  bin/${oai_build_date}
echo "OPENAIR2_DIR    = $OPENAIR2_DIR"  >>  bin/${oai_build_date}
echo "OPENAIR3_DIR    = $OPENAIR3_DIR"  >>  bin/${oai_build_date}
echo "OPENAIRCN_DIR   = $OPENAIRCN_DIR"  >>  bin/${oai_build_date}
echo "OPENAIR_TARGETS = $OPENAIR_TARGETS"  >>  bin/${oai_build_date}


build_enb(){

##########################################
# process parameters
#########################################

    echo_info "4. Process the parameters"

    echo_info "User-defined Parameters :  HW=$HW, TARGET=$TARGET, ENB_S1=$ENB_S1, REL=$REL, RT=$RT, DEBUG=$DEBUG XFORMS=$XFORMS"
    
    echo "User-defined Parameters :  HW=$HW, TARGET=$TARGET, ENB_S1=$ENB_S1, REL=$REL, RT=$RT, DEBUG=$DEBUG XFORMS=$XFORMS" >> bin/${oai_build_date}
    
 
############################################
# compilation directives 
############################################

    echo_info "5. building the compilation directives ..."
    
    SOFTMODEM_DIRECTIVES="ENB_S1=$ENB_S1 DEBUG=$DEBUG XFORMS=$XFORMS "
    OAISIM_DIRECTIVES="ENB_S1=$ENB_S1 DEBUG=$DEBUG XFORMS=$XFORMS "
    
    if [ $HW = "USRP" ]; then 
	SOFTMODEM_DIRECTIVES="$SOFTMODEM_DIRECTIVES USRP=1 "
    fi
    
    if [ $HW = "EXMIMO" ]; then 
	SOFTMODEM_DIRECTIVES="$SOFTMODEM_DIRECTIVES EXMIMO=1"
    fi
    
    if [ $HW = "ETHERNET" ]; then 
	SOFTMODEM_DIRECTIVES="$SOFTMODEM_DIRECTIVES ETHERNET=1"
    fi 
    
    if [ $ENB_S1 -eq 0 ]; then 
	SOFTMODEM_DIRECTIVES="$SOFTMODEM_DIRECTIVES NAS=1 "
	OAISIM_DIRECTIVES="$OAISIM_DIRECTIVES NAS=1 "
    fi 
    
    if [ $REL = "REL8" ]; then
	SOFTMODEM_DIRECTIVES="$SOFTMODEM_DIRECTIVES Rel8=1 "
	OAISIM_DIRECTIVES="$OAISIM_DIRECTIVES Rel8=1 "
    else 
	SOFTMODEM_DIRECTIVES="$SOFTMODEM_DIRECTIVES Rel10=1 "
	OAISIM_DIRECTIVES="$OAISIM_DIRECTIVES Rel10=1 "
    fi
    
    if [ $RT = "RTAI" ]; then 
	if [ ! -f /usr/realtime/modules/rtai_hal.ko ];   then
	    echo_warning "RTAI doesn't seem to be installed"
	    RT="NONE"
	    SOFTMODEM_DIRECTIVES="$SOFTMODEM_DIRECTIVES RTAI=0 "
	else 
	    SOFTMODEM_DIRECTIVES="$SOFTMODEM_DIRECTIVES HARD_RT=1 "
	fi
    else 
	SOFTMODEM_DIRECTIVES="$SOFTMODEM_DIRECTIVES RTAI=0 "
	RT="NONE"
    fi
    
    if [ $TARGET != "ALL" ]; then 
	if [ $TARGET  != "SOFTMODEM" ]; then 
	    HW="NONE"
	fi
    fi
    
    output=$(check_for_machine_type 2>&1) 
    MACHINE_ARCH=$?
    if [ $MACHINE_ARCH -eq 64 ]; then
	SOFTMODEM_DIRECTIVES="$SOFTMODEM_DIRECTIVES LIBCONFIG_LONG=1 "
	OAISIM_DIRECTIVES="$OASIM_DIRECTIVES LIBCONFIG_LONG=1 "
    fi
    
    echo_success "SOFTMODEM Compilation directives: $SOFTMODEM_DIRECTIVES"
    echo_success "OAISIM Compilation directives:    $OAISIM_DIRECTIVES"
    
    echo "SOFTMODEM Compilation directives: $SOFTMODEM_DIRECTIVES" >>  bin/${oai_build_date}
    echo "OAISIM Compilation directive:    $OAISIM_DIRECTIVES" >>  bin/${oai_build_date}
    
    
############################################
# check the installation
############################################

    echo_info "6. Checking the installation ..."

    check_install_oai_software  
    check_install_asn1c

############################################
# compile 
############################################

    echo_info "7. compiling and installing the OAI binaries ..."

    softmodem_compiled=1
    oaisim_compiled=1
    unisim_compiled=1
    
    if [ $TARGET = "ALL" ]; then
	echo "############# compile_ltesoftmodem #############" >> bin/install_log.txt 
	output=$(compile_ltesoftmodem  >> bin/install_log.txt  2>&1 )
	softmodem_compiled=$?
	check_for_ltesoftmodem_executable
	echo_info "7.1 finished ltesoftmodem target : check the installation log file bin/install_log.txt" 
	
	echo "################ compile_oaisim #################"  >> bin/install_log.txt 
	output=$(compile_oaisim      >> bin/install_log.txt   2>&1 )
	oaisim_compiled=$?
	check_for_oaisim_executable
	echo_info "7.2 finished oaisim target : check the installation log file bin/install_log.txt" 
	
	echo "################## compile_unisim ##################"  >> bin/install_log.txt 
	output=$(compile_unisim      >> bin/install_log.txt  2>&1 )
	unisim_compiled=$?
	check_for_dlsim_executable
	check_for_ulsim_executable
	check_for_pucchsim_executable
	check_for_prachsim_executable
	check_for_pdcchsim_executable
	check_for_pbchsim_executable
	check_for_mbmssim_executable
	echo_info "7.3 finished unisim target : check the installation log file bin/install_log.txt" 
	
	
    else
	
	if [ $TARGET = "SOFTMODEM" ]; then 
	    echo "############# compile_ltesoftmodem #############" >> bin/install_log.txt 
	    output=$(compile_ltesoftmodem   >> bin/install_log.txt 2>&1 )
	    softmodem_compiled=$?
	    check_for_ltesoftmodem_executable
	    echo_info "7.1 finished ltesoftmodem target: check the installation log file bin/install_log.txt" 
	    
	fi
	if [ $TARGET = "OAISIM" ]; then 
	    echo "################ compile_oaisim #################"  >> bin/install_log.txt 
	    output=$(compile_oaisim   >> bin/install_log.txt 2>&1 )
	    oaisim_compiled=$?	
	    check_for_oaisim_executable
	    echo_info "7.2 finished oaisim target: check the installation log file bin/install_log.txt" 
	    
	fi
	if [ $TARGET = "UNISIM" ]; then 
	    echo "################## compile_unisim ##################"  >> bin/install_log.txt 
	    output=$(compile_unisim   >> bin/install_log.txt 2>&1 )
	    unisim_compiled=$?
	    check_for_dlsim_executable
	    check_for_ulsim_executable
	    check_for_pucchsim_executable
	    check_for_prachsim_executable
	    check_for_pdcchsim_executable
	    check_for_pbchsim_executable
	    check_for_mbmssim_executable
	    echo_info "7.3 finished unisim target: check the installation log file bin/install_log.txt" 
	    
	fi
    fi


############################################
# install 
############################################

    echo_info "8. Installing ..."
    
    if [ $softmodem_compiled = 0 ]; then 
	echo_success "target lte-softmodem built and installed in the bin directory"
	echo "target lte-softmodem built and installed in the bin directory"  >>  bin/${oai_build_date}
	output=$(install_ltesoftmodem $RT $HW $ENB_S1 )
    fi
    if [ $oaisim_compiled = 0 ]; then 
	echo_success "target oaisim built and installed in the bin directory"
	echo "target oaisim built and installed in the bin directory"  >>  bin/${oai_build_date}
	output=$(install_oaisim $ENB_S1 )
    fi 
    if [ $unisim_compiled =  0 ]; then 
	echo_success "target unisim built and installed in the bin directory"
	echo "target unisim built and installed in the bin directory"  >>  bin/${oai_build_date}
    fi 
    
    echo_info "build terminated, binaries are located in bin/"
    echo_info "build terminated, logs are located in bin/${oai_build_date} and bin/install_log.txt"
    
############################################
# testing
############################################
    
    if [ $OAI_TEST = 1 ]; then 
	echo_info "9. Testing ..."
	python $OPENAIR_TARGETS/TEST/OAI/test01.py
    else 
	echo_info "9. Bypassing the Tests ..."
    fi 
    
############################################
# run 
############################################
    echo_info "10. Running ... To be done"


}
build_epc(){

    epc_compiled=1

    
    echo_info "Note: this scripts works only for Ubuntu 12.04"

######################################
# CHECK MISC SOFTWARES AND LIBS      #
######################################
    echo_info "4. check the required packages for HSS"

    check_install_epc_software
   
    check_install_asn1c
    
    if [ $OAI_CLEAN = 1 ]; then
	check_install_freediamter
    else 
	if [ ! -d /usr/local/etc/freeDiameter ]; then
	    check_install_freediamter
	fi
    fi
    check_s6a_certificate

###########################################
# configure and compile
##########################################

    echo_info "5. configure and compile epc"

    output=$(compile_epc $OAI_CLEAN  >> bin/install_log.txt  2>&1 )
    epc_compiled=$?
    check_for_epc_executable
    echo_info "finished epc target: check the installation log file bin/install_log.txt" 
	    
    
###########################################
# install the binary in bin
##########################################

    echo_info "6. install the binary file"

    if [ $epc_compiled = 0 ]; then 
	echo_success "target epc built and installed in the bin directory"
	echo "target epc built and installed in the bin directory"  >>  bin/${oai_build_date}
	cp -f $OPENAIR_TARGETS/PROJECTS/GENERIC-LTE-EPC/CONF/epc.generic.conf  $OPENAIR_TARGETS/bin
    fi
    
######################################
# run
######################################
    echo_info "7. run EPC (check the bin/epc.generic.conf params)"

    sudo bin/oai_epc -c bin/epc.generic.conf  -K /tmp/itti.log

}

build_hss(){

    hss_compiled=1
    
    echo_info "Note: this scripts works only for Ubuntu 12.04"

######################################
# CHECK MISC SOFTWARES AND LIBS      #
######################################
    echo_info "4. check the required packages for HSS"
    hss_certificate_generated=1;
    
    check_install_hss_software
    
    if [ $OAI_CLEAN = 1 ]; then
	check_install_freediamter
    else 
	if [ ! -d /usr/local/etc/freeDiameter ]; then
	    check_install_freediamter
	fi
    fi
    $(make_certs >> bin/install_log.txt  2>&1)
    output=$(check_s6a_certificate >> bin/install_log.txt  2>&1)
    hss_certificate_generated=$?
  
######################################
# compile HSS                        #
######################################
    echo_info "5. compile HSS"
    
     output=$(compile_hss  $OAI_CLEAN >> bin/install_log.txt  2>&1 )
     hss_compiled=$?
     check_for_hss_executable
     echo_info "finished hss target: check the installation log file bin/install_log.txt" 
 
######################################
# fill the HSS DB
######################################
     echo_info "6. create HSS database (for EURECOM SIM CARDS)"
     hss_db_created=1
     USER="hssadmin"
     PW=""
     output=$(create_hss_database $USER $PW )
     hss_db_created=$?
     if [ $hss_db_created = 0 ]; then
	 echo_warning "hss DB not created"
     fi
######################################
# install hss
######################################

     echo_info "7. install the binary file"
     if [ $hss_compiled = 0 ]; then
	 echo_success "target hss built, DB created  and installed in the bin directory"
	 echo "target hss built, DB created, and installed in the bin directory"  >>  bin/${oai_build_date}
         cp -rf $OPENAIRCN_DIR/OPENAIRHSS/conf  $OPENAIR_TARGETS/bin
	 $SUDO cp $OPENAIR_TARGETS/bin/conf/hss_fd.local.conf /etc/openair-hss
     fi

######################################
# LAUNCH HSS                         #
######################################
     echo_info "8. run hss in bin/:  ./openair-hss -c conf/hss.local.conf"
     cd bin
     ./openair-hss -c conf/hss.local.conf
}


 
############################################
# set the build 
############################################

echo_info "3. set the top-level build target"
case "$BUILD_LTE" in
    'ENB')           
	echo_success "build LTE eNB"
	build_enb
	;;
    'EPC')           
	echo_warning "build EPC(MME and xGW): Experimental"
	build_epc
	;;
    'HSS')           
	echo_warning "build HSS: Experimental"
	build_hss 
	;;
    *)
	echo_error "Unknown option $BUILD_LTE: do not build"
	;;
esac