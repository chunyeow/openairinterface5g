CCC = gcc
KERNEL_MAIN_TYPE=$(shell echo `uname -r | cut -d. -f-2  | tr "." "_"`)
export KERNEL_MAIN_TYPE

SUBVERSION=$(shell echo `grep '^SUBLEVEL =' /usr/src/linux/Makefile | sed -e 's, ,,g' | sed -e 's/SUBLEVEL=//'`)
IS_KERNEL_SUBVERSION_GREATER_THAN_20=$(shell if [ $(SUBVERSION) -ge 20 ] ; then echo true ; fi)

GRPCI_SOFTCONFIG_H=grpci_softconfig.h
GRPCI_SOFTREGS_H=grpci_softregs.h

TOP_DIR = .
OPENAIR1_TOP = .
OPENAIR2_TOP = ../openair2
OPENAIR3_TOP = ../openair3

ifdef RTAI
# Get the RTAI variables
CCC = $(shell rtai-config --cc)
RTAI_SUBVERSION=$(shell rtai-config --version | sed -e 's/^..\(.\).*$$/\1/')
IS_RTAI_SUBVERSION_LESS_THAN_FIVE=$(shell if [ $(RTAI_SUBVERSION) -lt 5 ] ; then echo true ; fi)

EXTRA_CFLAGS = -D__IN_RTAI__ $(shell rtai-config --module-cflags) -g -march=pentium4 -DNODE_RG -DBIGPHYSAREA -DRTAI -DRTAI_ENABLED $(if $(IS_KERNEL_SUBVERSION_GREATER_THAN_20),-mregparm=3 -fno-stack-protector,) -DHW_PREFIX_REMOVAL -Wall -D__KERNEL__ -DMODULE -D_LOOSE_KERNEL_NAMES -DWIDENS_DLC -I/lib/modules/$(shell uname -r)/build/include -I/lib/modules/$(shell uname -r)/build/include/asm/mach-default -include /lib/modules/$(shell uname -r)/build/include/linux/autoconf.h $(if $(IS_RTAI_SUBVERSION_LESS_THAN_FIVE),-DRTAI_ISNT_POSIX,)
endif

EXTRA_CFLAGS +=   -DKERNEL$(KERNEL_MAIN_TYPE)
#-DOLD_REMAP

# Old for 2.6.20
#EXTRA_CFLAGS += -Wall -Wstrict-prototypes -fno-common -fno-strict-aliasing -pipe -fno-strength-reduce -mmmx -msse -msse2 -falign-loops=2 -falign-jumps=2 -falign-functions=2 -DCPU=686 -DMODULE  -D_LOOSE_KERNEL_NAMES -O2

#EXTRA_CFLAGS += -Wall -Wstrict-prototypes -fno-common -fno-strict-aliasing -fno-unit-at-a-time -march=i686 -fno-strength-reduce  -fno-exceptions -pipe -mmmx -msse -msse2 -falign-loops=2 -falign-jumps=2 -falign-functions=2 -fno-strict-aliasing -fno-common -ffreestanding -fomit-frame-pointer -nostdinc -DMODULE  -D_LOOSE_KERNEL_NAMES -O2

EXTRA_CFLAGS += -Wall -Wstrict-prototypes -fno-common -fno-strict-aliasing -pipe -mpreferred-stack-boundary=4 -freg-struct-return -ffreestanding -maccumulate-outgoing-args -funroll-loops -march=i686 -fomit-frame-pointer -nostdinc -O2

SSE3PROC = $(shell echo `grep ssse3 /proc/cpuinfo`) 
SSE4PROC = $(shell echo `grep sse4 /proc/cpuinfo`) 
CPUFLAGS = -mmmx -msse -msse2 -m32 
CPUFLAGS += $(shell if [ -z $(SSE3PROC)]; then echo "" ; else echo "-mssse3"; fi)
CPUFLAGS += $(shell if [ -z $(SSE4PROC)]; then echo "" ; else echo "-msse4"; fi)

EXTRA_CFLAGS += $(CPUFLAGS)

EXTRA_CFLAGS += -DMAX_MODULES=1 -DNO_RRM -DMAC_CONTEXT -DPHY_CONTEXT -DPC_TARGET -DPC_DSP -UDLSCH_THREAD -DPHY_ABSTRACTION

ifdef DEBUG_PHY
EXTRA_CFLAGS += -DDEBUG_PHY
endif

ifdef NO_CARD_TEST
EXTRA_CFLAGS += -DNOCARD_TEST
endif

ifeq ($(EMOS),1)
EXTRA_CFLAGS += -DEMOS 
endif

ifdef DUALSTREAM
EXTRA_CFLAGS += -DDUALSTREAM
endif

ifndef NO_TXMUX
EXTRA_CFLAGS += -DBIT8_TXMUX -DBIT8_TX
endif

