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
# Author: Lionel GAUTHIER
###############################################################################
cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE ON)


project(at_nas_ue)


ENABLE_LANGUAGE(C)
#Sends the -std=c99 flag to the gcc compiler
#add_definitions(-std=c99)

ADD_DEFINITIONS(-D_GNU_SOURCE)

set (OPENAIR_DIR     $ENV{OPENAIR_DIR})

if( NOT DEFINED OPENAIR_DIR)
  message(SEND_ERROR "OPENAIR_DIR not set: ${OPENAIR_DIR}")
endif( NOT DEFINED OPENAIR_DIR)

set(OPENAIR1_DIR          ${OPENAIR_DIR}/openair1)
set(OPENAIR2_DIR          ${OPENAIR_DIR}/openair2)
set(OPENAIR3_DIR          ${OPENAIR_DIR}/openair3)
set(OPENAIRCN_DIR         ${OPENAIR_DIR}/openair-cn)
set(OPENAIR_TARGETS       ${OPENAIR_DIR}/targets)
set(OPENAIR_CMAKE_SCRIPTS ${OPENAIR_DIR}/cmake_targets/tools)
set(OPENAIR_BIN_DIR       ${OPENAIR_DIR}/targets/bin)
set(OPENAIR_NAS_DIR       ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS)


set(CMAKE_MODULE_PATH "${OPENAIR_DIR}/cmake_targets/tools/MODULES" "${CMAKE_MODULE_PATH}")

macro(add_option name val helpstr)
  if(DEFINED ${name})
    set(value ${${name}})
  else(DEFINED ${name})
    set(value ${val})
  endif()
  set(${name} ${value} CACHE STRING "${helpstr}")
  add_definitions("-D${name}=${value}")
endmacro(add_option)

macro(add_boolean_option name val helpstr)
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
endmacro(add_boolean_option)

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
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -ggdb -DMALLOC_CHECK_=3 -O2")


include(FindPkgConfig)

INCLUDE(FindNettle)
IF( NOT NETTLE_FOUND )
  MESSAGE( SEND_ERROR "Nettle is required" )
ENDIF( NOT NETTLE_FOUND )


pkg_search_module(OPENSSL openssl REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIRS})


pkg_search_module(CRYPTO libcrypto REQUIRED)
include_directories(${CRYPTO_INCLUDE_DIRS})


set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

set (FIRMWARE_VERSION "No svn information")
add_definitions("-DFIRMWARE_VERSION=\"${FIRMWARE_VERSION}\"")

add_boolean_option(LINUX   True  "????")
add_boolean_option(ENB_MODE   True  "????") # to avoid backtrace

##########################
# NAS LAYER OPTIONS
##########################
add_boolean_option(ENABLE_NAS_UE_LOGGING   True  "????")
add_boolean_option(NAS_BUILT_IN_EPC        False "MME NAS layer not present in this executable")
add_boolean_option(NAS_BUILT_IN_UE         False  "UE NAS layer present in this executable")
add_boolean_option(NAS_UE                  True  "NAS UE INSTANCE (<> NAS_MME)")
add_boolean_option(NAS_MME                 False "NAS_UE and NAS_MME are incompatible options")

################################################################################
# SECU LIB
################################################################################
set(secu_cn_SRC
  ${OPENAIRCN_DIR}/SECU/kdf.c
  ${OPENAIRCN_DIR}/SECU/rijndael.c
  ${OPENAIRCN_DIR}/SECU/snow3g.c
  ${OPENAIRCN_DIR}/SECU/key_nas_deriver.c
  ${OPENAIRCN_DIR}/SECU/nas_stream_eea1.c
  ${OPENAIRCN_DIR}/SECU/nas_stream_eia1.c
  ${OPENAIRCN_DIR}/SECU/nas_stream_eea2.c
  ${OPENAIRCN_DIR}/SECU/nas_stream_eia2.c
  )
add_library(secu_cn ${secu_cn_SRC})
target_include_directories(secu_cn PRIVATE 
                            ${OPENAIR_DIR}/common/utils/itti
                            ${OPENAIR_DIR}/openair2/COMMON
                            ${OPENAIRCN_DIR}/UTILS
)

