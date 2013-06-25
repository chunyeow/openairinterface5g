export KMODDIR?=       updates
KMODDIR_ARG:=   "INSTALL_MOD_DIR=$(KMODDIR)"
ifneq ($(origin KLIB), undefined)
KMODPATH_ARG:=  "INSTALL_MOD_PATH=$(KLIB)"
else
export KLIB:=          /lib/modules/$(shell uname -r)
endif
export KLIB_BUILD ?=	$(KLIB)/build
export MAKE

DESTDIR?=

ifneq ($(KERNELRELEASE),)

-include $(COMPAT_CONFIG)
include $(COMPAT_CONFIG_CW)

NOSTDINC_FLAGS := -I$(M)/include/ \
	-include $(M)/include/linux/compat-2.6.h \
	$(CFLAGS)

obj-y := compat/

#obj-$(CONFIG_COMPAT_RFKILL) += net/rfkill/

ifeq ($(BT),)
obj-$(CONFIG_COMPAT_WIRELESS) += net/wireless/ net/mac80211/
#obj-$(CONFIG_COMPAT_WIRELESS_MODULES) += drivers/net/wireless/

#obj-$(CONFIG_COMPAT_NET_USB_MODULES) += drivers/net/usb/

#obj-$(CONFIG_COMPAT_NETWORK_MODULES) += drivers/net/ethernet/atheros/
#obj-$(CONFIG_COMPAT_NETWORK_MODULES) += drivers/net/ethernet/broadcom/

#obj-$(CONFIG_COMPAT_VAR_MODULES) += drivers/ssb/
#obj-$(CONFIG_COMPAT_VAR_MODULES) += drivers/bcma/
#obj-$(CONFIG_COMPAT_VAR_MODULES) += drivers/misc/eeprom/

ifeq ($(CONFIG_STAGING_EXCLUDE_BUILD),)
endif

endif

#obj-$(CONFIG_COMPAT_BLUETOOTH) += net/bluetooth/
#obj-$(CONFIG_COMPAT_BLUETOOTH_MODULES) += drivers/bluetooth/

else

export PWD :=	$(shell pwd)

# The build will fail if there is any space in PWD.
ifneq (,$(findstring  $() ,$(PWD)))
$(error "The path to this compat-wireless directory has spaces in it." \
	"Please put it somewhere where there is no space")
endif

export CFLAGS += \
        -DCOMPAT_BASE_TREE="\"$(shell cat $(PWD)/compat_base_tree)\"" \
        -DCOMPAT_BASE_TREE_VERSION="\"$(shell cat $(PWD)/compat_base_tree_version)\"" \
        -DCOMPAT_PROJECT="\"Compat-wireless\"" \
        -DCOMPAT_VERSION="\"$(shell cat $(PWD)/compat_version)\""

# These exported as they are used by the scripts
# to check config and compat autoconf
export COMPAT_CONFIG_CW=$(PWD)/config.mk
export COMPAT_CONFIG=$(PWD)/.config
export CONFIG_CHECK=$(PWD)/.config.mk_md5sum.txt
export COMPAT_AUTOCONF=include/linux/compat_autoconf.h
export CREL=$(shell cat $(PWD)/compat_version)
export CREL_PRE:=.compat_autoconf_
export CREL_CHECK:=$(PWD)/$(CREL_PRE)$(CREL)

all: modules

$(COMPAT_CONFIG): ;

modules: $(CREL_CHECK)
	$(MAKE) -C $(KLIB_BUILD) M=$(PWD) modules
	@touch $@

bt: $(CREL_CHECK)
	+@./scripts/check_config.sh
	$(MAKE) -C $(KLIB_BUILD) M=$(PWD) BT=TRUE modules
	@touch $@

# We use a CREL_CHECK variable which will depend on the environment used to
# build. If the environment requirements change it forces a reconfiguration
# check.  This means we force a new reconfiguration check if a the user gets a
# new updates of compat-wireless or when the user updates the $(COMPAT_CONFIG)
# file.
# XXX: add kernel target to the CREL_CHECK mix, this would ensure we also
# reconfigure and build again fresh if we detect a new target kernel is
# being used.
$(CREL_CHECK):
	@# Force to regenerate compat autoconf
	+@./compat/scripts/gen-compat-config.sh > $(COMPAT_CONFIG)
	@rm -f $(CONFIG_CHECK)
	+@./scripts/check_config.sh
	@md5sum $(COMPAT_CONFIG_CW) > $(CONFIG_CHECK)
	@touch $@

btinstall: btuninstall bt-install-modules

bt-install-modules: bt
	$(MAKE) -C $(KLIB_BUILD) M=$(PWD) $(KMODDIR_ARG) $(KMODPATH_ARG) BT=TRUE \
		modules_install
	@/sbin/depmod -ae
	@echo
	@echo Now run:
	@echo
	@echo sudo make btunload:
	@echo
	@echo And then load the needed bluetooth modules. If unsure reboot.
	@echo

btuninstall:
	@# New location, matches upstream
	@rm -rf $(KLIB)/$(KMODDIR)/net/bluetooth/
	@rm -rf $(KLIB)/$(KMODDIR)/drivers/bluetooth/
	@# Lets only remove the stuff we are sure we are providing
	@# on the misc directory.
	@/sbin/depmod -ae
	@echo

btclean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) BT=TRUE clean
	@rm -f $(CREL_PRE)*

install: uninstall install-modules install-scripts

install-modules: modules
	$(MAKE) -C $(KLIB_BUILD) M=$(PWD) $(KMODDIR_ARG) $(KMODPATH_ARG) \
		modules_install
	@./scripts/update-initramfs

