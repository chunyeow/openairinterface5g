# UE IP Driver makefile
#
NAS_UPDIR	:= $(shell /bin/pwd)

KERNEL_ARCH=$(shell echo `uname -m`)
SET_X64=$(shell if [ $(KERNEL_ARCH) = 'x86_64' ]; then echo true ; fi)

####################################################
#      EXTRA COMPILER FLAGS
####################################################
EXTRA_CFLAGS = -I$(OPENAIR2_DIR)/COMMON -fno-common -fno-stack-protector -mpreferred-stack-boundary=4 $(if $(SET_X64),-DARCH_64,) $(if $(SET_X64),-mcmodel=kernel,) $(if $(SET_X64),-m64,) 



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
	@echo SET_X64             : $(SET_X64)

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