################################################################################
# UTIL LIB
################################################################################
set(util_SRC
    ${OPENAIR_NAS_DIR}/src/util/device.c
    ${OPENAIR_NAS_DIR}/src/util/memory.c
    ${OPENAIR_NAS_DIR}/src/util/nas_log.c
    ${OPENAIR_NAS_DIR}/src/util/nas_timer.c
    ${OPENAIR_NAS_DIR}/src/util/OctetString.c
    ${OPENAIR_NAS_DIR}/src/util/parser.c
    ${OPENAIR_NAS_DIR}/src/util/TLVDecoder.c
    ${OPENAIR_NAS_DIR}/src/util/TLVEncoder.c
    ${OPENAIR_NAS_DIR}/src/util/socket.c
    ${OPENAIR_NAS_DIR}/src/util/stty.c
)
set(util_HDR
    ${OPENAIR_NAS_DIR}/src/util/device.h
    ${OPENAIR_NAS_DIR}/src/util/memory.h
    ${OPENAIR_NAS_DIR}/src/util/nas_log.h
    ${OPENAIR_NAS_DIR}/src/util/nas_timer.h
    ${OPENAIR_NAS_DIR}/src/util/OctetString.h
    ${OPENAIR_NAS_DIR}/src/util/parser.h
    ${OPENAIR_NAS_DIR}/src/util/TLVDecoder.h
    ${OPENAIR_NAS_DIR}/src/util/TLVEncoder.h
    ${OPENAIR_NAS_DIR}/src/util/socket.h
)

add_library(util ${util_SRC} ${util_HDR})
target_include_directories(util PRIVATE ${OPENAIR_NAS_DIR}/src/util)


################################################################################
# api_user LIB
################################################################################
set(api_user_SRC
    ${OPENAIR_NAS_DIR}/src/api/user/at_command.c
    ${OPENAIR_NAS_DIR}/src/api/user/at_error.c
    ${OPENAIR_NAS_DIR}/src/api/user/at_response.c
    ${OPENAIR_NAS_DIR}/src/api/user/user_api.c
    ${OPENAIR_NAS_DIR}/src/api/user/user_indication.c
)

set(api_user_HDR
    ${OPENAIR_NAS_DIR}/src/api/user/at_command.h
    ${OPENAIR_NAS_DIR}/src/api/user/at_error.h
    ${OPENAIR_NAS_DIR}/src/api/user/at_response.h
    ${OPENAIR_NAS_DIR}/src/api/user/user_api.h
    ${OPENAIR_NAS_DIR}/src/api/user/user_indication.h
)

add_library(api_user ${api_user_SRC} ${api_user_HDR})
target_include_directories(api_user PRIVATE ${OPENAIR_NAS_DIR}/src/api/user)

################################################################################
# api_usim LIB
################################################################################
set(api_usim_SRC
    ${OPENAIR_NAS_DIR}/src/api/usim/aka_functions.c
    ${OPENAIR_NAS_DIR}/src/api/usim/usim_api.c
)

set(api_usim_HDR
    ${OPENAIR_NAS_DIR}/src/api/usim/aka_functions.h
    ${OPENAIR_NAS_DIR}/src/api/usim/usim_api.h

)

add_library(api_usim ${api_usim_SRC} ${api_usim_HDR})
target_include_directories(api_usim PRIVATE ${OPENAIR_NAS_DIR}/src/api/usim)

################################################################################
# api_network LIB
################################################################################
set(api_network_SRC
    ${OPENAIR_NAS_DIR}/src/api/network/as_message.c
    ${OPENAIR_NAS_DIR}/src/api/network/nas_message.c
    ${OPENAIR_NAS_DIR}/src/api/network/network_api.c
)
set(api_network_HDR
    ${OPENAIR_NAS_DIR}/src/api/network/as_message.h
    ${OPENAIR_NAS_DIR}/src/api/network/nas_message.h
    ${OPENAIR_NAS_DIR}/src/api/network/network_api.h
)

add_library(api_network ${api_network_SRC} ${api_network_HDR})
target_include_directories(api_network PRIVATE ${OPENAIR_NAS_DIR}/src/api/network)


