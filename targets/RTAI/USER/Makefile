# Include some shared directives
include $(OPENAIR_TARGETS)/Makerules

default: lte-softmodem
all: lte-softmodem

include $(OPENAIR_TARGETS)/Makefile.common

GCCVERSION = $(shell gcc --version | grep ^gcc | sed 's/^.* //g')
ifeq "$(GCCVERSION)"  "4.4.3"
    CFLAGS += -Wno-packed-bitfield-compat
endif
ifeq "$(GCCVERSION)" "4.5.2"
    CFLAGS += -Wno-packed-bitfield-compat
endif
ifeq "$(GCCVERSION)" "4.6.1"
    CFLAGS += -Wno-packed-bitfield-compat
endif

CFLAGS += -O2
CFLAGS += -DDRIVER2013 -I$(OPENAIR_TARGETS)/ARCH/EXMIMO/USERSPACE/LIB/ -I$(OPENAIR_TARGETS)/ARCH/EXMIMO/DEFS -DENABLE_VCD_FIFO

SRC = synctest.c condtest.c #lte-softmodem.c

ifndef RTAI
RTAI=1
endif

ifndef USRP
USRP=0
endif

ifeq ($(RTAI),1)
CFLAGS += -DENABLE_RTAI_CLOCK
CFLAGS += -DCONFIG_RTAI_LXRT_INLINE  #remend the RTAI warning
#RTAI_OBJ = sched_dlsch.o sched_ulsch.o sched_rx_pdsch.o lte-softmodem.o rt_wrapper.o 
RTAI_OBJ = sched_dlsch.o sched_ulsch.o sched_rx_pdsch.o rt_wrapper.o 
ifeq ($(USRP),1)
RTAI_OBJ += lte-softmodem-usrp.o
else
RTAI_OBJ += lte-softmodem.o
endif
else
#OBJ = sched_dlsch.o sched_ulsch.o sched_rx_pdsch.o lte-softmodem.o rt_wrapper.o
OBJ = sched_dlsch.o sched_ulsch.o sched_rx_pdsch.o rt_wrapper.o
ifeq ($(USRP),1)
OBJ += lte-softmodem-usrp.o
else
OBJ += lte-softmodem.o
endif
CFLAGS += -DENABLE_USE_CPU_EXECUTION_TIME
endif

OBJ += $(OPENAIR1_DIR)/SIMULATION/TOOLS/taus.o $(OPENAIR_TARGETS)/SIMU/USER/init_lte.o #$(OPENAIR_TARGETS)/ARCH/EXMIMO/USERSPACE/LIB/openair0_lib.o

ifeq ($(USRP),1)
include $(OPENAIR_TARGETS)/ARCH/USRP/USERSPACE/LIB/Makefile.inc
endif

OBJ += $(OPENAIR_TARGETS)/ARCH/EXMIMO/USERSPACE/LIB/openair0_lib.o
CFLAGS += -DDRIVER2013 -I$(OPENAIR_TARGETS)/ARCH/EXMIMO/USERSPACE/LIB/ -I$(OPENAIR_TARGETS)/ARCH/EXMIMO/DEFS -DENABLE_VCD_FIFO

TOP_DIR = $(OPENAIR1_DIR)
include $(OPENAIR1_DIR)/PHY/Makefile.inc
include $(OPENAIR1_DIR)/SCHED/Makefile.inc

OBJ += $(PHY_OBJS)
ifeq ($(RTAI),1)
RTAI_OBJ += $(SCHED_OBJS)
else
OBJ += $(SCHED_OBJS)
endif

OPENAIR2_TOP = $(OPENAIR2_DIR)
include $(OPENAIR2_DIR)/LAYER2/Makefile.inc
include $(OPENAIR2_DIR)/UTIL/Makefile.inc
include $(OPENAIR2_DIR)/RRC/NAS/Makefile.inc

ifeq ($(RTAI),1)
LOG_OBJS=
RTAI_OBJ+=$(LOG_DIR)/vcd_signal_dumper.o
RTAI_OBJ+=$(LOG_DIR)/log.o
endif

include $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.inc

OBJ += $(LOG_OBJS) 

ifndef OPENAIR2
OPENAIR2=1
endif
ifeq ($(OPENAIR2),1)
ASN1_MSG_OBJS1=$(addprefix $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/, $(ASN1_MSG_OBJS))
OBJ += $(L2_OBJS) $(LIST_OBJ) $(TIMER_OBJ) $(MEM_OBJ) $(OTG_OBJS) $(MATH_OBJS)
OBJ += $(OPENAIR1_DIR)/SIMULATION/ETH_TRANSPORT/netlink_init.o
#OBJ += $(PDCP_DIR)/pdcp_thread.o
CFLAGS += -DOPENAIR2 -DNO_RRM -DPUCCH -DMAC_CONTEXT=1
endif

ifdef ENABLE_ITTI
RTAI_OBJ += $(UTILS_OBJS)
endif

