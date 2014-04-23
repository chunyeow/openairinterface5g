CCC = gcc
KERNEL_MAIN_TYPE=$(shell echo `uname -r | cut -d. -f-2  | tr "." "_"`)
export KERNEL_MAIN_TYPE

SUBVERSION=$(shell echo `grep '^SUBLEVEL =' /usr/src/linux/Makefile | sed -e 's, ,,g' | sed -e 's/SUBLEVEL=//'`)
IS_KERNEL_SUBVERSION_GREATER_THAN_20=$(shell if [ $(SUBVERSION) -ge 20 ] ; then echo true ; fi)

GRPCI_SOFTCONFIG_H=grpci_softconfig.h
GRPCI_SOFTREGS_H=grpci_softregs.h

TOP_DIR = ../../..
OPENAIR1_TOP = ../../..
OPENAIR2_TOP = ../../../../openair2
OPENAIR3_TOP = ../../../../openair3

ifdef RTAI
# Get the RTAI variables
CCC = $(shell rtai-config --cc)
RTAI_SUBVERSION=$(shell rtai-config --version | sed -e 's/^..\(.\).*$$/\1/')
IS_RTAI_SUBVERSION_LESS_THAN_FIVE=$(shell if [ $(RTAI_SUBVERSION) -lt 5 ] ; then echo true ; fi)

EXTRA_CFLAGS = -D__IN_RTAI__ $(shell rtai-config --module-cflags) -ggdb -march=pentium4 -DNODE_RG -DBIGPHYSAREA -DRTAI -DRTAI_ENABLED $(if $(IS_KERNEL_SUBVERSION_GREATER_THAN_20),-mregparm=3 -fno-stack-protector,) -D__KERNEL__ -DMODULE -D_LOOSE_KERNEL_NAMES -DWIDENS_DLC -I/lib/modules/$(shell uname -r)/build/include -I/lib/modules/$(shell uname -r)/build/include/asm/mach-default -include /lib/modules/$(shell uname -r)/build/include/linux/autoconf.h $(if $(IS_RTAI_SUBVERSION_LESS_THAN_FIVE),-DRTAI_ISNT_POSIX,)
endif


GCCVERSION = $(shell gcc --version | grep ^gcc | sed 's/^.* //g')
ifeq "$(GCCVERSION)"  "4.4.3"
    EXTRA_CFLAGS += -Wno-packed-bitfield-compat
endif
ifeq "$(GCCVERSION)" "4.5.2"
    EXTRA_CFLAGS += -Wno-packed-bitfield-compat
endif

EXTRA_CFLAGS +=   -DKERNEL$(KERNEL_MAIN_TYPE)
#-DOLD_REMAP

# Old for 2.6.20
#EXTRA_CFLAGS += -Wall -Wstrict-prototypes -fno-common -fno-strict-aliasing -pipe -fno-strength-reduce -mmmx -msse -msse2 -falign-loops=2 -falign-jumps=2 -falign-functions=2 -DCPU=686 -DMODULE  -D_LOOSE_KERNEL_NAMES -O2

#EXTRA_CFLAGS += -Wall -Wstrict-prototypes -fno-common -fno-strict-aliasing -fno-unit-at-a-time -march=i686 -fno-strength-reduce  -fno-exceptions -pipe -mmmx -msse -msse2 -falign-loops=2 -falign-jumps=2 -falign-functions=2 -fno-strict-aliasing -fno-common -ffreestanding -fomit-frame-pointer -nostdinc -DMODULE  -D_LOOSE_KERNEL_NAMES -O2

#EXTRA_CFLAGS += -Wstrict-prototypes -fno-common -fno-strict-aliasing -pipe -mpreferred-stack-boundary=4 -freg-struct-return -ffreestanding -maccumulate-outgoing-args -funroll-loops -march=i686 -fomit-frame-pointer -nostdinc -O2

EXTRA_CFLAGS += -fno-common -fno-strict-aliasing -pipe -mpreferred-stack-boundary=4 -freg-struct-return -ffreestanding -maccumulate-outgoing-args -funroll-loops -march=i686 -nostdinc -O2 

SSE3PROC = $(shell echo `grep ssse3 /proc/cpuinfo`) 
#SSE4PROC = $(shell echo `grep sse4 /proc/cpuinfo`) 
CPUFLAGS = -mmmx -msse -msse2 -m32 
CPUFLAGS += $(shell if [ -z $(SSE3PROC)]; then echo "" ; else echo "-mssse3"; fi)
#CPUFLAGS += $(shell if [ -z $(SSE4PROC)]; then echo "" ; else echo "-msse4"; fi)

EXTRA_CFLAGS += $(CPUFLAGS)

EXTRA_CFLAGS += -DMAC_CONTEXT -DPHY_CONTEXT -DPC_TARGET -DPC_DSP 

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

ifdef BIT8_TX
EXTRA_CFLAGS += -DBIT8_TX
endif