################################################################################
# emm LIB
################################################################################
set(emm_SRC
    ${OPENAIR_NAS_DIR}/src/emm//Attach.c
    ${OPENAIR_NAS_DIR}/src/emm//Authentication.c
    ${OPENAIR_NAS_DIR}/src/emm//Detach.c
    ${OPENAIR_NAS_DIR}/src/emm//EmmCommon.c
    ${OPENAIR_NAS_DIR}/src/emm//emm_data_ctx.c
    ${OPENAIR_NAS_DIR}/src/emm//emm_main.c
    ${OPENAIR_NAS_DIR}/src/emm//EmmStatusHdl.c
    ${OPENAIR_NAS_DIR}/src/emm//Identification.c
    ${OPENAIR_NAS_DIR}/src/emm//IdleMode.c
    ${OPENAIR_NAS_DIR}/src/emm//LowerLayer.c
    ${OPENAIR_NAS_DIR}/src/emm//SecurityModeControl.c
    ${OPENAIR_NAS_DIR}/src/emm//ServiceRequestHdl.c
    ${OPENAIR_NAS_DIR}/src/emm//TrackingAreaUpdate.c

    ${OPENAIR_NAS_DIR}/src/emm/msg/emm_msg.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/AttachAccept.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/AttachComplete.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/AttachReject.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/AttachAccept.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/AttachComplete.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/AttachReject.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/AttachRequest.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/AuthenticationFailure.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/AuthenticationReject.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/AuthenticationRequest.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/AuthenticationResponse.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/CsServiceNotification.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/DetachAccept.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/DetachRequest.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/DownlinkNasTransport.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/EmmInformation.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/emm_msg.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/EmmStatus.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/ExtendedServiceRequest.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/GutiReallocationCommand.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/GutiReallocationComplete.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/IdentityRequest.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/IdentityResponse.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/SecurityModeCommand.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/SecurityModeComplete.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/SecurityModeReject.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/ServiceReject.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/ServiceRequest.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/TrackingAreaUpdateAccept.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/TrackingAreaUpdateComplete.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/TrackingAreaUpdateReject.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/TrackingAreaUpdateRequest.c
    ${OPENAIR_NAS_DIR}/src/emm/msg/UplinkNasTransport.c
    
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_as.c
#    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_cn.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmCommonProcedureInitiated.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmDeregisteredAttachNeeded.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmDeregisteredAttemptingToAttach.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmDeregistered.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmDeregisteredInitiated.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmDeregisteredLimitedService.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmDeregisteredNoCellAvailable.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmDeregisteredNoImsi.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmDeregisteredNormalService.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmDeregisteredPlmnSearch.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_esm.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_fsm.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmNull.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_recv.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_reg.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmRegisteredAttemptingToUpdate.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmRegistered.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmRegisteredImsiDetachInitiated.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmRegisteredInitiated.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmRegisteredLimitedService.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmRegisteredNoCellAvailable.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmRegisteredNormalService.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmRegisteredPlmnSearch.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmRegisteredUpdateNeeded.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_sap.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_send.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmServiceRequestInitiated.c
    ${OPENAIR_NAS_DIR}/src/emm/sap//EmmTrackingAreaUpdatingInitiated.c
    
)

set(emm_HDR
    ${OPENAIR_NAS_DIR}/src/emm//EmmCommon.h
    ${OPENAIR_NAS_DIR}/src/emm//emmData.h
    ${OPENAIR_NAS_DIR}/src/emm//emm_main.h
    ${OPENAIR_NAS_DIR}/src/emm//emm_proc.h
    ${OPENAIR_NAS_DIR}/src/emm//IdleMode.h
    ${OPENAIR_NAS_DIR}/src/emm//LowerLayer.h
    
    ${OPENAIR_NAS_DIR}/src/emm/msg/AttachAccept.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/AttachComplete.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/AttachReject.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/AttachRequest.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/AuthenticationFailure.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/AuthenticationReject.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/AuthenticationRequest.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/AuthenticationResponse.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/CsServiceNotification.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/DetachAccept.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/DetachRequest.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/DownlinkNasTransport.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/emm_cause.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/EmmInformation.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/emm_msgDef.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/emm_msg.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/EmmStatus.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/ExtendedServiceRequest.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/GutiReallocationCommand.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/GutiReallocationComplete.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/IdentityRequest.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/IdentityResponse.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/NASSecurityModeCommand.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/NASSecurityModeComplete.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/SecurityModeReject.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/ServiceReject.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/ServiceRequest.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/TrackingAreaUpdateAccept.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/TrackingAreaUpdateComplete.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/TrackingAreaUpdateReject.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/TrackingAreaUpdateRequest.h
    ${OPENAIR_NAS_DIR}/src/emm/msg/UplinkNasTransport.h
    
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_asDef.h
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_as.h
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_cnDef.h
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_cn.h
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_esmDef.h
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_esm.h
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_fsm.h
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_recv.h
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_regDef.h
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_reg.h
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_sap.h
    ${OPENAIR_NAS_DIR}/src/emm/sap//emm_send.h

)

