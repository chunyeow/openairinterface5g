################################################################################
#   OpenAirInterface
#   Copyright(c) 1999 - 2014 Eurecom
#
#    OpenAirInterface is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
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
#  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.
#
################################################################################
AC_PREREQ([2.68])

define([svnversion], esyscmd([sh -c "svnversion ..|tr -d '\n'"]))

AC_DEFINE(SVN_REVISION, "svnversion", [SVN Revision])

AC_INIT([oai_epc], [0.3.svnversion], [openair_admin@eurecom.fr])
AC_CANONICAL_BUILD
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([1.11 silent-rules])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_HEADERS([config.h])

AM_MAINTAINER_MODE

AM_SILENT_RULES([yes])

AM_PROG_LIBTOOL
AM_PROG_LEX

dnl *** Autoconf support ***
AC_ARG_ENABLE(autoconf,
  [  --disable-autoconf      disable automatic generation of configure script ],
  enable_autoconf=$enableval, enable_autoconf=yes
)
AC_PATH_PROG(AUTOCONF, autoconf, @echo autoconf not available)
AC_PATH_PROG(AUTOHEADER, autoheader, @echo autoheader not available)
if test -z "$AUTOCONF"; then enable_autoconf=no ; fi
if test -z "$AUTOHEADER"; then enable_autoconf=no ; fi
if test x$enable_autoconf = xyes; then
  CONFIGURE_DEPENDS="configure.in aclocal.m4"
fi
AC_SUBST(CONFIGURE_DEPENDS)

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN"; then
    AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
else
    # Check for presence of pdfLaTeX
    AC_CHECK_PROG(PDFLATEX, pdflatex, pdflatex)
    if test -z "$PDFLATEX"; then
        AC_MSG_WARN([Unable to create PDF version of the user manual.])
    fi
fi
AM_CONDITIONAL([HAVE_PDFLATEX], test -n "$PDFLATEX")

dnl ***asn1c support***
AC_CHECK_PROG(ASN1C_CHECK, asn1c, yes, [AC_MSG_ERROR(Please install asn1c before going further.)])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P
AC_PROG_YACC
AC_PROG_RANLIB

AC_CHECK_HEADERS([arpa/inet.h fcntl.h float.h inttypes.h limits.h malloc.h])
AC_CHECK_HEADERS([netinet/in.h stddef.h stdint.h stdlib.h string.h libintl.h])
AC_CHECK_HEADERS([sys/time.h sys/ioctl.h sys/socket.h unistd.h])
AC_CHECK_HEADERS([netdb.h termios.h])

# Checks for library functions.
AC_FUNC_MMAP
AC_FUNC_MALLOC
AC_FUNC_ALLOCA
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([ftruncate inet_ntoa memchr memmove memset munmap select socket])
AC_CHECK_FUNCS([strdup strerror strspn strchr strstr])
AC_CHECK_FUNCS([gethostname gethostbyname])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

AC_CHECK_TYPES([ptrdiff_t])

dnl *** libsctp-dev support ***
AC_CHECK_LIB([sctp], [sctp_sendmsg], [],
             [AC_MSG_ERROR([libsctp-dev needed!])])

dnl *** pthread support ***
AC_CHECK_LIB([pthread], [pthread_mutex_init], [],
             [AC_MSG_ERROR(pthread library needed!)])

dnl *** real time support ***
AC_CHECK_LIB([rt], [timer_create], [],
             [AC_MSG_ERROR(rt library needed!)])

dnl *** Cryptographic lib nettle support ***
PKG_CHECK_MODULES(nettle, [nettle >= 2.5],
                  [HAVE_NETTLE=true],
                  [AC_MSG_ERROR("Please install nettle >= 2.5")])
CFLAGS="$CFLAGS $nettle_CFLAGS"
LIBS="$LIBS $nettle_LIBS"

AC_ARG_ENABLE([standalone-epc],
              AS_HELP_STRING([--enable-standalone-epc],
                             [Compile MME, SGW and PGW in a single executable]),
              [],
              [STANDALONE_EPC="yes"])
AM_CONDITIONAL([STANDALONE_EPC], [test "x$enable_standalone_epc" = "xyes"])
if test "x$enable_standalone_epc" = "xyes"; then
    CFLAGS="$CFLAGS -DENABLE_STANDALONE_EPC"
else
    CFLAGS="$CFLAGS -DDISABLE_STANDALONE_EPC"
fi

