dnl $Id: configure.ac 1.45 06/05/07 17:37:11+03:00 anttit@tcs.hut.fi $
dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([MIPL Mobile IPv6 for Linux], [2.0.2-umip-0.4],
	[URL:http://www.mobile-ipv6.org/bugs/], mipv6)
AC_DEFINE_UNQUOTED([PACKAGE_COPYRIGHT],
	["Copyright (C) 2003-2006 Helsinki University of Technology, and others"], 
	[Define to the copyright statement])

AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

AC_REVISION($Revision: 1.45 $)
AC_CANONICAL_HOST
AC_GNU_SOURCE
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_GCC_TRADITIONAL
AC_PROG_EGREP
AC_PROG_LEX
if test x$LEX = xflex; then
	old_flex=`$LEX --version | $EGREP '2\.[[0-5]]\.[[0-4]]a?$'`
	if test -n "$old_flex"; then
		AC_MSG_WARN([
*** scan.l requires newer version of flex.  If you need to generate
*** scan.c, upgrade to the latest flex (>=2.5.31) version.
])
	fi
fi
AC_PROG_YACC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_CHECK_PROG([SED], [sed], [sed])
if test -z "$SED"; then
	AC_MSG_ERROR([sed required but not found

*** sed not found in the system.  Install sed to proceed.
])
fi
AC_CHECK_PROG([INDENT], [indent], [indent])
if test -z "$INDENT"; then
	AC_MSG_WARN([
*** indent not found in the system.  If you need to generate pmgr.c and 
*** pmgr.h you need to install indent.
])
fi

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_C_RESTRICT
AC_C_VOLATILE
AC_C_BIGENDIAN
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_HEADER_STDBOOL

AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h fcntl.h sys/ioctl.h sys/param.h time.h syslog.h])

dnl Checks for library functions.
AC_FUNC_FORK
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_FUNC_WAIT3
AC_CHECK_FUNCS([memset select socket strdup strerror])

AC_MSG_NOTICE([*** checking for additional external libraries])
AC_CHECK_FUNC([clock_gettime])
if test $ac_cv_func_clock_gettime = no; then
   AC_CHECK_LIB(rt, clock_gettime,,
		[AC_MSG_ERROR([librt required but not found])])
fi

AC_CHECK_FUNC([dlopen])
if test $ac_cv_func_dlopen = no; then
   AC_CHECK_LIB(dl, dlopen,,
		[AC_MSG_ERROR([libdl required but not found])])
fi

AC_CHECK_LIB(pthread, pthread_create, [], [
	AC_MSG_ERROR([POSIX thread library required but not found.])
])

AC_CHECK_LIB(pcap, pcap_compile, [], [
	AC_MSG_ERROR([PCAP library required but not found.])
])
AC_CHECK_LIB(pcap, pcap_open_live, [], [
	AC_MSG_ERROR([PCAP library required but not found.])
])
AC_CHECK_LIB(pcap, pcap_lookupnet, [], [
	AC_MSG_ERROR([PCAP library required but not found.])
])
AC_CHECK_LIB(pcap, pcap_setfilter, [], [
	AC_MSG_ERROR([PCAP library required but not found.])
])
AC_CHECK_LIB(pcap, pcap_next, [], [
	AC_MSG_ERROR([PCAP library required but not found.])
])

AC_ARG_WITH(pmip-use-radius,
			AC_HELP_STRING([--with-pmip-use-radius],[use radius auth]),
			[ac_cv_pmip_use_radius=$withval], [ac_cv_pmip_use_radius=no])
if test x$ac_cv_pmip_use_radius = xyes; then
	AC_CHECK_LIB(freeradius-client, rc_read_config, [], [
	AC_MSG_ERROR([freeradius-client library required but not found.])
	])
	AC_CHECK_LIB(freeradius-client, rc_read_dictionary, [], [
	AC_MSG_ERROR([freeradius-client library required but not found.])
	])
	AC_CHECK_LIB(freeradius-client, rc_avpair_add, [], [
	AC_MSG_ERROR([freeradius-client library required but not found.])
	])
	AC_CHECK_LIB(freeradius-client, rc_avpair_get, [], [
	AC_MSG_ERROR([freeradius-client library required but not found.])
	])
	AC_CHECK_LIB(freeradius-client, rc_avpair_free, [], [
	AC_MSG_ERROR([freeradius-client library required but not found.])
	])
