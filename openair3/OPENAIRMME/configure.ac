AC_PREREQ([2.68])

define([svnversion], esyscmd([sh -c "svnversion ..|tr -d '\n'"]))

AC_DEFINE(SVN_REVISION, "svnversion", [SVN Revision])

AC_INIT([oaisim_mme], [0.3.svnversion], [openair_admin@eurecom.fr])
AC_CANONICAL_BUILD
AC_CANONICAL_TARGET
#AM_INIT_AUTOMAKE(oaisim_mme, 0.1.svnversion)
AM_INIT_AUTOMAKE([1.11 silent-rules])

AC_CONFIG_HEADERS([config.h])

AM_MAINTAINER_MODE

AM_SILENT_RULES([yes])

AM_PROG_LIBTOOL

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
if test -z "$DOXYGEN";
then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi

# Check for presence of pdfLaTeX
AC_CHECK_PROG(PDFLATEX, pdflatex, pdflatex)
if test -z "$PDFLATEX"; then
  AC_MSG_WARN([Unable to create PDF version of the user manual.])
fi

AM_CONDITIONAL([HAVE_PDFLATEX], test -n "$PDFLATEX")

dnl ***asn1c support***
AC_CHECK_PROG(ASN1C_CHECK,asn1c,yes,[AC_MSG_ERROR(Please install asn1c before going further.)])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_MKDIR_P

AC_CHECK_HEADERS([arpa/inet.h fcntl.h float.h inttypes.h limits.h malloc.h netinet/in.h stddef.h stdint.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for library functions.
AC_FUNC_MMAP
AC_FUNC_MALLOC
AC_FUNC_ALLOCA
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([ftruncate inet_ntoa memchr memmove memset munmap socket strchr strerror strspn])

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
AC_CHECK_LIB([sctp], [sctp_sendmsg], [], [AC_MSG_ERROR([libsctp not found, consider installing libsctp-dev package])])

dnl *** pthread support ***
AC_CHECK_LIB([pthread], [pthread_mutex_init], [], [AC_MSG_ERROR(pthread library needed!)])

dnl *** real time support ***
AC_CHECK_LIB([rt], [timer_create], [], [AC_MSG_ERROR(rt library needed!)])

dnl *** Disable HSS support by default ***
AC_ARG_ENABLE([hss],
              AS_HELP_STRING([--enable-hss],
                             [Enable 3GPP S6A support]),
              [CFLAGS="$CFLAGS -DENABLE_USE_HSS"],
              [CFLAGS="$CFLAGS -DDISABLE_USE_HSS"])

dnl *** If user requested support for HSS, add some freediameter dependencies ***
AS_IF([test "x$enable_hss" == "xyes"], [
    dnl ***freediameter support***
    AC_CHECK_LIB([fdcore], [fd_core_initialize], [], [AC_MSG_ERROR(Free diameter lib not installed)])
    dnl *** Freediameter requirements ***
    AC_CHECK_HEADERS([signalent.h])
    AC_CHECK_FUNCS([ntohll strndup])
    AC_DEFINE([HAVE_AI_ADDRCONFIG], [], [Define to 1 if you have AI_ADDRCONFIG defined in netdb.h])
    AC_DEFINE([HAVE_CLOCK_GETTIME], [], [Define to 1 if you have clock_gettime in librt])
    AC_DEFINE([HAVE_PTHREAD_BAR], [], [Define to 1 if you have pthread_barrier_wait in libpthread])
    AC_DEFINE([SCTP_CONNECTX_4_ARGS], [], [Define to 1 if sctp_connectx function accepts 4 arguments])
    AC_CHECK_LIB([rt], [clock_gettime], [AC_DEFINE(HAVE_CLOCK_GETTIME, 1)], [])
    AC_CHECK_LIB([pthread], [pthread_barrier_wait], [AC_DEFINE(HAVE_PTHREAD_BAR, 1)], [])
    AC_CHECK_DECL([AI_ADDRCONFIG], [AC_DEFINE(HAVE_AI_ADDRCONFIG, 1)], [], [[#include <netdb.h>]])
    AC_MSG_CHECKING(if sctp_connectx accepts 4 arguments)
    AC_LINK_IFELSE([
        AC_LANG_SOURCE(
            [[int main() { return sctp_connectx(0, NULL, 0, NULL); }]])
    ], [AC_DEFINE(SCTP_CONNECTX_4_ARGS, 1)])
    AC_CHECK_LIB([gnutls], [gnutls_hash], [AC_DEFINE(GNUTLS_VERSION_210, 1, [Define to 1 if you have gnutls 2.10 installed])], [])
    AC_CHECK_LIB([gnutls], [gnutls_x509_trust_list_verify_crt], [AC_DEFINE(GNUTLS_VERSION_300, 1, [Define to 1 if you have gnutls 3.0 installed])], [])
    AC_CHECK_LIB([gnutls], [gnutls_handshake_set_timeout], [AC_DEFINE(GNUTLS_VERSION_310, 1, [Define to 1 if you have gnutls 3.1 installed])], [])
])

AM_CONDITIONAL([ENABLE_USE_HSS], [test x$enable_hss == xyes])
AC_SUBST(ADD_CFLAGS)

dnl *** Enable r9 update by default ***
AC_ARG_ENABLE([r9], AS_HELP_STRING([--disable-r9], [Disable 3GPP release 9 updates]), [], [CFLAGS="$CFLAGS -DUPDATE_RELEASE_9"])
if test x$enable_r9 != xno; then
    ASN1DIR=R9.8
else
    ASN1DIR=R8.10
fi
AM_CONDITIONAL([UPDATE_RELEASE_9], [test x$enable_r9 != xno])
AC_SUBST(ASN1DIR)

dnl Add these flags
CFLAGS="$CFLAGS -Wall"
CFLAGS="$CFLAGS -Wshadow"
CFLAGS="$CFLAGS -Wcast-qual"
CFLAGS="$CFLAGS -Wcast-align"
CFLAGS="$CFLAGS -Wchar-subscripts"
CFLAGS="$CFLAGS -Wmissing-prototypes"
CFLAGS="$CFLAGS -Wmissing-declarations"
CFLAGS="$CFLAGS -Werror=implicit-function-declaration"

AC_SUBST([AM_CFLAGS])

dnl Doxygen section
AM_CONDITIONAL([HAVE_DOXYGEN],
               [test -n "$DOXYGEN"])AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([DOCS/DOXYGEN/Doxyfile])])

AC_OUTPUT(                          \
    GTPV1-U/Makefile                \
    INTERTASK_INTERFACE/Makefile    \
    S1AP/MESSAGES/ASN1/Makefile     \
    S1AP/MESSAGES/Makefile          \
    S1AP/Makefile                   \
    S6A/Makefile                    \
    SCTP/Makefile                   \
    SGW-LITE/Makefile               \
    TEST/Makefile                   \
    UTILS/TIMER/Makefile            \
    UTILS/Makefile                  \
    UDP/Makefile                    \
    OAISIM_MME/Makefile             \
    DOCS/DOXYGEN/Makefile           \
    DOCS/Makefile                   \
    Makefile                        \
)

echo "
($PACKAGE_NAME) version $PACKAGE_VERSION
Prefix.........: $prefix
C Compiler.....: $CC $CFLAGS
Linker.........: $LD $LDFLAGS $LIBS
Doxygen........: ${DOXYGEN:-no}
HSS............: ${enable_hss:-no}"

if test x$enable_r9 != xno; then
    echo "Release........: 9.8"
else
    echo "Release........: 8.10"
fi
