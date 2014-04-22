# UE IP Driver makefile
#
NAS_UPDIR	:= $(shell /bin/pwd)

KERNEL_NAME=$(shell echo `uname -r`)
KERNEL_MAIN_TYPE=$(shell echo `uname -r | cut -d. -f-2  | tr "." "_"`)
export KERNEL_MAIN_TYPE
KERNEL_MAIN_VERSION=$(shell echo `uname -r | cut -d. -f-1`)
KERNEL_ARCH=$(shell echo `uname -m`)
#SET_REGPARM=$(shell if [ $(KERNEL_ARCH) = 'i686' -a $(SUBVERSION) -ge 20 ]; then echo true ; fi)


ifeq ($(KERNEL_MAIN_VERSION),2)
ifeq ($(IS_LINUX), 1)
SUBVERSION=$(shell echo `grep '^SUBLEVEL =' /usr/src/linux/Makefile | sed -e 's, ,,g' | sed -e 's/SUBLEVEL=//'`)
else
ifeq ($(IS_KERNEL_OPENAIRINTERFACE), 1)
SUBVERSION=$(shell echo `grep '^SUBLEVEL =' /usr/src/linux-$(KERNEL_NAME)/Makefile | sed -e 's, ,,g' | sed -e 's/SUBLEVEL=//'`)
else
SUBVERSION=$(shell echo `grep '^SUBLEVEL =' /usr/src/linux-headers-$(KERNEL_NAME)/Makefile | sed -e 's, ,,g' | sed -e 's/SUBLEVEL=//'`)
endif
endif
SET_X64=$(shell if [ $(KERNEL_ARCH) = 'x86_64' -a $(SUBVERSION) -ge 20 ]; then echo true ; fi)
IS_KERNEL2_SUBVERSION_GREATER_THAN_20=$(shell if [ $(SUBVERSION) -ge 20 ] ; then echo true ; fi)
IS_KERNEL2_SUBVERSION_GREATER_THAN_22=$(shell if [ $(SUBVERSION) -ge 22 ] ; then echo true ; fi)
IS_KERNEL2_SUBVERSION_GREATER_THAN_29=$(shell if [ $(SUBVERSION) -ge 29 ] ; then echo true ; fi)
IS_KERNEL2_SUBVERSION_GREATER_THAN_30=$(shell if [ $(SUBVERSION) -ge 30 ] ; then echo true ; fi)
IS_KERNEL2_SUBVERSION_GREATER_THAN_32=$(shell if [ $(SUBVERSION) -ge 32 ] ; then echo true ; fi)
IS_KERNEL_MAIN_VERSION_IS_2 = "true"
endif




# Add global rule for V3 kernels
ifeq ($(KERNEL_MAIN_VERSION),3)
ifeq ($(IS_LINUX), 1)
PATCHLEVEL=$(shell echo `grep '^PATCHLEVEL =' /usr/src/linux/Makefile | sed -e 's, ,,g' | sed -e 's/PATCHLEVEL=//'`)
else
ifeq ($(IS_KERNEL_OPENAIRINTERFACE), 1)
PATCHLEVEL=$(shell echo `grep '^PATCHLEVEL =' /usr/src/linux-$(KERNEL_NAME)/Makefile | sed -e 's, ,,g' | sed -e 's/PATCHLEVEL=//'`)
else
PATCHLEVEL=$(shell echo `grep '^PATCHLEVEL =' /usr/src/linux-headers-$(KERNEL_NAME)/Makefile | sed -e 's, ,,g' | sed -e 's/PATCHLEVEL=//'`)
endif
endif
   SET_X64=$(shell if [ $(KERNEL_ARCH) = 'x86_64' ]; then echo true ; fi)

   IS_KERNEL2_SUBVERSION_GREATER_THAN_22 = "true"
   IS_KERNEL2_SUBVERSION_GREATER_THAN_29 = "true"
   IS_KERNEL2_SUBVERSION_GREATER_THAN_30 = "true"
   IS_KERNEL2_SUBVERSION_GREATER_THAN_32 = "true"
   IS_KERNEL3_PATCHLEVEL_GREATER_THAN_2=$(shell if [ $(PATCHLEVEL) -ge 2 ] ; then echo true ; fi)
   IS_KERNEL3_PATCHLEVEL_GREATER_THAN_5=$(shell if [ $(PATCHLEVEL) -ge 5 ] ; then echo true ; fi)
   IS_KERNEL3_PATCHLEVEL_GREATER_THAN_8=$(shell if [ $(PATCHLEVEL) -ge 8 ] ; then echo true ; fi)
   IS_KERNEL3_PATCHLEVEL_GREATER_THAN_9=$(shell if [ $(PATCHLEVEL) -ge 9 ] ; then echo true ; fi)
   IS_KERNEL_MAIN_VERSION_IS_3 = "true"
