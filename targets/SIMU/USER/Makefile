CC = gcc
MPICC = gcc #mpicc

CPUFLAGS = -mmmx -msse -msse2 -msse4.1 -march=native
# FORCE ssse3 for compilation of openair on User Mode Linux
CPUFLAGS += $(shell if grep --silent ssse3 /proc/cpuinfo ; then echo "-mssse3" ; else if grep --silent User\ Mode\ Linux /proc/cpuinfo ; then echo "-mssse3" ; else echo ""; fi; fi)
CPUFLAGS += $(shell if grep --silent sse4 /proc/cpuinfo ; then echo "-msse4" ; else echo ""; fi)
linux = $(shell if [ `uname` = "Linux" ] ; then echo "1" ; else echo "0" ; fi)

NUM_CORES=$(shell cat /proc/cpuinfo | grep processor | wc -l)

TOP_DIR      = $(OPENAIR1_DIR)
OPENAIR1_TOP = $(OPENAIR1_DIR)
OPENAIR2_TOP = $(OPENAIR2_DIR)
OPENAIR3_TOP = $(OPENAIR3_DIR)
OPENAIR3     = $(OPENAIR3_DIR)

CFLAGS = -Wall -fno-strict-aliasing -DUSER_MODE -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TXRX=2 -DNB_ANTENNAS_TX=2 -g -ggdb $(CPUFLAGS) -I/usr/include/X11 #-Wno-packed-bitfield-compat

# Check if GCC version is greater or equal to 4.4
GCCVERSIONGTEQ44 := $(shell expr `gcc -dumpversion | cut -f1,2 -d. --output-delimiter=""` \>= 44)
ifeq "$(GCCVERSIONGTEQ44)" "1"
    CFLAGS += -Wno-packed-bitfield-compat
endif

ASN1_MSG_INC = $(OPENAIR2_DIR)/RRC/LITE/MESSAGES

CFLAGS += -DOPENAIR_LTE -DPUCCH #-DOFDMA_ULSCH -DIFFT_FPGA -DIFFT_FPGA_UE
CFLAGS += -DMAC_CONTEXT=1 -DPHY_CONTEXT=1 -DPHY_ABSTRACTION #-DPHY_ABSTRACTION_UL #-DRLC_UM_TEST_TRAFFIC=1
CFLAGS += -DNEW_FFT
#CFLAGS += -DLLR8

ifndef OPENAIR2
OPENAIR2=1
endif

ifndef OPENAIR_EMU
export OPENAIR_EMU=1
endif

ifdef EMOS
CFLAGS+=-DEMOS
endif

ifdef TRAFFIC_TM5
CFLAGS += -DRLC_UM_TEST_TRAFFIC=1 #-DFULL_BUFFER=1
endif

# activate OCG and libxml only under linux
ifeq ($(linux),1)
CFLAGS += -I/usr/include/libxml2 -L/usr/local/lib -I/usr/include/atlas -L/usr/X11R6/lib
OCG_FLAG = 1
cygwin=0
CFLAGS += -DLINUX
else
cygwin=1
OCG_FLAG = 0
endif

ifdef SMBV
CFLAGS += -DSMBV
endif

ifdef XFORMS
CFLAGS += -DXFORMS
endif

ifdef DB
# Check if libmysqlclient is installed and use it if found to store simulation data for postprocessing
ENABLE_DB = $(shell if [ `dpkg -l | grep libmysqlclient -c` = "0" ]; then  echo "0" ; else  echo "1" ; fi )
ifeq ($(ENABLE_DB), 1)
CFLAGS +=-I/usr/include/mysql -L/usr/lib/mysql -DENABLE_DB_STATS
DB_LDFLAGS = -lmysqlclient
endif 
endif

ifdef PRINT_STATS
CFLAGS += -DPRINT_STATS
endif

ifndef rrc_cellular
rrc_cellular = 0
rrc_cellular_eNB = 0
rrc_cellular_UE = 0
else
  ifeq ($(eNB_flag),1)
    rrc_cellular_eNB=1
  endif
  ifeq ($(UE_flag),1)
    rrc_cellular_UE=1
  endif
endif

ifdef DEBUG_PHY
CFLAGS += -DDEBUG_PHY
endif

