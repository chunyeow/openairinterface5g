#!/bin/bash

########################################
#
# SPECTRA DEMO INSTALLATION AND RUN
#
#
#
SCRIPT_VERSION="v1.12"
DATE="13 Oct 2014"
AUTHOR="Rui Costa"
EMAIL="ferreira@eurecom.fr"
########################################

OWN_TAG=$0

INSTALL_PATH=$PWD
OPENAIR_PATH=$INSTALL_PATH/openair4G
ODTONE_PATH=$INSTALL_PATH/ODTONE
BOOST_PATH=$INSTALL_PATH/boost_1_49_0
SPECTRA_SRC_PATH=$INSTALL_PATH/spectra_demo_src

NODE_TYPE=NONE
INSTALL_TYPE=NONE

########################################################################################### UTILS
function util_echo {
# return value passed in $1 (usually $?)
# return value = 0 - OK - Light green
if [ $1 -eq 0 ] ; then
   echo -e "\e[92m\e[1m[$OWN_TAG]\e[0m $2"
# return value != 0 - ERROR
else
# return value -gt 255 - WARNING - Light Yellow
   if [ $1 -gt 255 ] ; then
      echo -e "\e[93m\e[1m[$OWN_TAG]\e[0m $2"
   else
# return value 1 to 255 - ERROR - Light RED
      echo -e "\e[91m\e[1m[$OWN_TAG]\e[0m $2"
      exit
   fi
fi
}

########################################################################################### BOOST C++
function fetch_boost {
cd $INSTALL_PATH
util_echo $? "-------------------------------------------------" 
util_echo $? "BOOST C++ Library ..."
# Although new versions of boost exist, when compiling ODTONE he asks for v1.49.0 
# (we humour it so to not have linkage problems later)
wget --trust-server-names "http://downloads.sourceforge.net/project/boost/boost/1.49.0/boost_1_49_0.tar.gz?r=&ts=1412338583&use_mirror=optimate"
util_echo $? "Fetched BOOST C++ Library ..."
sleep 2
tar -xzf boost_1_49_0.tar.gz 
util_echo $? "Extracted BOOST C++ Library ..."
}

function build_boost {
cd $BOOST_PATH
util_echo $? "-------------------------------------------------" 
util_echo $? "BOOST C++ Library ..."
sleep 2
# Configuring Boost
./bootstrap.sh 
util_echo $? "Bootstrapped BOOST C++ Library ..."
sleep 2
# Compiling Boost (pthread link flag is vital)
./b2 --with-date_time --with-thread linkflags=-lpthread
util_echo $? "Built BOOST C++ Library ..."
}

########################################################################################### ODTONE
function fetch_odtone {
cd $INSTALL_PATH
util_echo $? "-------------------------------------------------" 
util_echo $? "ODTONE ..."
sleep 2
# Getting newest version of Odtone
git clone https://github.com/ATNoG/ODTONE.git
util_echo $? "Checked Out ODTONE from ITAveiro GIT..."
cd $ODTONE_PATH
git submodule update --init
util_echo $? "Updated ODTONE submodules..."
}

