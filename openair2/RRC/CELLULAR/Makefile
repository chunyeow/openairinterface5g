CCC = gcc
KERNEL_MAIN_TYPE=$(shell echo `uname -r | cut -d. -f-2  | tr "." "_"`)
export KERNEL_MAIN_TYPE

SUBVERSION=$(shell echo `grep '^SUBLEVEL =' /usr/src/linux/Makefile | sed -e 's, ,,g' | sed -e 's/SUBLEVEL=//'`)
IS_KERNEL_SUBVERSION_GREATER_THAN_20=$(shell if [ $(SUBVERSION) -ge 20 ] ; then echo true ; fi)

ifdef RTAI
# Get the RTAI variables
CCC = $(shell rtai-config --cc)
RTAI_SUBVERSION=$(shell rtai-config --version | sed -e 's/^..\(.\).*$$/\1/')
IS_RTAI_SUBVERSION_LESS_THAN_FIVE=$(shell if [ $(RTAI_SUBVERSION) -lt 5 ] ; then echo true ; fi)
CFLAGS = $(shell rtai-config --module-cflags) -march=pentium4 -DRTAI_ENABLED -DPC_TARGET -Wall -D__KERNEL__ -DMODULE -D_LOOSE_KERNEL_NAMES -I/lib/modules/$(shell uname -r)/build/include -I/lib/modules/$(shell uname -r)/build/include/asm/mach-default  $(if $(IS_KERNEL_SUBVERSION_GREATER_THAN_20),-mregparm=3,) $(if $(IS_RTAI_SUBVERSION_LESS_THAN_FIVE),-DRTAI_ISNT_POSIX,)
endif
CCC=gcc
#--------------------------------------------------------
AS_RLC_UM_DIR                 =../rlc/um_v6.1.0_lite
AS_RLC_AM_DIR                 =../rlc/am
AS_RLC_TM_DIR                 =../rlc/tm
AS_RLC_DIR                    =../rlc
AS_PDCP_DIR                   =../pdcp
AS_MPLS_DIR                   =../mpls
AS_MEM_DIR                    =../util/mem
AS_LIST_DIR                   =../util/lists
AS_MATH_DIR                   =../util/math
AS_TIMER_DIR                  =../util/timer
AS_RRM_CONFIG_DIR             =../rrm_config

export AS_RLC_UM_DIR
export AS_RLC_AM_DIR
export AS_RLC_TM_DIR
export AS_RLC_DIR
export AS_PDCP_DIR
export AS_MPLS_DIR
export AS_MEM_DIR
export AS_LIST_DIR
export AS_MATH_DIR
export AS_TIMER_DIR
export AS_RRM_CONFIG_DIR

ifdef MASTER
CFLAGS += -DNODE_RG
else
CFLAGS += -DNODE_MT
endif

EXTRA_CFLAGS +=   -DKERNEL$(KERNEL_MAIN_TYPE)  -DCELLULAR -DMAX_MODULES=1 -DBIGPHYSAREA -DNB_ANTENNAS=2 

EXTRA_CFLAGS += -Wall -Wstrict-prototypes -fno-common -fno-strict-aliasing -pipe -freg-struct-return -ffreestanding -maccumulate-outgoing-args -funroll-loops -march=i686 -mmmx -msse -msse2 -fomit-frame-pointer -nostdinc -DMODULE  -D_LOOSE_KERNEL_NAMES -O2

CFLAGS += -I$(obj)/$(AS_RRM_CONFIG_DIR) -I$(obj)/$(AS_TIMER_DIR) -I$(obj)/$(AS_MATH_DIR) -I$(obj)/$(AS_LIST_DIR) -I$(obj)/$(AS_MEM_DIR) -I$(obj)/$(AS_RLC_DIR) -I$(obj)/$(AS_RLC_UM_DIR) -I$(obj)/$(AS_RLC_TM_DIR) -I$(obj)/$(AS_RLC_AM_DIR) -I$(obj)/$(AS_PDCP_DIR) -I$(obj)/$(AS_MPLS_DIR) -I $(M)/include  -I$(M)/../include -I$(M)/../../../include -I$(M)/../../../phy/ofdm_phy/phy_coding -I$(M)/../../../phy/ofdm_phy -I $(M)/../../../phy/ofdm_phy/phy_init -I $(M)/../../../mac -I $(M)/../../../phy/phy_mac_interface -I $(M)/../../../sched   -I$(obj)/../../../arch/common -I$(obj)/../../../util/misc -I$(obj)/../../../arch/openair_CardBus_MIMO1/device_driver -I$(obj)/../../../arch/openair_CardBus_MIMO1/LEON3/hostpc -I$(obj)/../opeanir_rrc_L2_interface

obj-m += openair_rrc.o

openair_rrc-objs += ../openair_rrc_L2_interface/openair_rrc_L2_interface.o 