dnl *** Add freediameter dependencies ***
AC_CHECK_LIB([fdcore], [fd_core_initialize], [],
                [AC_MSG_ERROR(Free diameter lib not installed)])
AC_CHECK_LIB([fdproto], [fd_msg_search_avp], [],
                [AC_MSG_ERROR(Free diameter lib not installed)])

dnl *** Freediameter requirements ***
AC_CHECK_HEADERS([signalent.h])
AC_CHECK_FUNCS([ntohll strndup])
AC_DEFINE([HAVE_AI_ADDRCONFIG], [],
            [Define to 1 if you have AI_ADDRCONFIG defined in netdb.h])
AC_DEFINE([HAVE_CLOCK_GETTIME], [],
            [Define to 1 if you have clock_gettime in librt])
AC_DEFINE([HAVE_PTHREAD_BAR], [],
            [Define to 1 if you have pthread_barrier_wait in libpthread])
AC_DEFINE([SCTP_CONNECTX_4_ARGS], [],
            [Define to 1 if sctp_connectx function accepts 4 arguments])
AC_CHECK_LIB([rt], [clock_gettime], [AC_DEFINE(HAVE_CLOCK_GETTIME, 1)], [])
AC_CHECK_LIB([pthread], [pthread_barrier_wait],
                [AC_DEFINE(HAVE_PTHREAD_BAR, 1)], [])
AC_CHECK_DECL([AI_ADDRCONFIG],
                [AC_DEFINE(HAVE_AI_ADDRCONFIG, 1)],
                [], [[#include <netdb.h>]])
AC_MSG_CHECKING(if sctp_connectx accepts 4 arguments)
AC_LINK_IFELSE([
    AC_LANG_SOURCE(
        [[int main() { return sctp_connectx(0, NULL, 0, NULL); }]])
], [AC_DEFINE(SCTP_CONNECTX_4_ARGS, 1)])
AC_CHECK_LIB([gnutls],
                [gnutls_hash],
                [AC_DEFINE(GNUTLS_VERSION_210, 1,
                        [Define to 1 if you have gnutls 2.10 installed])],
                [])
AC_CHECK_LIB([gnutls],
                [gnutls_x509_trust_list_verify_crt],
                [AC_DEFINE(GNUTLS_VERSION_300, 1,
                        [Define to 1 if you have gnutls 3.0 installed])],
                [])
AC_CHECK_LIB([gnutls],
                [gnutls_handshake_set_timeout],
                [AC_DEFINE(GNUTLS_VERSION_310, 1,
                        [Define to 1 if you have gnutls 3.1 installed])],
                [])

AC_DEFINE(FREE_DIAMETER_MINIMUM_VERSION, "1.1.5", [freeDiameter minimum version])

AC_SUBST(ADD_CFLAGS)

dnl *** Enable r8 update ***
AC_ARG_ENABLE([r8],
              AS_HELP_STRING([--enable-r8], [Disable 3GPP release 9 and 10 updates]),
              [],
              [])

dnl *** Enable r9 update ***
AC_ARG_ENABLE([r9],
              AS_HELP_STRING([--enable-r9], [Disable 3GPP release 10 updates]),
              [],
              [])

dnl *** Disable NAS layer ***
AC_ARG_ENABLE([nas],
              AS_HELP_STRING([--disable-nas], [Disable NAS layer]),
              [CFLAGS="$CFLAGS -DDISABLE_USE_NAS"],
              [])
AM_CONDITIONAL([DISABLE_USE_NAS], [test x$enable_nas == xno])

AC_ARG_ENABLE([s11],
              AS_HELP_STRING([--disable-s11], [Disable S11 layer]),
              [CFLAGS="$CFLAGS -DDISABLE_S11"],
              [])
AM_CONDITIONAL([DISABLE_S11], [test x$enable_s11 == xno])


dnl *** Enable GTPU IN KERNEL ***
AC_ARG_ENABLE([gtp1u_in_kernel],
              AS_HELP_STRING([--enable-gtp1u-in-kernel], [Enable GTPU IN KERNEL]),
              [case "${enableval}" in
                 yes) gtp1u_in_kernel=true; CFLAGS="$CFLAGS -DENABLE_USE_GTPU_IN_KERNEL" ;;
                 no ) gtp1u_in_kernel=false;;
                 *) AC_MSG_ERROR(bad value ${enableval} for --enable-gtp1u-in-kernel) ;;
               esac],
              [gtp1u_in_kernel=false])
              
