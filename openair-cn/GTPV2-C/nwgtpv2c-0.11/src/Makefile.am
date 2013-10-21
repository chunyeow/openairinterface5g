# this is example-file: src/mylib/Makefile.am

# additional include paths necessary to compile the C++ library
AM_CFLAGS = -Wall -I$(top_srcdir)/src -I$(top_srcdir)/shared -I$(top_srcdir)/include @AM_CFLAGS@

###############################################################################
# THE LIBRARIES TO BUILD
###############################################################################

# the library names to build (note we are building static libs only)
lib_LIBRARIES = libNwGtpv2c.a

# where to install the headers on the system
libNwGtpv2c_adir = $(includedir)/mylib

# the list of header files that belong to the library (to be installed later)
libNwGtpv2c_a_HEADERS =    \
			$(top_srcdir)/shared/NwTypes.h\
			$(top_srcdir)/shared/NwUtils.h\
			$(top_srcdir)/shared/NwError.h\
			$(top_srcdir)/shared/NwLog.h\
			$(top_srcdir)/shared/NwGtpv2c.h\
			$(top_srcdir)/shared/NwGtpv2cMsg.h\
			$(top_srcdir)/shared/NwGtpv2cMsgParser.h\
			$(top_srcdir)/shared/NwGtpv2cIe.h\
			$(top_srcdir)/include/NwGtpv2cPrivate.h\
			$(top_srcdir)/include/NwGtpv2cTrxn.h\
			$(top_srcdir)/include/NwGtpv2cTunnel.h\
			$(top_srcdir)/include/NwGtpv2cMsgIeParseInfo.h\
			$(top_srcdir)/include/NwGtpv2cLog.h\
			$(top_srcdir)/include/queue.h\
			$(top_srcdir)/include/tree.h

# the sources to add to the library and to add to the source distribution
libNwGtpv2c_a_SOURCES =    \
			$(libNwGtpv2_a_HEADERS) \
			NwGtpv2cTrxn.c\
			NwGtpv2cTunnel.c\
			NwGtpv2cMsg.c\
			NwGtpv2cMsgIeParseInfo.c\
			NwGtpv2cMsgParser.c\
			NwGtpv2c.c

###############################################################################


