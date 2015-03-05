################################################################################
#   OpenAirInterface
#   Copyright(c) 1999 - 2014 Eurecom
#
#   OpenAirInterface is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   OpenAirInterface is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with OpenAirInterface.The full GNU General Public License is
#  included in this distribution in the file called "COPYING". If not,
#  see <http://www.gnu.org/licenses/>.
#
# Contact Information
# OpenAirInterface Admin: openair_admin@eurecom.fr
# OpenAirInterface Tech : openair_tech@eurecom.fr
# OpenAirInterface Dev  : openair4g-devel@eurecom.fr
#
# Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE
# Author: laurent THOMAS
###############################################################################

cmake_minimum_required (VERSION 2.8)


# Base directories, compatible with legacy OAI building
################################################
set (OPENAIR_DIR     $ENV{OPENAIR_DIR})
set (OPENAIR1_DIR    ${OPENAIR_DIR}/openair1)
set (OPENAIR2_DIR    ${OPENAIR_DIR}/openair2)
set (OPENAIR3_DIR    ${OPENAIR_DIR}/openair3)
set (OPENAIR_TARGETS ${OPENAIR_DIR}/targets)
set (OPENAIRCN_DIR   ${OPENAIR_DIR}/openair-cn)
set (OPENAIR_CMAKE   ${OPENAIR_DIR}/cmake_targets)
set (OPENAIR_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY})

project (OpenAirInterface)

# compilation flags
# added CMAKER conditional compilation to not bother legacy building system
#############################################
if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)

#
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse4.2 -std=gnu99 -Wall -Wstrict-prototypes -fno-strict-aliasing -rdynamic -funroll-loops -Wno-packed-bitfield-compat -DCMAKER")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -ggdb -DMALLOC_CHECK_=3")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -ggdb -DMALLOC_CHECK_=3 -O3")

find_package(Subversion)
if(SUBVERSION_FOUND)
  Subversion_WC_INFO(${OPENAIR_DIR} openair)
  set (FIRMWARE_VERSION "${openair_WC_REVISION} - ${openair_WC_LAST_CHANGED_DATE}")
  Subversion_WC_LOG(${OPENAIR_DIR} Project)
  message("Last changed log is ${Project_LAST_CHANGED_LOG}")
else()
  set (FIRMWARE_VERSION "No svn information")
endif()
add_definitions("-DFIRMWARE_VERSION=\"${FIRMWARE_VERSION}\"")

# Define options for compilation
################################################
function(add_boolean_option name val helpstr)
  if(DEFINED ${name})
    set(value ${${name}})
  else(DEFINED ${name})
    set(value ${val})
  endif()
  set(${name} ${value} CACHE STRING "${helpstr}")
  set_property(CACHE ${name} PROPERTY TYPE BOOL)
  if (${value})
    add_definitions("-D${name}")
  endif (${value})
endfunction(add_boolean_option)

function(add_integer_option name val helpstr)
  if(DEFINED ${name})
    set(value ${${name}})
  else(DEFINED ${name})
    set(value ${val})
  endif()
  set(${name} ${value} CACHE STRING "${helpstr}")
  add_definitions("-D${name}=${value}")
endfunction(add_integer_option)

# Debug related options
#########################################
add_boolean_option(MSG_PRINT  False "print debug messages")
add_boolean_option(DISABLE_XER_PRINT True "print XER Format")
add_boolean_option(RRC_MSG_PRINT False "print RRC messages")
add_boolean_option(PDCP_MSG_PRINT False "print PDCP messages")
add_boolean_option(DEBUG_PHY False "for unitary tests with dlsim")

add_boolean_option(XFORMS True "This adds the possibility to see the signal oscilloscope")
if (${XFORMS})
  include_directories ("/usr/include/X11")
  set(XFORMS_SOURCE ${OPENAIR1_DIR}/PHY/TOOLS/lte_phy_scope.c)
  set(option_lib_xforms "forms")
endif (${XFORMS})
  
# to be documented options
###################
#set(LOCALIZATION  False)
add_boolean_option(LOCALIZATION  True "???")
add_boolean_option(LINUX_LIST True "???")
add_boolean_option(SPECTRA True "???")
add_boolean_option(HARD_RT False "???")
add_boolean_option(ENABLE_SECURITY True "???")
add_boolean_option(SECU True "???")
add_boolean_option(JUMBO_FRAME True "???")
add_boolean_option(ENABLE_USE_MME True "???")
add_boolean_option(LINK_PDCP_TO_GTPV1U True "????")
add_boolean_option(USER_MODE True "????")
add_boolean_option(LOG_NO_THREAD True "Disable thread for log, seems always set to true")
add_boolean_option(ENABLE_NAS_UE_LOGGING True "????")
add_boolean_option(ENABLE_USE_CPU_EXECUTION_TIME True "????")
add_boolean_option(OPENAIR2 True "????")
add_boolean_option(NO_RRM True "????")
add_boolean_option(PUCCH True "????")
add_boolean_option(NEW_FFT True "????")
add_boolean_option(PHYSIM True "????")
add_boolean_option(PC_TARGET True "????")
add_boolean_option(PC_DSP True "????")
add_boolean_option(OPENAIR_LTE True "Seems legacy: keep it to true")
add_boolean_option(ENABLE_FXP True "????")
add_boolean_option(OPENAIR1 True "????")
add_boolean_option(DRIVER2013 True "????")
add_boolean_option(ENABLE_RAL True "????")
add_boolean_option(MIH_C_MEDIEVAL_EXTENSIONS True "????")
add_boolean_option(SMBV False "Rohde&Schwarz SMBV100A vector signal generator")
add_boolean_option(UE_BUILD True "????")
add_boolean_option(NAS_UE True "????")
add_boolean_option(PHYSIM False "for L1 simulators (dlsim, ulsim, ...)")
add_boolean_option(PHY_EMUL False "not clear: must remain False for dlsim")
add_boolean_option(MU_RECEIVER False "????")
add_boolean_option(RANDOM_BF False "????")
add_boolean_option(PBS_SIM False "????")
add_boolean_option(PERFECT_CE False "????")

add_boolean_option(EMOS False "????")
if(${EMOS})
  add_definitions("-D_FILE_OFFSET_BITS=64")
  add_library(gps)
endif(${EMOS})

add_integer_option(MAX_NUM_CCs 1 "????")
add_integer_option(MAX_CONTEXT 1 "????")
add_integer_option(PHY_CONTEXT 1 "????")

add_boolean_option(ENABLE_ITTI True "ITTI is internal messaging, should remain enabled")
set (ITTI_DIR ${OPENAIR_DIR}/common/utils/itti)
if (${ENABLE_ITTI})
  add_library(ITTI 
    # add .h files if depend on (this one is generated)
    ${ITTI_DIR}/intertask_interface.h
    ${ITTI_DIR}/intertask_interface.c
    ${ITTI_DIR}/intertask_interface_dump.c
    ${ITTI_DIR}/backtrace.c
    ${ITTI_DIR}/memory_pools.c
    ${ITTI_DIR}/signals.c
    ${ITTI_DIR}/timer.c
    )
  set(option_ITTI ITTI)
  set(EXMIMO_IOT)
endif (${ENABLE_ITTI})

add_boolean_option(RTAI False "Use RTAI")
if (${RTAI})
  set(ENABLE_RTAI_CLOCK)
  set(CONFIG_RTAI_LXRT_INLINE)
  include_directories ("/usr/realtime/include")
  include_directories ("/usr/realtime/include/asm")
  set(RTAI_SOURCE sched_dlsch.c sched_rx_pdsch.c rt_wrapper.c vcd_signal_dumper.c log.c)
endif (${RTAI})

#############################
# ASN.1 grammar C code generation & dependancies
################################
# A difficulty: asn1c generates C code of a un-predictable list of files
# so, generate the c from asn1c once at cmake run time
# So, if someone modify the asn.1 source file in such as way that it will create
# (so creating new asn.1 objects instead of modifying the object attributes)
# New C code source file, cmake must be re-run (instead of re-running make only) 
#############
set(asn1c_call "${OPENAIR_CMAKE}/generate_asn1.bash")
set(asn1_generated_dir ${OPENAIR_BIN_DIR})

# RRC
######
if (DEFINED RRC_ASN1_VERSION)
  set(RRC_ASN1_VERSION ${RRC_ASN1_VERSION} CACHE STRING "ASN.1 version of RRC interface")
else(DEFINED RRC_ASN1_VERSION)
  set(RRC_ASN1_VERSION "Rel10" CACHE STRING "ASN.1 version of RRC interface")
endif()

set_property(CACHE RRC_ASN1_VERSION PROPERTY STRINGS "Rel8" "Rel10" "CBA")

