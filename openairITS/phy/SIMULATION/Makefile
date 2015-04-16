export TOP_DIR = $(OPENAIR1_DIR)
export OPENAIR2_TOP = $(OPENAIR2_DIR)
export COMMON_UTILS_DIR = $(OPENAIR_HOME)/common/utils

include $(OPENAIR_HOME)/common/utils/Makefile.inc
include $(OPENAIR1_DIR)/SIMULATION/Makefile.inc
include $(OPENAIR2_DIR)/UTIL/Makefile.inc
include $(OPENAIRITS_DIR)/phy/DOT11/Makefile.inc

CFLAGS += -DMALLOC_CHECK_=1 -DEXECTIME 

CFLAGS += -DUSER_MODE -DNB_ANTENNAS_TX=1 -DNB_ANTENNAS_RX=2

CFLAGS += $(UTILS_incl) -I$(OPENAIR1_DIR) -I$(OPENAIRITS_DIR) -I$(OPENAIR2_DIR) -I$(OPENAIR2_DIR)/COMMON

LFLAGS = -lm -lblas -lxml2 -lrt 

ifdef DEBUG_PHY
CFLAGS += -DDEBUG_PHY
endif

ifdef XFORMS
CFLAGS += -DXFORMS
LFLAGS += -lforms
endif

ifdef PERFECT_CE
CFLAGS += -DPERFECT_CE
endif

CFLAGS += -I/usr/include/X11 -I/usr/X11R6/include

#TOOLS_OBJS += $(OPENAIR1_DIR)/PHY/TOOLS/file_output.o

OBJ = $(PHY_OBJS) $(SIMULATION_OBJS) $(TOOLS_OBJS) $(LOG_OBJS) $(ITTI_DIR)/backtrace.o

ifdef XFORMS
OBJ += dot11_scope.o
endif

ifdef STATIC
LFLAGS += -static -L/usr/lib/libblas
endif



all: dot11sim 

$(OBJ) : %.o : %.c
	@echo 
	@echo Compiling $< ...
	@$(CC) -c $(CFLAGS) -o $@ $<

dot11sim : $(OBJ) dot11sim.c
	@echo "Compiling dot11sim.c ..."
	@$(CC) dot11sim.c  -o dot11sim $(CFLAGS) $(OBJ) $(LFLAGS)

clean :
	rm -f $(OBJ)
	rm -f *.o

cleanall : clean
	rm -f dot11sim 
	rm -f *.exe*

showflags :
	@echo $(CFLAGS)
	@echo $(LFLAGS)
	@echo $(OBJ)