ifdef JF
CFLAGS += -DJUMBO_FRAME
endif
ifdef LS
CFLAGS += -DLARGE_SCALE
endif

# ln -sf EUTRA-RRC-Definitions-a20.asn EUTRA-RRC-Definitions.asn
# make cleanasn1
#make all Rel10=1


#export IS_REL10=$(shell if [ -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel10 ] ; then  echo "1" ; else  echo "0" ; fi)
#export IS_REL8=$(shell if [ -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel8 ] ; then  echo "1" ; else  echo "0" ; fi)

ifdef CBA
CFLAGS += -DRel10
CFLAGS += -DCBA
@echo "Rel10=1 CBA=1"

else ifdef Rel10
CFLAGS += -DRel10
@echo "Rel10=1"
#ifeq ($(IS_REL10), 0)
# $(/bin/bash cd $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/ ; rm EUTRA-RRC-Definitions.asn ln -s EUTRA-RRC-Definitions-a20.asn EUTRA-RRC-Definitions.asn ; cd $(OPENAIR_TARGETS)/SIMU/USER/ ; rm -f $(ASN1_MSG_OBJS1) ; rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.inc.generated ; rm -f $(L2_OBJS) ; rm -f *.o ; rm -f oaisim ; rm -f oaisim_pad; touch $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel10 ; rm $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel8 ; $(OPENAIR_TARGETS)/SCRIPTS/asn1_generate_rrc_messaging_c_code.bash )
#endif
else # default is rel 8
@echo "Rel8=1"
CFLAGS += -DRel8
#ifeq ($(IS_REL8), 0)
# $(/bin/bash cd $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/ ; rm EUTRA-RRC-Definitions.asn ln -s EUTRA-RRC-Definitions-86.asn EUTRA-RRC-Definitions.asn ;  cd $(OPENAIR_TARGETS)/SIMU/USER/ ; rm -f $(ASN1_MSG_OBJS1) ; rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.inc.generated ; rm -f $(L2_OBJS); rm -f *.o ; rm -f oaisim ; touch $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel8 ; rm $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel10 ; $(OPENAIR_TARGETS)/SCRIPTS/asn1_generate_rrc_messaging_c_code.bash )
#endif
endif

ifeq ($(NAS), 1)
CFLAGS += -DNAS_NETLINK -DLINUX
NAS_FLAG=1
endif

ifdef TEST_OMG
CFLAGS += -DTEST_OMG
#CFLAGS += -DDEBUG_OMG
endif
# after the execution, a profiling output "gmon.out" will be created,
#then we need to run "gprof ./oaisim > gmon.txt" in the same directory as "gmon.out"
# you could also use the script "grpof2dot.py gmon.txt" for visual diagrams
ifdef GPROF
CFLAGS += -pg
endif

#require kernel patch for oai driver to enable address autoconfiguration (IPv6 only)
ifdef ADDCONF
CFLAGS+=-DADDRCONF
endif

ifdef OAI_NW_DRIVER_TYPE_ETHERNET
CFLAGS+=-DOAI_NW_DRIVER_TYPE_ETHERNET
endif

CFLAGS += $(shell if [ `uname -o` = "Cygwin" ] ; then echo "-DCYGWIN" ;fi)

CFLAGS += -DENABLE_FXP -DOAI_EMU -DENABLE_USE_CPU_EXECUTION_TIME -DXER_PRINT

ifneq ($(USE_MME), R8)
UPDATE_RELEASE_9=1
endif

ifdef SECU
NETTLE_FOUND = $(shell if pkg-config --exists nettle; then echo "1" ; else echo "0"; fi)
OPENSSL_FOUND = $(shell if pkg-config --exists openssl; then echo "1" ; else echo "0"; fi)

ifeq ($(NETTLE_FOUND), 0)
@echo "Nettle library >= 2.5 is not installed on your system, continuing with security disabled"
SECU=0
else
ifeq ($(OPENSSL_FOUND), 0)
@echo "openssl library is not installed on your system, continuing with security disabled"
SECU=0
else
CFLAGS += -DENABLE_SECURITY
OSA_LDFLAGS += `pkg-config --libs nettle`
OSA_LDFLAGS += `pkg-config --libs openssl`
endif
endif
endif