if (${RRC_ASN1_VERSION} STREQUAL "Rel8")
  set (RRC_GRAMMAR ${OPENAIR2_DIR}/RRC/LITE/MESSAGES/asn1c/ASN1_files/EUTRA-RRC-Definitions-86.asn)
elseif (${RRC_ASN1_VERSION} STREQUAL "Rel10")
  set (RRC_GRAMMAR ${OPENAIR2_DIR}/RRC/LITE/MESSAGES/asn1c/ASN1_files/EUTRA-RRC-Definitions-a20.asn)
endif  (${RRC_ASN1_VERSION} STREQUAL "Rel8")

set (RRC_FULL_DIR ${asn1_generated_dir}/${RRC_ASN1_VERSION})
if(NOT EXISTS ${asn1c_call})
  message( FATAL_ERROR "The script ${asn1c_call} must be present" )
endif(NOT EXISTS ${asn1c_call})
execute_process(COMMAND ${asn1c_call}  ${RRC_FULL_DIR} ${RRC_GRAMMAR})
add_definitions("-D${RRC_ASN1_VERSION}")
file(GLOB rrc_source ${RRC_FULL_DIR}/*.c)
set(rrc_source  ${rrc_source} ${OPENAIR2_DIR}/RRC/LITE/MESSAGES/asn1_msg.c)
file(GLOB rrc_h ${RRC_FULL_DIR}/*.h)
set(rrc_h ${rrc_h} ${RRC_FULL_DIR}/asn1_constants.h)
add_library(rrc_asn1 ${rrc_h} ${rrc_source})
include_directories ("${RRC_FULL_DIR}")

# add the command to generate the source code
# Warning: if you modify ASN.1 source file to generate new C files, cmake should be re-run instead of make
add_custom_command (
  OUTPUT ${RRC_FULL_DIR}/asn1_constants.h
  COMMAND ${asn1c_call}  ${RRC_FULL_DIR} ${RRC_GRAMMAR}
  DEPENDS ${RRC_GRAMMAR}
  )

# S1AP
# Same limitation as described in RRC: unknown generated file list
# so we generate it at cmake time
##############
if (DEFINED USE_MME)
  set(USE_MME ${USE_MME} CACHE HELPSTRING "MME version")
else (DEFINED USE_MME)
  set(USE_MME "R10" CACHE HELPSTRING "MME version")
endif (DEFINED USE_MME)

set_property(CACHE USE_MME PROPERTY STRINGS False R8 R10)

if (NOT ${USE_MME} STREQUAL "False")
  add_definitions("-DUSE_MME=${USE_MME}")
  set(S1AP_DIR ${OPENAIRCN_DIR}/S1AP)
  if (${USE_MME} STREQUAL "R10")
    set (ASN1RELDIR R10.5)
    add_definitions("-DUPDATE_RELEASE_9 -DUPDATE_RELEASE_10")
  elseif (${USE_MME} STREQUAL "R9")
    set (ASN1RELDIR R9.8)
    add_definitions("-DUPDATE_RELEASE_9")
  else(${USE_MME} STREQUAL "R8")
    set (ASN1RELDIR R8.10)
  endif(${USE_MME} STREQUAL "R10")
  set(S1AP_ASN_DIR ${S1AP_DIR}/MESSAGES/ASN1/${ASN1RELDIR})
  set(S1AP_ASN_FILES
    ${S1AP_ASN_DIR}/S1AP-CommonDataTypes.asn 
    ${S1AP_ASN_DIR}/S1AP-Constants.asn 
    ${S1AP_ASN_DIR}/S1AP-IEs.asn 
    ${S1AP_ASN_DIR}/S1AP-PDU.asn
    )
  set(S1AP_C_DIR ${asn1_generated_dir}/${ASN1RELDIR})
  
  execute_process(COMMAND ${asn1c_call} ${S1AP_C_DIR} ${S1AP_ASN_FILES})
  file(GLOB S1AP_source ${S1AP_C_DIR}/*.c)
  set(S1AP_OAI_SRC
    ${S1AP_DIR}/s1ap_common.c
    ${S1AP_DIR}/s1ap_eNB.c
    ${S1AP_DIR}/s1ap_eNB_context_management_procedures.c
    ${S1AP_DIR}/s1ap_eNB_decoder.c
    ${S1AP_DIR}/s1ap_eNB_encoder.c
    ${S1AP_DIR}/s1ap_eNB_handlers.c
    ${S1AP_DIR}/s1ap_eNB_itti_messaging.c
    ${S1AP_DIR}/s1ap_eNB_management_procedures.c
    ${S1AP_DIR}/s1ap_eNB_nas_procedures.c
    ${S1AP_DIR}/s1ap_eNB_nnsf.c
    ${S1AP_DIR}/s1ap_eNB_overload.c
    ${S1AP_DIR}/s1ap_eNB_trace.c
    ${S1AP_DIR}/s1ap_eNB_ue_context.c
    )
  set(S1AP_OAI_generated 
    ${S1AP_C_DIR}/s1ap_decoder.c
    ${S1AP_C_DIR}/s1ap_encoder.c
    ${S1AP_C_DIR}/s1ap_xer_print.c
    ${S1AP_C_DIR}/s1ap_ies_defs.h
    )
  file(GLOB s1ap_h ${S1AP_C_DIR}/*.h)
  set(s1ap_h ${s1ap_h} ${S1AP_C_DIR}/s1ap_ies_defs.h)
  add_custom_command (
    OUTPUT ${S1AP_OAI_generated}
    COMMAND ${asn1c_call} ${S1AP_C_DIR} ${S1AP_ASN_FILES}
    COMMAND python ${S1AP_DIR}/MESSAGES/ASN1/asn1tostruct.py -f${S1AP_ASN_DIR}/S1AP-PDU-Contents.asn -o${S1AP_C_DIR}
    DEPENDS ${S1AP_ASN_FILES} 
    )
  add_library(s1ap_asn1 ${S1AP_OAI_SRC} ${S1AP_OAI_generated} ${S1AP_source})
  include_directories ("${S1AP_C_DIR}")
  include_directories ("${S1AP_DIR}")
endif(NOT ${USE_MME} STREQUAL "False")


# Hardware dependant options
###################################
if(DEFINED NB_ANTENNAS_RX)
  set(NB_ANTENNAS_RX ${NB_ANTENNAS_RX} CACHE HELPSTRING "Number of antennas in reception")
else(DEFINED NB_ANTENNAS_RX)
  set(NB_ANTENNAS_RX 2 CACHE HELPSTRING "Number of antennas in reception")
endif(DEFINED NB_ANTENNAS_RX)
set_property(CACHE NB_ANTENNAS_RX PROPERTY STRINGS 1 2 4)
add_definitions("-DNB_ANTENNAS_RX=${NB_ANTENNAS_RX}")

if(DEFINED NB_ANTENNAS_TX)
  set(NB_ANTENNAS_TX ${NB_ANTENNAS_TX} CACHE HELPSTRING "Number of antennas in transmission")
else(DEFINED NB_ANTENNAS_TX)
  set(NB_ANTENNAS_TX 2 CACHE HELPSTRING "Number of antennas in transmission")
endif(DEFINED NB_ANTENNAS_TX)
set_property(CACHE NB_ANTENNAS_TX PROPERTY STRINGS 1 2 4)
add_definitions("-DNB_ANTENNAS_TX=${NB_ANTENNAS_TX}")

if(DEFINED NB_ANTENNAS_TXRX)
  set(NB_ANTENNAS_TXRX ${NB_ANTENNAS_TXRX} CACHE HELPSTRING "Number of antennas in ????")
else(DEFINED NB_ANTENNAS_TXRX)
  set(NB_ANTENNAS_TXRX 2 CACHE HELPSTRING "Number of antennas in ????")
endif(DEFINED NB_ANTENNAS_TXRX)
set_property(CACHE NB_ANTENNAS_TXRX PROPERTY STRINGS 1 2 4)
add_definitions("-DNB_ANTENNAS_TXRX=${NB_ANTENNAS_TXRX}")

if(DEFINED RF_BOARD)
  set(RF_BOARD ${RF_BOARD} CACHE HELPSTRING "RF head type")
else(DEFINED RF_BOARD)
  set(RF_BOARD EXMIMO CACHE HELPSTRING "RF head type")
endif(DEFINED RF_BOARD)
set_property(CACHE RF_BOARD PROPERTY STRINGS None EXMIMO USRP ETHERNET)
add_definitions("-D${RF_BOARD}=1")

if (${RF_BOARD} STREQUAL EXMIMO)
  set(LOWLATENCY_KERNEL True)
  set(ENABLE_VCD_FIFO True)
  set(DRIVER2013)
  include_directories ("${OPENAIR_TARGETS}/ARCH/EXMIMO/USERSPACE/LIB/")
  include_directories ("${OPENAIR_TARGETS}/ARCH/EXMIMO/DEFS/")
  set(HW_SOURCE ${HW_SOURCE} 
    ${OPENAIR_TARGETS}/ARCH/EXMIMO/USERSPACE/LIB/openair0_lib.c 
    ${OPENAIR_TARGETS}/ARCH/EXMIMO/USERSPACE/LIB/gain_control.c)   
elseif (${RF_BOARD} STREQUAL USRP)
  include_directories("/opt/include/uhd")
  LINK_DIRECTORIES("/opt/lib")
  set(option_USRP "uhd")
elseif (${RF_BOARD} STREQUAL ETHERNET)
  include_directories ("${OPENAIR_TARGETS}/ARCH/ETHERNET/USERSPACE/LIB")
endif (${RF_BOARD} STREQUAL EXMIMO)
 
include_directories ("${OPENAIR_TARGETS}/ARCH/COMMON")

 # add the binary tree to the search path for include files
#######################################################
# We will find ConfigOAI.h after generation in target directory
include_directories("${OPENAIR_BIN_DIR}")
# add directories to find all include files
# the internal rule is to use generic names such as defs.h 
# but to make it uniq name as adding the relative path in the include directtive
# example: #include "RRC/LITE/defs.h"
#find_path (include_dirs_all *.h ${OPENAIR_DIR})
#find_path (include_dirs_all *.h PATHS /usr/include NO_CMAKE_PATH)
#include_directories("${include_dirs_all}")

# Legacy exact order
include_directories("${OPENAIR1_DIR}")
include_directories("${OPENAIR2_DIR}/NAS")
include_directories("${OPENAIR2_DIR}")
include_directories("${OPENAIR2_DIR}/COMMON")
include_directories("${OPENAIR2_DIR}/LAYER2/RLC")
include_directories("${OPENAIR2_DIR}/LAYER2/RLC/AM_v9.3.0")
include_directories("${OPENAIR2_DIR}/LAYER2/RLC/UM_v9.3.0")
include_directories("${OPENAIR2_DIR}/LAYER2/RLC/TM_v9.3.0")
include_directories("${OPENAIR2_DIR}/LAYER2/PDCP_v10.1.0")
include_directories("${OPENAIR2_DIR}/RRC/LITE/MESSAGES")
include_directories("${OPENAIR2_DIR}/RRC/LITE")
include_directories("${OPENAIR3_DIR}/RAL-LTE/INTERFACE-802.21/INCLUDE")
include_directories("${OPENAIR3_DIR}/RAL-LTE/LTE_RAL_ENB/INCLUDE")
include_directories("${OPENAIR3_DIR}/RAL-LTE/LTE_RAL_UE/INCLUDE")
include_directories("${OPENAIR_DIR}/common/utils")
include_directories("${OPENAIR_DIR}/common/utils/collection/hashtable")
include_directories("${OPENAIR_DIR}/common/utils/itti")
include_directories("${OPENAIRCN_DIR}/NAS/EURECOM-NAS/src/api/network")
include_directories("${OPENAIRCN_DIR}/NAS/EURECOM-NAS/src/include")
include_directories("${OPENAIRCN_DIR}/NAS/EURECOM-NAS/src/ies")
include_directories("${OPENAIRCN_DIR}/NAS/EURECOM-NAS/src/emm/msg")
include_directories("${OPENAIRCN_DIR}/NAS/EURECOM-NAS/src/esm/msg")
include_directories("${OPENAIRCN_DIR}/NAS/EURECOM-NAS/src/util")
include_directories("${OPENAIRCN_DIR}/SECU")
include_directories("${OPENAIRCN_DIR}/SCTP")
include_directories("${OPENAIR2_DIR}/X2AP")
include_directories("${OPENAIRCN_DIR}/S1AP")
include_directories("${OPENAIRCN_DIR}/UDP")
include_directories("${OPENAIRCN_DIR}/GTPV1-U")
include_directories("${OPENAIR_DIR}/targets/COMMON")
include_directories("${OPENAIR_DIR}/targets/ARCH/COMMON")
include_directories("${OPENAIR_DIR}/targets/ARCH/EXMIMO/USERSPACE/LIB/")
include_directories("${OPENAIR_DIR}/targets/ARCH/EXMIMO/DEFS")
include_directories("${OPENAIR2_DIR}/ENB_APP")
include_directories("${OPENAIR2_DIR}/UTIL")
include_directories("${OPENAIR2_DIR}/UTIL/OSA")
include_directories("${OPENAIR2_DIR}/UTIL/LFDS/liblfds6.1.1/liblfds611/inc")
include_directories("${OPENAIR2_DIR}/UTIL/MEM")
include_directories("${OPENAIR2_DIR}/UTIL/LISTS")
include_directories("${OPENAIR2_DIR}/UTIL/FIFO")
include_directories("${OPENAIR2_DIR}/UTIL/OCG")
include_directories("${OPENAIR2_DIR}/UTIL/LOG")
include_directories("${OPENAIR2_DIR}/UTIL/MATH")
include_directories("${OPENAIR2_DIR}/UTIL/TIMER")
include_directories("${OPENAIR2_DIR}/UTIL/OMG")
include_directories("${OPENAIR2_DIR}/UTIL/OTG")
include_directories("${OPENAIR2_DIR}/UTIL/CLI")
include_directories("${OPENAIR2_DIR}/UTIL/OPT")
include_directories("${OPENAIR2_DIR}/UTIL/OMV")
include_directories("${OPENAIR2_DIR}/RRC/LITE/MESSAGES")
include_directories("${OPENAIRCN_DIR}/COMMON")
include_directories("${OPENAIRCN_DIR}/UTILS")
include_directories("${OPENAIRCN_DIR}/NAS")
include_directories("${OPENAIRCN_DIR}/GTPV1-U/nw-gtpv1u/shared")
include_directories("${OPENAIRCN_DIR}/GTPV1-U/nw-gtpv1u/include")

# trial for modernisation of include directories to be done
#include_directories("${ITTI_DIR}")
#include_directories("${OPENAIR_DIR}/common/utils/collection/hashtable")
#include_directories("${OPENAIR1_DIR}")
#include_directories("${OPENAIR1_DIR}/PHY")
#include_directories("${OPENAIR2_DIR}/")
#include_directories("${OPENAIR2_DIR}/ENB_APP")
#include_directories("${OPENAIR2_DIR}/COMMON")
#include_directories("${OPENAIR2_DIR}/UTIL/LOG")
#include_directories("${OPENAIR2_DIR}/UTIL/LISTS")
#include_directories("${OPENAIR2_DIR}/UTIL/MEM")
#include_directories("${OPENAIR2_DIR}/LAYER2/RLC")
#include_directories("${OPENAIR2_DIR}/LAYER2/RLC/AM_v9.3.0")
#include_directories("${OPENAIR2_DIR}/LAYER2/RLC/TM_v9.3.0")
#include_directories("${OPENAIR2_DIR}/LAYER2/RLC/UM_v9.3.0")
#include_directories("${OPENAIR3_DIR}")
#include_directories("${OPENAIRCN_DIR}")
 
# define where are the source files
##################################
#add_subdirectory (${OPENAIR_DIR} .) 

# Generated code need to declare explicit dependancy
###############################################

set (ITTI_H ${ITTI_DIR}/intertask_interface_types.h)
add_custom_command (
  OUTPUT ${OPENAIR_BIN_DIR}/messages.xml
  COMMAND gccxml -fxml=${OPENAIR_BIN_DIR}/messages.xml ${ITTI_H} ${module_compiler_options}
  #COMMAND rm ${ITTI_H}.tmp
  )

add_custom_command (
  OUTPUT ${OPENAIR_BIN_DIR}/messages_xml.h
  COMMAND sed -e 's/ *//'   -e 's/\"/\\\\\"/g' -e 's/^/\"/' -e 's/$$/\\\\n\"/' ${OPENAIR_BIN_DIR}/messages.xml  > ${OPENAIR_BIN_DIR}/messages_xml.h
  DEPENDS ${OPENAIR_BIN_DIR}/messages.xml
  )

