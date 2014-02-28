# NASMESH Driver makefile
#
NAS_UPDIR	:= $(shell /bin/pwd)

KERNEL_NAME=$(shell echo `uname -r`)

KERNEL_MAIN_TYPE=$(shell echo `uname -r | cut -d. -f-2  | tr "." "_"`)
export KERNEL_MAIN_TYPE

KERNEL_MAIN_VERSION=$(shell echo `uname -r | cut -d. -f-1`)

ifeq ($(IS_LINUX), 1)
PATCHLEVEL=$(shell echo `grep '^PATCHLEVEL =' /usr/src/linux/Makefile | sed -e 's, ,,g' | sed -e 's/PATCHLEVEL=//'`)
SUBLEVEL=$(shell echo `grep '^SUBLEVEL =' /usr/src/linux/Makefile | sed -e 's, ,,g' | sed -e 's/SUBLEVEL=//'`)
else
ifeq ($(IS_KERNEL_OPENAIRINTERFACE), 1)
PATCHLEVEL=$(shell echo `grep '^PATCHLEVEL =' /usr/src/linux-$(KERNEL_NAME)/Makefile | sed -e 's, ,,g' | sed -e 's/PATCHLEVEL=//'`)
SUBLEVEL=$(shell echo `grep '^SUBLEVEL =' /usr/src/linux-$(KERNEL_NAME)/Makefile | sed -e 's, ,,g' | sed -e 's/SUBLEVEL=//'`)
else
PATCHLEVEL=$(shell echo `grep '^PATCHLEVEL =' /usr/src/linux-headers-$(KERNEL_NAME)/Makefile | sed -e 's, ,,g' | sed -e 's/PATCHLEVEL=//'`)
SUBLEVEL=$(shell echo `grep '^SUBLEVEL =' /usr/src/linux-headers-$(KERNEL_NAME)/Makefile | sed -e 's, ,,g' | sed -e 's/SUBLEVEL=//'`)
endif
endif

ifeq ($(KERNEL_MAIN_VERSION),2)
	ifeq ($(PATCHLEVEL),6)
		IS_KERNEL_GREATER_THAN_2620=$(shell if [ $(SUBLEVEL) -ge 20 ] ; then echo true ; fi)
		IS_KERNEL_GREATER_THAN_2622=$(shell if [ $(SUBLEVEL) -ge 22 ] ; then echo true ; fi)
		IS_KERNEL_GREATER_THAN_2629=$(shell if [ $(SUBLEVEL) -ge 29 ] ; then echo true ; fi)
		IS_KERNEL_GREATER_THAN_2630=$(shell if [ $(SUBLEVEL) -ge 30 ] ; then echo true ; fi)
		IS_KERNEL_GREATER_THAN_2632=$(shell if [ $(SUBLEVEL) -ge 32 ] ; then echo true ; fi)
		IS_KERNEL_MAIN_VERSION_IS_3 = "false"
		IS_KERNEL_GREATER_THAN_32 = "false"
		IS_KERNEL_GREATER_THAN_35 = "false"
	endif
else
	ifeq ($(KERNEL_MAIN_VERSION),3)
		IS_KERNEL_GREATER_THAN_2622 = "true"
		IS_KERNEL_GREATER_THAN_2629 = "true"
		IS_KERNEL_GREATER_THAN_2630 = "true"
		IS_KERNEL_GREATER_THAN_2632 = "true"
		IS_KERNEL_MAIN_VERSION_IS_3 = "true"
		IS_KERNEL_GREATER_THAN_32=$(shell if [ $(PATCHLEVEL) -ge 2 ] ; then echo true ; fi)
		IS_KERNEL_GREATER_THAN_35=$(shell if [ $(PATCHLEVEL) -ge 5 ] ; then echo true ; fi)
	endif
endif


KERNEL_ARCH=$(shell echo `uname -m`)
SET_X64=$(shell if [ $(KERNEL_ARCH) = 'x86_64' ]; then echo true ; fi)

