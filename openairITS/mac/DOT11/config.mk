ifeq ($(wildcard $(KLIB_BUILD)/.config),)
# These will be ignored by compat autoconf
 export CONFIG_PCI=y
 export CONFIG_USB=y
 export CONFIG_PCMCIA=y
 export CONFIG_SSB=m
else
include $(KLIB_BUILD)/.config
endif

ifneq ($(wildcard $(KLIB_BUILD)/Makefile),)

KERNEL_VERSION := $(shell $(MAKE) -C $(KLIB_BUILD) kernelversion | sed -n 's/^\([0-9]\)\..*/\1/p')

ifneq ($(KERNEL_VERSION),2)
else
KERNEL_26SUBLEVEL := $(shell $(MAKE) -C $(KLIB_BUILD) kernelversion | sed -n 's/^2\.6\.\([0-9]\+\).*/\1/p')
endif

ifdef CONFIG_COMPAT_KERNEL_2_6_24
$(error "ERROR: compat-wireless by default supports kernels >= 2.6.24, try enabling only one driver though")
endif #CONFIG_COMPAT_KERNEL_2_6_24

ifeq ($(CONFIG_CFG80211),y)
$(error "ERROR: your kernel has CONFIG_CFG80211=y, you should have it CONFIG_CFG80211=m if you want to use this thing.")
endif


# 2.6.27 has FTRACE_DYNAMIC borked, so we will complain if
# you have it enabled, otherwise you will very likely run into
# a kernel panic.
# XXX: move this to compat_autoconf.h script generation
ifeq ($(KERNEL_VERSION),2)
ifeq ($(shell test $(KERNEL_VERSION) -eq 2 -a $(KERNEL_26SUBLEVEL) -eq 27 && echo yes),yes)
ifeq ($(CONFIG_DYNAMIC_FTRACE),y)
$(error "ERROR: Your 2.6.27 kernel has CONFIG_DYNAMIC_FTRACE, please upgrade your distribution kernel as newer ones should not have this enabled (and if so report a bug) or remove this warning if you know what you are doing")
endif
endif
endif

# This is because with CONFIG_MAC80211 include/linux/skbuff.h will
# enable on 2.6.27 a new attribute:
#
# skb->do_not_encrypt
#
# and on 2.6.28 another new attribute:
#
# skb->requeue
#
# In kernel 2.6.32 both attributes were removed.
#
# XXX: move this to compat_autoconf.h script generation
ifeq ($(KERNEL_VERSION),2)
ifeq ($(shell test $(KERNEL_VERSION) -eq 2 -a $(KERNEL_26SUBLEVEL) -ge 27 -a $(KERNEL_26SUBLEVEL) -le 31 && echo yes),yes)
ifeq ($(CONFIG_MAC80211),)
$(error "ERROR: Your >=2.6.27 and <= 2.6.31 kernel has CONFIG_MAC80211 disabled, you should have it CONFIG_MAC80211=m if you want to use this thing.")
endif
endif
endif

ifneq ($(KERNELRELEASE),) # This prevents a warning

# We will warn when you don't have MQ support or NET_SCHED enabled.
#
# We could consider just quiting if MQ and NET_SCHED is disabled
# as I suspect all users of this package want 802.11e (WME) and
# 802.11n (HT) support.
ifeq ($(CONFIG_NET_SCHED),)
 QOS_REQS_MISSING+=CONFIG_NET_SCHED
endif

ifneq ($(QOS_REQS_MISSING),) # Complain about our missing dependencies
$(warning "WARNING: You are running a kernel >= 2.6.23, you should enable in it $(QOS_REQS_MISSING) for 802.11[ne] support")
endif

endif # build check
endif # kernel Makefile check

# These both are needed by compat-wireless || compat-bluetooth so enable them
 export CONFIG_COMPAT_RFKILL=y

ifeq ($(CONFIG_MAC80211),y)
$(error "ERROR: you have MAC80211 compiled into the kernel, CONFIG_MAC80211=y, as such you cannot replace its mac80211 driver. You need this set to CONFIG_MAC80211=m. If you are using Fedora upgrade your kernel as later version should this set as modular. For further information on Fedora see https://bugzilla.redhat.com/show_bug.cgi?id=470143. If you are using your own kernel recompile it and make mac80211 modular")
else
 export CONFIG_COMPAT_WIRELESS=y
 export CONFIG_COMPAT_WIRELESS_MODULES=m
 export CONFIG_COMPAT_VAR_MODULES=m
