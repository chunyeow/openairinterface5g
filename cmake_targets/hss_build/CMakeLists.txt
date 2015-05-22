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


project(HSS)


ENABLE_LANGUAGE(C)
#Sends the -std=c99 flag to the gcc compiler
#add_definitions(-std=c99)

ADD_DEFINITIONS(-D_GNU_SOURCE)

if( NOT DEFINED OPENAIR_DIR)
  message(SEND_ERROR "OPENAIR_DIR not set: ${OPENAIR_DIR}")
endif( NOT DEFINED OPENAIR_DIR)

set(OPENAIR1_DIR          ${OPENAIR_DIR}/openair1)
set(OPENAIR2_DIR          ${OPENAIR_DIR}/openair2)
set(OPENAIR3_DIR          ${OPENAIR_DIR}/openair3)
set(OPENAIRCN_DIR         ${OPENAIR_DIR}/openair-cn)
set(OPENAIR_TARGETS       ${OPENAIR_DIR}/targets)
set(OPENAIR_CMAKE_SCRIPTS ${OPENAIR_DIR}/cmake_targets/tools)
set(OPENAIRHSS_DIR        ${OPENAIRCN_DIR}/OPENAIRHSS)
set(OPENAIR_BIN_DIR       ${OPENAIR_TARGETS}/bin)


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


# compilation flags
# added CMAKER conditional compilation to not bother legacy building system
#############################################
if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)

#
set(CMAKE_C_FLAGS                  "${CMAKE_C_FLAGS} -msse4.2 -std=gnu99 -Wall -Wstrict-prototypes -fno-strict-aliasing -rdynamic -funroll-loops -Wno-packed-bitfield-compat -DCMAKER")
set(CMAKE_C_FLAGS_DEBUG            "${CMAKE_C_FLAGS_DEBUG} -ggdb -DMALLOC_CHECK_=3")
set(CMAKE_C_FLAGS_RELWITHDEBINFO   "${CMAKE_C_FLAGS_RELWITHDEBINFO} -ggdb -DMALLOC_CHECK_=3 -O2")


INCLUDE(FindMySQL)
IF( NOT MySQL_FOUND )
  MESSAGE( SEND_ERROR "MySQL Client is required" )
ENDIF( NOT MySQL_FOUND )

INCLUDE(FindNettle)
IF( NOT NETTLE_FOUND )
  MESSAGE( SEND_ERROR "Nettle is required" )
ENDIF( NOT NETTLE_FOUND )



INCLUDE(FindFreeDiameter)
IF( NOT FREEDIAMETER_FOUND )
  MESSAGE( SEND_ERROR "FreeDiameter is required (use OAI installer)" )
ENDIF( NOT FREEDIAMETER_FOUND )
IF( NOT FREEDIAMETER_HSS_S6A_ENABLED )
  MESSAGE( SEND_ERROR "FreeDiameter is not enabled for OPENAIRHSS" )
ENDIF( NOT FREEDIAMETER_HSS_S6A_ENABLED )
add_definitions("-DFREEDIAMETER_VERSION=${FREEDIAMETER_VERSION}")

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

#find_package(Subversion)
#if(SUBVERSION_FOUND)
#  Subversion_WC_INFO(${OPENAIR_DIR} openair)
#  set (PACKAGE_VERSION "${openair_WC_REVISION} - ${openair_WC_LAST_CHANGED_DATE}")
#  Subversion_WC_LOG(${OPENAIR_DIR} openair)
#  message("Last changed log is ${openair_LAST_CHANGED_LOG}")
#  add_definitions("-DPACKAGE_VERSION=\"${PACKAGE_VERSION}\"")
#else()
set (PACKAGE_VERSION "No svn information")
#endif()

add_definitions("-DPACKAGE_NAME=\"OPENAIRHSS\"")


################################################################################
# ACCESS RESTRICTION LIB
################################################################################
set(ar_SRC
    ${OPENAIRHSS_DIR}/access_restriction/access_restriction.c
)
set(ar_HDR
    ${OPENAIRHSS_DIR}/access_restriction/access_restriction.h
    ${OPENAIRHSS_DIR}/utils/conversion.h
)

