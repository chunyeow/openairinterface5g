#/*________________________Makefile________________________

# Authors : Hicham Anouar, Lionel Gauthier, Raymond Knopp
# Company : EURECOM
# Emails  : anouar@eurecom.fr, lionel.gauthier@eurecom.fr, knopp@eurecom.fr
#________________________________________________________________*/





include $(OPENAIR_HOME)/common/utils/Makefile.inc

KERNEL_ARCH:=$(shell echo `uname -m`)
SET_X64:=$(shell if [ $(KERNEL_ARCH) = 'x86_64' ]; then echo true ; fi)
ifdef RTAI
# Get the RTAI variables
CCC = $(shell rtai-config --cc)
RTAI_SUBVERSION=$(shell rtai-config --version | sed -e 's/^..\(.\).*$$/\1/')
IS_RTAI_SUBVERSION_LESS_THAN_FIVE=$(shell if [ $(RTAI_SUBVERSION) -lt 5 ] ; then echo true ; fi)

EXTRA_CFLAGS = $(shell rtai-config --module-cflags) -DRTAI_ENABLED -DPC_TARGET -D__KERNEL__ -DMODULE -D_LOOSE_KERNEL_NAMES -I$(KERNEL_DIR)/build/include -I$(KERNEL_DIR)/build/include/asm/mach-default -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TX=2 -DNB_ANTENNAS_TXRX -DMAC_CONTEXT -I/usr/include $(if $(IS_RTAI_SUBVERSION_LESS_THAN_FIVE),-DRTAI_ISNT_POSIX,)
endif





####################################################
#      SOURCE FOLDERS
####################################################


COMMON_DIR		      = ../COMMON
AS_RLC_UM_DIR                 = RLC/UM_v6.1.0_LITE
AS_RLC_AM_DIR                 = RLC/AM
AS_RLC_TM_DIR                 = RLC/TM
AS_RLC_DIR                    = RLC
AS_PDCP_DIR                   = PDCP_v10.1.0
AS_MEM_DIR                    = ../UTIL/MEM
AS_LIST_DIR                   = ../UTIL/LISTS
AS_FIFO_DIR	              = ../UTIL/FIFO
AS_MATH_DIR                   = ../UTIL/MATH
AS_TIMER_DIR                  = ../UTIL/TIMER
PHY_INTERFACE_DIR	      = ../PHY_INTERFACE

export AS_RLC_UM_DIR
export AS_RLC_AM_DIR
export AS_RLC_TM_DIR
export AS_RLC_DIR
export AS_PDCP_DIR
export AS_MPLS_DIR
export AS_MEM_DIR
export AS_LIST_DIR
export AS_FIFO_DIR
export AS_MATH_DIR
export AS_TIMER_DIR
export AS_RRM_CONFIG_DIR



obj-m += openair_l2.o




openair_l2-objs += layer2_top.o MAC/layer2_scheduler.o MAC/main.o MAC/nodeb_control_plane_procedures.o MAC/ue_scheduler.o MAC/nodeb_scheduler.o MAC/ue_control_plane_procedures.o MAC/lchan_interface.o MAC/utils.o MAC/openair2_proc.o

openair_l2-objs += register.o



openair_l2-objs +=  $(AS_LIST_DIR)/list.o
openair_l2-objs +=  $(AS_LIST_DIR)/list2.o

openair_l2-objs +=  $(AS_TIMER_DIR)/umts_timer.o

openair_l2-objs +=  $(AS_PDCP_DIR)/pdcp.o
openair_l2-objs +=  $(AS_PDCP_DIR)/pdcp_fifo.o

openair_l2-objs +=  $(AS_RLC_AM_DIR)/rlc_am.o
openair_l2-objs +=  $(AS_RLC_AM_DIR)/rlc_am_segment.o
openair_l2-objs +=  $(AS_RLC_AM_DIR)/rlc_am_fsm.o
openair_l2-objs +=  $(AS_RLC_AM_DIR)/rlc_am_mux.o
openair_l2-objs +=  $(AS_RLC_AM_DIR)/rlc_am_retrans.o
openair_l2-objs +=  $(AS_RLC_AM_DIR)/rlc_am_util.o
openair_l2-objs +=  $(AS_RLC_AM_DIR)/rlc_am_mac_status.o
openair_l2-objs +=  $(AS_RLC_AM_DIR)/rlc_am_demux.o
openair_l2-objs +=  $(AS_RLC_AM_DIR)/rlc_am_receiver.o
openair_l2-objs +=  $(AS_RLC_AM_DIR)/rlc_am_reassembly.o
openair_l2-objs +=  $(AS_RLC_AM_DIR)/rlc_am_discard_rx.o
openair_l2-objs +=  $(AS_RLC_AM_DIR)/rlc_am_control_primitives.o
openair_l2-objs +=  $(AS_RLC_AM_DIR)/rlc_am_reset.o
openair_l2-objs +=  $(AS_RLC_AM_DIR)/rlc_am_status.o
openair_l2-objs +=  $(AS_RLC_AM_DIR)/rlc_am_timers.o
openair_l2-objs +=  $(AS_RLC_AM_DIR)/rlc_am_discard_notif.o
openair_l2-objs +=  $(AS_RLC_AM_DIR)/rlc_am_discard_tx.o

