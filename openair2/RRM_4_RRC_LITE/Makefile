PLATFORM =	linux-x86-32
NUM_PROC = $(shell echo `cat /proc/cpuinfo | grep processor | wc -l`)
NUM_PROC = 1

LIBS =		-L/usr/local/lib -Llib -lSockets -lasn1rrc -lpthread -lssl -lcrypto -lssl -lxml2

#------------------------------------------------
# DIRS
#------------------------------------------------
UPDIR	:= $(shell /bin/pwd)
BIN_DIR             = bin
SRC_DIR             = $(UPDIR)/src
COM_DIR             = $(SRC_DIR)/com
FOREIGN_DIR         = $(SRC_DIR)/foreign
SOCKETS_DIR         = $(FOREIGN_DIR)/sockets-2.3.9.4
TCPIP_DIR           = $(FOREIGN_DIR)/tcpip
SQL_DIR             = $(FOREIGN_DIR)/sql
ASN1_RRC_SPEC_DIR   = $(FOREIGN_DIR)/generated_c_asn1_rrc/ASN1_files
RRC_RRM_ASN1_DIR    = $(COM_DIR)/asn1
RADIO_RESOURCES_DIR = $(SRC_DIR)/radio_resources
RRC_DIR             = ../RRC/LITE
TEST_DIR            = $(SRC_DIR)/test


#------------------------------------------------
# INCLUDES
#------------------------------------------------
include		Makefile.objs
include		Makefile.Defines.$(PLATFORM)
INCLUDE =	-I$(SRC_DIR) -I$(SOCKETS_DIR)  -I$(SQL_DIR)  -I$(COM_DIR)  -I$(COM_DIR)/rrc  -I$(COM_DIR)/rrm  \
-I$(TCPIP_DIR) -I$(RADIO_RESOURCES_DIR) -I$(RRC_RRM_ASN1_DIR) -I$(TEST_DIR)


#------------------------------------------------
# CFLAGS
#------------------------------------------------

CPPFLAGS =	 $(CFLAGS)
#CPPFLAGS +=	-fpermissive -std=gnu++98

#------------------------------------------------
# LIBS
#------------------------------------------------
libasn1rrc:	libasn1rrc.a
		mv libasn1rrc.a lib

libsqlite:	libsqlite.a
		mv libsqlite.a lib


libSockets:	libSockets.a
		mv libSockets.a lib


libasn1rrc.a:	$(ASN1RRC_OBJS)
		ar cru $@ $^
		ranlib $@

libSockets.a:	$(SOCKETS_OBJS)
		ar cru $@ $^
		ranlib $@

libsqlite.a:	$(SQLITE_OBJS)
		ar cru $@ $^
		ranlib $@

LDFLAGS +=	-rdynamic

#------------------------------------------------
# PROGS
#------------------------------------------------
PROGS =		rrm_exe test_exe

gen-c-from-asn1:
		#-fbless-SIZE
		# Allow SIZE() constraint for INTEGER, ENUMERATED, and other types
		# for which this constraint is normally prohibited	by  the  stan-
		# dard.  This  is  a violation of ASN.1 standard, and the compiler
		# may fail to produce a meaningful code.

		#-fcompound-names
		# Using this option prevents name collisions in the target	source
		# code  by	using  complex	names  for target language structures.
		# (Name collisions may occur if the ASN.1 module reuses  the  same
		# identifiers in multiple contexts).

		#-findirect-choice
		# When  generating code for a CHOICE type, compile the CHOICE mem-
		# bers as indirect pointers instead of declaring them inline. Con-
		# sider  using this option together with -fno-include-deps to pre-
		# vent circular references.

		#-fknown-extern-type=<name>
		# Pretend the specified type is known. The	compiler  will	assume
		# the  target  language  source files for the given type have been
		# provided manually.

		#-fno-constraints
		# Do not generate ASN.1 subtype constraint checking code. This may
		# make a shorter executable.

		#-fno-include-deps
		# Do  not  generate  courtesy #include lines for non-critical type
		# dependencies.  Helps prevent namespace collisions.

		#-funnamed-unions
		# Enable unnamed unions in the definitions	of  target  language's
		# structures.
		#cd $(RRC_RRM_ASN1_DIR);asn1c -gen-PER -fcompound-names -fnative-types -fskeletons-copy  $(ASN1_RRC_SPEC_DIR)/EUTRA-RRC-Definitions.asn  $(RRC_RRM_ASN1_DIR)/rrc-rrm.asn; cd -
		cd $(RRC_RRM_ASN1_DIR);asn1c -gen-PER  -fno-include-deps -fcompound-names -fnative-types -fskeletons-copy  $(ASN1_RRC_SPEC_DIR)/EUTRA-RRC-Definitions.asn  $(RRC_RRM_ASN1_DIR)/rrc-rrm.asn; cd -


libs:
	make -j$(NUM_PROC) libasn1rrc libsqlite libSockets


all:	libs
	make -j$(NUM_PROC) $(PROGS)


rrm:	$(RRM_OBJS) lib/libSockets.a lib/libasn1rrc.a
	g++ -o $@ $^ $(LIBS)
	mv rrm $(BIN_DIR)

rrm_exe:	clean_test_objs
	make -j$(NUM_PROC) rrm



clean_test_objs:
	find $(COM_DIR)/rrc/ -name '*Response.o' -delete -print;
	find $(COM_DIR)/rrc/ -name '*Reconfiguration.o' -delete -print;

dummy_rrc:	CFLAGS += -DTEST_SCENARIO=1

dummy_rrc:	$(TEST_OBJS) lib/libSockets.a lib/libasn1rrc.a
	g++  -o  $@ $^ $(LIBS)
	mv dummy_rrc $(BIN_DIR)

test_exe:	clean_test_objs
	make -j$(NUM_PROC) dummy_rrc






clean:
	find -name '*.o' -delete -print; \
	find -name '*.bak' -delete -print; \
	find -name '*.d' -delete -print; \
	find -name '*~' -delete -print; \
	rm -f bin/*; \
	rm -f lib/*

cleanrrm:
	find src/com/rrc -name '*.o'  -delete -print; \
	find src/com/rrm -name '*.o'  -delete -print; \
	find src/com -name 'Message.o'  -delete -print; \
	find src/radio_resources -name '*.o' -delete -print; \
	find src/com -name '*.bak' -delete -print; \
	find src/radio_resources -name '*.bak' -delete -print; \
	find src/com -name '*.d' -delete -print; \
	find src/radio_resources -name '*.d' -delete -print; \
	find src/com -name '*~' -delete -print; \
	find src/radio_resources -name '*~' -delete -print; \
	rm -f bin/*; \

-include	*.d
