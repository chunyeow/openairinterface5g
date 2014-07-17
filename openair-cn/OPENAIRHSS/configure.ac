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
AC_PREREQ([2.65])

define([svnversion], esyscmd([sh -c "svnversion ..|tr -d '\n'"]))

AC_DEFINE(SVN_REVISION, "svnversion", [SVN Revision])

AC_INIT([openair-hss], [1.0.0.svnversion], [openair_admin@eurecom.fr])
AC_CANONICAL_BUILD
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([1.11 silent-rules])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_HEADERS([config.h])

AM_MAINTAINER_MODE

AM_SILENT_RULES([yes])

PKG_CHECK_MODULES(CHECK, [check >= 0.9.4], [HAVE_CHECK=true], [HAVE_CHECK=false])
AM_CONDITIONAL(HAVE_CHECK, test x$HAVE_CHECK = xtrue)

AM_PROG_LEX
AM_PROG_LIBTOOL

AC_PROG_YACC
AC_PROG_CXX
AC_PROG_RANLIB

AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

AC_CHECK_HEADERS([libintl.h malloc.h netinet/in.h stddef.h])

AC_CHECK_FUNCS([memset strdup strerror])
AC_C_INLINE

AC_FUNC_ALLOCA
AC_FUNC_MALLOC
AC_FUNC_REALLOC

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
  CONFIGURE_DEPENDS="configure.in"
fi
AC_SUBST(CONFIGURE_DEPENDS)

AC_CHECK_LIB([mysqlclient],
             [mysql_init],
             [],
             [])

dnl ***freediameter support***
AC_CHECK_LIB([fdcore], [fd_core_initialize], [],
                [AC_MSG_ERROR(Free diameter lib not installed)])
AC_CHECK_LIB([fdproto], [fd_core_initialize], [],
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

AC_CHECK_LIB(gmp, __gmpz_init, ,
             [AC_MSG_ERROR([GNU MP not found, see http://gmplib.org/])])
AC_CHECK_LIB([nettle],
             [nettle_hmac_sha256_set_key],
             [],
             [AC_MSG_ERROR(nettle is not installed)])
AC_CHECK_LIB([pthread],
             [pthread_getspecific],
             [],
             [AC_MSG_ERROR(lib pthread is not installed)])

AC_SUBST(ADD_CFLAGS)

dnl Add these flags
CFLAGS="$CFLAGS -Wall"
CFLAGS="$CFLAGS -Wshadow"
CFLAGS="$CFLAGS -Wcast-align"
CFLAGS="$CFLAGS -Wchar-subscripts"
CFLAGS="$CFLAGS -Wmissing-prototypes"
CFLAGS="$CFLAGS -Wmissing-declarations"
CFLAGS="$CFLAGS -Werror=implicit-function-declaration"

AC_C_BIGENDIAN
if test "x$ac_cv_c_bigendian" = "xyes"; then
    CFLAGS="$CFLAGS -DBYTE_ORDER=BIG_ENDIAN"
else
    CFLAGS="$CFLAGS -DBYTE_ORDER=LITTLE_ENDIAN"
fi

AC_SUBST([AM_CFLAGS])

AC_ARG_ENABLE(dh_install, AS_HELP_STRING([--enable-dh-install], [Replace some variables for installation]),
              [],
              [])

if test x$enable_dh_install == xyes; then
    AC_SUBST(AM_CONF_DIR, $sysconfdir/openair-hss)
else
    AC_SUBST(AM_CONF_DIR, $srcdir/conf)
fi

AC_OUTPUT(                          \
    conf/hss.conf                   \
    access_restriction/Makefile     \
    auc/Makefile                    \
    db/Makefile                     \
    s6a/Makefile                    \
    utils/Makefile                  \
    tests/Makefile                  \
    Makefile                        \
)

echo "
($PACKAGE_NAME) version $PACKAGE_VERSION
Prefix.........: $prefix
C Compiler.....: $CC $CFLAGS
Linker.........: $LD $LDFLAGS $LIBS
Tests..........: ${HAVE_CHECK}
"