# We could technically separate these but not yet, we only have b44
# Note that we don't intend on backporting network drivers that
# use Multiqueue as that was a pain to backport to kernels older than
# 2.6.27. But -- we could just disable those drivers from kernels
# older than 2.6.27
 export CONFIG_COMPAT_NETWORK_MODULES=m
 export CONFIG_COMPAT_NET_USB_MODULES=m
endif

# The Bluetooth compatibility only builds on kernels >= 2.6.27 for now
ifndef CONFIG_COMPAT_KERNEL_2_6_27
ifeq ($(CONFIG_BT),y)
# we'll ignore compiling bluetooth
else
 export CONFIG_COMPAT_BLUETOOTH=y
 export CONFIG_COMPAT_BLUETOOTH_MODULES=m
endif
endif #CONFIG_COMPAT_KERNEL_2_6_27

#
# CONFIG_COMPAT_FIRMWARE_CLASS definition has no leading whitespace,
# because it gets passed-on through compat_autoconf.h.
#
ifdef CONFIG_COMPAT_KERNEL_2_6_33
ifndef CONFIG_COMPAT_RHEL_6_1
ifdef CONFIG_FW_LOADER
export CONFIG_COMPAT_FIRMWARE_CLASS=m
endif #CONFIG_FW_LOADER
endif #CONFIG_COMPAT_RHEL_6_1
endif #CONFIG_COMPAT_KERNEL_2_6_33

ifdef CONFIG_COMPAT_KERNEL_2_6_36
ifndef CONFIG_COMPAT_RHEL_6_1
 export CONFIG_COMPAT_KFIFO=y
endif #CONFIG_COMPAT_RHEL_6_1
endif #CONFIG_COMPAT_KERNEL_2_6_36

#
# CONFIG_COMPAT_BT_SOCK_CREATE_NEEDS_KERN definitions have no leading
# whitespace, because they get passed-on through compat_autoconf.h.
#
ifndef CONFIG_COMPAT_KERNEL_2_6_33
export CONFIG_COMPAT_BT_SOCK_CREATE_NEEDS_KERN=y
endif #CONFIG_COMPAT_KERNEL_2_6_33
ifdef CONFIG_COMPAT_RHEL_6_0
export CONFIG_COMPAT_BT_SOCK_CREATE_NEEDS_KERN=y
endif #CONFIG_COMPAT_RHEL_6_0

#
# CONFIG_COMPAT_FIRMWARE_DATA_RW_NEEDS_FILP definition has no leading
# whitespace, because it gets passed-on through compat_autoconf.h.
#
ifdef CONFIG_COMPAT_RHEL_6_0
export CONFIG_COMPAT_FIRMWARE_DATA_RW_NEEDS_FILP=y
endif #CONFIG_COMPAT_RHEL_6_0

# Wireless subsystem stuff
export CONFIG_MAC80211=m

ifndef CONFIG_COMPAT_KERNEL_2_6_33
export CONFIG_COMPAT_MAC80211_DRIVER_API_TRACER=y
endif #CONFIG_COMPAT_KERNEL_2_6_33

# CONFIG_MAC80211_DEBUGFS=y
# CONFIG_MAC80211_NOINLINE=y
# CONFIG_MAC80211_VERBOSE_DEBUG=y
# CONFIG_MAC80211_HT_DEBUG=y
# CONFIG_MAC80211_TKIP_DEBUG=y
# CONFIG_MAC80211_IBSS_DEBUG=y
# CONFIG_MAC80211_VERBOSE_PS_DEBUG=y
# CONFIG_MAC80211_VERBOSE_MPL_DEBUG=y
# CONFIG_MAC80211_VERBOSE_MHWMP_DEBUG=y
# CONFIG_MAC80211_VERBOSE_TDLS_DEBUG
# CONFIG_MAC80211_DEBUG_COUNTERS=y

# choose between pid and minstrel as default rate control algorithm
export CONFIG_MAC80211_RC_DEFAULT=minstrel_ht
export CONFIG_MAC80211_RC_DEFAULT_MINSTREL=y
# CONFIG_MAC80211_RC_DEFAULT_PID=y
# This is the one used by our compat-wireless net/mac80211/rate.c
# in case you have and old kernel which is overriding this to pid.
export CONFIG_COMPAT_MAC80211_RC_DEFAULT=minstrel_ht
export CONFIG_MAC80211_RC_PID=y
export CONFIG_MAC80211_RC_MINSTREL=y
export CONFIG_MAC80211_RC_MINSTREL_HT=y
ifdef CONFIG_LEDS_TRIGGERS
export CONFIG_MAC80211_LEDS=y
endif #CONFIG_LEDS_TRIGGERS