GT2622= $(if $(IS_KERNEL_GREATER_THAN_2622),-DKERNEL_VERSION_GREATER_THAN_2622=1)
GT2629= $(if $(IS_KERNEL_GREATER_THAN_2629),-DKERNEL_VERSION_GREATER_THAN_2629=1)
GT2632= $(if $(IS_KERNEL_GREATER_THAN_2632),-DKERNEL_VERSION_GREATER_THAN_2632=1)
GT32= $(if $(IS_KERNEL_GREATER_THAN_32),-DKERNEL_VERSION_GREATER_THAN_32=1)
GT35= $(if $(IS_KERNEL_GREATER_THAN_35),-DKERNEL_VERSION_GREATER_THAN_35=1)
V3= $(if $(IS_KERNEL_MAIN_VERSION_IS_3),-DKERNEL_MAIN_VERSION_IS_3=1)


####################################################
#      EXTRA COMPILER FLAGS
####################################################
EXTRA_CFLAGS = -I$(OPENAIR2_DIR)/COMMON -fno-common $(if $(IS_KERNEL_GREATER_THAN_2620),-mregparm=3  -fno-stack-protector -mpreferred-stack-boundary=4)  $(if $(SET_X64),-DARCH_64,) $(if $(SET_X64),-mcmodel=kernel,) $(if $(SET_X64),-m64,) $(GT2622) $(GT2629) $(V3) $(GT32) $(GT35)

ifdef ADDRCONF
EXTRA_CFLAGS += -DADDRCONF
endif

ifdef OAI_NW_DRIVER_TYPE_ETHERNET
EXTRA_CFLAGS += -DOAI_NW_DRIVER_TYPE_ETHERNET
endif

ifdef OAI_NW_DRIVER_USE_NETLINK
EXTRA_CFLAGS += -DOAI_NW_DRIVER_USE_NETLINK
else
EXTRA_CFLAGS += $(shell rtai-config --module-cflags) -DRTAI -D__IN_RTAI__ -Wall
endif


ifdef LOOPBACK
EXTRA_CFLAGS += -DLOOPBACK_TEST
endif

ifdef ADDRESS_FIX
EXTRA_CFLAGS += -DNAS_ADDRESS_FIX
endif

####################################################
#      LOADABLE MODULE GOALS
####################################################
obj-m += oai_nw_drv.o
oai_nw_drv-objs += device.o
oai_nw_drv-objs += common.o
oai_nw_drv-objs += ioctl.o
oai_nw_drv-objs += classifier.o
oai_nw_drv-objs += tool.o
ifdef OAI_NW_DRIVER_USE_NETLINK
oai_nw_drv-objs += netlink.o
endif

####################################################
#      REVOIR LE CLEAN
####################################################

#netlink.ko:
	#make $(x)$(y) NAS_NETLINK=1 V=1 -C /usr/src/linux-$(shell echo `uname -r`) M=`pwd` modules

#oai_nw_drv.ko:
#	make  V=1 -C /usr/src/linux-$(shell echo `uname -r`) M=`pwd` modules
print:
	@echo linux kernel ge 22: $(IS_KERNEL_SUBVERSION_GREATER_THAN_22)
	@echo kernel name : $(KERNEL_NAME)
	@echo subversion : $(SUBVERSION)
	@echo flag gt2622: $(GT2622)
	@echo  linux kernel ge 29: $(IS_KERNEL_SUBVERSION_GREATER_THAN_29)
	@echo flag gt2629: $(GT2629)
	@echo  linux kernel ge 30: $(IS_KERNEL_SUBVERSION_GREATER_THAN_30)
	@echo  linux kernel ge 32: $(IS_KERNEL_SUBVERSION_GREATER_THAN_32)
	@echo  linux kernel ge 35: $(IS_KERNEL_SUBVERSION_GREATER_THAN_35)
	@echo flag KERNEL_MAIN_VERSION $(KERNEL_MAIN_VERSION)
clean:
	rm -f *.ko
	rm -f .*.ko.cmd
	rm -f .*.o.cmd
	rm -f *.o
	rm -f *.mod.c
	find . -name *.ko     -delete
	find . -name .*.o     -delete
	find . -name *.o      -delete
	find . -name *.mod.c  -delete