add_library(emm ${emm_SRC} ${emm_HDR})

target_include_directories(emm PRIVATE 
                     ${OPENAIR_NAS_DIR}/src/api/usim 
                     ${OPENAIR_NAS_DIR}/src/api/mme 
                     ${OPENAIR_NAS_DIR}/src/emm
                     ${OPENAIR_NAS_DIR}/src/emm/msg
                     ${OPENAIR_NAS_DIR}/src/emm/sap
                     ${OPENAIR_NAS_DIR}/src/ies
)


################################################################################
# esm LIB
################################################################################
set(esm_SRC
    ${OPENAIR_NAS_DIR}/src/esm/esm_main.c
    ${OPENAIR_NAS_DIR}/src/esm/DedicatedEpsBearerContextActivation.c
    ${OPENAIR_NAS_DIR}/src/esm/DefaultEpsBearerContextActivation.c
    ${OPENAIR_NAS_DIR}/src/esm/EpsBearerContextDeactivation.c
    ${OPENAIR_NAS_DIR}/src/esm/esm_ebr.c
    ${OPENAIR_NAS_DIR}/src/esm/esm_ebr_context.c
    ${OPENAIR_NAS_DIR}/src/esm/esm_ip.c
    ${OPENAIR_NAS_DIR}/src/esm/esm_main.c
    ${OPENAIR_NAS_DIR}/src/esm/esm_pt.c
    ${OPENAIR_NAS_DIR}/src/esm/EsmStatusHdl.c
    ${OPENAIR_NAS_DIR}/src/esm/PdnConnectivity.c
    ${OPENAIR_NAS_DIR}/src/esm/PdnDisconnect.c
    
    ${OPENAIR_NAS_DIR}/src/esm/msg/ActivateDedicatedEpsBearerContextAccept.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/ActivateDedicatedEpsBearerContextReject.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/ActivateDedicatedEpsBearerContextRequest.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/ActivateDefaultEpsBearerContextAccept.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/ActivateDefaultEpsBearerContextReject.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/ActivateDefaultEpsBearerContextRequest.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/BearerResourceAllocationReject.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/BearerResourceAllocationRequest.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/BearerResourceModificationReject.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/BearerResourceModificationRequest.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/DeactivateEpsBearerContextAccept.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/DeactivateEpsBearerContextRequest.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/EsmInformationRequest.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/EsmInformationResponse.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/esm_msg.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/EsmStatus.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/ModifyEpsBearerContextAccept.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/ModifyEpsBearerContextReject.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/ModifyEpsBearerContextRequest.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/PdnConnectivityReject.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/PdnConnectivityRequest.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/PdnDisconnectReject.c
    ${OPENAIR_NAS_DIR}/src/esm/msg/PdnDisconnectRequest.c
    
    ${OPENAIR_NAS_DIR}/src/esm/sap/esm_recv.c
    ${OPENAIR_NAS_DIR}/src/esm/sap/esm_sap.c
    ${OPENAIR_NAS_DIR}/src/esm/sap/esm_send.c
)

set(esm_HDR
    ${OPENAIR_NAS_DIR}/src/esm/esm_main.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/ActivateDedicatedEpsBearerContextAccept.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/ActivateDedicatedEpsBearerContextReject.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/ActivateDedicatedEpsBearerContextRequest.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/ActivateDefaultEpsBearerContextAccept.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/ActivateDefaultEpsBearerContextReject.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/ActivateDefaultEpsBearerContextRequest.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/BearerResourceAllocationReject.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/BearerResourceAllocationRequest.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/BearerResourceModificationReject.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/BearerResourceModificationRequest.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/DeactivateEpsBearerContextAccept.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/DeactivateEpsBearerContextRequest.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/esm_cause.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/EsmInformationRequest.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/EsmInformationResponse.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/esm_msgDef.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/esm_msg.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/EsmStatus.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/ModifyEpsBearerContextAccept.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/ModifyEpsBearerContextReject.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/ModifyEpsBearerContextRequest.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/PdnConnectivityReject.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/PdnConnectivityRequest.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/PdnDisconnectReject.h
    ${OPENAIR_NAS_DIR}/src/esm/msg/PdnDisconnectRequest.h
    ${OPENAIR_NAS_DIR}/src/esm/sap/esm_recv.h
    ${OPENAIR_NAS_DIR}/src/esm/sap/esm_sap.h
    ${OPENAIR_NAS_DIR}/src/esm/sap/esm_send.h
)

