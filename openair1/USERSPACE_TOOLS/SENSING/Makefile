
CC = gcc

TOP_DIR = ../..

CFLAGS = -DNODE_RG -DUSER_MODE -DPC_TARGET -DPC_DSP -DNB_ANTENNAS=2 -DNB_ANTENNAS_TX=2 -DDEBUG_PHY -DCBMIMO1 -mmmx -msse -msse2 -g -ggdb

# PHY Initialization routines
OBJS	=$(TOP_DIR)/PHY/INIT/init.o 

# PHY TOOLS routines
OBJS +=$(TOP_DIR)/PHY/TOOLS/fft.o $(TOP_DIR)/PHY/TOOLS/cmult_vv.o $(TOP_DIR)/PHY/TOOLS/cmult_sv.o 
#ifdef CBMIMO1
OBJS +=$(TOP_DIR)/PHY/TOOLS/8bit_txmux.o
#endif

# PHY ESTIMATION routines 
OBJS += $(TOP_DIR)/PHY/ESTIMATION/channel_estimation.o 

# PHY MODULATION routines
OBJS +=$(TOP_DIR)/PHY/MODULATION/ofdm_mod.o

# PHY Transport routines
OBJS += $(TOP_DIR)/PHY/TRANSPORT/chsch.o $(TOP_DIR)/PHY/TRANSPORT/chbch.o $(TOP_DIR)/PHY/TRANSPORT/sch.o $(TOP_DIR)/PHY/TRANSPORT/sach.o $(TOP_DIR)/PHY/TRANSPORT/mrbch.o

# PHY Coding routines
OBJS+=$(TOP_DIR)/PHY/CODING/ccoding_byte.o $(TOP_DIR)/PHY/CODING/crc_byte.o $(TOP_DIR)/PHY/CODING/viterbi.o

# PHY Synch routines
OBJS +=$(TOP_DIR)/PHY/ESTIMATION/synch_time.o 

# Utility routines
OBJS +=$(TOP_DIR)/PHY/TOOLS/dB_routines.o $(TOP_DIR)/PHY/TOOLS/file_output.o $(TOP_DIR)/PHY/TOOLS/memory_routines.o $(TOP_DIR)/PHY/TOOLS/signal_energy.o $(TOP_DIR)/PHY/TOOLS/sqrt.o $(TOP_DIR)/PHY/TOOLS/log2_approx.o $(TOP_DIR)/SIMULATION/TOOLS/rangen_double.o $(TOP_DIR)/SIMULATION/TOOLS/random_channel.o 

# CONFIG routines
OBJS +=$(TOP_DIR)/PHY/CONFIG/openair_configure.o $(TOP_DIR)/PHY/CONFIG/openair_readconfigfile.o

$(OBJS) : %.o : %.c 
	$(CC) -c $(CFLAGS) -I$(TOP_DIR) -o $@ $<

main_sensing : $(OBJS) main_sensing.c
	$(CC)  main_sensing.c -I$(TOP_DIR) -o main_sensing $(CFLAGS) $(OBJS) -lm

all : main_sensing

clean:
	rm -f main_sensing
	rm -f $(OBJ)


