TOP_DIR = $(OPENAIR1_DIR)

CC = gcc

bin_PROGRAMS = emos_gui

#emos_gui_SOURCES = emos_gui.c emos_gui_forms.c $(TOP_DIR)/PHY/CONFIG/openair_configure.c $(TOP_DIR)/PHY/CONFIG/openair_readconfigfile.c FFT.c matrix_util.c
#emos_gui_HEADERS = emos_gui.h emos_gui_forms.h matrix_util.h
emos_gui_OBJS = emos_gui.o emos_gui_forms.o FFT.o matrix_util.o \
$(TOP_DIR)/PHY/CONFIG/openair_configure.o $(TOP_DIR)/PHY/CONFIG/openair_readconfigfile.o  $(TOP_DIR)/PHY/TOOLS/fft.o  $(TOP_DIR)/PHY/CODING/ccoding_byte.o $(TOP_DIR)/PHY/CODING/crc_byte.o $(TOP_DIR)/PHY/CODING/viterbi.o $(TOP_DIR)/PHY/TOOLS/cdot_prod.o $(TOP_DIR)/PHY/INIT/lte_init.o $(TOP_DIR)/PHY/LTE_ESTIMATION/lte_sync_time.o $(TOP_DIR)/PHY/LTE_REFSIG/lte_ul_ref.o $(TOP_DIR)/PHY/LTE_TRANSPORT/srs_modulation.o $(TOP_DIR)/PHY/CODING/ccoding_byte_lte.o $(TOP_DIR)/PHY/CODING/viterbi_lte.o $(TOP_DIR)/PHY/LTE_REFSIG/lte_gold.o $(TOP_DIR)/PHY/LTE_TRANSPORT/lte_mcs.o $(TOP_DIR)/PHY/LTE_TRANSPORT/pcfich.o $(TOP_DIR)/PHY/LTE_TRANSPORT/phich.o $(TOP_DIR)/PHY/LTE_TRANSPORT/dci_tools.o $(TOP_DIR)/PHY/LTE_TRANSPORT/uci_tools.o $(TOP_DIR)/SIMULATION/TOOLS/taus.o $(TOP_DIR)/PHY/LTE_TRANSPORT/dlsch_modulation.o

# $(TOP_DIR)/PHY/INIT/init_top.o 

# set the include path found by configure
INCLUDES = -I$(TOP_DIR) -I/usr/realtime/include -I/usr/include/X11 

# the library search path.
CFLAGS = -g -Wall -fno-common -mpreferred-stack-boundary=4 -fno-strict-aliasing\
	 -DUSER_MODE -DPC_TARGET -DPC_DSP -DNB_ANTENNAS_TXRX=2 -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TX=2 -DEMOS -DNODE_RG -DCBMIMO1 -DPHY_CONTEXT -DOPENAIR_LTE\
	 -mmmx -msse -msse2 -mssse3
emos_gui_LDADD = -L/usr/lib -L/usr/local/lib -L/usr/realtime/lib -L/usr/X11R6/lib -lm -lforms -llxrt -lpthread -lX11 -lXpm -lgps

all: emos_gui

$(emos_gui_OBJS) : %.o : %.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

emos_gui : $(emos_gui_OBJS)
	$(CC) -o emos_gui $(CFLAGS) $(emos_gui_OBJS) $(emos_gui_LDADD)

clean:
	rm -f $(emos_gui_OBJS)