CFLAGS += $(L2_incl) $(UTIL_incl) $(UTILS_incl)

CFLAGS += -I$(OPENAIR1_DIR) -I$(OPENAIR2_DIR)/RRC/LITE/MESSAGES	#-I$(OPENAIR3_DIR)/MESH -I$(OPENAIR3_DIR)/MESH/RRM

CFLAGS += -DNEW_FFT -DLOG_NO_THREAD

ifeq ($(XFORMS),1)
CFLAGS += -DXFORMS -I/usr/include/X11
LDFLAGS += -lforms
OBJ += $(OPENAIR1_DIR)/PHY/TOOLS/lte_phy_scope.o
OBJ += stats.o
endif

ifdef SMBV
CFLAGS += -DSMBV
endif

#CPUFLAGS = -mmmx -msse -msse2 -m32 -mssse3 -msse4.1
CPUFLAGS = -mmmx -msse -msse2 -mssse3 -msse4.1
#CFLAGS += -fno-common -mpreferred-stack-boundary=4  
CFLAGS += -Wall -fno-strict-aliasing -DPHYSIM -DUSER_MODE -DPC_TARGET -DPC_DSP -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TXRX=2 -DNB_ANTENNAS_TX=2 -DPHY_CONTEXT=1 -rdynamic $(CPUFLAGS) # -Wno-packed-bitfield-compat
CFLAGS += -DOPENAIR_LTE -DENABLE_FXP -DOPENAIR1 -DDLSCH_THREAD #-DULSCH_THREAD

#only for CBMIMO1
ifdef CBMIMO1
CFLAGS += -DFW2011 -DBIT8_TX -DCBMIMO1
else
#only for EXPRESS MIMO
CFLAGS += -DEXMIMO
#CFLAGS += -DEXMIMO -DTIMING_ADVANCE_HW=138 #this is for ExpressMIMO 1
#CFLAGS += -DEXMIMO -DTIMING_ADVANCE_HW=45 #this is for ExpressMIMO 2
endif

ifeq ($(HARD_RT),1)
CFLAGS += -DHARD_RT
endif

ifeq ($(EMOS),1)
CFLAGS += -DEMOS #-DEMOS_CHANNEL
LDFLAGS += -lgps
endif

ifeq ($(NAS),1)
CFLAGS += -DNAS_NETLINK -DLINUX
OBJ += $(NAS_OBJS)
endif

RTAI_CFLAGS += $(shell rtai-config --lxrt-cflags) -DRTAI
LDFLAGS += -lpthread -lm -lforms
ifeq ($(RTAI),1)
LDFLAGS += $(shell rtai-config --lxrt-ldflags)
else
LDFLAGS += -lrt
endif

ifeq ($(USRP),1)
CFLAGS += -I/opt/uhd/include -L/opt/uhd/lib -luhd -lpthread -lstdc++
endif
#CFLAGS += -I/usr/include/c++/4.6 -I/usr/include/c++/4.6/x86_64-linux-gnu -I/usr/include/rtai/

LFDS_OBJ_DIR		 = $(subst $(OPENAIR_HOME),$(OBJS_DIR),$(LFDS_DIR))
LFDS_LIB		 = $(LFDS_OBJ_DIR)/bin/liblfds611.a
LIBS 			+= $(LFDS_LIB)
SHARED_DEPENDENCIES 	+= $(LFDS_LIB)

-include $(OBJ:.o=.d)
-include $(ASN1_MSG_OBJS1:.o=.d)
-include $(RTAI_OBJ:.o=.d)

$(LFDS_LIB):
	@if [ ! -d $(LFDS_OBJ_DIR)/bin ]; then mkdir -p $(LFDS_OBJ_DIR)/bin; fi;
	@if [ ! -d $(LFDS_OBJ_DIR)/obj ]; then mkdir -p $(LFDS_OBJ_DIR)/obj; fi;
	$(MAKE) -C $(LFDS_DIR) -f makefile.linux OUTDIR=$(LFDS_OBJ_DIR)

$(RTAI_OBJ): %.o : %.c
	@echo Compiling $< ...
	@$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) $(RTAI_CFLAGS) -o $@ $<
	@$(CC) -MM $(CFLAGS) $(EXTRA_CFLAGS) $(RTAI_CFLAGS) $< > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

ifdef ENABLE_ITTI
$(OBJ) $(RTAI_OBJ): $(ITTI_MESSAGES_H)
endif

$(OBJ) $(ASN1_MSG_OBJS1): %.o : %.c
	@echo Compiling $< ...
	@$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $<
	@$(CC) -MM $(CFLAGS) $(EXTRA_CFLAGS) $< > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

$(USRP_OBJ):$(USRP_FILE_OBJ)
	@echo Compiling openair_usrp.cpp
	@$(CXX) -c $(USRP_CFLAGS) $(USRP_FILE_OBJ) -o $(USRP_OBJ)

condtest: condtest.c 
	$(CC) $(CFLAGS) $(LDFLAGS) condtest.c -o condtest