# enable mesh networking too
export CONFIG_MAC80211_MESH=y

export CONFIG_CFG80211=m
export CONFIG_CFG80211_DEFAULT_PS=y
# CONFIG_CFG80211_DEBUGFS=y
# CONFIG_NL80211_TESTMODE=y
# CONFIG_CFG80211_DEVELOPER_WARNINGS=y
# CONFIG_CFG80211_REG_DEBUG=y
# CONFIG_CFG80211_INTERNAL_REGDB=y
# See below for wext stuff

export CONFIG_LIB80211=m
export CONFIG_LIB80211_CRYPT_WEP=m
export CONFIG_LIB80211_CRYPT_CCMP=m
export CONFIG_LIB80211_CRYPT_TKIP=m
# CONFIG_LIB80211_DEBUG=y

export CONFIG_BT=m
export CONFIG_BT_RFCOMM=m
export CONFIG_BT_RFCOMM_TTY=y
export CONFIG_BT_BNEP=m
export CONFIG_BT_BNEP_MC_FILTER=y
export CONFIG_BT_BNEP_PROTO_FILTER=y
# CONFIG_BT_CMTP depends on ISDN_CAPI
ifdef CONFIG_ISDN_CAPI
export CONFIG_BT_CMTP=m
endif #CONFIG_ISDN_CAPI
ifndef CONFIG_COMPAT_KERNEL_2_6_28
export CONFIG_COMPAT_BT_HIDP=m
endif #CONFIG_COMPAT_KERNEL_2_6_28

export CONFIG_BT_HCIUART=M
export CONFIG_BT_HCIUART_H4=y
export CONFIG_BT_HCIUART_BCSP=y
export CONFIG_BT_HCIUART_ATH3K=y
export CONFIG_BT_HCIUART_LL=y

export CONFIG_BT_HCIVHCI=m
export CONFIG_BT_MRVL=m

ifdef CONFIG_PCMCIA
export CONFIG_BT_HCIDTL1=m
export CONFIG_BT_HCIBT3C=m
export CONFIG_BT_HCIBLUECARD=m
export CONFIG_BT_HCIBTUART=m
endif #CONFIG_PCMCIA


# We need CONFIG_WIRELESS_EXT for CONFIG_CFG80211_WEXT for every kernel 
# version. The new way CONFIG_CFG80211_WEXT is called from the kernel 
# does not work with compat-wireless because it calls some callback 
# function on struct wiphy. This struct is shipped with compat-wireless 
# and changes from kernel version to version. We are using the 
# wireless_handlers attribute which will be activated by 
# CONFIG_WIRELESS_EXT. 
ifdef CONFIG_WIRELESS_EXT
export CONFIG_CFG80211_WEXT=y
else #CONFIG_CFG80211_WEXT
$(warning "WARNING: CONFIG_CFG80211_WEXT will be deactivated or not working because kernel was compiled with CONFIG_WIRELESS_EXT=n. Tools using wext interface like iwconfig will not work. To activate it build your kernel e.g. with CONFIG_LIBIPW=m.")
endif #CONFIG_WIRELESS_EXT

ifdef CONFIG_STAGING
export CONFIG_COMPAT_STAGING=m
endif #CONFIG_STAGING

# mac80211 test driver
export CONFIG_MAC80211_HWSIM=m

export CONFIG_ATH5K=m
# CONFIG_ATH5K_DEBUG=y
# CONFIG_ATH5K_TRACER=y
# CONFIG_ATH5K_AHB=y

export CONFIG_ATH9K=m
export CONFIG_ATH9K_HW=m
export CONFIG_ATH9K_COMMON=m
# CONFIG_ATH9K_DEBUGFS=y
# CONFIG_ATH9K_AHB=y
# CONFIG_ATH9K_PKTLOG=y

# Disable this to get minstrel as default, we leave the ath9k
# rate control algorithm as the default for now as that is also
# default upstream on the kernel. We will move this to minstrel
# as default once we get minstrel properly tested and blessed by
# our systems engineering team. CCK rates also need to be used
# for long range considerations.
export CONFIG_COMPAT_ATH9K_RATE_CONTROL=y