add_library(hss_access_restriction ${ar_SRC} ${ar_HDR})
target_include_directories(hss_access_restriction PRIVATE ${OPENAIRHSS_DIR}/utils)


################################################################################
# AUC LIB
################################################################################
set(auc_SRC
    ${OPENAIRHSS_DIR}/auc/fx.c
    ${OPENAIRHSS_DIR}/auc/kdf.c
    ${OPENAIRHSS_DIR}/auc/random.c
    ${OPENAIRHSS_DIR}/auc/rijndael.c
    ${OPENAIRHSS_DIR}/auc/sequence_number.c
)
set(auc_HDR
    ${OPENAIRHSS_DIR}/auc/auc.h
)

add_library(hss_auc ${auc_SRC} ${auc_HDR})


################################################################################
# DB LIB
################################################################################
set(db_SRC
    ${OPENAIRHSS_DIR}/db/db_connector.c
    ${OPENAIRHSS_DIR}/db/db_epc_equipment.c
    ${OPENAIRHSS_DIR}/db/db_subscription_data.c
)
set(db_HDR
    ${OPENAIRHSS_DIR}/db/db_proto.h
    ${OPENAIRHSS_DIR}/utils/hss_config.h
)

add_library(hss_db ${db_SRC} ${db_HDR})
target_include_directories(hss_db PRIVATE ${OPENAIRHSS_DIR}/utils)


################################################################################
# S6A LIB
################################################################################
set(s6a_SRC
    ${OPENAIRHSS_DIR}/s6a/s6a_auth_info.c
    ${OPENAIRHSS_DIR}/s6a/s6a_common.c
    ${OPENAIRHSS_DIR}/s6a/s6a_error.c
    ${OPENAIRHSS_DIR}/s6a/s6a_fd.c
    ${OPENAIRHSS_DIR}/s6a/s6a_in_addr.c
    ${OPENAIRHSS_DIR}/s6a/s6a_peers.c
    ${OPENAIRHSS_DIR}/s6a/s6a_purge_ue.c
    ${OPENAIRHSS_DIR}/s6a/s6a_subscription_data.c
    ${OPENAIRHSS_DIR}/s6a/s6a_supported_features.c
    ${OPENAIRHSS_DIR}/s6a/s6a_up_loc.c
)
set(s6a_HDR
    ${OPENAIRHSS_DIR}/s6a/s6a_proto.h
    ${OPENAIRHSS_DIR}/db/db_proto.h
    ${OPENAIRHSS_DIR}/utils/hss_config.h
    ${OPENAIRHSS_DIR}/auc/auc.h
    ${OPENAIRHSS_DIR}/access_restriction/access_restriction.h
)

add_library(hss_s6a ${s6a_SRC} ${s6a_HDR})
target_include_directories(hss_s6a PRIVATE ${OPENAIRHSS_DIR}/utils)
target_include_directories(hss_s6a PRIVATE ${OPENAIRHSS_DIR}/db)
target_include_directories(hss_s6a PRIVATE ${OPENAIRHSS_DIR}/auc)
target_include_directories(hss_s6a PRIVATE ${OPENAIRHSS_DIR}/access_restriction)
target_include_directories(hss_s6a PRIVATE ${FREEDIAMETER_INCLUDE_DIR})

################################################################################
# UTILS LIB
################################################################################
# Create target for the parser
 ADD_CUSTOM_TARGET(HSSParser echo "Creating hss_parser.c")

set(UTILS_GENERATED_DIR ${OPENAIR_BIN_DIR})

# Create custom command for flex/lex (note the outputs)
 ADD_CUSTOM_COMMAND(
   SOURCE ${OPENAIRHSS_DIR}/utils/hss_scanner.l  
   COMMAND ${FLEX_EXECUTABLE} 
   ARGS -o${UTILS_GENERATED_DIR}/hss_scanner.c ${OPENAIRHSS_DIR}/utils/hss_scanner.l
   TARGET HSSParser
   OUTPUTS ${UTILS_GENERATED_DIR}/hss_scanner.c)