include $(OPENAIR1_DIR)/PHY/Makefile.inc
include $(OPENAIR1_DIR)/SCHED/Makefile.inc
include $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.inc
include $(OPENAIR2_DIR)/LAYER2/Makefile.inc
include $(OPENAIR1_DIR)/SIMULATION/ETH_TRANSPORT/Makefile.inc
include $(OPENAIR2_DIR)/RRC/NAS/Makefile.inc
include $(OPENAIR2_DIR)/UTIL/Makefile.inc
include $(OPENAIR2_DIR)/S1AP/Makefile.inc
include $(OPENAIR2_DIR)/S1AP/MESSAGES/Makefile.inc

#LOG_OBJS +=  $(LOG_DIR)/vcd_signal_dumper.o
#LOG_OBJS +=  $(LOG_DIR)/log.o

CFLAGS += -DENB_MODE
S1AP_BUILT_OBJS=

ifdef USE_MME
CFLAGS += -DENABLE_USE_MME
CFLAGS_S1AP += -DENB_MODE -DENABLE_USE_MME -DEMIT_ASN_DEBUG=1 -DUSER_MODE
S1AP_BUILT_OBJS += $(S1AP_OBJS) $(addprefix $(OPENAIR2_DIR)/S1AP/MESSAGES/, $(S1AP_ASN_MODULE_SOURCES))
endif

SIMULATION_OBJS  = $(TOP_DIR)/SIMULATION/TOOLS/gauss.o
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/TOOLS/random_channel.o
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/TOOLS/rangen_double.o
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/TOOLS/taus.o
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/TOOLS/multipath_channel.o
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/TOOLS/abstraction.o
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/RF/rf.o
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/RF/adc.o
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/RF/dac.o

ASN1_MSG_OBJS1=$(addprefix $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/, $(ASN1_MSG_OBJS))
#L2_OBJS =
#EXTRA_CFLAGS =

OAISIM_OBJS = channel_sim.o
OAISIM_OBJS += init_lte.o
OAISIM_OBJS += oaisim_config.o
OAISIM_OBJS += sinr_sim.o
OAISIM_OBJS += cor_SF_sim.o
#OAISIM_OBJS += phy_procedures_sim_form.o
OAISIM_OBJS += oaisim_functions.o
ifdef XFORMS
TOOLS_OBJS += $(OPENAIR1_DIR)/PHY/TOOLS/lte_phy_scope.o
endif

ifdef PAD
CFLAGS += -DPAD
endif

OAISIM_PAD_OBJS = discrete_event_generator.o
OAISIM_PAD_OBJS += threadpool.o
#deactivate the log offloading 
ifndef LOG_THREAD
CFLAGS +=-DLOG_NO_THREAD
endif

ifdef PROC
INT_OBJS +=$(OPENAIR_TARGETS)/SIMU/PROC/channel_sim_proc.o
INT_OBJS +=$(OPENAIR_TARGETS)/SIMU/PROC/Process.o
CFLAGS +=-DPROC
endif

CFLAGS += -DENABLE_VCD_FIFO
CFLAGS += -DENABLE_NEW_MULTICAST
# CFLAGS += -DENABLE_LOG_FIFO

# Check if libpgm is installed and use it if found instead of the unreliable
# multicast
ENABLE_PGM = $(shell if pkg-config --exists openpgm-5.1; then echo "1" ; else echo "0"; fi)
ifeq ($(ENABLE_PGM), 1)
CFLAGS += `pkg-config --cflags openpgm-5.1` -DENABLE_PGM_TRANSPORT
PGM_LDFLAGS = `pkg-config --libs openpgm-5.1`
endif

OBJ = $(PHY_OBJS) $(SIMULATION_OBJS) $(ETHERNET_TRANSPORT_OBJS) $(TOOLS_OBJS) $(SCHED_OBJS) $(STATS_OBJS) $(OAISIM_OBJS) $(NAS_OBJS) $(INT_OBJS) $(UTIL_OBJ)
ifeq ($(OPENAIR2),1)
OBJ += $(L2_OBJS)
endif

all: oaisim

pad: oaisim_pad

userclean: cleanall oaisim

