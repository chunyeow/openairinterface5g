dnl Process this file with autoconf to produce a configure script.
AC_INIT(Radio Resource Manager, 0.1.0)
AC_CONFIG_SRCDIR(src/rrm.cpp)
AC_CANONICAL_SYSTEM

dnl create an config-header
AC_CONFIG_HEADER(src/config.h)

dnl enable automake-support
AM_INIT_AUTOMAKE([gnu dist-zip tar-ustar dist-bzip2])



AC_DEFINE([OUTPUT_ACCURACY], [2], [defines the number of digits after the comma in output])


dnl Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl enable libtool-support
dnl gives a deprecated warning for RANLIB with newer libtools
AC_PROG_RANLIB
AM_PROG_LIBTOOL


dnl Check for enable-features that shall go into config.h


dnl   ... for xerces 3.0
PKG_CHECK_MODULES([XERCES], [xerces-c >= 3.0],
                  [AC_DEFINE([XERCES3_SIZE_t],[XMLSize_t],[Define length for Xerces 3.])],
                  [AC_DEFINE([XERCES3_SIZE_t],[unsigned int],[Define length for Xerces 2.])])



AM_CONDITIONAL([WITH_VERSION_H], test -f .svn/entries)
if test -f .svn/entries; then
   AC_DEFINE([HAVE_VERSION_H], [1], [Define if auto-generated version.h should be used.])
else
   AC_DEFINE_UNQUOTED(VERSION_STRING, "$PACKAGE_VERSION", [Defined if auto-generated version.h is unavailable.])
fi

dnl ---------------------------------------------
dnl check for libraries...
dnl - - - - - - - - - - - - - - - - - - - - - - -

dnl  ... for xerces
AC_ARG_WITH([xerces], [AS_HELP_STRING([--with-xerces=DIR],[where Xerces-C is installed (libraries in DIR/lib, headers in DIR/include).])])
if test x"$with_xerces" != x; then
   if test x"$with_xerces_libraries" = x; then
      with_xerces_libraries=$with_xerces/lib
   fi
   if test x"$with_xerces_includes" = x; then
      with_xerces_includes=$with_xerces/include
   fi
fi

dnl   ... for libs
LIB_XERCES="xerces-c"
AC_ARG_WITH([xerces-libraries], [AS_HELP_STRING([--with-xerces-libraries=DIR],[where the Xerces-C library is installed.])])
if test x"$with_xerces_libraries" = x; then
   AC_CHECK_LIB([$LIB_XERCES], [main], [true],
                [AC_MSG_ERROR([xerces-lib not found. Specify path with option --with-xerces-libraries=DIR.])])
else
   save_ldflags=$LDFLAGS
   LDFLAGS="-L$with_xerces_libraries"
   export LDFLAGS
   AC_CHECK_LIB([$LIB_XERCES], [main], [XERCES_LDFLAGS=$LDFLAGS],
                [AC_MSG_ERROR([xerces-lib not found in "$with_xerces_libraries".])])
   LDFLAGS=$save_ldflags
   AC_SUBST(XERCES_LDFLAGS)
fi
AC_SUBST(LIB_XERCES)


dnl   ... for includes
AC_ARG_WITH([xerces-includes], [AS_HELP_STRING([--with-xerces-includes=DIR],[where the Xerces-C includes are.])])
save_cppflags=$CPPFLAGS
if test x"$with_xerces_includes" = x; then
    CPPFLAGS="$CPPFLAGS -I/usr/local/include -I/usr/include"
    export CPPFLAGS
    AC_CHECK_HEADER([xercesc/util/XercesVersion.hpp], [AM_CPPFLAGS="$CPPFLAGS $AM_CPPFLAGS"],
                    [AC_MSG_ERROR([xerces-includes not found. Specify path with option --with-xerces-includes=DIR.])])
else
    CPPFLAGS="-I$with_xerces_includes"
    export CPPFLAGS
    AC_CHECK_HEADER([xercesc/util/XercesVersion.hpp], [AM_CPPFLAGS="$CPPFLAGS $AM_CPPFLAGS"],
                    [AC_MSG_ERROR([xerces-includes not found in "$with_xerces_includes".])])
fi
CPPFLAGS=$save_cppflags




AM_CPPFLAGS="-I$PWD/$srcdir/src $AM_CPPFLAGS"
AC_SUBST(AM_CPPFLAGS)
AC_SUBST(AM_CXXFLAGS)


dnl ---------------------------------------------
dnl Checks proposed by autoscan.
dnl Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS([float.h limits.h malloc.h math.h stddef.h stdlib.h string.h sys/time.h unistd.h wchar.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE

dnl Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_CHECK_FUNCS([floor getcwd gettimeofday memset pow select socket sqrt])


dnl ---------------------------------------------
dnl add here all your Makefiles. These will be created by configure
AC_CONFIG_FILES([
src/Makefile
src/com/Makefile
src/foreign/Makefile
src/foreign/sockets-2.3.9.4/Makefile
src/foreign/sql/Makefile
src/foreign/tcpip/Makefile
src/utils/Makefile
src/utils/options/Makefile
Makefile])
AC_OUTPUT

dnl ---------------------------------------------
dnl Summary

AC_MSG_RESULT()