dnl *** Enable PCAP for SGI ***
AC_ARG_ENABLE([pcap_for_sgi],
              AS_HELP_STRING([--enable-pcap-for-sgi], [Enable PCAP for SGI layer]),
              [case "${enableval}" in
                 yes) pcap_for_sgi=true; CFLAGS="$CFLAGS -DENABLE_USE_PCAP_FOR_SGI" ;;
                 no ) pcap_for_sgi=false;;
                 *) AC_MSG_ERROR(bad value ${enableval} for --enable-pcap-for-sgi) ;;
               esac],
              [pcap_for_sgi=false])

dnl *** Enable NETFILTER for SGI ***
AC_ARG_ENABLE([netfilter_for_sgi],
              AS_HELP_STRING([--enable-netfilter-for-sgi], [Enable netfilter for SGI layer]),
              [case "${enableval}" in
                 yes) netfilter_for_sgi=true; CFLAGS="$CFLAGS -DENABLE_USE_NETFILTER_FOR_SGI" ;;
                 no ) netfilter_for_sgi=false;;
                 *) AC_MSG_ERROR(bad value ${enableval} for --enable-netfilter-for-sgi) ;;
               esac],
              [netfilter_for_sgi=false])

dnl *** Enable RAW socket for SGI ***
AC_ARG_ENABLE([raw_socket_for_sgi],
              AS_HELP_STRING([--enable-raw-socket-for-sgi], [Enable netfilter for SGI layer]),
              [case "${enableval}" in
                 yes) raw_socket_for_sgi=true; CFLAGS="$CFLAGS -DENABLE_USE_RAW_SOCKET_FOR_SGI" ;;
                 no ) raw_socket_for_sgi=false;;
                 *) AC_MSG_ERROR(bad value ${enableval} for --enable-raw-socket-for-sgi) ;;
               esac],
              [if test x$pcap_for_sgi != xtrue && test x$netfilter_for_sgi != xtrue && test x$gtp1u_in_kernel != xtrue; then 
                  raw_socket_for_sgi=true; CFLAGS="$CFLAGS -DENABLE_USE_RAW_SOCKET_FOR_SGI"
               else
                  raw_socket_for_sgi=false
               fi])

AM_CONDITIONAL([ENABLE_USE_PCAP_FOR_SGI],       [test x$pcap_for_sgi == xtrue])
AM_CONDITIONAL([ENABLE_USE_NETFILTER_FOR_SGI],  [test x$netfilter_for_sgi == xtrue])
AM_CONDITIONAL([ENABLE_USE_RAW_SOCKET_FOR_SGI], [test x$raw_socket_for_sgi == xtrue])
AM_CONDITIONAL([ENABLE_USE_RAW_SOCKET_FOR_SGI], [ test x$pcap_for_sgi != xtrue && test x$netfilter_for_sgi != xtrue ])
AM_CONDITIONAL([ENABLE_USE_GTPU_IN_KERNEL],     [ test x$gtp1u_in_kernel == xtrue ])
AM_CONDITIONAL([ENABLE_USE_GTPU_IN_KERNEL],     [ test x$raw_socket_for_sgi == xtrue && x$pcap_for_sgi != xtrue && test x$netfilter_for_sgi != xtrue ])

AS_IF([test "x$enable_pcap_for_sgi" == "xyes"], [
    dnl *** libpcap support ***
    AC_CHECK_LIB([pcap], [pcap_compile], [], [
        AC_MSG_ERROR([PCAP library needed.])
    ])
])

AS_IF([test "x$enable_netfilter_for_sgi" == "xyes"], [
    dnl *** libnfnetlink support ***
    AC_CHECK_LIB([nfnetlink], [nfnl_fd], [],
                 [AC_MSG_ERROR([libnfnetlink needed!])])

    dnl *** libnetfilter_queue support ***
    AC_CHECK_LIB([netfilter_queue], [nfq_open], [],
                 [AC_MSG_ERROR([libnetfilter_queue needed!])])
])


if test x$enable_r8 == xyes; then
    ASN1DIR=R8.10
else
    if test x$enable_r9 == xyes; then
        ASN1DIR=R9.8
        CFLAGS="$CFLAGS -DUPDATE_RELEASE_9"
    else
        ASN1DIR=R10.5
        CFLAGS="$CFLAGS -DUPDATE_RELEASE_9 -DUPDATE_RELEASE_10"
    fi
fi
AC_DEFINE(ASN1_MINIMUM_VERSION, 924, [asn1c minimum version])
AM_CONDITIONAL([UPDATE_RELEASE_9], [test x$enable_r8 != xyes])
AM_CONDITIONAL([UPDATE_RELEASE_10], [test x$enable_r8 != xyes && test x$enable_r9 != xyes])