add_library(esm ${esm_SRC} ${esm_HDR})

target_include_directories(esm PRIVATE 
                     ${OPENAIR_NAS_DIR}/src/api/user 
                     ${OPENAIR_NAS_DIR}/src/api/mme 
                     ${OPENAIR_NAS_DIR}/src/esm
                     ${OPENAIR_NAS_DIR}/src/esm/msg
                     ${OPENAIR_NAS_DIR}/src/esm/sap
                     ${OPENAIR_NAS_DIR}/src/ies
)

################################################################################
# ies LIB
################################################################################
set(ies_SRC
    ${OPENAIR_NAS_DIR}/src/ies/AccessPointName.c
    ${OPENAIR_NAS_DIR}/src/ies/AdditionalUpdateResult.c
    ${OPENAIR_NAS_DIR}/src/ies/AdditionalUpdateType.c
    ${OPENAIR_NAS_DIR}/src/ies/ApnAggregateMaximumBitRate.c
    ${OPENAIR_NAS_DIR}/src/ies/AuthenticationFailureParameter.c
    ${OPENAIR_NAS_DIR}/src/ies/AuthenticationParameterAutn.c
    ${OPENAIR_NAS_DIR}/src/ies/AuthenticationParameterRand.c
    ${OPENAIR_NAS_DIR}/src/ies/AuthenticationResponseParameter.c
    ${OPENAIR_NAS_DIR}/src/ies/CipheringKeySequenceNumber.c
    ${OPENAIR_NAS_DIR}/src/ies/Cli.c
    ${OPENAIR_NAS_DIR}/src/ies/CsfbResponse.c
    ${OPENAIR_NAS_DIR}/src/ies/DaylightSavingTime.c
    ${OPENAIR_NAS_DIR}/src/ies/DetachType.c
    ${OPENAIR_NAS_DIR}/src/ies/DrxParameter.c
    ${OPENAIR_NAS_DIR}/src/ies/EmergencyNumberList.c
    ${OPENAIR_NAS_DIR}/src/ies/EmmCause.c
    ${OPENAIR_NAS_DIR}/src/ies/EpsAttachResult.c
    ${OPENAIR_NAS_DIR}/src/ies/EpsAttachType.c
    ${OPENAIR_NAS_DIR}/src/ies/EpsBearerContextStatus.c
    ${OPENAIR_NAS_DIR}/src/ies/EpsBearerIdentity.c
    ${OPENAIR_NAS_DIR}/src/ies/EpsMobileIdentity.c
    ${OPENAIR_NAS_DIR}/src/ies/EpsNetworkFeatureSupport.c
    ${OPENAIR_NAS_DIR}/src/ies/EpsQualityOfService.c
    ${OPENAIR_NAS_DIR}/src/ies/EpsUpdateResult.c
    ${OPENAIR_NAS_DIR}/src/ies/EpsUpdateType.c
    ${OPENAIR_NAS_DIR}/src/ies/EsmCause.c
    ${OPENAIR_NAS_DIR}/src/ies/EsmInformationTransferFlag.c
    ${OPENAIR_NAS_DIR}/src/ies/EsmMessageContainer.c
    ${OPENAIR_NAS_DIR}/src/ies/GprsTimer.c
    ${OPENAIR_NAS_DIR}/src/ies/GutiType.c
    ${OPENAIR_NAS_DIR}/src/ies/IdentityType2.c
    ${OPENAIR_NAS_DIR}/src/ies/ImeisvRequest.c
    ${OPENAIR_NAS_DIR}/src/ies/KsiAndSequenceNumber.c
    ${OPENAIR_NAS_DIR}/src/ies/LcsClientIdentity.c
    ${OPENAIR_NAS_DIR}/src/ies/LcsIndicator.c
    ${OPENAIR_NAS_DIR}/src/ies/LinkedEpsBearerIdentity.c
    ${OPENAIR_NAS_DIR}/src/ies/LlcServiceAccessPointIdentifier.c
    ${OPENAIR_NAS_DIR}/src/ies/LocationAreaIdentification.c
    ${OPENAIR_NAS_DIR}/src/ies/MessageType.c
    ${OPENAIR_NAS_DIR}/src/ies/MobileIdentity.c
    ${OPENAIR_NAS_DIR}/src/ies/MobileStationClassmark2.c
    ${OPENAIR_NAS_DIR}/src/ies/MobileStationClassmark3.c
    ${OPENAIR_NAS_DIR}/src/ies/MsNetworkCapability.c
    ${OPENAIR_NAS_DIR}/src/ies/NasKeySetIdentifier.c
    ${OPENAIR_NAS_DIR}/src/ies/NasMessageContainer.c
    ${OPENAIR_NAS_DIR}/src/ies/NasRequestType.c
    ${OPENAIR_NAS_DIR}/src/ies/NasSecurityAlgorithms.c
    ${OPENAIR_NAS_DIR}/src/ies/NetworkName.c
    ${OPENAIR_NAS_DIR}/src/ies/Nonce.c
    ${OPENAIR_NAS_DIR}/src/ies/PacketFlowIdentifier.c
    ${OPENAIR_NAS_DIR}/src/ies/PagingIdentity.c
    ${OPENAIR_NAS_DIR}/src/ies/PdnAddress.c
    ${OPENAIR_NAS_DIR}/src/ies/PdnType.c
    ${OPENAIR_NAS_DIR}/src/ies/PlmnList.c
    ${OPENAIR_NAS_DIR}/src/ies/ProcedureTransactionIdentity.c
    ${OPENAIR_NAS_DIR}/src/ies/ProtocolConfigurationOptions.c
    ${OPENAIR_NAS_DIR}/src/ies/ProtocolDiscriminator.c
    ${OPENAIR_NAS_DIR}/src/ies/PTmsiSignature.c
    ${OPENAIR_NAS_DIR}/src/ies/QualityOfService.c
    ${OPENAIR_NAS_DIR}/src/ies/RadioPriority.c
    ${OPENAIR_NAS_DIR}/src/ies/SecurityHeaderType.c
    ${OPENAIR_NAS_DIR}/src/ies/ServiceType.c
    ${OPENAIR_NAS_DIR}/src/ies/ShortMac.c
    ${OPENAIR_NAS_DIR}/src/ies/SsCode.c
    ${OPENAIR_NAS_DIR}/src/ies/SupportedCodecList.c
    ${OPENAIR_NAS_DIR}/src/ies/TimeZoneAndTime.c
    ${OPENAIR_NAS_DIR}/src/ies/TimeZone.c
    ${OPENAIR_NAS_DIR}/src/ies/TmsiStatus.c
    ${OPENAIR_NAS_DIR}/src/ies/TrackingAreaIdentity.c
    ${OPENAIR_NAS_DIR}/src/ies/TrackingAreaIdentityList.c
    ${OPENAIR_NAS_DIR}/src/ies/TrafficFlowAggregateDescription.c
    ${OPENAIR_NAS_DIR}/src/ies/TrafficFlowTemplate.c
    ${OPENAIR_NAS_DIR}/src/ies/TransactionIdentifier.c
    ${OPENAIR_NAS_DIR}/src/ies/UeNetworkCapability.c
    ${OPENAIR_NAS_DIR}/src/ies/UeRadioCapabilityInformationUpdateNeeded.c
    ${OPENAIR_NAS_DIR}/src/ies/UeSecurityCapability.c
    ${OPENAIR_NAS_DIR}/src/ies/VoiceDomainPreferenceAndUeUsageSetting.c
)
set(ies_HDR
    ${OPENAIR_NAS_DIR}/src/ies/AccessPointName.h
    ${OPENAIR_NAS_DIR}/src/ies/AdditionalUpdateResult.h
    ${OPENAIR_NAS_DIR}/src/ies/AdditionalUpdateType.h
    ${OPENAIR_NAS_DIR}/src/ies/ApnAggregateMaximumBitRate.h
    ${OPENAIR_NAS_DIR}/src/ies/AuthenticationFailureParameter.h
    ${OPENAIR_NAS_DIR}/src/ies/AuthenticationParameterAutn.h
    ${OPENAIR_NAS_DIR}/src/ies/AuthenticationParameterRand.h
    ${OPENAIR_NAS_DIR}/src/ies/AuthenticationResponseParameter.h
    ${OPENAIR_NAS_DIR}/src/ies/CipheringKeySequenceNumber.h
    ${OPENAIR_NAS_DIR}/src/ies/Cli.h
    ${OPENAIR_NAS_DIR}/src/ies/CsfbResponse.h
    ${OPENAIR_NAS_DIR}/src/ies/DaylightSavingTime.h
    ${OPENAIR_NAS_DIR}/src/ies/DetachType.h
    ${OPENAIR_NAS_DIR}/src/ies/DrxParameter.h
    ${OPENAIR_NAS_DIR}/src/ies/EmergencyNumberList.h
    ${OPENAIR_NAS_DIR}/src/ies/EmmCause.h
    ${OPENAIR_NAS_DIR}/src/ies/EpsAttachResult.h
    ${OPENAIR_NAS_DIR}/src/ies/EpsAttachType.h
    ${OPENAIR_NAS_DIR}/src/ies/EpsBearerContextStatus.h
    ${OPENAIR_NAS_DIR}/src/ies/EpsBearerIdentity.h
    ${OPENAIR_NAS_DIR}/src/ies/EpsMobileIdentity.h
    ${OPENAIR_NAS_DIR}/src/ies/EpsNetworkFeatureSupport.h
    ${OPENAIR_NAS_DIR}/src/ies/EpsQualityOfService.h
    ${OPENAIR_NAS_DIR}/src/ies/EpsUpdateResult.h
    ${OPENAIR_NAS_DIR}/src/ies/EpsUpdateType.h
    ${OPENAIR_NAS_DIR}/src/ies/EsmCause.h
    ${OPENAIR_NAS_DIR}/src/ies/EsmInformationTransferFlag.h
    ${OPENAIR_NAS_DIR}/src/ies/EsmMessageContainer.h
    ${OPENAIR_NAS_DIR}/src/ies/GprsTimer.h
    ${OPENAIR_NAS_DIR}/src/ies/GutiType.h
    ${OPENAIR_NAS_DIR}/src/ies/IdentityType2.h
    ${OPENAIR_NAS_DIR}/src/ies/ImeisvRequest.h
    ${OPENAIR_NAS_DIR}/src/ies/KsiAndSequenceNumber.h
    ${OPENAIR_NAS_DIR}/src/ies/LcsClientIdentity.h
    ${OPENAIR_NAS_DIR}/src/ies/LcsIndicator.h
    ${OPENAIR_NAS_DIR}/src/ies/LinkedEpsBearerIdentity.h
    ${OPENAIR_NAS_DIR}/src/ies/LlcServiceAccessPointIdentifier.h
    ${OPENAIR_NAS_DIR}/src/ies/LocationAreaIdentification.h
    ${OPENAIR_NAS_DIR}/src/ies/MessageType.h
    ${OPENAIR_NAS_DIR}/src/ies/MobileIdentity.h
    ${OPENAIR_NAS_DIR}/src/ies/MobileStationClassmark2.h
    ${OPENAIR_NAS_DIR}/src/ies/MobileStationClassmark3.h
    ${OPENAIR_NAS_DIR}/src/ies/MsNetworkCapability.h
    ${OPENAIR_NAS_DIR}/src/ies/NasKeySetIdentifier.h
    ${OPENAIR_NAS_DIR}/src/ies/NasMessageContainer.h
    ${OPENAIR_NAS_DIR}/src/ies/NasRequestType.h
    ${OPENAIR_NAS_DIR}/src/ies/NasSecurityAlgorithms.h
    ${OPENAIR_NAS_DIR}/src/ies/NetworkName.h
    ${OPENAIR_NAS_DIR}/src/ies/Nonce.h
    ${OPENAIR_NAS_DIR}/src/ies/PacketFlowIdentifier.h
    ${OPENAIR_NAS_DIR}/src/ies/PagingIdentity.h
    ${OPENAIR_NAS_DIR}/src/ies/PdnAddress.h
    ${OPENAIR_NAS_DIR}/src/ies/PdnType.h
    ${OPENAIR_NAS_DIR}/src/ies/PlmnList.h
    ${OPENAIR_NAS_DIR}/src/ies/ProcedureTransactionIdentity.h
    ${OPENAIR_NAS_DIR}/src/ies/ProtocolConfigurationOptions.h
    ${OPENAIR_NAS_DIR}/src/ies/ProtocolDiscriminator.h
    ${OPENAIR_NAS_DIR}/src/ies/PTmsiSignature.h
    ${OPENAIR_NAS_DIR}/src/ies/QualityOfService.h
    ${OPENAIR_NAS_DIR}/src/ies/RadioPriority.h
    ${OPENAIR_NAS_DIR}/src/ies/SecurityHeaderType.h
    ${OPENAIR_NAS_DIR}/src/ies/ServiceType.h
    ${OPENAIR_NAS_DIR}/src/ies/ShortMac.h
    ${OPENAIR_NAS_DIR}/src/ies/SsCode.h
    ${OPENAIR_NAS_DIR}/src/ies/SupportedCodecList.h
    ${OPENAIR_NAS_DIR}/src/ies/TimeZoneAndTime.h
    ${OPENAIR_NAS_DIR}/src/ies/TimeZone.h
    ${OPENAIR_NAS_DIR}/src/ies/TmsiStatus.h
    ${OPENAIR_NAS_DIR}/src/ies/TrackingAreaIdentity.h
    ${OPENAIR_NAS_DIR}/src/ies/TrackingAreaIdentityList.h
    ${OPENAIR_NAS_DIR}/src/ies/TrafficFlowAggregateDescription.h
    ${OPENAIR_NAS_DIR}/src/ies/TrafficFlowTemplate.h
    ${OPENAIR_NAS_DIR}/src/ies/TransactionIdentifier.h
    ${OPENAIR_NAS_DIR}/src/ies/UeNetworkCapability.h
    ${OPENAIR_NAS_DIR}/src/ies/UeRadioCapabilityInformationUpdateNeeded.h
    ${OPENAIR_NAS_DIR}/src/ies/UeSecurityCapability.h
    ${OPENAIR_NAS_DIR}/src/ies/VoiceDomainPreferenceAndUeUsageSetting.h

)

