CC=gcc
MAKE=/usr/bin/make
CONFIG_IEEE80211P=m
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

NOSTDINC_FLAGS := -I$(M)/../../mac/DOT11/include/ \

obj-$(CONFIG_IEEE80211P) += ieee80211p.o
ieee80211p-y += ieee80211p-driver.o

default :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

show :
	@echo $(KERNELDIR)
	@echo $(NOSTDINC_FLAGS)