AC_SUBST(ASN1DIR)

PKG_CHECK_MODULES(OPENSSL,
                  [openssl >= 1.0.1],
                  [HAVE_OPENSSL=true],
                  [AC_MSG_ERROR("Please install libcrypto >= 1.0.1")])
CFLAGS="$CFLAGS $OPENSSL_CFLAGS"
LIBS="$LIBS $OPENSSL_LIBS"

PKG_CHECK_MODULES(CHECK, [check >= 0.9.4], [HAVE_CHECK=true], [HAVE_CHECK=false])
AM_CONDITIONAL(HAVE_CHECK, test x$HAVE_CHECK = xtrue)

AC_CHECK_PROG(HAVE_GCCXML,gccxml,yes,no)
AM_CONDITIONAL(HAVE_GCCXML, test x$HAVE_GCCXML = xyes)

dnl Add these flags
CFLAGS="$CFLAGS -Wall -g -O"
CFLAGS="$CFLAGS -Wcast-align"
CFLAGS="$CFLAGS -Wchar-subscripts"
CFLAGS="$CFLAGS -Wmissing-prototypes"
CFLAGS="$CFLAGS -Wmissing-declarations"
CFLAGS="$CFLAGS -Werror=shadow"
CFLAGS="$CFLAGS -Werror=implicit-function-declaration"

dnl *** Check for endianness of platform ***
AC_C_BIGENDIAN
if test "x$ac_cv_c_bigendian" = "xyes"; then
    CFLAGS="$CFLAGS -DBYTE_ORDER=BIG_ENDIAN"
else
    CFLAGS="$CFLAGS -DBYTE_ORDER=LITTLE_ENDIAN"
fi

dnl AC_SUBST([AM_CFLAGS])

dnl Doxygen section
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([DOCS/DOXYGEN/Doxyfile])])

dnl *** Retrieving hostname for substition in freediameter configuration file ***
HOSTNAME=$(uname -n)
AC_MSG_CHECKING([host name])
AC_MSG_RESULT([$HOSTNAME])
AC_SUBST([HOSTNAME])

AC_CONFIG_FILES(                    \
    GTPV1-U/Makefile                \
    GTPV2-C/Makefile                \
    INTERTASK_INTERFACE/Makefile    \
    SGI/Makefile                    \
    NAS/Makefile                    \
    S11/Makefile                    \
    S1AP/MESSAGES/ASN1/Makefile     \
    S1AP/MESSAGES/Makefile          \
    S1AP/Makefile                   \
    S6A/Makefile                    \
    SCTP/Makefile                   \
    SECU/Makefile                   \
    SGW-LITE/Makefile               \
    UTILS/CONF/s6a.conf             \
    UTILS/HASHTABLE/Makefile        \
    UTILS/Makefile                  \
    UDP/Makefile                    \
    MME_APP/Makefile                \
    OAISIM_MME/Makefile             \
    OAI_EPC/Makefile                \
    OAI_SGW/Makefile                \
    DOCS/DOXYGEN/Makefile           \
    DOCS/Makefile                   \
    TEST/Makefile                   \
    Makefile                        \
)

dnl Call for configure script in gtpv2c subdir
AC_CONFIG_SUBDIRS([GTPV2-C/nwgtpv2c-0.11])

dnl configure makefiles
AC_OUTPUT

echo "
($PACKAGE_NAME) version $PACKAGE_VERSION
Prefix.........: $prefix
C Compiler.....: $CC $CFLAGS
Linker.........: $LD $LDFLAGS $LIBS
Doxygen........: ${DOXYGEN:-no}
Tests..........: ${HAVE_CHECK}
GCCXML.........: ${HAVE_GCCXML}
Standalone ....: ${enable_standalone_epc:-no}"

if test x$enable_r8 == xyes; then
    echo "Release........: 8.10"
else
    if test x$enable_r9 == xyes; then
        echo "Release........: 9.8"
    else
        echo "Release........: 10.5"
    fi
fi

if test x$pcap_for_sgi == xtrue; then
    echo "SGI pcap.......: yes"
fi

if test x$netfilter_for_sgi == xtrue; then
    echo "SGI netfilter..: yes"
fi

if test x$raw_socket_for_sgi == xtrue; then
    echo "SGI raw socket.: yes"
fi

if test x$gtp1u_in_kernel == xtrue; then
    echo "GTPU in kernel.: yes"
fi


