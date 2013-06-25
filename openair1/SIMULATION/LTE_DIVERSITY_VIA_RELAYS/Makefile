CC = $(shell if [ $$OSTYPE = "cygwin" ] ; then echo "gcc-4";  else echo "gcc"; fi)
#CC = gcc

SSE3PROC = $(shell echo `grep ssse3 /proc/cpuinfo`) 
SSE4PROC = $(shell echo `grep sse4 /proc/cpuinfo`) 
CPUFLAGS = -mmmx -msse -msse2 -m32 #-mssse3 -msse4
CPUFLAGS += $(shell if [ -z $(SSE3PROC)]; then echo "" ; else echo "-mssse3"; fi)
CPUFLAGS += $(shell if [ -z $(SSE4PROC)]; then echo "" ; else echo "-msse4"; fi)

TOP_DIR = ../..

CFLAGS = -Wall -fno-common -mpreferred-stack-boundary=4 -fno-strict-aliasing -DPHYSIM -DNODE_RG -DUSER_MODE -DPC_TARGET -DPC_DSP -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TXRX=2 -DNB_ANTENNAS_TX=2 -DMAX_MODULES=1 -DPHY_CONTEXT=1 -g -ggdb $(CPUFLAGS) 

#CFLAGS = -Wall -Wno-packed-bitfield-compat -fno-common -mpreferred-stack-boundary=4 -fno-strict-aliasing -DNODE_RG -DUSER_MODE -DPC_TARGET -DPC_DSP -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TXRX=2 -DNB_ANTENNAS_TX=2 -DMAX_MODULES=1 -DPHY_CONTEXT=1 -g -ggdb $(CPUFLAGS)

CFLAGS += -DOPENAIR_LTE  -DOFDMA_ULSCH #-DIFFT_FPGA #-O2
CFLAGS += -DTBS_FIX

ifdef EMOS
CFLAGS += -DEMOS
endif

ifdef DEBUG_PHY
CFLAGS += -DDEBUG_PHY
endif

ifdef MeNBMUE
CFLAGS += -DMeNBMUE
endif

ifdef PBS_SIM
CFLAGS += -DPBS_SIM
endif

ifdef OPENAIR2
CFLAGS += -DOPENAIR2 
endif

CFLAGS += -I$(OPENAIR2_DIR) -I$(OPENAIR3_DIR)/MESH -I$(OPENAIR2_DIR)/COMMON -I$(OPENAIR2_DIR)/LAYER2/RLC -I$(OPENAIR2_DIR)/UTIL/MEM -I$(OPENAIR2_DIR)/UTIL/MATH -I$(OPENAIR2_DIR)/UTIL/LISTS -I$(OPENAIR2_DIR)/LAYER2/RLC/AM -I$(OPENAIR2_DIR)/LAYER2/RLC/UM_v6.1.0_LITE -I$(OPENAIR2_DIR)/LAYER2/RLC/TM -I$(OPENAIR2_DIR)/UTIL/TIMER



PHY_OBJS = $(TOP_DIR)/PHY/LTE_TRANSPORT/pss.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/pilots.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/dlsch_coding.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/dlsch_modulation.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/dlsch_demodulation.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/dlsch_decoding.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/dci_tools.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/uci_tools.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/lte_mcs.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/pbch.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/dci.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/srs_modulation.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/drs_modulation.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/ulsch_modulation.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/ulsch_demodulation.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/ulsch_coding.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/ulsch_decoding.o
#PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/rar_tools.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/phich.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_TRANSPORT/pcfich.o
PHY_OBJS += $(TOP_DIR)/PHY/MODULATION/ofdm_mod.o
PHY_OBJS += $(TOP_DIR)/PHY/MODULATION/slot_fep.o
PHY_OBJS += $(TOP_DIR)/PHY/MODULATION/slot_fep_ul.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_ESTIMATION/lte_sync_time.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_ESTIMATION/lte_adjust_sync.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_ESTIMATION/lte_dl_channel_estimation.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_ESTIMATION/lte_ul_channel_estimation.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_ESTIMATION/lte_est_freq_offset.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_ESTIMATION/lte_ue_measurements.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_ESTIMATION/lte_eNB_measurements.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_ESTIMATION/adjust_gain.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_REFSIG/lte_dl_cell_spec.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_REFSIG/lte_gold.o
PHY_OBJS += $(TOP_DIR)/PHY/LTE_REFSIG/lte_ul_ref.o
PHY_OBJS += $(TOP_DIR)/PHY/CODING/lte_segmentation.o
PHY_OBJS += $(TOP_DIR)/PHY/CODING/ccoding_byte.o
PHY_OBJS += $(TOP_DIR)/PHY/CODING/ccoding_byte_lte.o
PHY_OBJS += $(TOP_DIR)/PHY/CODING/3gpplte.o
PHY_OBJS += $(TOP_DIR)/PHY/CODING/crc_byte.o
PHY_OBJS += $(TOP_DIR)/PHY/CODING/3gpplte_turbo_decoder_sse.o
PHY_OBJS += $(TOP_DIR)/PHY/CODING/lte_rate_matching.o
PHY_OBJS += $(TOP_DIR)/PHY/CODING/rate_matching.o
PHY_OBJS += $(TOP_DIR)/PHY/CODING/viterbi.o
PHY_OBJS += $(TOP_DIR)/PHY/CODING/viterbi_lte.o
PHY_OBJS += $(TOP_DIR)/PHY/INIT/lte_init.o
ifdef MeNBMUE
#PHY_OBJS += $(TOP_DIR)/PHY/INIT/secsys_init.o
endif
PHY_OBJS += $(TOP_DIR)/PHY/INIT/init_top.o
ifdef OPENAIR2
PHY_OBJS += $(TOP_DIR)/MAC_INTERFACE/init.o
endif
ifdef EMOS
PHY_OBJS += $(TOP_DIR)/PHY/LTE_ESTIMATION/lte_dl_channel_estimation_emos.o
endif
ifdef OPENAIR2
PHY_OBJS += $(OPENAIR2_DIR)/PHY_INTERFACE/mac_phy_primitives.o
endif