ifdef IDROMEL_RF 
EXTRA_CFLAGS += -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TX=1 -DNB_ANTENNAS_TXRX=2
else
EXTRA_CFLAGS += -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TX=2 -DNB_ANTENNAS_TXRX=2 
endif

ifdef CBMIMO1
EXTRA_CFLAGS += -DCBMIMO1
endif

ifeq ($(FIRMWARE2010),1)
EXTRA_CFLAGS += -DIFFT_FPGA -DIFFT_FPGA_UE -DOFDMA_ULSCH -DHW_PREFIX_REMOVAL
else
EXTRA_CFLAGS += -DFW2011 -DPUCCH
endif

ifeq ($(OPENAIR_LTE),1)
EXTRA_CFLAGS += -DOPENAIR_LTE 
endif

ifeq ($(OPENAIR1),1)
EXTRA_CFLAGS += -DDLSCH_THREAD -DOPENAIR1
endif

ifeq ($(OPENAIR2),1)
EXTRA_CFLAGS += -DOPENAIR2
endif

EXTRA_CFLAGS += -I/usr/include -I/usr/realtime/include 
EXTRA_CFLAGS += -I$(OPENAIR1_DIR) 
#EXTRA_CFLAGS += -DEMIT_ASN_DEBUG=0

include $(OPENAIR1_DIR)/SCHED/Makefile.inc
include $(OPENAIR1_DIR)/PHY/Makefile.inc

#ifeq ($(OPENAIR2),1)
include $(OPENAIR2_DIR)/LAYER2/Makefile.inc
#else
include $(OPENAIR1_DIR)/MAC_INTERFACE/Makefile.inc
#endif
EXTRA_CFLAGS += $(subst $(OPENAIR2_TOP), $(OPENAIR2_DIR), $(L2_incl))

include $(OPENAIR2_DIR)/UTIL/Makefile.inc
EXTRA_CFLAGS += $(subst $(OPENAIR2_TOP), $(OPENAIR2_DIR), $(UTIL_incl))

ccflags-y  += $(EXTRA_CFLAGS)
EXTRA_CFLAGS= ""

include $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.inc


obj-m += openair_rf.o

# Device driver
openair_rf-objs += $(OPENAIR1_TOP)/ARCH/COMMON/bigphys.o
openair_rf-objs += $(OPENAIR1_TOP)/ARCH/COMMON/reserve_mem.o
#openair_rf-objs += $(OPENAIR1_TOP)/ARCH/COMMON/amp.o

openair_rf-objs += cbmimo1_device.o cbmimo1_rf_cntl.o cbmimo1_init.o cbmimo1_dma.o cbmimo1_get_frame.o cbmimo1_fileops.o fifo_printf.o cbmimo1_proc.o exmimo_fw.o
#openair_rf-objs += cbmimo1_generate_ofdm.o cbmimo1_generate_fs4.o  

# MAC Interface
#openair_rf-objs += $(OPENAIR1_TOP)/MAC_INTERFACE/register.o
#openair_rf-objs += $(OPENAIR1_TOP)/MAC_INTERFACE/init.o

# Sched 
ifeq ($(FIRMWARE2010),1)
openair_rf-objs += $(OPENAIR1_TOP)/SCHED/sched_lte.o
else
#openair_rf-objs += $(OPENAIR1_TOP)/SCHED/sched_lte_fw2011.o
openair_rf-objs += $(OPENAIR1_TOP)/SCHED/sched_lte_user.o
endif
ifeq ($(OPENAIR1),1)
openair_rf-objs += $(OPENAIR1_TOP)/SCHED/sched_dlsch.o
endif

# Init
openair_rf-objs += $(OPENAIR1_TOP)/PHY/INIT/init_top.o
openair_rf-objs += $(OPENAIR1_TOP)/PHY/INIT/lte_parms.o

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
openair_rf-objs += $(LIST_OBJ)
openair_rf-objs += $(TIMER_OBJ)
openair_rf-objs += $(MEM_OBJ)
else
openair_rf-objs += $(MAC_XFACE_OBJS)
endif

#openair_rf-objs += $(LOG_DIR)/log.o

openair_rf_cbmimo1_softmodem.ko:  
	(cd $(OPENAIR1_TOP) ; make openair_rf_cbmimo1_softmodem.ko)

oai_user_cbmimo1.ko:
	(cd $(OPENAIR1_TOP) ; make oai_user_cbmimo1.ko)

oai_user_exmimo:
	(cd $(OPENAIR1_TOP) ; make oai_user_exmimo.ko)

test:
	echo $(openair_rf-objs)
	echo $(obj-m)
#	echo $(CFLAGS)
	echo $(ccflags-y) #echo$(EXTRA_CFLAGS)

clean:
	rm -f $(openair_rf-objs) $(obj-m)
	rm -f openair_rf.mod.c .*.cmd *.o modules.order Module.symvers 
	rm -rf .tmp_versions/

ultraclean: clean
	rm -f *.ko