# define libraries
#add_library(MathFunctions mysqrt.cxx)
function(add_whole_dir dir lib_name)
  file(GLOB mylist ${dir})
  add_library(${lib_name} ${mylist})
  set (EXTRA_LIBS ${EXTRA_LIBS} ${lib_name} PARENT_SCOPE)
endfunction(add_whole_dir)

function(add_whole_tree dir lib_name)
  file(GLOB_RECURSE mylist "${dir}/*.c")
  add_library(${lib_name} ${mylist})
  set (EXTRA_LIBS ${EXTRA_LIBS} ${lib_name} PARENT_SCOPE)
endfunction(add_whole_tree)

# Utilities Library
################
set(UTIL_SRC 
  ${OPENAIR_DIR}/common/utils/collection/hashtable/hashtable.c
  ${OPENAIR_DIR}/common/utils/collection/hashtable/obj_hashtable.c
  ${OPENAIR2_DIR}/UTIL/CLI/cli.c
  ${OPENAIR2_DIR}/UTIL/CLI/cli_cmd.c
  ${OPENAIR2_DIR}/UTIL/CLI/cli_server.c
  ${OPENAIR2_DIR}/UTIL/FIFO/pad_list.c
  ${OPENAIR2_DIR}/UTIL/LISTS/list.c
  ${OPENAIR2_DIR}/UTIL/LISTS/list2.c
  ${OPENAIR2_DIR}/UTIL/LOG/log.c
  ${OPENAIR2_DIR}/UTIL/LOG/vcd_signal_dumper.c
  ${OPENAIR2_DIR}/UTIL/MATH/oml.c
  ${OPENAIR2_DIR}/UTIL/MEM/mem_block.c
  ${OPENAIR2_DIR}/UTIL/OCG/OCG.c
  ${OPENAIR2_DIR}/UTIL/OCG/OCG_create_dir.c
  ${OPENAIR2_DIR}/UTIL/OCG/OCG_detect_file.c
  ${OPENAIR2_DIR}/UTIL/OCG/OCG_generate_report.c
  ${OPENAIR2_DIR}/UTIL/OCG/OCG_parse_filename.c
  ${OPENAIR2_DIR}/UTIL/OCG/OCG_parse_XML.c
  ${OPENAIR2_DIR}/UTIL/OCG/OCG_save_XML.c
  ${OPENAIR2_DIR}/UTIL/OMG/client_traci_OMG.c
  ${OPENAIR2_DIR}/UTIL/OMG/common.c
  ${OPENAIR2_DIR}/UTIL/OMG/grid.c
  ${OPENAIR2_DIR}/UTIL/OMG/id_manager.c
  ${OPENAIR2_DIR}/UTIL/OMG/job.c
  ${OPENAIR2_DIR}/UTIL/OMG/mobility_parser.c
  ${OPENAIR2_DIR}/UTIL/OMG/omg.c
  ${OPENAIR2_DIR}/UTIL/OMG/omg_hashtable.c
  ${OPENAIR2_DIR}/UTIL/OMG/rwalk.c
  ${OPENAIR2_DIR}/UTIL/OMG/rwp.c
  ${OPENAIR2_DIR}/UTIL/OMG/socket_traci_OMG.c
  ${OPENAIR2_DIR}/UTIL/OMG/static.c
  ${OPENAIR2_DIR}/UTIL/OMG/steadystaterwp.c
  ${OPENAIR2_DIR}/UTIL/OMG/storage_traci_OMG.c
  ${OPENAIR2_DIR}/UTIL/OMG/trace.c
  ${OPENAIR2_DIR}/UTIL/OMG/trace_hashtable.c
  ${OPENAIR2_DIR}/UTIL/OPT/probe.c
  ${OPENAIR2_DIR}/UTIL/OTG/otg_tx.c
  ${OPENAIR2_DIR}/UTIL/OTG/otg.c
  ${OPENAIR2_DIR}/UTIL/OTG/otg_rx.c
  ${OPENAIR2_DIR}/UTIL/OTG/otg_kpi.c
  ${OPENAIR2_DIR}/UTIL/OTG/otg_form.c
  ${OPENAIR2_DIR}/UTIL/OTG/otg_models.c
  )
