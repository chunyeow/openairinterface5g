#CC = $(shell if [ $(OSTYPE) = "cygwin" ]; then echo "gcc-4";  else echo "gcc"; fi)
#CC = gcc-4.3

SSE3PROC = $(shell echo `grep ssse3 /proc/cpuinfo`) 
SSE4PROC = $(shell echo `grep sse4 /proc/cpuinfo`) 
CPUFLAGS = -mmmx -msse -msse2 -m32 -mssse3 #-msse4
#CPUFLAGS += $(shell if [ -z $(SSE3PROC) ]; then echo "" ; else echo "-mssse3"; fi)
#CPUFLAGS += $(shell if [ -z $(SSE4PROC) ]; then echo "" ; else echo "-msse4"; fi)

TOP_DIR = $(OPENAIR1_DIR)
OPENAIR1_TOP = $(OPENAIR1_DIR)
OPENAIR2_TOP = $(OPENAIR2_DIR)
OPENAIR3 = $(OPENAIR3_DIR)

CFLAGS = -Wall -fno-common -mpreferred-stack-boundary=4 -fno-strict-aliasing -DPHYSIM -DNODE_RG -DUSER_MODE -DPC_TARGET -DPC_DSP -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TXRX=2 -DNB_ANTENNAS_TX=2 -DPHY_CONTEXT=1 -g -ggdb -rdynamic $(CPUFLAGS) # -Wno-packed-bitfield-compat

LFLAGS = -lm -lblas

GCCVERSION = $(shell gcc --version | grep ^gcc | sed 's/^.* //g')
ifeq "$(GCCVERSION)"  "4.4.3"
    CFLAGS += -Wno-packed-bitfield-compat
endif
ifeq "$(GCCVERSION)" "4.5.2"
    CFLAGS += -Wno-packed-bitfield-compat
endif

CFLAGS += -DOPENAIR_LTE #-DOFDMA_ULSCH -DIFFT_FPGA -DIFFT_FPGA_UE
#CFLAGS += -DTBS_FIX
CFLAGS += -DCELLULAR

ASN1_MSG_INC = $(OPENAIR2_DIR)/RRC/LITE/MESSAGES

ifdef EMOS
CFLAGS += -DEMOS
endif

ifdef DEBUG_PHY
CFLAGS += -DDEBUG_PHY
endif

ifdef MeNBMUE
CFLAGS += -DMeNBMUE
endif

ifdef MU_RECEIVER
CFLAGS += -DMU_RECEIVER
endif

ifdef ZBF_ENABLED
CFLAGS += -DNULL_SHAPE_BF_ENABLED
endif

ifdef RANDOM_BF
CFLAGS += -DRANDOM_BF
endif

ifdef PBS_SIM
CFLAGS += -DPBS_SIM
endif

ifdef XFORMS
CFLAGS += -DXFORMS
LFLAGS += -lforms
endif

ifdef PERFECT_CE
CFLAGS += -DPERFECT_CE
endif

CFLAGS += -DNO_RRM -DOPENAIR2 -DPHY_ABSTRACTION

CFLAGS += -I/usr/include/X11 -I/usr/X11R6/include


include $(TOP_DIR)/PHY/Makefile.inc
SCHED_OBJS = $(TOP_DIR)/SCHED/phy_procedures_lte_common.o $(TOP_DIR)/SCHED/phy_procedures_lte_eNb.o $(TOP_DIR)/SCHED/phy_procedures_lte_ue.o
#include $(TOP_DIR)/SCHED/Makefile.inc
include $(TOP_DIR)/SIMULATION/Makefile.inc
include $(OPENAIR2_DIR)/LAYER2/Makefile.inc
include $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.inc

CFLAGS += $(L2_incl) -I$(ASN1_MSG_INC) -I$(TOP_DIR) -I$(OPENAIR3)
EXTRA_CFLAGS = 

#STATS_OBJS += $(TOP_DIR)/ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_proc.o

#LAYER2_OBJ += $(OPENAIR2_DIR)/LAYER2/MAC/rar_tools.o
LAYER2_OBJ = $(OPENAIR2_DIR)/LAYER2/MAC/lte_transport_init.o

OBJ = $(PHY_OBJS) $(SIMULATION_OBJS) $(TOOLS_OBJS) $(SCHED_OBJS) $(LAYER2_OBJ) #$(ASN1_MSG_OBJS) 
#OBJ2 = $(PHY_OBJS) $(SIMULATION_OBJS) $(TOOLS_OBJS)  

ifdef XFORMS
OBJ += ../../USERSPACE_TOOLS/SCOPE/lte_scope.o
endif

all: dlsim relay_DF_sim relay_QF_sim pbchsim pdcchsim ulsim pucchsim

test: $(SIMULATION_OBJS) $(TOOLS_OBJS) $(TOP_DIR)/PHY/INIT/lte_init.o test.c
	  $(CC)  test.c -I$(TOP_DIR) -o test $(CFLAGS) $(SIMULATION_OBJS) $(TOOLS_OBJS) -lm 

$(OBJ) : %.o : %.c
	@echo 
	@echo Compiling $< ...
	@$(CC) -c $(CFLAGS) -o $@ $<

dlsim : $(OBJ) dlsim.c
	@echo "Compiling dlsim.c ..."
	@$(CC) dlsim.c  -o dlsim $(CFLAGS) $(OBJ) $(LFLAGS) #-static -L/usr/lib/libblas

relay_DF_sim : $(OBJ) relay_DF_sim.c
	@echo "Compiling relay_DF_sim.c ..."
	@$(CC) relay_DF_sim.c  -o relay_DF_sim $(CFLAGS) $(OBJ) $(LFLAGS) #-static -L/usr/lib/libblas

relay_QF_sim : $(OBJ) relay_QF_sim.c 
	@echo "Compiling relay_QF_sim.c ..."
	@$(CC) relay_QF_sim.c  -o relay_QF_sim $(CFLAGS) $(OBJ) $(LFLAGS) #-static -L/usr/lib/libblas

framegen: $(OBJ) framegen.c
	@echo "Compiling framegen.c"
	@$(CC) framegen.c -o framegen $(CFLAGS) $(OBJ) $(LFLAGS) #-static -L/usr/lib/libblas #-lm -lblas

pbchsim : $(OBJ) pbchsim.c
	@echo "Compiling pbchsim.c"
	@$(CC) pbchsim.c  -o pbchsim $(CFLAGS) $(OBJ) $(LFLAGS) #-static -L/usr/lib/libblas #-lm -lblas

pdcchsim : $(OBJ) pdcchsim.c
	@echo "Compiling pdcchsim.c"
	@$(CC) pdcchsim.c  -o pdcchsim $(CFLAGS) $(OBJ) $(LFLAGS) #-static -L/usr/lib/libblas #-lm -lblas

pucchsim : $(OBJ) pucchsim.c
	@echo "Compiling pucchsim.c"
	@$(CC) pucchsim.c  -o pucchsim $(CFLAGS) $(OBJ) $(LFLAGS) #-static -L/usr/lib/libblas #-lm -lblas

ulsim : $(OBJ) ulsim.c #ulsim_form.c
	@echo "Compiling [ulsim.c]"
	@$(CC) ulsim.c -o ulsim $(CFLAGS) $(OBJ) $(LFLAGS) #-static -L/usr/lib/libblas #-lm  -lblas

clean :
	rm -f $(OBJ)
	rm -f *.o

cleanall : clean
	rm -f relay_DF_sim relay_QF_sim dlsim pbchsim pdcchsim ulsim pucchsim 
	rm -f *.exe*

showcflags :
	@echo $(CFLAGS)

