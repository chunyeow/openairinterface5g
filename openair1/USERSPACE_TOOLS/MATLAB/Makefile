include $(OPENAIR1_DIR)/SIMULATION/LTE_PHY/Makefile

CFLAGS += -DMEX
CFLAGS += -I/opt/MATLAB/extern/include/

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
	rm $(LIBDIR)/lib$(LIBNAME).*

cleanmex :
	rm -f $(MEX_FILES)

show :
	echo $(CFLAGS)