add_library(UTIL ${UTIL_SRC})

set(SECU_OSA_SRC
  ${OPENAIR2_DIR}/UTIL/OSA/osa_key_deriver.c
  ${OPENAIR2_DIR}/UTIL/OSA/osa_rijndael.c
  ${OPENAIR2_DIR}/UTIL/OSA/osa_snow3g.c
  ${OPENAIR2_DIR}/UTIL/OSA/osa_stream_eea.c
  ${OPENAIR2_DIR}/UTIL/OSA/osa_stream_eia.c
  )
add_library(SECU_OSA ${SECU_OSA_SRC})

set(SECU_CN_SRC
  ${OPENAIRCN_DIR}/SECU/kdf.c
  ${OPENAIRCN_DIR}/SECU/rijndael.c
  ${OPENAIRCN_DIR}/SECU/snow3g.c
  ${OPENAIRCN_DIR}/SECU/key_nas_deriver.c
  ${OPENAIRCN_DIR}/SECU/nas_stream_eea1.c
  ${OPENAIRCN_DIR}/SECU/nas_stream_eia1.c
  ${OPENAIRCN_DIR}/SECU/nas_stream_eea2.c
  ${OPENAIRCN_DIR}/SECU/nas_stream_eia2.c
  )
add_library(SECU_CN ${SECU_CN_SRC})

