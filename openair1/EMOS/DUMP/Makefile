SRC = emos_dump.c dump_size.c

OBJ = emos_dump.o dump_size.o

CFLAGS = $(shell rtai-config --lxrt-cflags)
LDFLAGS = $(shell rtai-config --lxrt-ldflags) -lpthread -llxrt -lm -lgps -lforms

CFLAGS += -I$(OPENAIR1_DIR) -I$(OPENAIR2_DIR)/RRC/LITE/MESSAGES	

CPUFLAGS = -mmmx -msse -msse2 -m32 -mssse3
CFLAGS += -fno-strict-aliasing -DPHYSIM -DNODE_RG -DUSER_MODE -DPC_TARGET -DPC_DSP -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TXRX=2 -DNB_ANTENNAS_TX=2 -DPHY_CONTEXT=1 -g -ggdb -rdynamic $(CPUFLAGS) 

CFLAGS += -DOPENAIR_LTE -DENABLE_FXP -DOPENAIR1 #-DDLSCH_THREAD -DULSCH_THREAD

CFLAGS += -DEMOS -DEXMIMO

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

all: emos_dump dump_size

$(OBJ): %.o : %.c
	@echo Compiling $< ...
	@$(CC) -c $(CFLAGS) -o $@ $<

emos_dump: emos_dump.c
	$(CC) $(CFLAGS) $(LDFLAGS) emos_dump.c -o emos_dump

dump_size: dump_size.c
	$(CC) $(CFLAGS) $(LDFLAGS) dump_size.c -o dump_size

clean:
	rm -rf emos_dump dump_size
	rm -rf $(OBJ)

show:
	@echo $(CFLAGS)
	@echo $(OBJ)

beautiful:
	astyle --style=gnu -s2 $(SRC)