install-scripts:
	@# All the scripts we can use
	@mkdir -p $(DESTDIR)/usr/lib/compat-wireless/
	@install scripts/modlib.sh	$(DESTDIR)/usr/lib/compat-wireless/
	@install scripts/madwifi-unload	$(DESTDIR)/usr/sbin/
	@# This is to allow switching between drivers without blacklisting
	@install scripts/athenable	$(DESTDIR)/usr/sbin/
	@install scripts/b43enable	$(DESTDIR)/usr/sbin/
	@install scripts/iwl-enable	$(DESTDIR)/usr/sbin/
	@install scripts/alx-enable	$(DESTDIR)/usr/sbin/
	@install scripts/athload	$(DESTDIR)/usr/sbin/
	@install scripts/b43load	$(DESTDIR)/usr/sbin/
	@install scripts/iwl-load	$(DESTDIR)/usr/sbin/
	@if [ $(shell modinfo ath_pci > /dev/null 2>&1 && echo 1) ]; then \
		echo -n "Note: madwifi detected, we're going to disable it. "  ;\
		echo "If you would like to enable it later you can run:"  ;\
		echo "    sudo athenable madwifi"  ;\
		echo ;\
		echo Running athenable ath5k...;\
		$(DESTDIR)/usr/sbin/athenable ath5k ;\
	fi
	@if [ $(shell modinfo iwl4965 > /dev/null 2>&1 && echo 1) ]; then \
		echo ;\
		echo -n "Note: iwl4965 detected, we're going to disable it. "  ;\
		echo "If you would like to enable it later you can run:"  ;\
		echo "    sudo iwl-load iwl4965"  ;\
		echo ;\
		echo Running iwl-enable iwlagn...;\
		$(DESTDIR)/usr/sbin/iwl-enable iwlagn ;\
	fi
	@if [ $(shell modinfo iwlagn > /dev/null 2>&1 && echo 1) ] \
	 && [ $(shell modinfo iwlwifi > /dev/null 2>&1 && echo 1) ]; then \
		echo ;\
		echo -n "Note: iwlagn detected, we're going to disable it. " ;\
		echo "If you would like to enable it later you can run:"  ;\
		echo "    sudo iwl-load iwlagn"  ;\
		echo ;\
		echo Running iwl-enable iwlwifi...;\
		$(DESTDIR)/usr/sbin/iwl-enable iwlwifi ;\
	fi
	@if [ $(shell modinfo atl1c > /dev/null 2>&1 && echo 1) ]; then \
		echo ;\
		echo -n "Note: atl1c detected, we're going to disable it. "  ;\
		echo "If you would like to enable it later you can run:"  ;\
		echo "    sudo alx-load atl1c"  ;\
		echo ;\
		echo Running alx-enable alx...;\
		$(DESTDIR)/usr/sbin/alx-enable alx;\
	fi
	@# If on distributions like Mandriva which like to
	@# compress their modules this will find out and do
	@# it for you. Reason is some old version of modutils
	@# won't know mac80211.ko should be used instead of
	@# mac80211.ko.gz
	@./scripts/compress_modules
	@# Mandrake doesn't have a depmod.d/ conf file to prefer
	@# the updates/ dir which is what we use so we add one for it
	@# (or any other distribution that doens't have this).
	@./scripts/check_depmod
	@# Udev stuff needed for the new compat_firmware_class.
	@./compat/scripts/compat_firmware_install
	@/sbin/depmod -a
	@echo 
	@echo Now run:
	@echo 
	@echo sudo make unload to unload all: wireless, bluetooth and ethernet modules
	@echo sudo make wlunload to unload wireless modules
	@echo sudo make btunload to unload bluetooth modules
	@echo
	@echo Run sudo modprobe 'driver-name' to load your desired driver. 
	@echo If unsure reboot.
	@echo

uninstall:
	@# New location, matches upstream
	@rm -rf $(KLIB)/$(KMODDIR)/compat/
	@rm -rf $(KLIB)/$(KMODDIR)/net/mac80211/
	@rm -rf $(KLIB)/$(KMODDIR)/net/rfkill/
	@rm -rf $(KLIB)/$(KMODDIR)/net/wireless/
	@rm -rf $(KLIB)/$(KMODDIR)/drivers/ssb/
	@rm -rf $(KLIB)/$(KMODDIR)/drivers/net/usb/
	@rm -rf $(KLIB)/$(KMODDIR)/drivers/net/wireless/
	@rm -rf $(KLIB)/$(KMODDIR)/drivers/staging/
	@rm -rf $(KLIB)/$(KMODDIR)/drivers/net/atl*
	@# Lets only remove the stuff we are sure we are providing
	@# on the misc directory.
	@rm -f $(KLIB)/$(KMODDIR)/drivers/misc/eeprom/eeprom_93cx6.ko*
	@rm -f $(KLIB)/$(KMODDIR)/drivers/misc/eeprom_93cx6.ko*
	@rm -f $(KLIB)/$(KMODDIR)/drivers/net/b44.ko*
	@/sbin/depmod -a
	@echo 

clean:
	@if [ -d net -a -d $(KLIB_BUILD) ]; then \
		$(MAKE) -C $(KLIB_BUILD) M=$(PWD) clean ;\
	fi
	@rm -f $(CREL_PRE)*
unload:
	@./scripts/unload.sh

btunload:
	@./scripts/btunload.sh

wlunload:
	@./scripts/wlunload.sh


.PHONY: all clean install uninstall unload btunload wlunload modules bt Makefile

endif

clean-files += Module.symvers Module.markers modules modules.order
clean-files += $(CREL_CHECK) $(CONFIG_CHECK) $(COMPAT_CONFIG)
