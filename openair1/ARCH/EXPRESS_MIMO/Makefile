TOP_DIR      = ../..
INST         = $(ECOS_INSTALL)/ecos_install
OPENAIR0_SIM = $(OPENAIR0_DIR)/express-mimo/simulator/baseband/C

ifdef ECOS
CC=/opt/sparc-elf-3.4.4/bin/sparc-elf-gcc
else
CC=gcc
endif

CFLAGS=-g -Wall -DNODE_RG -DUSER_MODE -DEXPRESSMIMO_TARGET -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TXRX=2 -DNB_ANTENNAS_TX=2 -DMAX_MODULES=1 -DPHY_CONTEXT=1 -DFILE_OUTPUT

LDFLAGS= -lm -lfftw3

INCLUDE=-I$(OPENAIR0_DIR) -I$(OPENAIR1_DIR) -I/usr/include -I/usr/lib/gcc/i486-linux-gnu/4.1.3/include

ifdef ECOS
CFLAGS+=-msoft-float -ffunction-sections -fdata-sections
LDFLAGS+=-nostartfiles -nostdlib -Wl,--gc-sections -Wl,--Map -Wl,openair1.map -L$(INST)/lib -Ttarget.ld 
INCLUDE+=-I$(INST)/include   
DEPS=$(INST)/lib/libtarget.a $(INST)/lib/target.ld $(INST)/lib/vectors.o
endif

ifdef DEBUGPHY
CFLAGS += -DDEBUG_PHY
endif

ifdef DUALSTREAM
CFLAGS += -DDUALSTREAM
endif

# PHY Initialization routines
PHY_OBJS	=$(TOP_DIR)/PHY/INIT/init.o 

# PHY TOOLS routines
# PHY_OBJS +=$(TOP_DIR)/PHY/TOOLS/fft.o $(TOP_DIR)/PHY/TOOLS/cmult_vv.o $(TOP_DIR)/PHY/TOOLS/cmult_sv.o $(TOP_DIR)/PHY/TOOLS/cmult_vvh.o $(TOP_DIR)/PHY/TOOLS/cmult_mm.o $(TOP_DIR)/PHY/TOOLS/cadd_sv.o $(TOP_DIR)/PHY/TOOLS/cadd_vv.o 

PHY_OBJS += $(OPENAIR0_SIM)/framing/framing.o $(OPENAIR0_SIM)/front_end_processor/fep.o $(OPENAIR0_SIM)/front_end_processor/fep_api.o $(OPENAIR0_SIM)/channel_decoder/3gpp_intlv.o $(OPENAIR0_SIM)/channel_decoder/lte_intlv.o $(OPENAIR0_SIM)/channel_decoder/3gpp_encoder.o $(OPENAIR0_SIM)/channel_decoder/decoder.o $(OPENAIR0_SIM)/channel_decoder/decoder_api.o

# PHY ESTIMATION routines 
PHY_OBJS += $(TOP_DIR)/PHY/ESTIMATION/synch_time.o
#PHY_OBJS += $(TOP_DIR)/PHY/ESTIMATION/channel_estimation.o  $(TOP_DIR)/PHY/ESTIMATION/adjust_sync.o $(TOP_DIR)/PHY/ESTIMATION/adjust_sync2.o $(TOP_DIR)/PHY/ESTIMATION/calc_timing_offset.o  

# PHY MODULATION routines
PHY_OBJS +=$(TOP_DIR)/PHY/MODULATION/ofdm_mod.o

# PHY Transport routines
PHY_OBJS += $(TOP_DIR)/PHY/TRANSPORT/chsch.o $(TOP_DIR)/PHY/TRANSPORT/chbch.o $(TOP_DIR)/PHY/TRANSPORT/sch.o $(TOP_DIR)/PHY/TRANSPORT/sach.o $(TOP_DIR)/PHY/TRANSPORT/mrbch.o


# PHY Coding routines
PHY_OBJS+=$(TOP_DIR)/PHY/CODING/ccoding_byte.o $(TOP_DIR)/PHY/CODING/crc_byte.o $(TOP_DIR)/PHY/CODING/viterbi.o $(TOP_DIR)/PHY/CODING/rate_matching.o

# Utility routines
PHY_OBJS +=$(TOP_DIR)/PHY/TOOLS/dB_routines.o $(TOP_DIR)/PHY/TOOLS/file_output.o $(TOP_DIR)/PHY/TOOLS/memory_routines.o $(TOP_DIR)/PHY/TOOLS/signal_energy.o $(TOP_DIR)/PHY/TOOLS/sqrt.o $(TOP_DIR)/PHY/TOOLS/log2_approx.o 


# CONFIG routines
PHY_OBJS +=$(TOP_DIR)/PHY/CONFIG/openair_configure.o $(TOP_DIR)/PHY/CONFIG/openair_readconfigfile.o

PHY_OBJS += openair1.o

OBJ = $(PHY_OBJS) 

all: $(OBJ) openair1

$(OBJ) : %.o : %.c 
	$(CC) -c $(CFLAGS) -Wall -I$(TOP_DIR) $(INCLUDE) -o $@ $<

openair1: $(DEPS) $(OBJ) openair1.o
	$(CC) $(INCLUDE) -I$(TOP_DIR) $(CFLAGS) $(LDFLAGS) -o openair1 $(PHY_OBJS)


clean:
	rm -f openair1
	rm -f $(PHY_OBJS)
	rm -f *.o
	rm -f $(TOP_DIR)/PHY/INIT/*.o
	rm -f $(TOP_DIR)/PHY/TOOLS/*.o
	rm -f $(TOP_DIR)/PHY/MODULATION/*.o
	rm -f $(TOP_DIR)/PHY/CODING/*.o
	rm -f $(TOP_DIR)/PHY/TRANSPORT/*.o
	rm -f $(TOP_DIR)/PHY/ESTIMATION/*.o
	rm -f $(TOP_DIR)/PHY/CONFIG/*.o
	rm -f $(TOP_DIR)/SIMULATION/TOOLS/*.o