fi




AC_CHECK_LIB(netlink, rtnl_talk)
AM_CONDITIONAL(LOCAL_NETLINK, test x$ac_cv_lib_netlink_rtnl_talk = xno)

AC_ARG_WITH(builtin-crypto,
	AC_HELP_STRING([--with-builtin-crypto],[use builtin crypto code]),
	[ac_cv_use_builtin_crypto=$withval], [ac_cv_use_builtin_crypto=no])
if test x$ac_cv_use_builtin_crypto = xyes; then
	ac_cv_lib_crypto_HMAC_Init=no
else
	AC_CHECK_LIB(crypto, HMAC_Init)
	if test x$ac_cv_lib_crypto_HMAC_Init = xno; then
		ac_cv_use_builtin_crypto=yes
	fi
fi

AC_MSG_NOTICE([*** checking for optional features])
AC_CACHE_CHECK([whether to use builtin crypto],
	       [ac_cv_use_builtin_crypto], [ac_cv_use_builtin_crypto=no])
AM_CONDITIONAL(BUILTIN_CRYPTO, test x$ac_cv_lib_crypto_HMAC_Init = xno)

if test ac_cv_pmip_use_radius = xyes; then
        AC_DEFINE([USE_RADIUS], [1], [USE RADIUS AUTH])
fi
AC_CACHE_CHECK([whether to use RADIUS client library],
			   [ac_cv_pmip_use_radius], [ac_cv_pmip_use_radius=no])
AM_CONDITIONAL(USE_RADIUS, test x$ac_cv_pmip_use_radius = xyes)

dnl Option --disable-debug
AC_ARG_ENABLE(debug,
	AC_HELP_STRING([--disable-debug],[disable debug messages and asserts]),
        [ac_cv_enable_debug=$enableval], [ac_cv_enable_debug=yes])
if test x$ac_cv_enable_debug = xno; then
        AC_DEFINE([MIP6_NDEBUG], [1], [Disable debugging])
fi
AC_CACHE_CHECK([whether to enable debug messages],
	       [ac_cv_enable_debug], [ac_cv_enable_debug=no])

dnl Option --enable-vt
AC_ARG_ENABLE(vt,
	AC_HELP_STRING([--enable-vt],[enable virtual terminal for debugging]),
        [ac_cv_enable_vt=$enableval], [ac_cv_enable_vt=no])
if test x$ac_cv_enable_vt = xyes; then
        AC_DEFINE([ENABLE_VT], [1], [Enable VT])
fi
AC_CACHE_CHECK([whether to enable debug virtual terminal],
	       [ac_cv_enable_vt], [ac_cv_enable_vt=no])
AM_CONDITIONAL(ENABLE_VT, test x$enable_vt = xyes)

dnl Check Advanced IPv6 Socket API destination options header function availability
AC_MSG_NOTICE([*** checking inet6_opt for RFC3542: Advanced IPv6 Socket API support])
AC_REPLACE_FUNCS(inet6_opt_find)

dnl Check Advanced IPv6 Socket API routing header function availability
AC_MSG_NOTICE([*** checking inet6_rth for RFC3542: Advanced IPv6 Socket API support])
dnl Check Extension to Socket API for MIPv6 availability
AC_MSG_NOTICE([*** checking RH2 for RFC4584: Extension to Socket API for MIPv6])
AC_CHECK_DECLS([IPV6_RTHDR_TYPE_2],
	[AC_REPLACE_FUNCS(inet6_rth_space)
	 AC_REPLACE_FUNCS(inet6_rth_init)
	 AC_REPLACE_FUNCS(inet6_rth_add)
	 AC_REPLACE_FUNCS(inet6_rth_getaddr)
	 AC_REPLACE_FUNCS(inet6_rth_gettype)],
	[AC_LIBOBJ(inet6_rth_space)
	 AC_LIBOBJ(inet6_rth_init)
	 AC_LIBOBJ(inet6_rth_add)
	 AC_LIBOBJ(inet6_rth_getaddr)
	 AC_LIBOBJ(inet6_rth_gettype)],)