openair_l2-objs +=  $(AS_RLC_TM_DIR)/rlc_tm.o
openair_l2-objs +=  $(AS_RLC_TM_DIR)/rlc_tm_control_primitives.o
openair_l2-objs +=  $(AS_RLC_TM_DIR)/rlc_tm_fsm.o
openair_l2-objs +=  $(AS_RLC_TM_DIR)/rlc_tm_segment.o
openair_l2-objs +=  $(AS_RLC_TM_DIR)/rlc_tm_reassembly.o

openair_l2-objs +=  $(AS_RLC_UM_DIR)/rlc_um.o
openair_l2-objs +=  $(AS_RLC_UM_DIR)/rlc_um_fsm.o
openair_l2-objs +=  $(AS_RLC_UM_DIR)/rlc_um_control_primitives.o
openair_l2-objs +=  $(AS_RLC_UM_DIR)/rlc_um_segment.o
openair_l2-objs +=  $(AS_RLC_UM_DIR)/rlc_um_reassembly.o
openair_l2-objs +=  $(AS_RLC_UM_DIR)/rlc_um_receiver.o

openair_l2-objs +=  $(AS_RLC_DIR)/rlc_mac.o
openair_l2-objs +=  $(AS_RLC_DIR)/rlc.o
openair_l2-objs +=  $(AS_RLC_DIR)/rlc_rrc.o
openair_l2-objs +=  $(AS_RLC_DIR)/rlc_mpls.o


openair_l2-objs +=  $(AS_MATH_DIR)/random.o
openair_l2-objs +=  $(AS_MATH_DIR)/crc_byte.o

openair_l2-objs +=  $(AS_MEM_DIR)/mem_block.o


openair_l2-objs += $(PHY_INTERFACE_DIR)/mac_phy_primitives.o


ifdef MASTER
EXTRA_CFLAGS += -DNODE_RG
else
EXTRA_CFLAGS += -DNODE_MT
endif


ifdef NO_RRM
EXTRA_CFLAGS += -DNO_RRM
else
ifndef USER_MODE
EXTRA_CFLAGS += -DRRC_RRM_FIFOS_XFACE
endif
endif


CCC=gcc

ifdef PHYEMUL
EXTRA_CFLAGS += -DPHY_EMUL
else
EXTRA_CFLAGS += -I$(OPENAIR1_DIR) -DOPENAIR1 -DOPENAIR2
endif

#--------------------------------------------------------
EXTRA_CFLAGS +=   -DKERNEL$(KERNEL_MAIN_TYPE)  -DBIGPHYSAREA #-DNO_RRM  -DRRC_RRM_XFACE -DRRC_RRM_FIFOS_XFACE 


EXTRA_CFLAGS += -Wall -Wstrict-prototypes  $(if $(SET_X64),-DARCH_64,) $(if $(SET_X64),-mcmodel=kernel,) $(if $(SET_X64),-m64,) -fno-common -fno-strict-aliasing -pipe -freg-struct-return -ffreestanding -maccumulate-outgoing-args -funroll-loops -mmmx -msse -msse2 -fomit-frame-pointer -nostdinc -DMODULE  -D_LOOSE_KERNEL_NAMES -O2  $(GT2629)


EXTRA_CFLAGS += -I$(OPENAIR2_DIR)/COMMON -I$(OPENAIR2_DIR)/PHY_INTERFACE -I$(OPENAIR2_DIR)/UTIL/TIMER -I$(OPENAIR2_DIR)/UTIL/MATH -I$(OPENAIR2_DIR)/UTIL/LISTS -I$(OPENAIR2_DIR)/UTIL/MEM -I$(OPENAIR2_DIR)/LAYER2/RLC -I$(OPENAIR2_DIR)/LAYER2/RLC/UM_v6.1.0_LITE -I$(OPENAIR2_DIR)/LAYER2/RLC/TM -I$(OPENAIR2_DIR)/LAYER2/RLC/AM -I$(OPENAIR2_DIR)/LAYER2/PDCP -I$(OPENAIR2_DIR) -I$(OPENAIR3_DIR)/MESH -I$(OPENAIR3_DIR)/MESH/RRM 