export CONFIG_ATH9K_BTCOEX_SUPPORT=y

# PCI Drivers
ifdef CONFIG_PCI

export CONFIG_ATH5K_PCI=y
export CONFIG_ATH9K_PCI=y

export CONFIG_IWLWIFI=m
export CONFIG_IWLWIFI_P2P=y
# CONFIG_IWLWIFI_DEBUG=y
# CONFIG_IWLWIFI_DEBUGFS=y
# CONFIG_IWLWIFI_DEVICE_TRACING=y
# CONFIG_IWLWIFI_DEBUG_EXPERIMENTAL_UCODE=y

export CONFIG_IWLEGACY=m
export CONFIG_COMPAT_IWL4965=m
export CONFIG_IWL3945=m
# CONFIG_IWLEGACY_DEBUG=y
# CONFIG_IWLEGACY_DEBUGFS=y


export CONFIG_B43=m
export CONFIG_B43_HWRNG=y
export CONFIG_B43_PCI_AUTOSELECT=y
ifdef CONFIG_PCMCIA
export CONFIG_B43_PCMCIA=y
endif #CONFIG_PCMCIA
ifdef CONFIG_MAC80211_LEDS
export CONFIG_B43_LEDS=y
endif #CONFIG_MAC80211_LEDS
export CONFIG_B43_PHY_LP=y
export CONFIG_B43_PHY_N=y
export CONFIG_B43_PHY_HT=y
# CONFIG_B43_PHY_LCN=y
# CONFIG_B43_DEBUG=y

export CONFIG_B43LEGACY=m
export CONFIG_B43LEGACY_HWRNG=y
export CONFIG_B43LEGACY_PCI_AUTOSELECT=y
ifdef CONFIG_MAC80211_LEDS
export CONFIG_B43LEGACY_LEDS=y
endif #CONFIG_MAC80211_LEDS
# CONFIG_B43LEGACY_DEBUG=y
export CONFIG_B43LEGACY_DMA=y
export CONFIG_B43LEGACY_PIO=y

ifdef CONFIG_WIRELESS_EXT
# The Intel ipws
export CONFIG_LIBIPW=m
# CONFIG_LIBIPW_DEBUG=y

export CONFIG_IPW2100=m
export CONFIG_IPW2100_MONITOR=y
# CONFIG_IPW2100_DEBUG=y
export CONFIG_IPW2200=m
export CONFIG_IPW2200_MONITOR=y
export CONFIG_IPW2200_RADIOTAP=y
export CONFIG_IPW2200_PROMISCUOUS=y
export CONFIG_IPW2200_QOS=y
# CONFIG_IPW2200_DEBUG=y
# The above enables use a second interface prefixed 'rtap'.
#           Example usage:
#
# % modprobe ipw2200 rtap_iface=1
# % ifconfig rtap0 up
# % tethereal -i rtap0
#
# If you do not specify 'rtap_iface=1' as a module parameter then
# the rtap interface will not be created and you will need to turn
# it on via sysfs:
#
# % echo 1 > /sys/bus/pci/drivers/ipw2200/*/rtap_iface
endif #CONFIG_WIRELESS_EXT

ifdef CONFIG_SSB
# Sonics Silicon Backplane
export CONFIG_SSB_SPROM=y

export CONFIG_SSB_BLOCKIO=y
export CONFIG_SSB_PCIHOST=y
export CONFIG_SSB_B43_PCI_BRIDGE=y
ifdef CONFIG_PCMCIA
export CONFIG_SSB_PCMCIAHOST=y
endif #CONFIG_PCMCIA
# CONFIG_SSB_DEBUG=y
export CONFIG_SSB_DRIVER_PCICORE=y
export CONFIG_B43_SSB=y
endif #CONFIG_SSB

export CONFIG_BCMA=m
export CONFIG_BCMA_BLOCKIO=y
export CONFIG_BCMA_HOST_PCI=y
# CONFIG_BCMA_DEBUG=y
export CONFIG_B43_BCMA=y
export CONFIG_B43_BCMA_EXTRA=y

export CONFIG_P54_PCI=m

export CONFIG_B44=m
export CONFIG_B44_PCI=y

export CONFIG_RTL8180=m

export CONFIG_ADM8211=m

