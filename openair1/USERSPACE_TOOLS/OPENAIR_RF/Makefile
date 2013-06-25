CCC = gcc

TOP_DIR = ../..
OPENAIR2_DIR = ../../../openair2
OPENAIR2_TOP = $(OPENAIR2_DIR)

CFLAGS = -Wall -DNODE_RG -DUSER_MODE -DPC_TARGET -DPC_DSP -DMAX_MODULES=1 -DPHY_CONTEXT=1  -g -mtune=pentium4 -mmmx -msse2 -mssse3 -O -funroll-loops -I$(TOP_DIR) -I/usr/realtime/include/ -I$(OPENAIR2_DIR)/RRC/LITE/MESSAGES
#-DICC 

include $(OPENAIR2_DIR)/LAYER2/Makefile.inc
include $(OPENAIR2_DIR)/UTIL/Makefile.inc
CFLAGS += $(L2_incl) $(UTIL_incl)

ifdef DEBUGPHY
CFLAGS += -DDEBUG_PHY
endif

ifeq ($(CBMIMO1),1)
CFLAGS += -DHW_PREFIX_REMOVAL -DCBMIMO1 -DNB_ANTENNAS_RX=2 -DOPEN_OUTPUT_FILE 
endif

ifeq ($(OPENAIR_LTE), 1)
CFLAGS += -DOPENAIR_LTE -DIFFT_FPGA 
FNAME = cbmimo1_lte
else
FNAME = cbmimo1
endif

CFLAGS += -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TX=2 -DNB_ANTENNAS_TXRX=2	

ifeq ($(EMOS),1)
CFLAGS += -DEMOS
endif

# PHY Initialization routines
ifeq ($(OPENAIR_LTE), 0)
PHY_OBJS        +=$(TOP_DIR)/PHY/INIT/init.o
else
PHY_OBJS	+=$(TOP_DIR)/PHY/INIT/lte_parms.o 
#PHY_OBJS	+=$(TOP_DIR)/PHY/INIT/lte_init.o 
endif

# PHY TOOLS routines
#PHY_OBJS +=$(TOP_DIR)/PHY/TOOLS/fft.o 

USERRFOBJS += $(PHY_OBJS) openair_rf.o #openair_generate_ofdm.o

all: openair_rf

$(USERRFOBJS) : %.o : %.c
	$(CCC) -c $(CFLAGS) -o $@ $<

openair_rf: $(USERRFOBJS)
	$(CCC) -o openair_rf_$(FNAME) $(CFLAGS) $(USERRFOBJS) -lm  

read_rx_sig: read_rx_sig.c
	$(CCC) -o read_rx_sig $(CFLAGS) read_rx_sig.c 

clean:
	rm -f $(USERRFOBJS)