synctest: $(OBJ) $(ASN1_MSG_OBJS1) $(RTAI_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) $(RTAI_OBJ) $(ASN1_MSG_OBJS1) -o synctest

lte-softmodem: $(OBJ) $(ASN1_MSG_OBJS1) $(RTAI_OBJ) $(SHARED_DEPENDENCIES)
	@echo Linking $@
	@$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(OBJ) $(RTAI_OBJ) $(ASN1_MSG_OBJS1) -o lte-softmodem $(LDFLAGS) $(LIBS)

lte-softmodem-usrp: $(OBJ) $(ASN1_MSG_OBJS1) $(RTAI_OBJ) $(USRP_OBJ) $(SHARED_DEPENDENCIES)
	@echo Linking $@
	@$(CC) $(USRP_OBJ) $(CFLAGS) $(OBJ) $(RTAI_OBJ) $(ASN1_MSG_OBJS1) -o lte-softmodem-usrp $(LDFLAGS) $(LIBS)

synctest_eNB: synctest
	cp synctest synctest_eNB

synctest_UE: synctest
	cp synctest synctest_UE

drivers:
	cd $(OPENAIR2_DIR) && make clean && make nasmesh_netlink.ko
	cd $(OPENAIR2_DIR)/NAS/DRIVER/MESH/RB_TOOL && make clean && make
	cd $(OPENAIR_TARGETS)/ARCH/EXMIMO/DRIVER/eurecom && make clean && make 
	cd $(OPENAIR_TARGETS)/ARCH/EXMIMO/USERSPACE/OAI_FW_INIT && make clean && make 

run: condtest
	rtai-load condtest --verbose

run_eNB_test: 
	rtai-load eNB_test --verbose

run_eNB: 
	rtai-load eNB --verbose

run_UE: 
	rtai-load UE --verbose

run_UE0:
	rtai-load UE0 --verbose

run_UE0_smbv:
	rtai-load UE0_smbv --verbose

run_UE850:
	rtai-load UE850 --verbose

run_eNB850:
	rtai-load eNB850 --verbose

run_UE0prach:
	rtai-load UE0prach --verbose

run_UE1prach:
	rtai-load UE1prach --verbose

run_UE2prach:
	rtai-load UE2prach --verbose

run_UE0noL2:
	rtai-load UE0noL2 -- verbose

run_UE1noL2: 
	rtai-load UE1noL2 --verbose

run_UE2noL2: 
	rtai-load UE2noL2 --verbose

run_UE0calib:
	rtai-load UE0calib --verbose

run_UE0calibmed:
	rtai-load UE0calibmed --verbose

run_UE0calibbyp:
	rtai-load UE0calibbyp --verbose

run_UE1: 
	rtai-load UE1 --verbose

run_UE2: 
	rtai-load UE2 --verbose

run_eNB0:
	rtai-load eNB0 --verbose

run_eNB1:
	rtai-load eNB1 --verbose

run_eNB2:
	rtai-load eNB2 --verbose

clean: common-clean
	@$(RM_F_V) $(OBJ) $(RTAI_OBJ)
	@$(RM_F_V) $(OBJ:.o=.d) $(RTAI_OBJ:.o=.d)
	@$(RM_F_V) $(ASN1_MSG_INC)/asn1_msg.o $(ASN1_MSG_INC)/asn1_msg.d

cleanasn1:
	rm -f $(ASN1_MSG_OBJS1)
	$(shell cd $(OPENAIR2_DIR)/RRC/LITE/MESSAGES ; rm -f $(ASN_MODULE_SOURCES) $(ASN_MODULE_HEADERS) *.o *.d )
	rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.am.sample
	rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.inc.generated
	rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel8
	rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel10

cleancell:
	rm -f $(OPENAIR2_DIR)/RRC/CELLULAR/*.o
	rm -f $(OPENAIR2_DIR)/RRC/CELLULAR/*.d
	rm -f $(OPENAIR2_DIR)/RRC/L2_INTERFACE/*.o
	rm -f $(OPENAIR2_DIR)/RRC/L2_INTERFACE/*.d
	rm -f $(OPENAIR2_DIR)/NAS/SIMU_CELLULAR/*.o
	rm -f $(OPENAIR2_DIR)/NAS/SIMU_CELLULAR/*.d

cleanalmostall: clean 
	rm -f $(ASN1_MSG_OBJS1)
	rm -rf condtest synctest lte-softmodem
	rm -rf synctest_eNB synctest_UE

cleanall: clean cleanasn1
	rm -rf condtest synctest lte-softmodem lte-softmodem-usrp
	rm -rf synctest_eNB synctest_UE

show:
	@echo $(CFLAGS)
	@echo $(EXTRA_CFLAGS)
	@echo $(OBJ) $(RTAI_OBJ) 
	@echo $(USRP_OBJ)
	@echo IS_REL8 is $(IS_REL8)
	@echo IS_REL10 is $(IS_REL10)

beautiful:
	astyle --style=gnu -s2 $(SRC)
