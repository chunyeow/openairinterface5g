CC = $(shell if [ $$OSTYPE = "cygwin" ] ; then echo "gcc-4";  else echo "gcc"; fi)
#CC = gcc-4.3

SSE3PROC = $(shell echo `grep ssse3 /proc/cpuinfo`) 
SSE4PROC = $(shell echo `grep sse4 /proc/cpuinfo`) 
CPUFLAGS = -mmmx -msse -msse2 -m32  #-mssse3 -msse4
CPUFLAGS += $(shell if [ -z $$SSE3PROC]; then echo "" ; else echo "-mssse3"; fi)
CPUFLAGS += $(shell if [ -z $$SSE4PROC]; then echo "" ; else echo "-msse4"; fi)

TOP_DIR      = ../..
OPENAIR1_TOP = ../..
OPENAIR2_TOP = ../../../openair2
OPENAIR3_TOP = ../../../openair3
OPENAIR3     = $(OPENAIR3_DIR)

CFLAGS = -Wall -fno-strict-aliasing -DPHYSIM -DNODE_RG -DUSER_MODE -DPC_TARGET -DPC_DSP -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TXRX=2 -DNB_ANTENNAS_TX=2 -DMAX_MODULES=1 -g -ggdb $(CPUFLAGS) -I/usr/include/X11 #-Wno-packed-bitfield-compat

ASN1_MSG_INC = ../../../openair2/RRC/LITE/MESSAGES

CFLAGS += -DOPENAIR_LTE -DOPENAIR2 #-DOFDMA_ULSCH -DIFFT_FPGA -DIFFT_FPGA_UE 

#CFLAGS += -DXFORMS

ifdef DEBUG_PHY
CFLAGS += -DDEBUG_PHY
endif

ifdef NAS_NETLINK
CFLAGS += -DNAS_NETLINK -DLINUX
LINUX=1
endif

CFLAGS += -DPHY_ABSTRACTION #:wq
-DEMIT_ASN_DEBUG=1

ifdef OCG
CFLAGS += -I/usr/include/libxml2 -lxml2
endif

include $(OPENAIR1_DIR)/PHY/Makefile.inc
include $(OPENAIR1_DIR)/SCHED/Makefile.inc
include $(OPENAIR2_DIR)/LAYER2/Makefile.inc
include $(OPENAIR1_DIR)/SIMULATION/ETH_TRANSPORT/Makefile.inc
include $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.inc

SIMULATION_OBJS  = $(TOP_DIR)/SIMULATION/TOOLS/gauss.o  
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/TOOLS/random_channel.o  
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/TOOLS/rangen_double.o  
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/TOOLS/taus.o  
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/TOOLS/multipath_channel.o
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/RF/rf.o
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/RF/adc.o
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/RF/dac.o

OBJ = $(PHY_OBJS) $(SIMULATION_OBJS) $(ETHERNET_TRANSPORT_OBJS) $(TOOLS_OBJS) $(SCHED_OBJS) $(STATS_OBJS) $(ASN1_MSG_OBJS)

all: physim


$(OBJ) : %.o : %.c
	$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) -DPHY_CONTEXT=1 -I$(TOP_DIR) $(L2_incl) -I$(ASN1_MSG_INC) -o $@ $<

$(L2_OBJS) : %.o : %.c
	$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) -DMAC_CONTEXT=1 -DPHY_CONTEXT=1 -I$(TOP_DIR) $(L2_incl) -I$(ASN1_MSG_INC) -o $@ $<

physim : $(OBJ) $(L2_OBJS) phy_procedures_sim.c phy_procedures_sim_form.c 
	$(CC)  phy_procedures_sim.c phy_procedures_sim_form.c -I$(TOP_DIR) $(L2_incl) -I$(ASN1_MSG_INC) -o physim $(CFLAGS) $(EXTRA_CFLAGS) $(OBJ) $(L2_OBJS) -lm -lforms -L/usr/local/lib -lforms -lX11 -L/usr/X11R6/lib -lXpm -lblas -lpthread

clean:
	rm -f physim
	rm -f $(OBJ)
	rm -f $(L2_OBJS)
	rm -f *.o
	rm -f *.exe*

cleanl1:
	rm -f physim
	rm -f $(OBJ)
	rm -f *.o
	rm -f *.exe
cleanl2:
	rm -f $(L2_OBJS)
