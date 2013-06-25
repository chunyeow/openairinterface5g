SRC = dump_size.c 

#CFLAGS = $(shell rtai-config --lxrt-cflags)
#LDFLAGS = $(shell rtai-config --lxrt-ldflags) -lpthread -lm -lgps -lforms #-llxrt 

CFLAGS += -I$(OPENAIR1_DIR) -I$(OPENAIR2_DIR)/RRC/LITE/MESSAGES	

CPUFLAGS = -mmmx -msse -msse2 -m32 -mssse3
CFLAGS += -fno-strict-aliasing -DPHYSIM -DNODE_RG -DUSER_MODE -DPC_TARGET -DPC_DSP -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TXRX=2 -DNB_ANTENNAS_TX=2 -DPHY_CONTEXT=1 -g -ggdb -rdynamic $(CPUFLAGS) 

CFLAGS += -DOPENAIR_LTE -DENABLE_FXP -DOPENAIR1 #-DDLSCH_THREAD -DULSCH_THREAD

CFLAGS += -DEMOS -DEXMIMO -DNO_RTAI

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

ifeq ($(EMOS_CHANNEL),1)
CFLAGS += -DEMOS_CHANNEL
endif

all: dump_size cstruct

$(OBJ): %.o : %.c
	@echo Compiling $< ...
	@$(CC) -c $(CFLAGS) -o $@ $<

cstruct: cstruct.c MatlabCStruct.c
	mkoctfile --mex cstruct.c MatlabCStruct.c

dump_size: dump_size.c
	$(CC) $(CFLAGS) $(LDFLAGS) dump_size.c -o dump_size

cleanall:
	rm -rf dump_size cstruct.oct cstruct.mex

clean:
	rm -rf dump_size 

show:
	@echo $(CFLAGS)

beautiful:
	astyle --style=gnu -s2 $(SRC)
