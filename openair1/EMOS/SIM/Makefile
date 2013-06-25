CC = gcc

TOP_DIR 		   =../..
PHY_DIR                    =../../PHY
PHY_INIT_DIR               =$(PHY_DIR)/INIT
PHY_MODULATION_DIR         =$(PHY_DIR)/MODULATION
PHY_CODING_DIR             =$(PHY_DIR)/CODING
PHY_ESTIMATION_DIR         =$(PHY_DIR)/ESTIMATION
PHY_CONFIG_DIR             =$(PHY_DIR)/CONFIG
PHY_TOOLS_DIR		   =$(PHY_DIR)/TOOLS
PHY_TRANSPORT_DIR	   =$(PHY_DIR)/TRANSPORT
SIM_TOOLS_DIR 		   =../../SIMULATION/TOOLS
PHY_MAC_INTERFACE_DIR      =../MAC_INTERFACE
SCHED_DIR                  =../SCHED


CFLAGS = -DEMOS -DNODE_RG -DUSER_MODE -DPC_TARGET -DPC_DSP -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TX=2 -DNB_ANTENNAS_TXRX=2 -DMAX_MODULES=1 -DPHY_CONTEXT=1 -g -mtune=pentium4 -msse2 -mmmx -msse2 -O -funroll-loops -I/usr/realtime/include/
#-DICC 

ifdef DEBUGPHY
CFLAGS += -DDEBUG_PHY
endif


# PHY Initialization routines
PHY_OBJS = $(PHY_INIT_DIR)/init.o 

# PHY Modulation routines
PHY_OBJS += $(PHY_MODULATION_DIR)/ofdm_mod.o 

# PHY Coding routines
PHY_OBJS += $(PHY_CODING_DIR)/ccoding_byte.o $(PHY_CODING_DIR)/crc_byte.o $(PHY_CODING_DIR)/viterbi.o 

# PHY Transport routines
PHY_OBJS += $(PHY_TRANSPORT_DIR)/chsch.o $(PHY_TRANSPORT_DIR)/chbch.o $(PHY_TRANSPORT_DIR)/sch.o $(PHY_TRANSPORT_DIR)/sach.o $(PHY_TRANSPORT_DIR)/mrbch.o

# PHY Estimation routines
PHY_OBJS += $(PHY_ESTIMATION_DIR)/synch_time.o $(PHY_ESTIMATION_DIR)/channel_estimation.o $(PHY_ESTIMATION_DIR)/channel_estimation_emos.o 

# Utility routines
UTIL_OBJS +=$(PHY_TOOLS_DIR)/dB_routines.o $(PHY_TOOLS_DIR)/file_output.o $(PHY_TOOLS_DIR)/memory_routines.o $(PHY_TOOLS_DIR)/signal_energy.o $(PHY_TOOLS_DIR)/sqrt.o $(PHY_TOOLS_DIR)/log2_approx.o $(SIM_TOOLS_DIR)/random_channel.o $(SIM_TOOLS_DIR)/rangen_double.o $(PHY_TOOLS_DIR)/fft.o $(PHY_TOOLS_DIR)/cmult_sv.o $(PHY_TOOLS_DIR)/cmult_vv.o $(PHY_TOOLS_DIR)/phase_comp.o $(TOP_DIR)/PHY/TOOLS/cmult_vvh.o $(TOP_DIR)/PHY/TOOLS/cmult_mm.o $(TOP_DIR)/PHY/TOOLS/cadd_sv.o

# CONFIG routines
CONFIG_OBJS +=$(PHY_CONFIG_DIR)/openair_configure.o $(PHY_CONFIG_DIR)/openair_readconfigfile.o

OBJ = $(PHY_OBJS) $(UTIL_OBJS) $(CONFIG_OBJS)

$(OBJ) : %.o : %.c 
	$(CC) -c $(CFLAGS) -I$(TOP_DIR) -o $@ $<

emos_sim : $(OBJ) emos_sim.c
	$(CC)  emos_sim.c -I$(TOP_DIR) -o emos_sim $(CFLAGS) $(PHY_OBJS) $(UTIL_OBJS) $(CONFIG_OBJS) -lm

all: emos_sim

clean:
	rm -f emos_sim
	rm -f *.o
	rm -f $(PHY_DIR)/*.o
	rm -f $(PHY_INIT_DIR)/*.o
	rm -f $(PHY_MODULATION_DIR)/*.o
	rm -f $(PHY_CODING_DIR)/*.o
	rm -f $(PHY_ESTIMATION_DIR)/*.o
	rm -f $(PHY_CONFIG_DIR)/*.o
	rm -f $(PHY_TOOLS_DIR)/*.o
	rm -f $(PHY_TRANSPORT_DIR)/*.o
	rm -f $(PHY_MAC_INTERFACE_DIR)/*.o
	rm -f $(SCHED_DIR)/*.o