# Create custom command for bison/yacc (note the DEPENDS)
 ADD_CUSTOM_COMMAND(
   SOURCE ${OPENAIRHSS_DIR}/utils/hss_parser.y
   COMMAND ${BISON_EXECUTABLE} 
   ARGS -y ${OPENAIRHSS_DIR}/utils/hss_parser.y
        -o ${UTILS_GENERATED_DIR}/hss_parser.c
   TARGET HSSParser
   DEPENDS ${UTILS_GENERATED_DIR}/hss_scanner.c
   OUTPUTS ${UTILS_GENERATED_DIR}/hss_parser.c)

set(utils_SRC
    ${UTILS_GENERATED_DIR}/hss_scanner.c
    ${UTILS_GENERATED_DIR}/hss_parser.c
    ${OPENAIRHSS_DIR}/utils/conversion.c
    ${OPENAIRHSS_DIR}/utils/hss_config.c
)
set(utils_HDR
    ${UTILS_GENERATED_DIR}/hss_parser.h
    ${UTILS_GENERATED_DIR}/hss_scanner.h
    ${OPENAIRHSS_DIR}/utils/conversion.h
    ${OPENAIRHSS_DIR}/utils/hss_config.h
    ${OPENAIRHSS_DIR}/utils/queue.h
)
# Since parser.c does not exists yet when cmake is run, mark
# it as generated
SET_SOURCE_FILES_PROPERTIES(${UTILS_GENERATED_DIR}/hss_parser.c GENERATED)
SET_SOURCE_FILES_PROPERTIES(${UTILS_GENERATED_DIR}/hss_scanner.c GENERATED)
SET_SOURCE_FILES_PROPERTIES(${UTILS_GENERATED_DIR}/hss_parser.h GENERATED)
SET_SOURCE_FILES_PROPERTIES(${UTILS_GENERATED_DIR}/hss_scanner.h GENERATED)
SET_SOURCE_FILES_PROPERTIES(${OPENAIRHSS_DIR}/utils/hss_config.c DEPENDS ${UTILS_GENERATED_DIR}/hss_parser.c)
add_library(hss_utils ${utils_SRC} ${utils_HDR})

################################################################################
# EXECUTABLE openair-hss
################################################################################
include_directories( ${UTILS_GENERATED_DIR} ${OPENAIRHSS_DIR}/utils ${OPENAIRHSS_DIR}/s6a ${OPENAIRHSS_DIR}/db ${OPENAIRHSS_DIR}/auc ${OPENAIRHSS_DIR}/access_restriction)
ADD_EXECUTABLE(openair-hss  ${OPENAIRHSS_DIR}/hss_main.c)
target_link_libraries (openair-hss 
                       -Wl,-whole-archive 
                       hss_access_restriction  
                       hss_auc
                       hss_db
                       hss_s6a 
                       hss_utils
                       -Wl,-no-whole-archive
                       ${MySQL_LIBRARY} 
                       ${NETTLE_LIBRARIES} 
                       ${FREEDIAMETER_LIBRARIES} 
                       ${CMAKE_THREAD_LIBS_INIT} )

# Default parameters
# Does not work on simple install (fqdn in /etc/hosts 127.0.1.1)
cmake_host_system_information(RESULT SITE_HSS_FQDN QUERY FQDN)
add_option(HSS_FQDN          ${SITE_HSS_FQDN} "HSS Fully qualified domain name")
IF( NOT DEFINED REALM )
  string(FIND ${HSS_FQDN} "." pos)
  math(EXPR pos1 "${pos}+1")
  string(SUBSTRING ${HSS_FQDN} ${pos1} -1 realm_string)
  message("REALM extracted from HSS FQDN is ${realm_string}")
  set(REALM             ${realm_string}    CACHE STRING  "HSS realm")
