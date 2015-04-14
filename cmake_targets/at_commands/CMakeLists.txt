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



INCLUDE(FindNettle)
IF( NOT NETTLE_FOUND )
  MESSAGE( SEND_ERROR "Nettle is required" )
ENDIF( NOT NETTLE_FOUND )


set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

set (FIRMWARE_VERSION "No svn information")
add_definitions("-DFIRMWARE_VERSION=\"${FIRMWARE_VERSION}\"")

add_boolean_option(LINUX   True  "????")

##########################
# NAS LAYER OPTIONS
##########################
add_boolean_option(ENABLE_NAS_UE_LOGGING   True  "????")
add_boolean_option(NAS_BUILT_IN_EPC        False "MME NAS layer not present in this executable")
add_boolean_option(NAS_BUILT_IN_UE         False  "UE NAS layer present in this executable")
add_boolean_option(NAS_UE                  True  "NAS UE INSTANCE (<> NAS_MME)")
add_boolean_option(NAS_MME                 False "NAS_UE and NAS_MME are incompatible options")


################################################################################
# UTIL LIB
################################################################################
set(util_SRC
    ${OPENAIR_NAS_DIR}/src/util/nas_log.c
    ${OPENAIR_NAS_DIR}/src/util/parser.c
)
set(util_HDR
    ${OPENAIR_NAS_DIR}/src/util/nas_log.h
    ${OPENAIR_NAS_DIR}/src/util/parser.h
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
                          ${OPENAIR_NAS_DIR}/src/nas_user.c  
                          )
#                           ${OPENAIR_NAS_DIR}/src/nas_proc.c  
                          
target_link_libraries (at_nas_ue 
                       -Wl,-whole-archive 
                       util api_user api_network
                       -Wl,-no-whole-archive
                       ${NETTLE_LIBRARIES} 
                       ${CMAKE_THREAD_LIBS_INIT} )


                



