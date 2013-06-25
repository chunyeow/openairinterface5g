CC = gcc

TOP_DIR = ../..
OPENAIR2_DIR = ../../../openair2
OPENAIR2_TOP = $(OPENAIR2_DIR)

CFLAGS = -Wall -g -ggdb -DNODE_RG -DUSER_MODE -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TX=2 -DNB_ANTENNAS_TXRX=2 -DWIDENS_DLC -DPHY_CONTEXT -DPC_DSP -DPC_TARGET -DMAX_MODULES=1 -DCBMIMO1 -mtune=pentium4 -mmmx -msse2 -O -funroll-loops -fno-stack-check -I$(TOP_DIR) -I/usr/include/X11 -I/usr/X11R6/include -I/usr/realtime/include -I$(OPENAIR2_DIR)/RRC/LITE/MESSAGES

include $(OPENAIR2_DIR)/LAYER2/Makefile.inc
CFLAGS += $(L2_incl)

LTE_OBJS = lte_scope.o lte.o $(TOP_DIR)/PHY/TOOLS/file_output.o $(TOP_DIR)/PHY/INIT/init_top.o $(TOP_DIR)/PHY/TOOLS/fft.o
LTE_UL_OBJS = lte_scope_ul.o lte_ul.o $(TOP_DIR)/PHY/TOOLS/file_output.o $(TOP_DIR)/PHY/INIT/init_top.o $(TOP_DIR)/PHY/TOOLS/fft.o

ifeq ($(EMOS),1)
CFLAGS += -DEMOS
endif

all: lte lte_ul

lte: $(LTE_OBJS) 
	$(CC)  -o lte_scope $(CFLAGS) $(LTE_OBJS)  -lm -L/usr/local/lib -lforms -lX11 -L/usr/X11R6/lib -lXpm -lpthread  

lte_ul: $(LTE_UL_OBJS) 
	$(CC)  -o lte_scope_ul $(CFLAGS) $(LTE_UL_OBJS)  -lm -L/usr/local/lib -lforms -lX11 -L/usr/X11R6/lib -lXpm -lpthread  


$(LTE_OBJS) $(LTE_UL_OBJS): %.o: %.c
	$(CC) -c $(DEBUG_FLAGS) $(CFLAGS) -DOPENAIR_LTE $(INCLUDE) -o $@ $<

clean:
	rm -f  $(LTE_OBJS) $(LTE_UL_OBJS)