printvars:
	@echo LINUX variable is $(linux)
	@echo CYGWIN variable is $(cygwin)
	@echo rrc_cellular variable is $(rrc_cellular)
	@echo EXTRA_CFLAGS = $(EXTRA_CFLAGS)
	@echo OAI Config generator is $(OCG_FLAG)
	@echo OAI NETLINK FLAG is $(NAS_FLAG)
	@echo SSE3 proc is $(SSE3PROC)
	@echo L2 objs are $(L2_OBJS)
	@echo eNB_flag is $(eNB_flag)
	@echo UE_flag is $(UE_flag)
	@echo S1AP objs: $(S1AP_BUILT_OBJS)
	@echo CFLAGS: $(CFLAGS)
	@echo Enable PGM: $(ENABLE_PGM)

ASN1RELDIR=R9.8
ifeq ($(USE_MME), R8)
	ASN1RELDIR=R8.10
else
	CFLAGS_S1AP += -DUPDATE_RELEASE_9
endif
S1AP_DIR=$(OPENAIR2_DIR)/S1AP
ASN1MESSAGESDIR=$(S1AP_DIR)/MESSAGES
ASN1DIR=$(ASN1MESSAGESDIR)/ASN1

-include $(OBJ:.o=.d)
-include $(ASN1_MSG_OBJS1:.o=.d)

$(OBJ) $(OAISIM_PAD_OBJS) $(ASN1_MSG_OBJS1) : %.o : %.c
	@echo Compiling $<
	@$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) -DPHY_CONTEXT=1 -I$(TOP_DIR) $(L2_incl) $(UTIL_incl) -I$(ASN1_MSG_INC) -o $@ $<
	@$(CC) -MM $(CFLAGS) $(EXTRA_CFLAGS) -DPHY_CONTEXT=1 -I$(TOP_DIR) $(L2_incl) $(UTIL_incl) -I$(ASN1_MSG_INC) $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

$(S1AP_BUILT_OBJS): %.o : %.c
	@echo Compiling $<
	@$(CC) -Wall -c $(CFLAGS_S1AP) $(S1AP_Incl) $(UTIL_incl) -o $@ $<

$(ASN1MESSAGESDIR)/s1ap_ieregen.stamp: $(ASN1DIR)/$(ASN1RELDIR)/S1AP-PDU-Contents.asn $(ASN1DIR)/asn1tostruct.py
	echo Timestamp > $@
	(cd $(ASN1DIR) && python $(ASN1DIR)/asn1tostruct.py -f$<)

$(ASN1MESSAGESDIR)/s1ap_asn1regen.stamp: $(ASN1DIR)/$(ASN1RELDIR)/S1AP-CommonDataTypes.asn $(ASN1DIR)/$(ASN1RELDIR)/S1AP-Constants.asn $(ASN1DIR)/$(ASN1RELDIR)/S1AP-IEs.asn $(ASN1DIR)/$(ASN1RELDIR)/S1AP-PDU.asn
	echo Timestamp > $@
	(cd $(ASN1MESSAGESDIR) && asn1c -gen-PER -fnative-types -fskeletons-copy $^)

$(S1AP_DIR)/libs1ap.a: $(ASN1MESSAGESDIR)/s1ap_ieregen.stamp $(ASN1MESSAGESDIR)/s1ap_asn1regen.stamp $(S1AP_BUILT_OBJS)
	@echo Creating $@
	$(AR) rcs $@ $^

$(LFDS_DIR)/bin/liblfds611.a:
	$(MAKE) -C $(LFDS_DIR) -f makefile.linux

oaisim_pad : $(OBJ) $(OAISIM_PAD_OBJS) $(ASN1_MSG_OBJS1) oaisim_pad.c
	@echo "Compiling oaisim_pad.c ..."
	@$(MPICC) -I$(TOP_DIR) $(L2_incl) $(UTIL_incl) -I$(ASN1_MSG_INC) $(S1AP_Incl) -o oaisim_pad $(CFLAGS) $(EXTRA_CFLAGS) $^ \
	 -lm -lblas -lpthread -llapack_atlas -lforms -lxml2 -lX11 -lXpm -lrt