export CONFIG_RT2X00_LIB_PCI=m
export CONFIG_RT2400PCI=m
export CONFIG_RT2500PCI=m
ifdef CONFIG_CRC_CCITT
export CONFIG_RT2800PCI=m
export CONFIG_RT2800PCI_RT33XX=y
export CONFIG_RT2800PCI_RT35XX=y
export CONFIG_RT2800PCI_RT53XX=y
endif #CONFIG_CRC_CCITT
NEED_RT2X00=y

# Two rt2x00 drivers require firmware: rt61pci and rt73usb. They depend on
# CRC to check the firmware. We check here first for the PCI
# driver as we're in the PCI section.
ifdef CONFIG_CRC_ITU_T
export CONFIG_RT61PCI=m
endif #CONFIG_CRC_ITU_T

export CONFIG_MWL8K=m

# Ethernet drivers go here
export CONFIG_ATL1=m
export CONFIG_ATL2=m
export CONFIG_ATL1E=m
ifdef CONFIG_COMPAT_KERNEL_2_6_27
export CONFIG_ATL1C=n
export CONFIG_ALX=m
else #CONFIG_COMPAT_KERNEL_2_6_27
export CONFIG_ATL1C=n
export CONFIG_ALX=m
endif #CONFIG_COMPAT_KERNEL_2_6_27

ifdef CONFIG_WIRELESS_EXT
export CONFIG_HERMES=m
export CONFIG_HERMES_CACHE_FW_ON_INIT=y
ifdef CONFIG_PPC_PMAC
export CONFIG_APPLE_AIRPORT=m
endif #CONFIG_PPC_PMAC
export CONFIG_PLX_HERMES=m
export CONFIG_TMD_HERMES=m
export CONFIG_NORTEL_HERMES=m
export CONFIG_PCI_HERMES=m
ifdef CONFIG_PCMCIA
export CONFIG_PCMCIA_HERMES=m
export CONFIG_PCMCIA_SPECTRUM=m
endif #CONFIG_PCMCIA
endif #CONFIG_WIRELESS_EXT

export CONFIG_RTL8192CE=m
export CONFIG_RTL8192SE=m
export CONFIG_RTL8192DE=m

export CONFIG_BRCMSMAC=m

export CONFIG_MWIFIEX_PCIE=m

endif #CONFIG_PCI
## end of PCI

ifdef CONFIG_PCMCIA

ifdef CONFIG_COMPAT_KERNEL_2_6_27
export CONFIG_LIBERTAS=n
export CONFIG_LIBERTAS_CS=n
else #CONFIG_COMPAT_KERNEL_2_6_27
export CONFIG_LIBERTAS_CS=m
NEED_LIBERTAS=y
endif #CONFIG_COMPAT_KERNEL_2_6_27

endif #CONFIG_PCMCIA
## end of PCMCIA

# This is required for some cards
export CONFIG_EEPROM_93CX6=m

# USB Drivers
ifdef CONFIG_USB
ifndef CONFIG_COMPAT_KERNEL_2_6_29
export CONFIG_COMPAT_ZD1211RW=m
# CONFIG_ZD1211RW_DEBUG=y
endif #CONFIG_COMPAT_KERNEL_2_6_29

# Sorry, rndis_wlan uses cancel_work_sync which is new and can't be done in compat...

# Wireless RNDIS USB support (RTL8185 802.11g) A-Link WL54PC
# All of these devices are based on Broadcom 4320 chip which
# is only wireless RNDIS chip known to date.
# Note: this depends on CONFIG_USB_NET_RNDIS_HOST and CONFIG_USB_NET_CDCETHER
# it also requires new RNDIS_HOST and CDC_ETHER modules which we add
ifdef CONFIG_COMPAT_KERNEL_2_6_29
export CONFIG_USB_COMPAT_USBNET=n
export CONFIG_USB_NET_COMPAT_RNDIS_HOST=n
export CONFIG_USB_NET_COMPAT_RNDIS_WLAN=n
export CONFIG_USB_NET_COMPAT_CDCETHER=n
else #CONFIG_COMPAT_KERNEL_2_6_29
export CONFIG_USB_COMPAT_USBNET=m
ifdef CONFIG_USB_NET_CDCETHER
export CONFIG_USB_NET_COMPAT_RNDIS_HOST=m
export CONFIG_USB_NET_COMPAT_RNDIS_WLAN=m
endif #CONFIG_USB_NET_CDCETHER
ifdef CONFIG_USB_NET_CDCETHER_MODULE
export CONFIG_USB_NET_COMPAT_RNDIS_HOST=m
export CONFIG_USB_NET_COMPAT_RNDIS_WLAN=m
endif #CONFIG_USB_NET_CDCETHER
export CONFIG_USB_NET_COMPAT_CDCETHER=m
endif #CONFIG_COMPAT_KERNEL_2_6_29


