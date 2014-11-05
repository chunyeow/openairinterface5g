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
# brief
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

while getopts "bcdmsxzhe:w:r:t:" OPTION; do
   case "$OPTION" in
       b)
	   ENB_S1=1
	   echo "setting eNB S1 flag to: $ENB_S1"
	   ;;
       c)
	   rm -rf ./.lock_oaibuild
	   OAI_CLEAN=1
	   echo "setting clean flag to: $OAI_CLEAN"
	   ;;
       d)
	   DEBUG=1
	   echo "setting debug flag to: $DEBUG"
	   ;;
       e)
	   RT="$OPTARG"
	   echo "setting realtime flag to: $RT"
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
	   RT="RTAI"
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
echo_info "3. Creating the bin dir ..." 
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

##########################################
# process parameters
#########################################

echo_info "2. Process the parameters"

echo_info "User-defined Parameters :  HW=$HW, TARGET=$TARGET, ENB_S1=$ENB_S1, REL=$REL, RT=$RT, DEBUG=$DEBUG XFORMS=$XFORMS"

echo "User-defined Parameters :  HW=$HW, TARGET=$TARGET, ENB_S1=$ENB_S1, REL=$REL, RT=$RT, DEBUG=$DEBUG XFORMS=$XFORMS" >> bin/${oai_build_date}

 
############################################
# compilation directives 
############################################

echo_info "3. building the compilation directives ..."

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
# setting and printing OAI envs, we should check here
############################################

echo_info "3. Setting the OAI PATHS ..."

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

############################################
# check the installation
############################################

echo_info "6. Checking the installation ..."

check_install_oai_software  

############################################
# compile 
############################################

echo_info "7. compiling and installing the OAI binaries ..."

softmodem_compiled=1
oaisim_compiled=1
unisim_compiled=1

if [ $TARGET = "ALL" ]; then
    echo "############# compile_ltesoftmodem #############" 
    output=$(compile_ltesoftmodem $OAI_CLEAN  )
    softmodem_compiled=$?
    check_for_ltesoftmodem_executable
    
    echo "################ compile_oaisim #################" 
    output=$(compile_oaisim   $OAI_CLEAN      )
    oaisim_compiled=$?
    check_for_oaisim_executable

    echo "################## compile_unisim ##################" 
    output=$(compile_unisim  $OAI_CLEAN      )
    unisim_compiled=$?
    check_for_dlsim_executable
    check_for_ulsim_executable
    check_for_pucchsim_executable
    check_for_prachsim_executable
    check_for_pdcchsim_executable
    check_for_pbchsim_executable
    check_for_mbmssim_executable
    
else
    
    if [ $TARGET = "SOFTMODEM" ]; then 
	echo "################ compile_ltesoftmodem #################" 
	output=$(compile_ltesoftmodem  $OAI_CLEAN  )
	softmodem_compiled=$?
	check_for_ltesoftmodem_executable
    fi
    if [ $TARGET = "OAISIM" ]; then 
	echo "################ compile_oaisim ###############" 
	output=$(compile_oaisim  $OAI_CLEAN )
	oaisim_compiled=$?	
	check_for_oaisim_executable
    fi
    if [ $TARGET = "UNISIM" ]; then 
	echo "################ compile_unisim ###############"
	output=$(compile_unisim  $OAI_CLEAN )
	unisim_compiled=$?
	check_for_dlsim_executable
	check_for_ulsim_executable
	check_for_pucchsim_executable
	check_for_prachsim_executable
	check_for_pdcchsim_executable
	check_for_pbchsim_executable
	check_for_mbmssim_executable
    fi
fi


############################################
# install 
############################################

echo_info "8. Installing ..."

if [ $softmodem_compiled = 0 ]; then 
    echo_success "target lte-softmodem built "
    echo "target lte-softmodem built "  >>  bin/${oai_build_date}
    output=$(install_ltesoftmodem $RT $HW $ENB_S1 )
fi
if [ $oaisim_compiled = 0 ]; then 
    echo_success "target oaisim built "
    echo "target oaisim built "  >>  bin/${oai_build_date}
    output=$(install_oaisim $ENB_S1 )
fi 
if [ $unisim_compiled =  0 ]; then 
    echo_success "target unisim built "
    echo "target unisim built "  >>  bin/${oai_build_date}
fi 

echo_info "build terminated, binaries are located in bin/"
echo_info "build terminated, logs are located in bin/${oai_build_date}"

   

############################################
# testing
############################################

if [ $OAI_TEST = 1 ]; then 
    echo_info "9. Testing ..."
    python $OPENAIR_TARGETS/TEST/OAI/test01.py
else 
    echo_info "9. Bypassing the Tests ..."
fi 
 