ifdef USE_MME
oaisim : $(ASN1_MSG_OBJS1) $(OBJ) $(S1AP_DIR)/libs1ap.a oaisim.c $(LFDS_DIR)/bin/liblfds611.a
else
oaisim : $(ASN1_MSG_OBJS1) $(OBJ) oaisim.c $(LFDS_DIR)/bin/liblfds611.a
endif
	@echo "Compiling oaisim.c ..."
	@$(CC) -I$(TOP_DIR) $(L2_incl) $(UTIL_incl) -I$(ASN1_MSG_INC) $(S1AP_Incl) -o oaisim $(CFLAGS) $(EXTRA_CFLAGS) $^ \
	-lm -lblas -lpthread -llapack_atlas -lforms -lxml2 -lX11 -lXpm -lrt \
	$(LFDS_DIR)/bin/liblfds611.a $(PGM_LDFLAGS) $(DB_LDFLAGS) $(OSA_LDFLAGS)

ifeq ($(rrc_cellular_eNB),1)
	mv oaisim oaisim_eNB
endif
ifeq ($(rrc_cellular_UE),1)
	mv oaisim oaisim_UE
endif

oaisimCROWN : $(OBJ) $(ASN1_MSG_OBJS1) oaisimCROWN.c
	@echo "Compiling oaisimCROWN.c ..."
	@$(CC) oaisimCROWN.c -I$(TOP_DIR) $(L2_incl) $(UTIL_incl) -I$(ASN1_MSG_INC) $(S1AP_Incl) -o oaisimCROWN $(CFLAGS) $(EXTRA_CFLAGS) $(OBJ) $(ASN1_MSG_OBJS1) -lm -lblas -lpthread -llapack_atlas -lforms -lxml2 -lX11 -lXpm -lrt 

test_nasmesh_fix:
	(cd $(OPENAIR2_DIR)/NAS/DRIVER/MESH/RB_TOOL && $(MAKE))
	(cd $(OPENAIR2_DIR) && $(MAKE) nasmesh_netlink_address_fix.ko)
	(cp $(OPENAIR2_DIR)/NAS/DRIVER/MESH/nasmesh.ko . )
nasmesh_fix:
	(cd $(OPENAIR2_DIR)/NAS/DRIVER/MESH/RB_TOOL && $(MAKE))
	(cd $(OPENAIR2_DIR) && $(MAKE) nasmesh_netlink_address_fix.ko)
	(sudo insmod $(OPENAIR2_DIR)/NAS/DRIVER/MESH/nasmesh.ko)
nasmesh_nl:
	(cd $(OPENAIR2_DIR)/NAS/DRIVER/MESH/RB_TOOL && $(MAKE))
	(cd $(OPENAIR2_DIR) && $(MAKE) nasmesh_netlink.ko)
	(sudo insmod $(OPENAIR2_DIR)/NAS/DRIVER/MESH/nasmesh.ko)
rb_tool:
	(cd $(OPENAIR2_DIR)/NAS/DRIVER/MESH/RB_TOOL && $(MAKE))

nasmesh_install:
	(sudo rmmod nasmesh)
	(sudo insmod $(OPENAIR2_DIR)/NAS/DRIVER/MESH/nasmesh.ko)

nas_sim_mt_cellular:
	( cd $(OPENAIR2_DIR)/NAS/SIMU_CELLULAR && make nasmt_sim CELLULAR=1 NO_RRM=1)

nas_sim_rg_cellular:
	( cd $(OPENAIR2_DIR)/NAS/SIMU_CELLULAR && make nasrg_sim MASTER=1 CELLULAR=1 NO_RRM=1)

rrm_std_cellular:
	( cd $(OPENAIR2_DIR)/NAS/SIMU_CELLULAR && make cell_rrm CELL_RRM=1 CELLULAR=1 NO_RRM=1)


cleanall: clean cleanasn1 cleanmmeasn1

clean:
	$(MAKE) -C $(LFDS_DIR) -f makefile.linux clean
	rm -f oaisim
	rm -f oaisim_pad
	rm -f $(OBJ)
	rm -f $(S1AP_BUILT_OBJS)
	rm -f $(ASN1MESSAGESDIR)/libs1ap.a
	rm -f *.o
	rm -f *.exe*
	rm -f $(ASN1_MSG_INC)/asn1_msg.o
	rm -f  $(OBJ:.o=.d) $(ASN1_MSG_OBJS1:.o=.d)

