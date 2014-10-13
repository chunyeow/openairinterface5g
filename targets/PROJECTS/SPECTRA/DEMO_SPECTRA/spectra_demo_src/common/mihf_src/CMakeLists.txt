project(libodtone)

find_package(Boost 1.46 COMPONENTS system program_options thread REQUIRED)

set(libodtone_MAJOR_VERSION ${odtone_MAJOR_VERSION})
set(libodtone_MINOR_VERSION ${odtone_MINOR_VERSION})
set(libodtone_MICRO_VERSION ${odtone_MICRO_VERSION})
set(libodtone_VERSION "${libodtone_MAJOR_VERSION}.${libodtone_MINOR_VERSION}.${libodtone_MICRO_VERSION}")
set(libodtone_SOVERSION "${libodtone_MAJOR_VERSION}.${libodtone_MINOR_VERSION}")

set(libodtone_SRC
strutil.cpp
conf.cpp
mih/types/address.cpp
mih/config.cpp
mih/archive.cpp
mih/message.cpp
net/dns/message.cpp
net/dns/resolver.cpp
net/dns/utils.cpp
net/link/address_mac.cpp
net/ip/icmp/icmp_parser.cpp
net/ip/prefix.cpp
debug.cpp
sap/link.cpp
sap/sap.cpp
sap/user.cpp
logger.cpp
win32.cpp
)

if(MSVC)
	LIST(APPEND libodtone_SRC debug_win32.cpp)
elseif(UNIX)
	LIST(APPEND libodtone_SRC debug_linux.cpp)
endif()

set(libodtone_base_FILE_HEADERS
../../inc/odtone/base.hpp
../../inc/odtone/bind_rv.hpp
../../inc/odtone/buffer.hpp
../../inc/odtone/cast.hpp
../../inc/odtone/conf.hpp
../../inc/odtone/debug.hpp
../../inc/odtone/exception.hpp
../../inc/odtone/list_node.hpp
../../inc/odtone/logger.hpp
../../inc/odtone/namespace.hpp
../../inc/odtone/random.hpp
../../inc/odtone/string.hpp
../../inc/odtone/strutil.hpp
)

set(libodtone_mih_DIR_HEADERS ../../inc/odtone/mih)
set(libodtone_sap_DIR_HEADERS ../../inc/odtone/sap)
set(libodtone_net_DIR_HEADERS ../../inc/odtone/net)

include_directories(${INCLUDE_DIRECTORIES} ${Boost_INCLUDE_DIRS} "../../inc")
add_library(libodtone SHARED ${libodtone_SRC})
target_link_libraries(libodtone ${Boost_LIBRARIES})
set_target_properties(libodtone PROPERTIES OUTPUT_NAME "odtone"
                                           VERSION ${libodtone_VERSION}
                                           SOVERSION ${libodtone_SOVERSION}
                                           DEFINE_SYMBOL LIBODTONE_EXPORTS)

# install libodtone
install(FILES ${libodtone_base_FILE_HEADERS} DESTINATION include/odtone)
install(DIRECTORY ${libodtone_mih_DIR_HEADERS} DESTINATION include/odtone)
install(DIRECTORY ${libodtone_sap_DIR_HEADERS} DESTINATION include/odtone)
install(DIRECTORY ${libodtone_net_DIR_HEADERS} DESTINATION include/odtone)
install(TARGETS libodtone EXPORT odtone
                          LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
                          ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
                          RUNTIME DESTINATION bin)
