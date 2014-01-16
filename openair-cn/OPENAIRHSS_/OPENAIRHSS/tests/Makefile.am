# if HAVE_CHECK
# TESTS = up_loc_test
# else
# TESTS =
# endif
AM_CFLAGS = @CHECK_CFLAGS@	\
	-I$(top_srcdir)/auc

if HAVE_CHECK
TESTS = up_loc_test test_security_f1	\
	test_security_f2_f3_f5		\
	test_security_f4_f5star		\
	test_kdf	\
	test_security_kasme
else
TESTS =
endif

test_common = \
	$(top_builddir)/auc/libauc.la	\
	libtest_utils.la	\
	@CHECK_LIBS@

up_loc_test_LDADD  = \
	libtest_utils.la	\
	@CHECK_LIBS@

test_security_f1_LDADD = \
	$(test_common)

test_security_f2_f3_f5_LDADD = \
	$(test_common)

test_security_f4_f5star_LDADD = \
	$(test_common)

test_security_kasme_LDADD = \
	$(test_common)

test_kdf_LDADD = \
	$(test_common)

noinst_LTLIBRARIES = libtest_utils.la
libtest_utils_la_SOURCES =	\
	test_utils.h 	test_utils.c	\
	test_fd.h	test_fd.c

check_PROGRAMS = 			\
	up_loc_test test_security_f1 	\
	test_security_f2_f3_f5 		\
	test_security_f4_f5star 	\
	test_security_kasme	\
	test_kdf

up_loc_test_SOURCES = up_loc_test.c
test_security_f1_SOURCE = test_security_f1.c
test_security_f2_f3_f5_SOURCE = test_security_f2_f3_f5.c
test_security_f4_f5star_SOURCE = test_security_f4_f5star.c
test_security_kasme_SOURCE = test_security_kasme.c
test_kdf_SOURCE = test_kdf.c