add_library(ies ${ies_SRC} ${ies_HDR})

target_include_directories(ies PRIVATE 
                     ${OPENAIR_NAS_DIR}/src/ies 
)



################################################################################
# EXECUTABLE at_nas_ue
################################################################################
include_directories( 
                     ${OPENAIRCN_DIR}/COMMON 
                     ${OPENAIRCN_DIR}/SECU 
                     ${OPENAIR_NAS_DIR}/src 
                     ${OPENAIR_NAS_DIR}/src/api/network 
                     ${OPENAIR_NAS_DIR}/src/api/user 
                     ${OPENAIR_NAS_DIR}/src/emm 
                     ${OPENAIR_NAS_DIR}/src/emm/msg 
                     ${OPENAIR_NAS_DIR}/src/emm/sap 
                     ${OPENAIR_NAS_DIR}/src/esm 
                     ${OPENAIR_NAS_DIR}/src/esm/sap 
                     ${OPENAIR_NAS_DIR}/src/esm/msg 
                     ${OPENAIR_NAS_DIR}/src/ies 
                     ${OPENAIR_NAS_DIR}/src/include 
                     ${OPENAIR_NAS_DIR}/src/util )
                     
ADD_EXECUTABLE(at_nas_ue  ${OPENAIR_NAS_DIR}/src/UEprocess.c 
                          ${OPENAIR_NAS_DIR}/src/nas_network.c  
                          ${OPENAIR_NAS_DIR}/src/nas_parser.c
                          ${OPENAIR_NAS_DIR}/src/nas_proc.c  
                          ${OPENAIR_NAS_DIR}/src/nas_user.c  
                          )
                          
target_link_libraries (at_nas_ue 
                       -Wl,-whole-archive 
                       api_network api_user api_usim emm esm ies util secu_cn
                       -Wl,-no-whole-archive
                       ${NETTLE_LIBRARIES} 
                       ${OPENSSL_LIBRARIES} 
                       ${CMAKE_THREAD_LIBS_INIT} m )


                