# Scheduler
################################"
add_whole_dir(${OPENAIR1_DIR}/SCHED/*.c sched)

# Layer 1
#############################
set(PHY_SRC 
  # depend on code generation from asn1c
  ${RRC_FULL_DIR}/asn1_constants.h
  # actual source
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/pss.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/sss.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/pilots.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/pilots_mbsfn.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/dlsch_coding.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/dlsch_modulation.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/dlsch_demodulation.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/dlsch_llr_computation.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/power_control.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/dlsch_decoding.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/dlsch_scrambling.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/dci_tools.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/uci_tools.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/lte_mcs.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/pbch.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/dci.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/phich.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/pcfich.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/pucch.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/prach.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/pmch.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/group_hopping.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/srs_modulation.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/drs_modulation.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/ulsch_modulation.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/ulsch_demodulation.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/ulsch_coding.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/ulsch_decoding.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/rar_tools.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/print_stats.c
  ${OPENAIR1_DIR}/PHY/LTE_TRANSPORT/initial_sync.c
  ${OPENAIR1_DIR}/PHY/MODULATION/ofdm_mod.c
  ${OPENAIR1_DIR}/PHY/MODULATION/slot_fep.c
  ${OPENAIR1_DIR}/PHY/MODULATION/slot_fep_mbsfn.c
  ${OPENAIR1_DIR}/PHY/MODULATION/slot_fep_ul.c
  ${OPENAIR1_DIR}/PHY/MODULATION/ul_7_5_kHz.c
  ${OPENAIR1_DIR}/PHY/LTE_ESTIMATION/freq_equalization.c
  ${OPENAIR1_DIR}/PHY/LTE_ESTIMATION/lte_sync_time.c
  ${OPENAIR1_DIR}/PHY/LTE_ESTIMATION/lte_sync_timefreq.c
  ${OPENAIR1_DIR}/PHY/LTE_ESTIMATION/lte_adjust_sync.c
  ${OPENAIR1_DIR}/PHY/LTE_ESTIMATION/lte_dl_channel_estimation.c
  ${OPENAIR1_DIR}/PHY/LTE_ESTIMATION/lte_dl_mbsfn_channel_estimation.c
  ${OPENAIR1_DIR}/PHY/LTE_ESTIMATION/lte_ul_channel_estimation.c
  ${OPENAIR1_DIR}/PHY/LTE_ESTIMATION/lte_est_freq_offset.c
  ${OPENAIR1_DIR}/PHY/LTE_ESTIMATION/lte_ue_measurements.c
  ${OPENAIR1_DIR}/PHY/LTE_ESTIMATION/lte_eNB_measurements.c
  ${OPENAIR1_DIR}/PHY/LTE_ESTIMATION/adjust_gain.c
  ${OPENAIR1_DIR}/PHY/LTE_REFSIG/lte_dl_cell_spec.c
  ${OPENAIR1_DIR}/PHY/LTE_REFSIG/lte_dl_uespec.c
  ${OPENAIR1_DIR}/PHY/LTE_REFSIG/lte_gold.c
  ${OPENAIR1_DIR}/PHY/LTE_REFSIG/lte_gold_mbsfn.c
  ${OPENAIR1_DIR}/PHY/LTE_REFSIG/lte_dl_mbsfn.c
  ${OPENAIR1_DIR}/PHY/LTE_REFSIG/lte_ul_ref.c
  ${OPENAIR1_DIR}/PHY/CODING/lte_segmentation.c
  ${OPENAIR1_DIR}/PHY/CODING/ccoding_byte.c
  ${OPENAIR1_DIR}/PHY/CODING/ccoding_byte_lte.c
  ${OPENAIR1_DIR}/PHY/CODING/3gpplte_sse.c
  ${OPENAIR1_DIR}/PHY/CODING/crc_byte.c
  ${OPENAIR1_DIR}/PHY/CODING/3gpplte_turbo_decoder_sse_8bit.c
  ${OPENAIR1_DIR}/PHY/CODING/3gpplte_turbo_decoder_sse_16bit.c
  ${OPENAIR1_DIR}/PHY/CODING/lte_rate_matching.c
  ${OPENAIR1_DIR}/PHY/CODING/rate_matching.c
  ${OPENAIR1_DIR}/PHY/CODING/viterbi.c
  ${OPENAIR1_DIR}/PHY/CODING/viterbi_lte.c
  ${OPENAIR1_DIR}/PHY/INIT/lte_init.c
  ${OPENAIR1_DIR}/PHY/INIT/lte_parms.c
  ${OPENAIR1_DIR}/PHY/INIT/init_top.c
  ${OPENAIR1_DIR}/PHY/TOOLS/file_output.c
  ${OPENAIR1_DIR}/PHY/TOOLS/fft.c
  ${OPENAIR1_DIR}/PHY/TOOLS/lte_dfts.c
  ${OPENAIR1_DIR}/PHY/TOOLS/log2_approx.c
  ${OPENAIR1_DIR}/PHY/TOOLS/cmult_sv.c
  ${OPENAIR1_DIR}/PHY/TOOLS/cmult_vv.c
  ${OPENAIR1_DIR}/PHY/TOOLS/cadd_vv.c
  ${OPENAIR1_DIR}/PHY/TOOLS/cdot_prod.c
  ${OPENAIR1_DIR}/PHY/TOOLS/signal_energy.c
  ${OPENAIR1_DIR}/PHY/TOOLS/dB_routines.c
  ${OPENAIR1_DIR}/PHY/TOOLS/sqrt.c
  ${OPENAIR1_DIR}/PHY/TOOLS/time_meas.c
  ${OPENAIR1_DIR}/PHY/TOOLS/lut.c
  )
if (${SMBV})
  set(PHY_SRC "${PHY_SRC} ${OPENAIR1_DIR}/PHY/TOOLS/smbv.c")
endif  (${SMBV})
add_library(PHY ${PHY_SRC})

#Layer 2 library
#####################
set(MAC_DIR ${OPENAIR2_DIR}/LAYER2/MAC)
set(RLC_DIR ${OPENAIR2_DIR}/LAYER2/RLC)
set(RLC_UM_DIR ${OPENAIR2_DIR}/LAYER2/RLC/UM_v9.3.0)
set(RLC_AM_DIR ${OPENAIR2_DIR}/LAYER2/RLC/AM_v9.3.0)
set(RLC_TM_DIR ${OPENAIR2_DIR}/LAYER2/RLC/TM_v9.3.0)
set(RRC_DIR ${OPENAIR2_DIR}/RRC/LITE)
set(PDCP_DIR  ${OPENAIR2_DIR}/LAYER2/PDCP_v10.1.0)
set(L2_SRC
  ${OPENAIR2_DIR}/LAYER2/openair2_proc.c
  ${PDCP_DIR}/pdcp.c
  ${PDCP_DIR}/pdcp_fifo.c
  ${PDCP_DIR}/pdcp_sequence_manager.c
  ${PDCP_DIR}/pdcp_primitives.c
  ${PDCP_DIR}/pdcp_util.c
  ${PDCP_DIR}/pdcp_security.c
  ${PDCP_DIR}/pdcp_netlink.c
  ${RLC_AM_DIR}/rlc_am.c
  ${RLC_AM_DIR}/rlc_am_init.c
  ${RLC_AM_DIR}/rlc_am_timer_poll_retransmit.c
  ${RLC_AM_DIR}/rlc_am_timer_reordering.c
  ${RLC_AM_DIR}/rlc_am_timer_status_prohibit.c
  ${RLC_AM_DIR}/rlc_am_segment.c
  ${RLC_AM_DIR}/rlc_am_segments_holes.c
  ${RLC_AM_DIR}/rlc_am_in_sdu.c
  ${RLC_AM_DIR}/rlc_am_receiver.c
  ${RLC_AM_DIR}/rlc_am_retransmit.c
  ${RLC_AM_DIR}/rlc_am_windows.c
  ${RLC_AM_DIR}/rlc_am_rx_list.c
  ${RLC_AM_DIR}/rlc_am_reassembly.c
  ${RLC_AM_DIR}/rlc_am_status_report.c
  ${RLC_TM_DIR}/rlc_tm.c
  ${RLC_TM_DIR}/rlc_tm_init.c
  ${RLC_UM_DIR}/rlc_um.c
  ${RLC_UM_DIR}/rlc_um_fsm.c
  ${RLC_UM_DIR}/rlc_um_control_primitives.c
  ${RLC_UM_DIR}/rlc_um_segment.c
  ${RLC_UM_DIR}/rlc_um_reassembly.c
  ${RLC_UM_DIR}/rlc_um_receiver.c
  ${RLC_UM_DIR}/rlc_um_dar.c
  ${RLC_DIR}/rlc_mac.c
  ${RLC_DIR}/rlc.c
  ${RLC_DIR}/rlc_rrc.c
  ${RLC_DIR}/rlc_mpls.c
  ${RRC_DIR}/rrc_UE.c
  ${RRC_DIR}/rrc_eNB.c
  ${RRC_DIR}/rrc_eNB_S1AP.c
  ${RRC_DIR}/rrc_common.c
  ${RRC_DIR}/L2_interface.c
  )
set (MAC_SRC
  ${MAC_DIR}/lte_transport_init.c
  ${MAC_DIR}/main.c
  ${MAC_DIR}/ue_procedures.c
  ${MAC_DIR}/ra_procedures.c
  ${MAC_DIR}/l1_helpers.c
  ${MAC_DIR}/rar_tools.c
  ${MAC_DIR}/eNB_scheduler.c
  ${MAC_DIR}/eNB_scheduler_dlsch.c
  ${MAC_DIR}/eNB_scheduler_ulsch.c
  ${MAC_DIR}/eNB_scheduler_mch.c
  ${MAC_DIR}/eNB_scheduler_bch.c
  ${MAC_DIR}/eNB_scheduler_primitives.c
  ${MAC_DIR}/eNB_scheduler_RA.c
  ${MAC_DIR}/pre_processor.c
  ${MAC_DIR}/config.c
  )

set (ENB_APP_SRC
  ${OPENAIR2_DIR}/ENB_APP/enb_app.c
  ${OPENAIR2_DIR}/ENB_APP/enb_config.c
  )

if (${ENABLE_RAL})
  set(L2_SRC ${L2_SRC}
    ${RRC_DIR}/rrc_UE_ral.c
    ${RRC_DIR}/rrc_eNB_ral.c)
endif(${ENABLE_RAL})

if (NOT ${USE_MME} STREQUAL "False")
  set(L2_SRC ${L2_SRC}
    ${RRC_DIR}/rrc_eNB_GTPV1U.c)
endif(NOT ${USE_MME} STREQUAL "False")

add_library(L2 
  ${L2_SRC} 
  ${MAC_SRC} 
  ${ENB_APP_SRC}
  ${OPENAIR2_DIR}/RRC/L2_INTERFACE/openair_rrc_L2_interface.c)

# L3 Libs
##########################

set(RAL_LTE_DIR ${OPENAIR3_DIR}/RAL-LTE/)
set(RAL_LTE_SRC
  ${RAL_LTE_DIR}INTERFACE-802.21/C/MIH_C_header_codec.c
  ${RAL_LTE_DIR}INTERFACE-802.21/C/MIH_C_msg_codec.c
  ${RAL_LTE_DIR}INTERFACE-802.21/C/MIH_C_primitive_codec.c
  ${RAL_LTE_DIR}INTERFACE-802.21/C/MIH_C_F1_basic_data_types_codec.c
  ${RAL_LTE_DIR}INTERFACE-802.21/C/MIH_C_F2_general_data_types_codec.c
  ${RAL_LTE_DIR}INTERFACE-802.21/C/MIH_C_F3_data_types_for_address_codec.c
  ${RAL_LTE_DIR}INTERFACE-802.21/C/MIH_C_F4_data_types_for_links_codec.c
  ${RAL_LTE_DIR}INTERFACE-802.21/C/MIH_C_F9_data_types_for_qos_codec.c
  ${RAL_LTE_DIR}INTERFACE-802.21/C/MIH_C_F13_data_types_for_information_elements_codec.c
  ${RAL_LTE_DIR}INTERFACE-802.21/C/MIH_C_L2_type_values_for_tlv_encoding.c
  ${RAL_LTE_DIR}INTERFACE-802.21/C/MIH_C_Medieval_extensions.c
  ${RAL_LTE_DIR}INTERFACE-802.21/C/MIH_C_bit_buffer.c
  ${RAL_LTE_DIR}INTERFACE-802.21/C/MIH_C.c
  ${RAL_LTE_DIR}LTE_RAL_ENB/SRC/lteRALenb_action.c
  ${RAL_LTE_DIR}LTE_RAL_ENB/SRC/lteRALenb_main.c
  ${RAL_LTE_DIR}LTE_RAL_ENB/SRC/lteRALenb_mih_msg.c
  ${RAL_LTE_DIR}LTE_RAL_ENB/SRC/lteRALenb_parameters.c
  ${RAL_LTE_DIR}LTE_RAL_ENB/SRC/lteRALenb_process.c
  ${RAL_LTE_DIR}LTE_RAL_ENB/SRC/lteRALenb_rrc_msg.c
  ${RAL_LTE_DIR}LTE_RAL_ENB/SRC/lteRALenb_subscribe.c
  ${RAL_LTE_DIR}LTE_RAL_ENB/SRC/lteRALenb_thresholds.c
  ${RAL_LTE_DIR}LTE_RAL_UE/SRC/lteRALue_action.c
  ${RAL_LTE_DIR}LTE_RAL_UE/SRC/lteRALue_main.c
  ${RAL_LTE_DIR}LTE_RAL_UE/SRC/lteRALue_mih_msg.c
  ${RAL_LTE_DIR}LTE_RAL_UE/SRC/lteRALue_parameters.c
  ${RAL_LTE_DIR}LTE_RAL_UE/SRC/lteRALue_process.c
  ${RAL_LTE_DIR}LTE_RAL_UE/SRC/lteRALue_rrc_msg.c
  ${RAL_LTE_DIR}LTE_RAL_UE/SRC/lteRALue_subscribe.c
  ${RAL_LTE_DIR}LTE_RAL_UE/SRC/lteRALue_thresholds.c
  )
add_library(RAL_LTE ${RAL_LTE_SRC})

# CN libs
##########################
set(GTPV1U_SRC ${OPENAIRCN_DIR}/GTPV1-U)
set (libgtpv1u_SRC
  ${GTPV1U_SRC}/nw-gtpv1u/src/NwGtpv1uTunnelEndPoint.c
  ${GTPV1U_SRC}/nw-gtpv1u/src/NwGtpv1uTrxn.c
  ${GTPV1U_SRC}/nw-gtpv1u/src/NwGtpv1uMsg.c
  ${GTPV1U_SRC}/nw-gtpv1u/src/NwGtpv1u.c
  ${GTPV1U_SRC}/gtpv1u_eNB.c
  ${GTPV1U_SRC}/gtpv1u_teid_pool.c
)
add_library(GTPV1U ${libgtpv1u_SRC})

set(SCTP_SRC
  ${OPENAIRCN_DIR}/SCTP/sctp_common.c
  ${OPENAIRCN_DIR}/SCTP/sctp_eNB_task.c
  ${OPENAIRCN_DIR}/SCTP/sctp_eNB_itti_messaging.c
)
add_library(SCTP ${SCTP_SRC})

add_library(UDP ${OPENAIRCN_DIR}/UDP/udp_eNB_task.c)

set(NAS_SRC ${OPENAIRCN_DIR}/NAS/EURECOM-NAS/src/)

set(libnas_api_OBJS
  ${NAS_SRC}api/mme/mme_api.c
  ${NAS_SRC}api/network/nas_message.c
  )

set(libnas_emm_OBJS
  ${NAS_SRC}emm/Attach.c
  ${NAS_SRC}emm/Authentication.c
  ${NAS_SRC}emm/Detach.c
  ${NAS_SRC}emm/EmmCommon.c
  ${NAS_SRC}emm/emm_data_ctx.c
  ${NAS_SRC}emm/emm_main.c
  ${NAS_SRC}emm/EmmStatusHdl.c
  ${NAS_SRC}emm/Identification.c
  ${NAS_SRC}emm/IdleMode.c
  ${NAS_SRC}emm/LowerLayer.c
  ${NAS_SRC}emm/SecurityModeControl.c
  ${NAS_SRC}emm/ServiceRequestHdl.c
  ${NAS_SRC}emm/TrackingAreaUpdate.c
  )
  
set(libnas_emm_msg_OBJS
  ${NAS_SRC}emm/msg/AttachAccept.c
  ${NAS_SRC}emm/msg/AuthenticationFailure.c
  ${NAS_SRC}emm/msg/CsServiceNotification.c
  ${NAS_SRC}emm/msg/ExtendedServiceRequest.c
  ${NAS_SRC}emm/msg/IdentityResponse.c
  ${NAS_SRC}emm/msg/EmmInformation.c
  ${NAS_SRC}emm/msg/ServiceReject.c
  ${NAS_SRC}emm/msg/TrackingAreaUpdateReject.c
  ${NAS_SRC}emm/msg/AttachComplete.c
  ${NAS_SRC}emm/msg/AuthenticationReject.c
  ${NAS_SRC}emm/msg/DetachAccept.c
  ${NAS_SRC}emm/msg/GutiReallocationCommand.c
  ${NAS_SRC}emm/msg/emm_msg.c
  ${NAS_SRC}emm/msg/SecurityModeCommand.c
  ${NAS_SRC}emm/msg/ServiceRequest.c
  ${NAS_SRC}emm/msg/TrackingAreaUpdateRequest.c
  ${NAS_SRC}emm/msg/AttachReject.c
  ${NAS_SRC}emm/msg/AuthenticationRequest.c
  ${NAS_SRC}emm/msg/DetachRequest.c
  ${NAS_SRC}emm/msg/GutiReallocationComplete.c
  ${NAS_SRC}emm/msg/SecurityModeComplete.c
  ${NAS_SRC}emm/msg/TrackingAreaUpdateAccept.c
  ${NAS_SRC}emm/msg/UplinkNasTransport.c
  ${NAS_SRC}emm/msg/AttachRequest.c
  ${NAS_SRC}emm/msg/AuthenticationResponse.c
  ${NAS_SRC}emm/msg/DownlinkNasTransport.c
  ${NAS_SRC}emm/msg/EmmStatus.c
  ${NAS_SRC}emm/msg/IdentityRequest.c
  ${NAS_SRC}emm/msg/SecurityModeReject.c
  ${NAS_SRC}emm/msg/TrackingAreaUpdateComplete.c
)

set(libnas_emm_sap_OBJS
  ${NAS_SRC}emm/sap/emm_as.c
  ${NAS_SRC}emm/sap/emm_cn.c
  ${NAS_SRC}emm/sap/EmmDeregisteredNoImsi.c
  ${NAS_SRC}emm/sap/emm_reg.c
  ${NAS_SRC}emm/sap/EmmRegisteredUpdateNeeded.c
  ${NAS_SRC}emm/sap/EmmDeregisteredNormalService.c
  ${NAS_SRC}emm/sap/emm_sap.c
  ${NAS_SRC}emm/sap/EmmDeregisteredPlmnSearch.c
  ${NAS_SRC}emm/sap/emm_esm.c
  ${NAS_SRC}emm/sap/EmmRegisteredAttemptingToUpdate.c
  ${NAS_SRC}emm/sap/emm_send.c
  ${NAS_SRC}emm/sap/EmmCommonProcedureInitiated.c
  ${NAS_SRC}emm/sap/EmmRegistered.c
  ${NAS_SRC}emm/sap/EmmDeregisteredAttachNeeded.c
  ${NAS_SRC}emm/sap/EmmRegisteredImsiDetachInitiated.c
  ${NAS_SRC}emm/sap/EmmServiceRequestInitiated.c
  ${NAS_SRC}emm/sap/EmmDeregisteredAttemptingToAttach.c
  ${NAS_SRC}emm/sap/emm_fsm.c
  ${NAS_SRC}emm/sap/EmmRegisteredInitiated.c
  ${NAS_SRC}emm/sap/EmmTrackingAreaUpdatingInitiated.c
  ${NAS_SRC}emm/sap/EmmDeregistered.c
  ${NAS_SRC}emm/sap/EmmRegisteredLimitedService.c
  ${NAS_SRC}emm/sap/EmmDeregisteredInitiated.c
  ${NAS_SRC}emm/sap/EmmNull.c
  ${NAS_SRC}emm/sap/EmmRegisteredNoCellAvailable.c
  ${NAS_SRC}emm/sap/EmmDeregisteredLimitedService.c
  ${NAS_SRC}emm/sap/emm_recv.c
  ${NAS_SRC}emm/sap/EmmRegisteredNormalService.c
  ${NAS_SRC}emm/sap/EmmDeregisteredNoCellAvailable.c
  ${NAS_SRC}emm/sap/EmmRegisteredPlmnSearch.c
)

set (libnas_esm_OBJS
  ${NAS_SRC}esm/esm_ebr_context.c
  ${NAS_SRC}esm/PdnConnectivity.c
  ${NAS_SRC}esm/esm_ebr.c
  ${NAS_SRC}esm/EpsBearerContextDeactivation.c
  ${NAS_SRC}esm/DefaultEpsBearerContextActivation.c
  ${NAS_SRC}esm/DedicatedEpsBearerContextActivation.c
  ${NAS_SRC}esm/PdnDisconnect.c
  ${NAS_SRC}esm/EsmStatusHdl.c
  ${NAS_SRC}esm/esm_main.c
  ${NAS_SRC}esm/esm_pt.c
  ${NAS_SRC}esm/esm_ip.c
)

set(libnas_esm_msg_OBJS
  ${NAS_SRC}esm/msg/ModifyEpsBearerContextRequest.c
  ${NAS_SRC}esm/msg/DeactivateEpsBearerContextRequest.c
  ${NAS_SRC}esm/msg/BearerResourceAllocationReject.c
  ${NAS_SRC}esm/msg/BearerResourceModificationRequest.c
  ${NAS_SRC}esm/msg/ActivateDedicatedEpsBearerContextRequest.c
  ${NAS_SRC}esm/msg/EsmInformationResponse.c
  ${NAS_SRC}esm/msg/PdnDisconnectReject.c
  ${NAS_SRC}esm/msg/ActivateDedicatedEpsBearerContextAccept.c
  ${NAS_SRC}esm/msg/PdnDisconnectRequest.c
  ${NAS_SRC}esm/msg/BearerResourceModificationReject.c
  ${NAS_SRC}esm/msg/ActivateDefaultEpsBearerContextAccept.c
  ${NAS_SRC}esm/msg/ActivateDedicatedEpsBearerContextReject.c
  ${NAS_SRC}esm/msg/ModifyEpsBearerContextAccept.c
  ${NAS_SRC}esm/msg/DeactivateEpsBearerContextAccept.c
  ${NAS_SRC}esm/msg/esm_msg.c
  ${NAS_SRC}esm/msg/EsmInformationRequest.c
  ${NAS_SRC}esm/msg/ActivateDefaultEpsBearerContextRequest.c
  ${NAS_SRC}esm/msg/ModifyEpsBearerContextReject.c
  ${NAS_SRC}esm/msg/EsmStatus.c
  ${NAS_SRC}esm/msg/PdnConnectivityReject.c
  ${NAS_SRC}esm/msg/ActivateDefaultEpsBearerContextReject.c
  ${NAS_SRC}esm/msg/PdnConnectivityRequest.c
  ${NAS_SRC}esm/msg/BearerResourceAllocationRequest.c
)
  
set(libnas_esm_sap_OBJS
  ${NAS_SRC}esm/sap/esm_recv.c
  ${NAS_SRC}esm/sap/esm_send.c
  ${NAS_SRC}esm/sap/esm_sap.c
)

set(libnas_ies_OBJS
  ${NAS_SRC}ies/UeSecurityCapability.c
  ${NAS_SRC}ies/SecurityHeaderType.c
  ${NAS_SRC}ies/RadioPriority.c
  ${NAS_SRC}ies/EpsAttachResult.c
  ${NAS_SRC}ies/ImeisvRequest.c
  ${NAS_SRC}ies/EmergencyNumberList.c
  ${NAS_SRC}ies/SupportedCodecList.c
  ${NAS_SRC}ies/EsmCause.c
  ${NAS_SRC}ies/NetworkName.c
  ${NAS_SRC}ies/AuthenticationResponseParameter.c
  ${NAS_SRC}ies/Cli.c
  ${NAS_SRC}ies/AccessPointName.c
  ${NAS_SRC}ies/DaylightSavingTime.c
  ${NAS_SRC}ies/LcsIndicator.c
  ${NAS_SRC}ies/PagingIdentity.c
  ${NAS_SRC}ies/EpsMobileIdentity.c
  ${NAS_SRC}ies/GutiType.c
  ${NAS_SRC}ies/NasKeySetIdentifier.c
  ${NAS_SRC}ies/CsfbResponse.c
  ${NAS_SRC}ies/PacketFlowIdentifier.c
  ${NAS_SRC}ies/ApnAggregateMaximumBitRate.c
  ${NAS_SRC}ies/EpsBearerContextStatus.c
  ${NAS_SRC}ies/MobileIdentity.c
  ${NAS_SRC}ies/AuthenticationFailureParameter.c
  ${NAS_SRC}ies/EpsUpdateResult.c
  ${NAS_SRC}ies/TimeZone.c
  ${NAS_SRC}ies/MessageType.c
  ${NAS_SRC}ies/EmmCause.c
  ${NAS_SRC}ies/MobileStationClassmark2.c
  ${NAS_SRC}ies/AuthenticationParameterRand.c
  ${NAS_SRC}ies/PdnAddress.c
  ${NAS_SRC}ies/MobileStationClassmark3.c
  ${NAS_SRC}ies/ServiceType.c
  ${NAS_SRC}ies/NasRequestType.c
  ${NAS_SRC}ies/PlmnList.c
  ${NAS_SRC}ies/EpsQualityOfService.c
  ${NAS_SRC}ies/NasMessageContainer.c
  ${NAS_SRC}ies/EsmInformationTransferFlag.c
  ${NAS_SRC}ies/TrafficFlowTemplate.c
  ${NAS_SRC}ies/LinkedEpsBearerIdentity.c
  ${NAS_SRC}ies/NasSecurityAlgorithms.c
  ${NAS_SRC}ies/LlcServiceAccessPointIdentifier.c
  ${NAS_SRC}ies/ShortMac.c
  ${NAS_SRC}ies/TrafficFlowAggregateDescription.c
  ${NAS_SRC}ies/PTmsiSignature.c
  ${NAS_SRC}ies/ProtocolDiscriminator.c
  ${NAS_SRC}ies/EpsBearerIdentity.c
  ${NAS_SRC}ies/TimeZoneAndTime.c
  ${NAS_SRC}ies/TrackingAreaIdentityList.c
  ${NAS_SRC}ies/QualityOfService.c
  ${NAS_SRC}ies/EpsUpdateType.c
  ${NAS_SRC}ies/PdnType.c
  ${NAS_SRC}ies/UeRadioCapabilityInformationUpdateNeeded.c
  ${NAS_SRC}ies/AuthenticationParameterAutn.c
  ${NAS_SRC}ies/EpsAttachType.c
  ${NAS_SRC}ies/IdentityType2.c
  ${NAS_SRC}ies/LcsClientIdentity.c
  ${NAS_SRC}ies/DetachType.c
  ${NAS_SRC}ies/LocationAreaIdentification.c
  ${NAS_SRC}ies/TrackingAreaIdentity.c
  ${NAS_SRC}ies/CipheringKeySequenceNumber.c
  ${NAS_SRC}ies/MsNetworkCapability.c
  ${NAS_SRC}ies/UeNetworkCapability.c
  ${NAS_SRC}ies/ProtocolConfigurationOptions.c
  ${NAS_SRC}ies/AdditionalUpdateType.c
  ${NAS_SRC}ies/TmsiStatus.c
  ${NAS_SRC}ies/EsmMessageContainer.c
  ${NAS_SRC}ies/Nonce.c
  ${NAS_SRC}ies/ProcedureTransactionIdentity.c
  ${NAS_SRC}ies/TransactionIdentifier.c
  ${NAS_SRC}ies/AdditionalUpdateResult.c
  ${NAS_SRC}ies/GprsTimer.c
  ${NAS_SRC}ies/EpsNetworkFeatureSupport.c
  ${NAS_SRC}ies/SsCode.c
  ${NAS_SRC}ies/DrxParameter.c
  ${NAS_SRC}ies/KsiAndSequenceNumber.c
  ${NAS_SRC}ies/VoiceDomainPreferenceAndUeUsageSetting.c
)

set (libnas_utils_OBJS
  ${NAS_SRC}util/device.c
  ${NAS_SRC}util/memory.c
  ${NAS_SRC}util/nas_log.c
  ${NAS_SRC}util/nas_timer.c
  ${NAS_SRC}util/socket.c
  ${NAS_SRC}util/stty.c
  ${NAS_SRC}util/TLVEncoder.c
  ${NAS_SRC}util/TLVDecoder.c
  ${NAS_SRC}util/OctetString.c
)

set (libnas_user_OBJS
  ${NAS_SRC}api/user/user_api.c
  ${NAS_SRC}api/user/user_indication.c
  ${NAS_SRC}api/user/at_command.c
  ${NAS_SRC}api/user/at_error.c
  ${NAS_SRC}api/user/at_response.c
)
    
set(libnas_usim_OBJS
  ${NAS_SRC}api/usim/aka_functions.c
  ${NAS_SRC}api/usim/usim_api.c
)

set(libnas_OBJS
  ${OPENAIRCN_DIR}/NAS/nas_ue_task.c
  ${OPENAIRCN_DIR}/NAS/nas_itti_messaging.c
  ${NAS_SRC}nas_parser.c
  ${NAS_SRC}nas_proc.c
  ${NAS_SRC}nas_user.c
  ${libnas_utils_OBJS}
  ${libnas_user_OBJS}
  ${libnas_usim_OBJS}
  ${libnas_emm_OBJS}
  ${libnas_emm_msg_OBJS}
  ${libnas_emm_sap_OBJS}
  ${libnas_esm_OBJS}
  ${libnas_esm_msg_OBJS}
  ${libnas_esm_sap_OBJS}
  ${libnas_ies_OBJS}
  ${libnas_api_OBJS}
  )
add_library(NAS ${libnas_OBJS})
include_directories(${NAS_SRC})
include_directories(${NAS_SRC}/api/user)
include_directories(${NAS_SRC}/esm)
include_directories(${NAS_SRC}/esm/sap)
include_directories(${NAS_SRC}/emm)
include_directories(${NAS_SRC}/emm/sap)
include_directories(${NAS_SRC}/api/usim)
include_directories(${NAS_SRC}/api/mme)

# Make lfds as a own source code (even if it is a outside library)
# For better intergration with compilation flags & structure of cmake
###################################################################
set(lfds ${OPENAIR2_DIR}/UTIL/LFDS/liblfds6.1.1/liblfds611/src/)
file(GLOB lfds_queue ${lfds}/lfds611_queue/*.c)
file(GLOB lfds_ring ${lfds}/lfds611_ringbuffer/*.c)
file(GLOB lfds_slist ${lfds}/lfds611_slist/*.c)
file(GLOB lfds_stack ${lfds}/lfds611_stack/*.c/)
file(GLOB lfds_freelist ${lfds}/lfds611_freelist/*.c)

include_directories(${lfds})
add_library(LFDS 
  ${lfds_queue} ${lfds_ring} ${lfds_slist} ${lfds_stack} ${lfds_freelist}
  ${lfds}/lfds611_liblfds/lfds611_liblfds_abstraction_test_helpers.c
  ${lfds}/lfds611_liblfds/lfds611_liblfds_aligned_free.c
  ${lfds}/lfds611_liblfds/lfds611_liblfds_aligned_malloc.c
  ${lfds}/lfds611_abstraction/lfds611_abstraction_free.c
  ${lfds}/lfds611_abstraction/lfds611_abstraction_malloc.c
)

# Simulation library
##########################
add_library(SIMU
${OPENAIR1_DIR}/SIMULATION/TOOLS/random_channel.c
${OPENAIR1_DIR}/SIMULATION/TOOLS/rangen_double.c
${OPENAIR1_DIR}/SIMULATION/TOOLS/taus.c
${OPENAIR1_DIR}/SIMULATION/TOOLS/multipath_channel.c
${OPENAIR1_DIR}/SIMULATION/TOOLS/abstraction.c
${OPENAIR1_DIR}/SIMULATION/TOOLS/multipath_tv_channel.c
${OPENAIR1_DIR}/SIMULATION/RF/rf.c
${OPENAIR1_DIR}/USERSPACE_TOOLS/SCOPE/lte_scope.c
)

# System packages that are required
# the modules definition for cmake, in ubuntu are in: /usr/share/cmake*/Modules/
###################################
find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

if (${RF_BOARD} STREQUAL "USRP")
  find_package(Boost REQUIRED)
  include_directories(${LIBBOOST_INCLUDE_DIR})
  target_link_libraries (lte-softmodem ${LIBBOOST_LIBRARIES})
endif (${RF_BOARD} STREQUAL "USRP")

# add executables

add_executable(lte-softmodem 
${rrc_h}
${s1ap_h}
${OPENAIR_BIN_DIR}/messages_xml.h
${OPENAIR_TARGETS}/RT/USER/sched_dlsch.c
${OPENAIR_TARGETS}/RT/USER/sched_rx_pdsch.c
${OPENAIR_TARGETS}/RT/USER/rt_wrapper.c
${OPENAIR_TARGETS}/RT/USER/stats.c
${OPENAIR_TARGETS}/RT/USER/lte-ue.c
${OPENAIR_TARGETS}/RT/USER/lte-softmodem.c
${OPENAIR1_DIR}/SIMULATION/TOOLS/taus.c
${OPENAIR_TARGETS}/SIMU/USER/init_lte.c
${OPENAIR_TARGETS}/COMMON//create_tasks.c
${OPENAIR1_DIR}/SIMULATION/ETH_TRANSPORT/netlink_init.c
${HW_SOURCE}
${RTAI_SOURCE}
${XFORMS_SOURCE}
)

target_link_libraries (lte-softmodem 
  -Wl,--start-group 
  rrc_asn1 s1ap_asn1 GTPV1U SECU_CN SECU_OSA UTIL SCTP UDP sched PHY LFDS L2 RAL_LTE NAS ${option_ITTI}  
  -Wl,--end-group )
target_link_libraries (lte-softmodem ${LIBXML2_LIBRARIES})
target_link_libraries (lte-softmodem pthread m config rt crypt crypto ssl nettle sctp ${option_USRP} ${option_lib_xforms} )

foreach(myExe dlsim ulsim pbchsim scansim mbmssim pdcchsim pucchsim prachsim syncsim)
add_executable(${myExe}
  ${OPENAIR1_DIR}/SIMULATION/LTE_PHY/${myExe}.c
  ${XFORMS_SOURCE} 
  )
target_link_libraries (${myExe}
  -Wl,--start-group SIMU UTIL sched PHY LFDS  ${option_ITTI} -Wl,--end-group 
  pthread m config rt blas ${option_lib_xforms}
)
endforeach(myExe)

# Set compiler options for kernel modules
# we need to get out cmake to use the regular Linux Kernel process
# this is documented as https://www.kernel.org/doc/Documentation/kbuild/modules.txt
######################################

# Common to all modules
########################
get_directory_property( DirDefs COMPILE_DEFINITIONS )
foreach( d ${DirDefs} )
  if(NOT ${d} STREQUAL "USER_MODE")
    set(module_compile_options "${module_compile_options} -D${d}")
  endif()
endforeach()
get_directory_property( DirDefs INCLUDE_DIRECTORIES )
foreach( d ${DirDefs} )
    set(module_compile_options "${module_compile_options} -I${d}")
endforeach()

EXECUTE_PROCESS(COMMAND uname -r
                OUTPUT_VARIABLE os_release
                OUTPUT_STRIP_TRAILING_WHITESPACE)
SET(module_build_path /lib/modules/${os_release}/build)
message("Kernel build path = ${module_build_path}")

function(make_driver name dir)
  file(MAKE_DIRECTORY ${OPENAIR_BIN_DIR}/${name})
  foreach(f  IN  ITEMS ${ARGN})
    list(APPEND src_path_list ${dir}/${f})
    string(REGEX REPLACE "c *$" "o" obj ${f})
    set(objs "${objs} ${obj}")
  endforeach()
  CONFIGURE_FILE(${OPENAIR_CMAKE}/Kbuild.cmake ${OPENAIR_BIN_DIR}/${name}/Kbuild)
  add_custom_command(OUTPUT ${name}.ko
    COMMAND make -C ${module_build_path} M=${OPENAIR_BIN_DIR}/${name}
    WORKING_DIRECTORY ${OPENAIR_BIN_DIR}/${name}
    COMMENT "building ${module}.ko"
    VERBATIM
    SOURCES  ${src_path_list}
    )
  add_custom_target(${name} DEPENDS ${name}.ko)
endfunction(make_driver name dir src)

# nashmesh module 
################
list(APPEND nasmesh_src device.c common.c ioctl.c classifier.c tool.c mesh.c)
set(module_compile_options "${module_compile_options} -DNAS_NETLINK")
make_driver(nasmesh  ${OPENAIR2_DIR}/NAS/DRIVER/MESH ${nasmesh_src})

# Next module
####################


# add the install targets
#install (TARGETS Tutorial DESTINATION bin)
#install (FILES "${PROJECT_BIN_DIR}/TutorialConfig.h"        DESTINATION include)