export CONFIG_P54_USB=m
export CONFIG_RTL8187=m
ifdef CONFIG_MAC80211_LEDS
export CONFIG_RTL8187_LEDS=y
endif #CONFIG_MAC80211_LEDS

export CONFIG_AT76C50X_USB=m

ifndef CONFIG_COMPAT_KERNEL_2_6_29
export CONFIG_CARL9170=m
ifdef CONFIG_MAC80211_LEDS
export CONFIG_CARL9170_LEDS=y
endif #CONFIG_MAC80211_LEDS
# CONFIG_CARL9170_DEBUGFS=y
export CONFIG_CARL9170_WPC=y
endif #CONFIG_COMPAT_KERNEL_2_6_29

# This activates a threading fix for usb urb.
# this is mainline commit: b3e670443b7fb8a2d29831b62b44a039c283e351
# This fix will be included in some stable releases.
export CONFIG_COMPAT_USB_URB_THREAD_FIX=y

export CONFIG_ATH9K_HTC=m
# CONFIG_ATH9K_HTC_DEBUGFS=y

# RT2500USB does not require firmware
export CONFIG_RT2500USB=m
ifdef CONFIG_CRC_CCITT
export CONFIG_RT2800USB=m
export CONFIG_RT2800USB_RT33XX=y
export CONFIG_RT2800USB_RT35XX=y
export CONFIG_RT2800USB_RT53XX=y
export CONFIG_RT2800USB_UNKNOWN=y
endif #CONFIG_CRC_CCITT
export CONFIG_RT2X00_LIB_USB=m
NEED_RT2X00=y
# RT73USB requires firmware
ifdef CONFIG_CRC_ITU_T
export CONFIG_RT73USB=m
endif #CONFIG_CRC_ITU_T

ifdef CONFIG_COMPAT_KERNEL_2_6_27
export CONFIG_LIBERTAS_THINFIRM_USB=n
export CONFIG_LIBERTAS_USB=n
NEED_LIBERTAS=n
else #CONFIG_COMPAT_KERNEL_2_6_27
export CONFIG_LIBERTAS_THINFIRM_USB=m
export CONFIG_LIBERTAS_USB=m
NEED_LIBERTAS=y
endif #CONFIG_COMPAT_KERNEL_2_6_27

export CONFIG_ORINOCO_USB=m

export CONFIG_BT_HCIBTUSB=m
export CONFIG_BT_HCIBCM203X=m
export CONFIG_BT_HCIBPA10X=m
export CONFIG_BT_HCIBFUSB=m
export CONFIG_BT_ATH3K=m

export CONFIG_RTL8192CU=m

endif #CONFIG_USB end of USB driver list

ifdef CONFIG_SPI_MASTER
ifndef CONFIG_COMPAT_KERNEL_2_6_25

ifdef CONFIG_CRC7
export CONFIG_WL1251_SPI=m
export CONFIG_WL12XX_SPI=m
endif #CONFIG_CRC7
export CONFIG_P54_SPI=m

ifdef CONFIG_COMPAT_KERNEL_2_6_27
export CONFIG_LIBERTAS_SPI=n
NEED_LIBERTAS=n
else #CONFIG_COMPAT_KERNEL_2_6_27
export CONFIG_LIBERTAS_SPI=m
NEED_LIBERTAS=y
endif #CONFIG_COMPAT_KERNEL_2_6_27

endif #CONFIG_COMPAT_KERNEL_2_6_25
endif #CONFIG_SPI_MASTER end of SPI driver list

ifdef CONFIG_MMC

export CONFIG_SSB_SDIOHOST=y
export CONFIG_B43_SDIO=y

ifdef CONFIG_CRC7
ifdef CONFIG_WL12XX_PLATFORM_DATA
export CONFIG_COMPAT_WL1251_SDIO=m
endif #CONFIG_WL12XX_PLATFORM_DATA