endif

GT2620= $(if $(IS_KERNEL2_SUBVERSION_GREATER_THAN_20),-DKERNEL_VERSION_GREATER_THAN_2622=1)
GT2622= $(if $(IS_KERNEL2_SUBVERSION_GREATER_THAN_22),-DKERNEL_VERSION_GREATER_THAN_2622=1)
GT2629= $(if $(IS_KERNEL2_SUBVERSION_GREATER_THAN_29),-DKERNEL_VERSION_GREATER_THAN_2629=1)
GT2632= $(if $(IS_KERNEL2_SUBVERSION_GREATER_THAN_32),-DKERNEL_VERSION_GREATER_THAN_2632=1)
V3=     $(if $(IS_KERNEL_MAIN_VERSION_IS_3),-DKERNEL_MAIN_VERSION_IS_3=1)
GT32=   $(if $(IS_KERNEL3_PATCHLEVEL_GREATER_THAN_2),-DKERNEL_VERSION_GREATER_THAN_3200=1)
GT35=   $(if $(IS_KERNEL3_PATCHLEVEL_GREATER_THAN_5),-DKERNEL_VERSION_GREATER_THAN_3500=1)
GT38=   $(if $(IS_KERNEL3_PATCHLEVEL_GREATER_THAN_8),-DKERNEL_VERSION_GREATER_THAN_3800=1)
GT39=   $(if $(IS_KERNEL3_PATCHLEVEL_GREATER_THAN_9),-DKERNEL_VERSION_GREATER_THAN_3900=1)
####################################################
#  D E B U G   F L A G S
####################################################



####################################################
#      EXTRA COMPILER FLAGS
####################################################
EXTRA_CFLAGS = -I$(OPENAIR2_DIR)/COMMON -fno-common $(if $(IS_KERNEL_SUBVERSION_GREATER_THAN_20),-mregparm=3  -fno-stack-protector -mpreferred-stack-boundary=4)  $(if $(SET_X64),-DARCH_64,) $(if $(SET_X64),-mcmodel=kernel,) $(if $(SET_X64),-m64,) $(GT2622) $(GT2629) $(GT2632) $(V3) $(GT32) $(GT35) $(GT38) $(GT39)



ifdef OAI_NW_DRIVER_USE_NETLINK
EXTRA_CFLAGS += -DOAI_NW_DRIVER_USE_NETLINK
else
EXTRA_CFLAGS += $(shell rtai-config --module-cflags) -DRTAI -D__IN_RTAI__ -Wall
endif


####################################################
#      LOADABLE MODULE GOALS
####################################################
obj-m += ue_ip.o
ue_ip-objs += device.o
ue_ip-objs += common.o
ifdef OAI_NW_DRIVER_USE_NETLINK
ue_ip-objs += netlink.o
endif

####################################################
#      REVOIR LE CLEAN
####################################################


print:
	@echo KERNEL_NAME         : $(KERNEL_NAME)
	@echo KERNEL_MAIN_VERSION : $(KERNEL_MAIN_VERSION) 
	@echo KERNEL_SUB_VERSION  : $(SUBVERSION) 
	@echo KERNEL_PATCHLEVEL   : $(PATCHLEVEL) 
	@echo SET_X64             : $(SET_X64)
	@echo flag gt2620         : $(GT2620)
	@echo flag gt2622         : $(GT2622)
	@echo flag gt2629         : $(GT2629)
	@echo flag gt2632         : $(GT2632)
	@echo flag gt32           : $(GT32)
	@echo flag gt35           : $(GT35)
	@echo flag gt38           : $(GT38)
	@echo flag gt39           : $(GT39)

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
