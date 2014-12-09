include $(OPENAIR_HOME)/common/utils/Makefile.inc
#include $(OPENAIR1_DIR)/SIMULATION/LTE_PHY/Makefile

COMMON_UTILS_DIR = $(OPENAIR_HOME)/common/utils
TOP_DIR = $(OPENAIR1_DIR)
OPENAIR1_TOP = $(OPENAIR1_DIR)
OPENAIR2_TOP = $(OPENAIR2_DIR)
OPENAIR3 = $(OPENAIR3_DIR)

CFLAGS += -DMAX_NUM_CCs=1 -Wall -DPHYSIM -DNODE_RG -DUSER_MODE -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TXRX=2 -DNB_ANTENNAS_TX=2 -DPHY_CONTEXT=1 -DMALLOC_CHECK_=1 -DENABLE_VCD_FIFO 

CFLAGS += -DOPENAIR_LTE -DNO_RRM -DOPENAIR1 #-DPHY_ABSTRACTION #-DOPENAIR2

CFLAGS += -I/usr/include/X11 -I/usr/X11R6/include

include $(COMMON_UTILS_DIR)/Makefile.inc
include $(TOP_DIR)/PHY/Makefile.inc
include $(TOP_DIR)/SCHED/Makefile.inc
SCHED_OBJS = $(TOP_DIR)/SCHED/phy_procedures_lte_common.o #$(TOP_DIR)/SCHED/phy_procedures_lte_eNb.o $(TOP_DIR)/SCHED/phy_procedures_lte_ue.o
include $(TOP_DIR)/SIMULATION/Makefile.inc
include $(OPENAIR2_DIR)/LAYER2/Makefile.inc
include $(OPENAIR2_DIR)/UTIL/Makefile.inc
include $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.inc

ASN1_MSG_INC = $(OPENAIR2_DIR)/RRC/LITE/MESSAGES

CFLAGS += $(L2_incl) -I$(ASN1_MSG_INC) -I$(TOP_DIR) -I$(OPENAIR2_DIR) -I$(OPENAIR3) $(UTIL_incl) $(UTILS_incl)

CFLAGS += -DMEX -fPIC 
CFLAGS += -I/opt/MATLAB/R2014a/extern/include/

# Set mex extension, can be determined with mexext function in Matlab
MEXEXT = mexglx


# Just compile all the PHY objects
OBJ = $(PHY_OBJS) $(TOP_DIR)/SIMULATION/TOOLS/taus.o $(LOG_DIR)/log.o $(LOG_DIR)/vcd_signal_dumper.o 

# List of all mex-files to compile
MEX_FILES += ./PHY/LTE_TRANSPORT/mexfiles/get_tbs.$(MEXEXT)
MEX_FILES += ./PHY/LTE_TRANSPORT/mexfiles/dlsch_encoding.$(MEXEXT)
MEX_FILES += ./PHY/LTE_TRANSPORT/mexfiles/dlsch_decoding.$(MEXEXT)
MEX_FILES += ./PHY/LTE_TRANSPORT/mexfiles/dlsch_decoding_init.$(MEXEXT)
MEX_FILES += ./PHY/LTE_TRANSPORT/mexfiles/dlsch_channel_level_prec.$(MEXEXT)
MEX_FILES += ./PHY/LTE_TRANSPORT/mexfiles/dlsch_channel_compensation_prec.$(MEXEXT)
MEX_FILES += ./PHY/LTE_TRANSPORT/mexfiles/dlsch_dual_stream_correlation.$(MEXEXT)
MEX_FILES += ./PHY/LTE_TRANSPORT/mexfiles/dlsch_detection_mrc.$(MEXEXT)
MEX_FILES += ./PHY/LTE_TRANSPORT/mexfiles/dlsch_mu_mimo_llr.$(MEXEXT)
MEX_FILES += ./PHY/TOOLS/mexfiles/log2_approx.$(MEXEXT)
MEX_FILES += ./SIMULATION/TOOLS/mexfiles/taus.$(MEXEXT)
MEX_FILES += ./SIMULATION/TOOLS/mexfiles/set_taus_seed.$(MEXEXT)

# ===========================================================================================
# Commands to compile the static and shared lib for mex-files

# Libpath
LIBDIR := ./lib
LIBNAME := oai

sharedlib : $(OBJ)
	@$(CC) -shared -Wl,-soname,lib$(LIBNAME).so.1 -o $(LIBDIR)/lib$(LIBNAME).so.1.0.1 $(OBJ) -lc

staticlib : $(OBJ)
	@ar ruv $(LIBDIR)/lib$(LIBNAME).a $(OBJ)

libs : $(OBJ)
	@$(CC) -shared -Wl,-soname,lib$(LIBNAME).so.1 -o $(LIBDIR)/lib$(LIBNAME).so.1.0.1 $(OBJ) -lc
	@ar ruv $(LIBDIR)/lib$(LIBNAME).a $(OBJ)

$(MEX_FILES) : %.$(MEXEXT) : %.c
	@echo 
	@echo Compiling $< ...
	@mex CFLAGS="$(CFLAGS)" -l$(LIBNAME) -L$(LIBDIR) -output $@ $<

mex : $(MEX_FILES)

cleanlibs :
	rm -f $(LIBDIR)/lib$(LIBNAME).*

cleanmex :
	rm -f $(MEX_FILES)

clean:
	rm -f $(OBJ)

cleanall: clean cleanlibs cleanmex

show :
	echo $(ASN1_MSG_INC)
