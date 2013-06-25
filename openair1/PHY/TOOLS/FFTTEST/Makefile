CC = gcc-4

TOP_DIR                    = ../../..

CFLAGS = -Wall -O2 -fno-common -mpreferred-stack-boundary=4  -DNODE_RG -DUSER_MODE -DPC_TARGET -DPC_DSP -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TXRX=2 -DNB_ANTENNAS_TX=2 -DMAX_MODULES=1 -DPHY_CONTEXT=1 -DBIT8_TXMUX -DBIT8_TX -g -msse2 -mmmx -msse 
PHY_OBJS +=$(TOP_DIR)/PHY/TOOLS/fft.o $(TOP_DIR)/SIMULATION/TOOLS/rangen_double.o $(TOP_DIR)/SIMULATION/TOOLS/taus.o $(TOP_DIR)/PHY/TOOLS/file_output.o

OBJ = $(PHY_OBJS) 

all: ffttest

$(OBJ) : %.o : %.c 
	$(CC) -c $(CFLAGS) -Wall -I$(TOP_DIR) -o $@ $<

ffttest: $(OBJ) ffttest.c
	$(CC) ffttest.c -I$(TOP_DIR) -o ffttest $(CFLAGS) $(PHY_OBJS) -lm

clean: 
	rm -f $(TOP_DIR)/PHY/TOOLS/*.o
	rm -f $(TOP_DIR)/SIMULATION/TOOLS/*.o
	rm -f ffttest
	rm -f *.m
