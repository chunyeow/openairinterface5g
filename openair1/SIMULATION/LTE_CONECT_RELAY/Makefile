include $(OPENAIR_HOME)/common/utils/Makefile.inc

TOP_DIR = $(OPENAIR1_DIR)
OPENAIR1_TOP = $(OPENAIR1_DIR)
OPENAIR2_TOP = $(OPENAIR2_DIR)
OPENAIR3 = $(OPENAIR3_DIR)

CFLAGS += -DPHYSIM -DNODE_RG -DUSER_MODE -DPC_TARGET -DPC_DSP -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TXRX=2 -DNB_ANTENNAS_TX=2 -DPHY_CONTEXT=1  -rdynamic -DMALLOC_CHECK_=1 # -Wno-packed-bitfield-compat

LFLAGS = -lm -lblas -lxml2 -lrt

CFLAGS += -m32 -DOPENAIR_LTE #-DOFDMA_ULSCH #-DIFFT_FPGA -DIFFT_FPGA_UE
#CFLAGS += -DTBS_FIX
CFLAGS += -DCELLULAR

ASN1_MSG_INC = $(OPENAIR2_DIR)/RRC/LITE/MESSAGES

ifdef EMOS
CFLAGS += -DEMOS
endif

ifdef DEBUG_PHY
CFLAGS += -DDEBUG_PHY
endif

ifdef MeNBMUE
CFLAGS += -DMeNBMUE
endif

ifdef MU_RECEIVER
CFLAGS += -DMU_RECEIVER
endif

ifdef ZBF_ENABLED
CFLAGS += -DNULL_SHAPE_BF_ENABLED
endif

ifdef RANDOM_BF
CFLAGS += -DRANDOM_BF
endif

ifdef PBS_SIM
CFLAGS += -DPBS_SIM
endif

# Debugging flags
#CFLAGS += -DDEBUG_DLSCH_CODING
#CFLAGS += -DDEBUG_DLSCH_DECODING 
#CFLAGS += -DDEBUG_LOGMAP

# Always use this flag with relaysim. Will be fixed in future versions.
CFLAGS += -DREL_AMPLIFY_FORWARD

# Use this flag if you want to run relaysim with only 1 relay node. 
#CFLAGS += -DSINGLE_RELAY

# This is a flag to use different patterns for RVI (redundancy versions)
#  Normally it is (0,0,1,1). If you use this flag, it is used as (0,1,1,0).
CFLAGS += -DRVI_PATTERN_ALT

# This flag is used in relayQMFsim where relays are Quantize-Forward relays. 
#  with this option you get results which assumes there is no error in the first link (Source-Relays).
# This kind of an upper bound to the performance of QMF relays in this scenario after LLR adjustment at the destination.
#CFLAGS += -DQF_UPPER_BOUND

CFLAGS += -DNO_RRM #-DOPENAIR2 -DPHY_ABSTRACTION

ifdef XFORMS
CFLAGS += -DXFORMS
LFLAGS += -lforms
endif

ifdef PERFECT_CE
CFLAGS += -DPERFECT_CE
endif

ifdef BIT8_TX
CFLAGS += -DBIT8_TX
endif

CFLAGS += -DNO_RRM -DOPENAIR1 #-DOPENAIR2 #-DPHY_ABSTRACTION

CFLAGS += -I/usr/include/X11 -I/usr/X11R6/include

ifdef ENABLE_FXP
CFLAGS += -DENABLE_FXP # Fxp only
else
 ifdef ENABLE_FLP
 CFLAGS += -DENABLE_FLP # dual_stream_correlation(), channel_compensation_prec() and qam16_qam16_mu_mimo() are flp (independently)	
 else
  ifdef ENABLE_FULL_FLP
  CFLAGS += -DENABLE_FULL_FLP # Flp inside of rx_pdsch() (dlsch_detection_mrc(), dual_stream_correlation(), channel_compensation_prec(), qam16_qam16_mu_mimo() and dlsch_16qam_16qam_llr)
  else
  CFLAGS += -DENABLE_FXP # Fxp only by default
  endif
 endif
endif

ifdef COMPARE_FLP_AND_FXP
CFLAGS += -DCOMPARE_FLP_AND_FXP
endif

include $(TOP_DIR)/PHY/Makefile.inc
#SCHED_OBJS = $(TOP_DIR)/SCHED/phy_procedures_lte_common.o $(TOP_DIR)/SCHED/phy_procedures_lte_eNb.o $(TOP_DIR)/SCHED/phy_procedures_lte_ue.o
include $(TOP_DIR)/SCHED/Makefile.inc
include $(TOP_DIR)/SIMULATION/Makefile.inc
include $(OPENAIR2_DIR)/LAYER2/Makefile.inc
include $(OPENAIR2_DIR)/UTIL/Makefile.inc
include $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.inc

CFLAGS += $(L2_incl) -I$(ASN1_MSG_INC) -I$(TOP_DIR) -I$(OPENAIR3) $(UTIL_incl)
# EXTRA_CFLAGS = 

#STATS_OBJS += $(TOP_DIR)/ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_proc.o

#LAYER2_OBJ += $(OPENAIR2_DIR)/LAYER2/MAC/rar_tools.o
LAYER2_OBJ = $(OPENAIR2_DIR)/LAYER2/MAC/lte_transport_init.o

OBJ = $(PHY_OBJS) $(SIMULATION_OBJS) $(TOOLS_OBJS) $(SCHED_OBJS) $(LAYER2_OBJ) $(LOG_OBJS) #$(ASN1_MSG_OBJS) 
#OBJ2 = $(PHY_OBJS) $(SIMULATION_OBJS) $(TOOLS_OBJS)  

ifdef XFORMS
OBJ += ../../USERSPACE_TOOLS/SCOPE/lte_scope.o
endif

OBJ += openair_hw.o

all:  relaysim

$(OBJ) : %.o : %.c
	@echo 
	@echo Compiling $< ...
	$(CC) -c $(CFLAGS) -o $@ $<

relaysim : $(OBJ) relaysim.c
	@echo "Compiling relaysim.c ..."
	$(CC) relaysim.c  -o relaysim $(CFLAGS) $(OBJ) $(LFLAGS) #-static -L/usr/lib/libblas

clean :
	rm -f $(OBJ)
	rm -f *.o

cleanall : clean
	rm -f relaysim
	rm -f *.exe*

showcflags :
	@echo $(CFLAGS)