SIMULATION_OBJS  = $(TOP_DIR)/SIMULATION/TOOLS/gauss.o  
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/TOOLS/random_channel.o  
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/TOOLS/rangen_double.o  
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/TOOLS/taus.o  
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/TOOLS/multipath_channel.o
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/RF/rf.o
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/RF/adc.o
SIMULATION_OBJS += $(TOP_DIR)/SIMULATION/RF/dac.o

TOOLS_OBJS = $(TOP_DIR)/PHY/TOOLS/file_output.o
TOOLS_OBJS += $(TOP_DIR)/PHY/TOOLS/fft.o
TOOLS_OBJS += $(TOP_DIR)/PHY/TOOLS/log2_approx.o
TOOLS_OBJS += $(TOP_DIR)/PHY/TOOLS/cmult_sv.o
TOOLS_OBJS += $(TOP_DIR)/PHY/TOOLS/cmult_vv.o
TOOLS_OBJS += $(TOP_DIR)/PHY/TOOLS/cdot_prod.o
TOOLS_OBJS += $(TOP_DIR)/PHY/TOOLS/signal_energy.o
TOOLS_OBJS += $(TOP_DIR)/PHY/TOOLS/dB_routines.o
TOOLS_OBJS += $(TOP_DIR)/PHY/TOOLS/sqrt.o
TOOLS_OBJS += $(TOP_DIR)/PHY/TOOLS/lte_dfts.o

SCHED_OBJS += $(TOP_DIR)/SCHED/phy_procedures_lte_common.o
SCHED_OBJS += $(TOP_DIR)/SCHED/phy_procedures_lte_eNb.o
SCHED_OBJS += $(TOP_DIR)/SCHED/phy_procedures_lte_ue.o
ifdef MeNBMUE
SCHED_OBJS += $(TOP_DIR)/SCHED/phy_procedures_lte_MeNbMue.o
endif

#STATS_OBJS += $(TOP_DIR)/ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_proc.o

LAYER2_OBJ += $(OPENAIR2_DIR)/LAYER2/MAC/rar_tools.o

OBJ = $(PHY_OBJS) $(SIMULATION_OBJS) $(TOOLS_OBJS) $(SCHED_OBJS) $(STATS_OBJS) $(LAYER2_OBJ)	
OBJ2 = $(PHY_OBJS) $(SIMULATION_OBJS) $(TOOLS_OBJS) 

all: dlsim dlsim2 ulsim ulsim2 physim physim_secsys relays_sim relays_sim_dl relays_sim_harq relays_sim_harq2

test:
	echo $(OPENAIR2_DIR) $(LAYER2_OBJ)

$(OBJ) : %.o : %.c
	$(CC) -c $(CFLAGS) -I$(TOP_DIR) -o $@ $<

dlsim : $(OBJ2) dlsim.c
	$(CC) dlsim.c -I$(TOP_DIR) -o dlsim $(CFLAGS) $(OBJ2) -lm

dlsim2 : $(OBJ2) dlsim2.c
	$(CC) dlsim2.c -I$(TOP_DIR) -o dlsim2 $(CFLAGS) $(OBJ2) -lm

dlsim_secsys : $(OBJ2) dlsim_secsys.c
	$(CC) dlsim_secsys.c -I$(TOP_DIR) -o dlsim_secsys $(CFLAGS) $(OBJ2) -lm

ulsim : $(OBJ2) ulsim.c
	$(CC) ulsim.c -I$(TOP_DIR) -o ulsim $(CFLAGS) $(OBJ2) -lm

ulsim2 : $(OBJ2) ulsim2.c
	$(CC) ulsim2.c -I$(TOP_DIR) -o ulsim2 $(CFLAGS) $(OBJ2) -lm

physim : $(OBJ) phy_procedures_sim.c 
	$(CC)  phy_procedures_sim.c -I$(TOP_DIR) -o physim $(CFLAGS) $(OBJ) -lm

physim_secsys : $(OBJ) phy_procedures_sim_secsys.c 
	$(CC)  phy_procedures_sim_secsys.c -I$(TOP_DIR) -o physim_secsys $(CFLAGS) $(OBJ) -lm

relays_sim : $(OBJ2) relays_sim.c
	$(CC)  relays_sim.c -I$(TOP_DIR) -o relaysim $(CFLAGS) $(OBJ2) -lm

relays_sim_dl : $(OBJ2) relays_sim_dl.c
	$(CC)  relays_sim_dl.c -I$(TOP_DIR) -o relaysim_dl $(CFLAGS) $(OBJ2) -lm

relays_sim_harq : $(OBJ2) relays_sim_harq.c
	$(CC)  relays_sim_harq.c -I$(TOP_DIR) -o relaysim_harq $(CFLAGS) $(OBJ2) -lm

relays_sim_harq2 : $(OBJ2) relays_sim_harq2.c
	$(CC)  relays_sim_harq2.c -I$(TOP_DIR) -o relaysim_harq2 $(CFLAGS) $(OBJ2) -lm

clean:
	rm -f dlsim dlsim2 dlsim_secsys
	rm -f $(OBJ)
	rm -f *.o
	rm -f *.exe*

showcflags:
	@echo $(CFLAGS)

