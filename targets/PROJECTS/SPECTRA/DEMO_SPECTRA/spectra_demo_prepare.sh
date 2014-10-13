#!/bin/bash

########################################
#
# SPECTRA DEMO PREPARATION
#
#
#
SCRIPT_VERSION="v1.11"
DATE="01 Oct 2014"
AUTHOR="Rui Costa"
EMAIL="ferreira@eurecom.fr"
########################################

OWN_TAG=$0

INSTALL_PATH=$PWD
OPENAIR_PATH=$INSTALL_PATH/openair4G
ODTONE_PATH=$INSTALL_PATH/ODTONE
BOOST_PATH=$INSTALL_PATH/boost_1_49_0
SPECTRA_SRC_PATH=$INSTALL_PATH/spectra_demo_src
SPECTRA_OAI_PATH=$OPENAIR_PATH/targets/PROJECTS/SPECTRA/

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

########################################################################################### MAIN
clear
# Starting...
util_echo $? " -------------------------------------------------" 
util_echo $? " SPECTRA DEMO "
util_echo $? " Prepararation Script" 
util_echo $? " " 
util_echo $? " $SCRIPT_VERSION $DATE" 
util_echo $? " " 
util_echo $? " Author: $AUTHOR <$EMAIL> " 
util_echo $? " -------------------------------------------------" 
util_echo $? " " 
sleep 3
util_echo $? " -------------------------------------------------" 
util_echo $? " >> Fetching sources"
util_echo $? " -------------------------------------------------" 

fetch_openair

cd $SPECTRA_OAI_PATH/
util_echo $? " >> Finished Prepare Script!!"
util_echo $? " >> Entering Install Script!!"
./spectra_demo_install.sh -oai_svn $1

########################################################################################### EOF