ifndef CONFIG_COMPAT_KERNEL_2_6_32
ifdef CONFIG_WL12XX_PLATFORM_DATA
export CONFIG_COMPAT_WL12XX_SDIO=m
endif #CONFIG_WL12XX_PLATFORM_DATA
endif #CONFIG_COMPAT_KERNEL_2_6_32

endif #CONFIG_CRC7

export CONFIG_MWIFIEX_SDIO=m

ifndef CONFIG_COMPAT_KERNEL_2_6_32
export CONFIG_COMPAT_LIBERTAS_SDIO=m
NEED_LIBERTAS=y
endif #CONFIG_COMPAT_KERNEL_2_6_32

export CONFIG_IWM=m
# CONFIG_IWM_DEBUG=y
# CONFIG_IWM_TRACING=y

export CONFIG_BT_HCIBTSDIO=m
export CONFIG_BT_MRVL_SDIO=m

ifndef CONFIG_COMPAT_KERNEL_2_6_27
export CONFIG_ATH6KL=m
# CONFIG_ATH6KL_DEBUG=y
endif #CONFIG_COMPAT_KERNEL_2_6_27

ifndef CONFIG_COMPAT_KERNEL_2_6_29
export CONFIG_BRCMFMAC=m
export CONFIG_BRCMFMAC_SDIO=y
endif #CONFIG_COMPAT_KERNEL_2_6_29

endif #CONFIG_MMC

export CONFIG_RTLWIFI=m
export CONFIG_RTL8192C_COMMON=m

# Common rt2x00 requirements
ifeq ($(NEED_RT2X00),y)
export CONFIG_RT2X00=y
export CONFIG_RT2X00_LIB=m
export CONFIG_RT2800_LIB=m
export CONFIG_RT2X00_LIB_FIRMWARE=y
export CONFIG_RT2X00_LIB_CRYPTO=y
# CONFIG_RT2X00_LIB_SOC=y
ifdef CONFIG_COMPAT_KERNEL_2_6_25
export CONFIG_RT2X00_LIB_LEDS=n
else #CONFIG_COMPAT_KERNEL_2_6_25
ifdef CONFIG_LEDS_CLASS
export CONFIG_RT2X00_LIB_LEDS=y
endif #CONFIG_LEDS_CLASS
endif #CONFIG_COMPAT_KERNEL_2_6_25
# CONFIG_RT2X00_DEBUG=y
# CONFIG_RT2X00_LIB_DEBUGFS=y
endif

# p54
export CONFIG_P54_COMMON=m
ifdef CONFIG_MAC80211_LEDS
export CONFIG_P54_LEDS=y
endif #CONFIG_MAC80211_LEDS

# Atheros
export CONFIG_ATH_COMMON=m
# CONFIG_ATH_DEBUG=y

export CONFIG_BRCMUTIL=m
# CONFIG_BRCMDBG=y

ifdef CONFIG_CRC7
export CONFIG_WL1251=m
export CONFIG_WL12XX=m
endif #CONFIG_CRC7

export CONFIG_MWIFIEX=m

ifndef CONFIG_CORDIC
export CONFIG_COMPAT_CORDIC=y
endif #CONFIG_CORDIC

ifndef CONFIG_CRC8
export CONFIG_COMPAT_CRC8=y
endif #CONFIG_CRC8

ifdef CONFIG_COMPAT_KERNEL_2_6_27
export CONFIG_LIBERTAS=n
else #CONFIG_COMPAT_KERNEL_2_6_27
ifeq ($(NEED_LIBERTAS),y)
export CONFIG_LIBERTAS_THINFIRM=m
export CONFIG_LIBERTAS=m
export CONFIG_LIBERTAS_MESH=y
# CONFIG_LIBERTAS_DEBUG=y
endif
endif #CONFIG_COMPAT_KERNEL_2_6_27

# We need the backported rfkill module on kernel < 2.6.31.
# In more recent kernel versions use the in kernel rfkill module.
ifdef CONFIG_COMPAT_KERNEL_2_6_31
export CONFIG_RFKILL_BACKPORT=m
ifdef CONFIG_LEDS_TRIGGERS
export CONFIG_RFKILL_BACKPORT_LEDS=y
endif #CONFIG_LEDS_TRIGGERS
export CONFIG_RFKILL_BACKPORT_INPUT=y
endif #CONFIG_COMPAT_KERNEL_2_6_31