dnl Check some functions are really missing so we don't build empty libmissing
AM_CONDITIONAL(MISSING,
   test x$ac_cv_func_inet6_opt_find != xyes -o \
	x$ac_cv_func_inet6_rth_space != xyes -o \
	x$ac_cv_func_inet6_rth_init != xyes -o \
	x$ac_cv_func_inet6_rth_add != xyes -o \
	x$ac_cv_func_inet6_rth_getaddr != xyes -o \
	x$ac_cv_func_inet6_rth_gettype != xyes)

AC_MSG_NOTICE([*** checking for Mobile IPv6 support in kernel headers])
AC_CHECK_DECLS([IFA_F_HOMEADDRESS],,
	[AC_MSG_ERROR([kernel headers do not support MIPv6.

*** If working Linux kernel headers are not in the default include path,
*** please use CPPFLAGS variable to specify where they can be found 
*** (e.g. CPPFLAGS="-isystem /usr/src/linux/include")
])], [#include <sys/socket.h>
#include <linux/if_addr.h>])

AC_MSG_NOTICE([*** checking for Mobile IPv6 extensions in system headers])
AC_CHECK_HEADERS([netinet/in.h netinet/ip6.h netinet/icmp6.h netinet/ip6mh.h])
AC_CHECK_MEMBER(struct ip6_ext.ip6e_nxt, AC_DEFINE([HAVE_STRUCT_IP6_EXT],,
		[Define to 1 if struct ip6_ext is in <netinet/ip6.h>.]),,
		[#include <netinet/ip6.h>])
AC_CHECK_MEMBER(struct ip6_opt_home_address.ip6oha_type, 
		AC_DEFINE([HAVE_STRUCT_IP6_OPT_HOME_ADDRESS],,
		[Define to 1 if struct ip6_opt_home_address is in <netinet/ip6.h>.]),,
		[#include <netinet/ip6.h>])
AC_CHECK_MEMBER(struct ip6_rthdr2.ip6r2_nxt,
		AC_DEFINE([HAVE_STRUCT_IP6_RTHDR2],,
		[Define to 1 if struct ip6_rthdr2 is in <netinet/ip6.h>.]),,
		[#include <netinet/ip6.h>])

AC_CHECK_MEMBER(struct mip_dhaad_req.mip_dhreq_hdr,
		AC_DEFINE([HAVE_STRUCT_MIP_DHAAD_REQ],,
		[Define to 1 if <netinet/icmp6.h> RFC3775 compatible.]),,
		[#include <netinet/icmp6.h>])
AC_CHECK_MEMBER(struct mip_dhaad_rep.mip_dhrep_hdr,
		AC_DEFINE([HAVE_STRUCT_MIP_DHAAD_REP],,
		[Define to 1 if <netinet/icmp6.h> RFC3775 compatible.]),,
		[#include <netinet/icmp6.h>])
AC_CHECK_MEMBER(struct mip_prefix_solicit.mip_ps_hdr,
		AC_DEFINE([HAVE_STRUCT_MIP_PREFIX_SOLICIT],,
		[Define to 1 if <netinet/icmp6.h> RFC3775 compatible.]),,
		[#include <netinet/icmp6.h>])
AC_CHECK_MEMBER(struct mip_prefix_advert.mip_pa_hdr,
		AC_DEFINE([HAVE_STRUCT_MIP_PREFIX_ADVERT],,
		[Define to 1 if <netinet/icmp6.h> RFC3775 compatible.]),,
		[#include <netinet/icmp6.h>])
AC_CHECK_MEMBER(struct nd_opt_homeagent_info.nd_opt_hai_type,
		AC_DEFINE([HAVE_STRUCT_ND_OPT_HOMEAGENT_INFO],,
		[Define to 1 if <netinet/icmp6.h> RFC3775 compatible.]),,
		[#include <netinet/icmp6.h>])

AC_CONFIG_FILES([Makefile libnetlink/Makefile libmissing/Makefile src/Makefile man/Makefile])
AC_OUTPUT

AC_MSG_NOTICE([settings

AC_PACKAGE_NAME version AC_PACKAGE_VERSION

  Use built-in crypto: ............ $ac_cv_use_builtin_crypto
  Use OpenSSL crypto library: ..... $ac_cv_lib_crypto_HMAC_Init
  Use RADIUS client library: ...... $ac_cv_pmip_use_radius
  Enable debugging mode: .......... $ac_cv_enable_debug
  Enable virtual debug terminal: .. $ac_cv_enable_vt

  Build target: $host
  CPPFLAGS: $CPPFLAGS
])