cleanl1:
	rm -f oaisim
	rm -f $(PHY_OBJS) $(SCHED_OBJS)
	rm -f *.o
	rm -f *.exe

cleanl2:
	rm -f $(L2_OBJS)
	rm $(ASN1_MSG_INC)/asn1_msg.o

cleanasn1:
	rm -f $(ASN1_MSG_OBJS1)
	$(shell cd $(OPENAIR2_DIR)/RRC/LITE/MESSAGES ; rm -f $(ASN_MODULE_SOURCES) $(ASN_MODULE_HEADERS) *.o *.d )
	rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.am.sample
	rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.inc.generated
	rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel10
	rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel8

cleanmmeasn1:
	rm -f $(S1AP_BUILT_OBJS)
	rm -f $(S1AP_BUILT_OBJS:.o:.c)
	rm -f $(ASN1MESSAGESDIR)/libs1ap.a
	rm -f $(ASN1MESSAGESDIR)/s1ap_asn1regen.stamp
	rm -f $(ASN1MESSAGESDIR)/s1ap_ieregen.stamp

cleancell:
	rm -f $(OPENAIR2_DIR)/RRC/CELLULAR/*.o
	rm -f $(OPENAIR2_DIR)/RRC/CELLULAR/*.d
	rm -f $(OPENAIR2_DIR)/RRC/L2_INTERFACE/*.o
	rm -f $(OPENAIR2_DIR)/RRC/L2_INTERFACE/*.d
	rm -f $(OPENAIR2_DIR)/NAS/SIMU_CELLULAR/*.o
	rm -f $(OPENAIR2_DIR)/NAS/SIMU_CELLULAR/*.d

print:
#	@echo $(cygwin)
	@echo $(OBJ)
	@echo $(OCG_FLAG)
	@echo "CFLAGS are "      $(CFLAGS)
	@echo "EXTRA_CFLAGS are" $(EXTRA_CFLAGS)

	@echo "IS_REL8 is "      $(IS_REL10)
	@echo "IS_REL8 is "      $(IS_REL8)
	@echo "ENABLE_DB is "    $(ENABLE_DB)

showcflags:
	@echo $(CFLAGS)


otg_all : otg_latency otg_gp

# need to check if the file exists before calling otgplot
otg_latency:
	export TITLE="Application OWD(ms)" 
	cp /tmp/otg_latency.dat /tmp/onewaydelay.dat	
	$(OPENAIR2_DIR)/UTIL/OTG/OTGplot "/tmp/otg_latency.dat" "[$(COL1):$(COL2)]"
otg_latency_bg:
	export TITLE="Background OWD(ms)" 
	$(OPENAIR2_DIR)/UTIL/OTG/OTGplot "/tmp/otg_latency_bg.dat" "[$(COL1):$(COL2)]"
otg_gp:
	export TITLE="Application Goodput(Kb/s)" 
	cp /tmp/otg_GP.dat /tmp/goodput.dat
	$(OPENAIR2_DIR)/UTIL/OTG/OTGplot "/tmp/goodput.dat" "[$(COL1):$(COL2)]"
otg_gp_bg:
	export TITLE="Background Goodput(Kb/s)" 
	$(OPENAIR2_DIR)/UTIL/OTG/OTGplot "/tmp/otg_GP_bg.dat" "[$(COL1):$(COL2)]"
otg_jitter:
	export TITLE="Aplication Jitter(ms)" 
	$(OPENAIR2_DIR)/UTIL/OTG/OTGplot "/tmp/otg_jitter.dat" "[$(COL1):$(COL2)]"

pexpect = $(shell if [ `dpkg -l | grep pexpect -c` = "0" ]; then  echo "0" ; else  echo "1" ; fi )

pre-ci: 
	@echo "Pre-commit check"

ifeq 	($(pexpect), 1) 
ifndef	PW
	@echo "Usage: make pre-ci PW=mypassword"
	@echo "Password is used to access the local host through ssh."
	python $(OPENAIR_TARGETS)/TEST/OAI/test01.py 
else
	python $(OPENAIR_TARGETS)/TEST/OAI/test01.py -w $(PW)
endif
else
	@echo "Python pexpect package is missing, please install"
endif 