ENDIF( NOT DEFINED REALM )
set(MYSQL_server      "127.0.0.1"      CACHE STRING  "Database server IP address")
set(MYSQL_admin       root             CACHE STRING  "Database admin login")
set(MYSQL_admin_pass  linux            CACHE STRING  "Database admin password")
set(MYSQL_user        hssadmin         CACHE STRING  "Database username login")
set(MYSQL_pass        admin            CACHE STRING  "Database username password")
set(MYSQL_db          oai_db           CACHE STRING  "Database name")
set(TRANSPORT_option  "#No_TCP"        CACHE STRING  "No_TCP or No_SCTP or comment string, FreeDiameter config option")
set(TRANSPORT_PREFER_TCP_option  "#Prefer_TCP"   CACHE STRING  "Prefer_TCP or comment string, FreeDiameter config option")
set(AppServThreads    2                CACHE STRING  "FreeDiameter AppServThreads config option")
set(OPERATOR_key      "11111111111111111111111111111111" CACHE STRING  "LTE operator clear text key (hex bytes)")
set(OPERATOR_ckey      "HSS_CAN_DO_THE_JOB"   CACHE STRING  "LTE operator ciphered text key (hex bytes)")
set(REMOTE_PEER_WHITELIST "*.${REALM}" CACHE STRING  "Remote peer whitelist (separated by spaces), for freediameter acl.conf config file")
IF( FD_SERVER_IP_BIND_LIST )
    set(ListenOn          "ListenOn = \"${FD_SERVER_IP_BIND_LIST}\"" CACHE STRING  "FreeDiameter server IP bind addresses list")
ELSE( FD_SERVER_IP_BIND_LIST )
    set(ListenOn          "#ListenOn = \"\"" CACHE STRING  "FreeDiameter server IP bind addresses list")
ENDIF( FD_SERVER_IP_BIND_LIST )

IF( HSS_CONNECT_TO_MME )
  set(ConnectPeer        "ConnectPeer"     CACHE STRING  "HSS connect to MME")
  set(DIAMETER_PORT      "#Port = 3868"    CACHE STRING  "Diameter bind port")
  set(DIAMETER_SEC_PORT  "#SecPort = 3869" CACHE STRING  "Diameter bind sec port")
ELSE( HSS_CONNECT_TO_MME )
  set(ConnectPeer        "#ConnectPeer"    CACHE STRING  "HSS act as a server")
  set(DIAMETER_PORT      "Port = 3868"     CACHE STRING  "Diameter bind port")
  set(DIAMETER_SEC_PORT  "SecPort = 3869"  CACHE STRING  "Diameter bind sec port")
ENDIF( HSS_CONNECT_TO_MME )


configure_file(${CMAKE_CURRENT_SOURCE_DIR}/hss_fd.conf.in ${CMAKE_CURRENT_BINARY_DIR}/hss_fd.conf)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/hss.conf.in    ${CMAKE_CURRENT_BINARY_DIR}/hss.conf)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/acl.conf.in    ${CMAKE_CURRENT_BINARY_DIR}/acl.conf)

execute_process(COMMAND sudo ${OPENAIR_CMAKE_SCRIPTS}/check_hss_s6a_certificate ${FREEDIAMETER_PATH}/.. ${HSS_FQDN} WORKING_DIRECTORY ${OPENAIR_CMAKE_SCRIPTS} TIMEOUT 3)

# This does not overwrite database if it exist
execute_process(COMMAND ${OPENAIR_CMAKE_SCRIPTS}/create_hss_database
                ${MYSQL_admin} ${MYSQL_admin_pass} ${MYSQL_user} ${MYSQL_pass} ${MYSQL_db}
                 WORKING_DIRECTORY ${OPENAIR_CMAKE_SCRIPTS} 
                 ERROR_VARIABLE ERROR_DB
                 TIMEOUT 5)

IF( ERROR_DB )
  MESSAGE( SEND_ERROR "Database setup failed: ${ERROR_DB}" )
ENDIF( ERROR_DB )
                
install (TARGETS openair-hss DESTINATION ${OPENAIR_TARGETS}/bin)