ifdef IDROMEL_RF 
EXTRA_CFLAGS += -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TX=1 -DNB_ANTENNAS_TXRX=2
else
EXTRA_CFLAGS += -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TX=2 -DNB_ANTENNAS_TXRX=2 
endif

ifdef CBMIMO1
EXTRA_CFLAGS += -DCBMIMO1
endif

ifeq ($(OPENAIR_LTE),1)
EXTRA_CFLAGS += -DOPENAIR_LTE -DIFFT_FPGA -DIFFT_FPGA_UE -DOFDMA_ULSCH
endif

EXTRA_CFLAGS += -I$(OPENAIR1_DIR) -I$(OPENAIR2_DIR) -I$(OPENAIR2_DIR)/COMMON -I$(OPENAIR2_DIR)/LAYER2/MAC -I$(OPENAIR2_DIR)/LAYER2/PDCP -I$(OPENAIR2_DIR)/LAYER2/RLC -I$(OPENAIR2_DIR)/LAYER2/RLC/AM -I$(OPENAIR2_DIR)/LAYER2/RLC/TM -I$(OPENAIR2_DIR)/LAYER2/RLC/UM_v9.3.0 -I$(OPENAIR2_DIR)/PHY_INTERFACE -I$(OPENAIR2_DIR)/RRC/L2_INTERFACE -I$(OPENAIR2_DIR)/RRC/LITE -I$(OPENAIR2_DIR)/RRC/LITE/MESSAGES -I$(OPENAIR2_DIR)/UTIL/LISTS -I$(OPENAIR2_DIR)/UTIL/LOG -I$(OPENAIR2_DIR)/UTIL/MATH -I$(OPENAIR2_DIR)/UTIL/MEM -I$(OPENAIR2_DIR)/UTIL/TIMER -I/usr/include -I$(OPENAIR3_DIR)/MESH

include $(OPENAIR1_DIR)/SCHED/Makefile.inc
include $(OPENAIR1_DIR)/PHY/Makefile.inc

ifeq ($(OPENAIR2),1)
include $(OPENAIR2_DIR)/LAYER2/Makefile.inc
include $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.inc
else
include $(OPENAIR1_DIR)/MAC_INTERFACE/Makefile.inc
endif


obj-m += openair_rf.o

# Device driver
openair_rf-objs += $(OPENAIR1_TOP)/ARCH/COMMON/bigphys.o
openair_rf-objs += $(OPENAIR1_TOP)/ARCH/COMMON/reserve_mem.o
#openair_rf-objs += $(OPENAIR1_TOP)/ARCH/COMMON/amp.o

openair_rf-objs += $(OPENAIR1_TOP)/ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_device.o 
openair_rf-objs += $(OPENAIR1_TOP)/ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_rf_cntl.o 
openair_rf-objs += $(OPENAIR1_TOP)/ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_init.o 
openair_rf-objs += $(OPENAIR1_TOP)/ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_dma.o 
openair_rf-objs += $(OPENAIR1_TOP)/ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_get_frame.o 
openair_rf-objs += $(OPENAIR1_TOP)/ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_fileops.o 
openair_rf-objs += $(OPENAIR1_TOP)/ARCH/CBMIMO1/DEVICE_DRIVER/fifo_printf.o 
openair_rf-objs += $(OPENAIR1_TOP)/ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_proc.o 
#openair_rf-objs += cbmimo1_generate_ofdm.o cbmimo1_generate_fs4.o  

# MAC Interface
#openair_rf-objs += $(OPENAIR1_TOP)/MAC_INTERFACE/register.o
#openair_rf-objs += $(OPENAIR1_TOP)/MAC_INTERFACE/init.o
openair_rf-objs += $(MAC_XFACE_OBJS)

# Sched 
openair_rf-objs += $(OPENAIR1_TOP)/SCHED/sched_lte.o
#openair_rf-objs += $(OPENAIR1_TOP)/SCHED/sched_dlsch.o

# Init
openair_rf-objs += $(OPENAIR1_TOP)/PHY/INIT/init_top.o

# Tools
openair_rf-objs += $(OPENAIR1_TOP)/PHY/TOOLS/memory_routines.o
openair_rf-objs += $(OPENAIR1_TOP)/PHY/TOOLS/fft.o
openair_rf-objs += $(OPENAIR1_TOP)/SIMULATION/TOOLS/taus.o

#include files for MODEM
ifeq ($(OPENAIR1),1)
openair_rf-objs += $(SCHED_OBJS)
openair_rf-objs += $(PHY_OBJS)
endif

#include files for OPENAIR2
ifeq ($(OPENAIR2),1)
openair_rf-objs += $(L2_OBJS)
#openair_rf-objs += $(ASN1_MSG_OBJS)
endif

