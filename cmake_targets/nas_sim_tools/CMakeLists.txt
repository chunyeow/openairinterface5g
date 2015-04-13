cmake_minimum_required(VERSION 2.8)

project(NAS_SIM_TOOLS)


ENABLE_LANGUAGE(C)
#Sends the -std=c99 flag to the gcc compiler
add_definitions(-std=c99)
add_definitions(-DNAS_UE)

set(OPENAIR_DIR     $ENV{OPENAIR_DIR})
set(OPENAIR1_DIR    $ENV{OPENAIR_DIR}/openair1)
set(OPENAIR2_DIR    $ENV{OPENAIR_DIR}/openair2)
set(OPENAIR3_DIR    $ENV{OPENAIR_DIR}/openair3)
set(OPENAIRCN_DIR   $ENV{OPENAIR_DIR}/openair-cn)
set(OPENAIR_TARGETS $ENV{OPENAIR_DIR}/targets)

#set(EXECUTABLE_OUTPUT_PATH ${OPENAIR_DIR}/targets/bin)


# Add .h files for dependancies
set(usim_SRC
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/tools/usim_data.c
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/api/usim/usim_api.c
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/api/usim/aka_functions.c
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/util/memory.c
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/util/nas_log.c
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/util/OctetString.c
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/util/TLVEncoder.c
)
set(usim_HDR
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/tools/network.h
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/api/usim/usim_api.h
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/api/usim/aka_functions.h
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/util/memory.h
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/util/nas_log.h
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/util/OctetString.h
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/util/TLVEncoder.h
)
include_directories(
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/include
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/api/usim
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/emm/
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/esm/
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/ies/
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/util
)
ADD_EXECUTABLE(usim  ${usim_SRC} ${usim_HDR})




set(nvram_SRC
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/tools/ue_data.c
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/util/memory.c
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/util/nas_log.c
)

set(nvram_HDR
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/emm/emmData.h
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/util/memory.h
    ${OPENAIR_DIR}/openair-cn/NAS/EURECOM-NAS/src/include/userDef.h
)

ADD_EXECUTABLE(nvram  ${nvram_SRC} ${nvram_HDR})

#install (TARGETS usim  DESTINATION ${EXECUTABLE_OUTPUT_PATH})
#install (TARGETS nvram DESTINATION ${EXECUTABLE_OUTPUT_PATH})

#install(CODE "EXECUTE_PROCESS (COMMAND ${OPENAIR_TARGETS}/bin/nvram --gen WORKING_DIRECTORY ${OPENAIR_TARGETS}/bin)")
#install(CODE "EXECUTE_PROCESS (COMMAND ${OPENAIR_TARGETS}/bin/usim  --gen WORKING_DIRECTORY ${OPENAIR_TARGETS}/bin)")