function build_odtone { 
cd $ODTONE_PATH
util_echo $? "-------------------------------------------------" 
util_echo $? "ODTONE ..."

# /***** REPLACED WITH Git spectra PATCH
#
# Establishing the pointer to a local compilation of boost instead of the usual in /usr/local/src/
#touch boost-build.jam
#echo "boost-build $BOOST_PATH/tools/build/v2 ;" > boost-build.jam
# Copying our new MIH Users
#cp -r $SPECTRA_SRC_PATH/mih_users/* $ODTONE_PATH/app/
# we MUST patch these files! ODTONE by default has no knowledge of LTE links
# this overwrite of files ensures that full LTE link parameters exist.
# A better way of paching the files should be implemented later!!
#cp -r $SPECTRA_SRC_PATH/mihf/link.hpp $ODTONE_PATH/inc/odtone/mih/types/  
#cp -r $SPECTRA_SRC_PATH/mihf/archive.hpp $ODTONE_PATH/inc/odtone/mih/detail/  
#cp -r $SPECTRA_SRC_PATH/mihf/address.hpp $ODTONE_PATH/inc/odtone/mih/types/  
#cp -r $SPECTRA_SRC_PATH/mihf/bin_query.hpp $ODTONE_PATH/inc/odtone/mih/types/  
#cp -r $SPECTRA_SRC_PATH/mihf/CMakeLists.txt $ODTONE_PATH/lib/odtone/  
#cp -r $SPECTRA_SRC_PATH/mihf/conf.cpp $ODTONE_PATH/lib/odtone/  
#cp -r $SPECTRA_SRC_PATH/mihf/conf.hpp $ODTONE_PATH/inc/odtone/  
#cp -r $SPECTRA_SRC_PATH/mihf/Jamfile $ODTONE_PATH/lib/odtone/  
#cp -r $SPECTRA_SRC_PATH/mihf/command_service.cpp $ODTONE_PATH/src/mihf/  
#cp -r $SPECTRA_SRC_PATH/mihf/dst_transaction.cpp $ODTONE_PATH/src/mihf/  
#cp -r $SPECTRA_SRC_PATH/mihf/event_service.cpp $ODTONE_PATH/src/mihf/  
#cp -r $SPECTRA_SRC_PATH/mihf/link_book.cpp $ODTONE_PATH/src/mihf/  
#cp -r $SPECTRA_SRC_PATH/mihf/main.cpp $ODTONE_PATH/src/mihf/  
#cp -r $SPECTRA_SRC_PATH/mihf/service_access_controller.cpp $ODTONE_PATH/src/mihf/  
#cp -r $SPECTRA_SRC_PATH/mihf/service_management.cpp $ODTONE_PATH/src/mihf/  
#cp -r $SPECTRA_SRC_PATH/mihf/src_transaction.cpp $ODTONE_PATH/src/mihf/  
#cp -r $SPECTRA_SRC_PATH/mihf/transaction_pool.cpp $ODTONE_PATH/src/mihf/  
#
# *****/
# Applying SPECTRA patch to ODTONE source code
git apply --check --stat $SPECTRA_SRC_PATH/common/mihf_src/0001-SPECTRA-patch-for-ODTONE-v0.1.patch 
util_echo $? "Checking if SPECTRA patch can be aplied..."
git apply  $SPECTRA_SRC_PATH/common/mihf_src/0001-SPECTRA-patch-for-ODTONE-v0.1.patch
util_echo $? "Applying SPECTRA patch to ODTONE..."
# Changing hostname on the boost build
sed -i "s%nikaia%$USER%g" $ODTONE_PATH/boost-build.jam
util_echo $? "Modifying boost-build.jam..."
# Compiling Odtone core
$BOOST_PATH/b2 linkflags=-lpthread
util_echo $? "Compiled ODTONE..."
# Copying lte_test_user for this node
rm -rf $ODTONE_PATH/app/lte_test_user
cp -r $SPECTRA_SRC_PATH/$NODE_TYPE/mih_user/* $ODTONE_PATH/app/
util_echo $? "Installed ODTONE mih user for: $NODE_TYPE..."
# Compiling lte_test_user
$BOOST_PATH/b2 app/lte_test_user/  linkflags=-lpthread linkflags=-lrt
util_echo $? "Compiled ODTONE apps..."
# Copying odtone confs
cp $SPECTRA_SRC_PATH/$NODE_TYPE/mih_conf/*.conf $ODTONE_PATH/dist/
util_echo $? "Installed ODTONE configuration files for: $NODE_TYPE..."
}

########################################################################################### OPENAIR
function fetch_openair {
mkdir $OPENAIR_PATH
cd $INSTALL_PATH
util_echo $? "-------------------------------------------------" 
util_echo $? "OPENAIR EURECOM ..."
sleep 2
svn co http://svn.eurecom.fr/openair/openair4G/trunk $OPENAIR_PATH
util_echo $? "Checked Out openair4G/trunk from EURECOM SVN..."
}

function build_openair {
cd $OPENAIR_PATH
util_echo $? "-------------------------------------------------" 
util_echo $? "OPENAIR ..."
sleep 2
# Putting in place pre-made scripts for the build of openair
cp $SPECTRA_SRC_PATH/common/openair_scripts/* $OPENAIR_PATH/targets/PROJECTS/SPECTRA/ 
util_echo $? "Installed OpenAir common build scripts ..."
# Putting in place pre-made scripts for the conf of openair
cp $SPECTRA_SRC_PATH/$NODE_TYPE/oai_conf/* $OPENAIR_PATH/targets/PROJECTS/SPECTRA/ 
util_echo $? "Installed OpenAir startup scripts and conf for $NODE_TYPE..."
cd $OPENAIR_PATH/targets/PROJECTS/SPECTRA/ 
# Updating the path location of ODTONE and BOOST on the script, according to the local machine.
sed -i "s%path_to_boost_folder%$BOOST_PATH%g" env_802dot21.bash
util_echo $? " Fixed BOOST path in env_802dot21.bash... $BOOST_PATH"
sed -i "s%path_to_odtone_folder%$ODTONE_PATH%g" env_802dot21.bash
util_echo $? " Fixed ODTONE path in env_802dot21.bash... $ODTONE_PATH"
if [ $NODE_TYPE == "enb2" ] ; then
   sed -i "s%enb_lte_user%enb2_lte_user%g" env_802dot21.bash
   util_echo $? " Fixed exec name (enb2_lte_user) for $NODE_TYPE..."
#   sed -i "s%ENB_MIH_USER_CONF_FILE=enb_lte_user.conf%ENB_MIH_USER_CONF_FILE=enb2_lte_user.conf%g" env_802dot21.bash
   util_echo $? " Fixed conf name (enb2_lte_user.conf) for path $NODE_TYPE..."
fi
if [ $NODE_TYPE == "ue2" ] ; then
   sed -i "s%ue_lte_user%ue2_user%g" env_802dot21.bash
   util_echo $? " Fixed exec name (enb2_lte_user) for $NODE_TYPE..."
   util_echo $? " Fixed conf name (enb2_lte_user.conf) for path $NODE_TYPE..."
fi
# executing the (re)build of all components
# superuser necessary for installing the ITTY tool of openair
./build_all.bash
util_echo $? "Finished OpenAir build..."
}

########################################################################################### PRINT_HELP
function print_help {
util_echo 333 " -------------------------------------------------" 
util_echo 333 " USAGE: "
util_echo 333 " $ $OWN_TAG  <install_type> <node_type>" 
util_echo 333 " " 
util_echo 333 " <install_type>" 
util_echo 333 "      -full           If you start from scratch" 
util_echo 333 "      -oai_svn        If you got this script after" 
util_echo 333 "                      checking out the Openair SVN" 
util_echo 333 " <node_type>" 
util_echo 333 "      -ue1            For installing ue1 files" 
util_echo 333 "      -ue2            For installing ue2 files" 
util_echo 333 "      -enb1           For installing eNB1 files" 
util_echo 333 "      -enb2           For installing eNB2 files" 
util_echo 333 " " 
util_echo 333 " -------------------------------------------------" 
util_echo 333 " " 
exit
}

########################################################################################### MAIN
clear
# Testing Args
if [ $# -ne 2 ]; then
   util_echo 333 " Incorrect Number of Arguments!!"
   print_help
fi
if [ $1 == "-full" ]; then
   INSTALL_TYPE=FULL
fi
if [ $1 == "-oai_svn" ]; then
   INSTALL_TYPE=OAISVN
fi
if [ $2 == "-ue1" ]; then
   NODE_TYPE=ue1
fi
if [ $2 == "-ue2" ]; then
   NODE_TYPE=ue2
fi
if [ $2 == "-enb1" ]; then
   NODE_TYPE=enb1
fi
if [ $2 == "-enb2" ]; then 
   NODE_TYPE=enb2
fi
if [ $INSTALL_TYPE == "NONE" ]; then
   util_echo 333 " Incorrect Arguments (type of installation) !!"
   print_help
fi
if [ $NODE_TYPE == "NONE" ]; then
   util_echo 333 " Incorrect Arguments (type of node) !!"
   print_help
fi
# Starting...
util_echo $? " -------------------------------------------------" 
util_echo $? " SPECTRA DEMO "
util_echo $? " Installation Script" 
util_echo $? " " 
util_echo $? " $SCRIPT_VERSION $DATE" 
util_echo $? " " 
util_echo $? " Author: $AUTHOR <$EMAIL> " 
util_echo $? " -------------------------------------------------" 
util_echo 333 " Performing installation type: $INSTALL_TYPE for $NODE_TYPE"
sleep 3
util_echo $? " -------------------------------------------------" 
util_echo $? " >> Fetching sources"
util_echo $? " -------------------------------------------------" 
if [ $INSTALL_TYPE == "OAISVN" ]; then
   INSTALL_PATH=$PWD/../../../../
   OPENAIR_PATH=$PWD/../../../
   ODTONE_PATH=$INSTALL_PATH/ODTONE
   BOOST_PATH=$INSTALL_PATH/boost_1_49_0
   SPECTRA_SRC_PATH=$PWD/spectra_demo_src
else
   fetch_openair
fi
fetch_odtone
fetch_boost
sleep 1
util_echo $? " -------------------------------------------------" 
util_echo $? " >> Building"
util_echo $? " -------------------------------------------------" 
sleep 2
build_boost
build_odtone
build_openair
util_echo 333 " Open Air Configuration files at:"
util_echo 333 " $OPENAIR_PATH/targets/PROJECTS/SPECTRA/"
util_echo 333 " "
util_echo 333 " ODTONE Configuration files at:"
util_echo 333 " $ODTONE_PATH/dist/"
util_echo $? " "
util_echo $? " "
util_echo $? " -------------------------------------------------" 
util_echo $? " SPECTRA DEMO preparation script ... finished!" 
util_echo $? " -------------------------------------------------" 
cd $INSTALL_PATH
exit 0
########################################################################################### EOF


