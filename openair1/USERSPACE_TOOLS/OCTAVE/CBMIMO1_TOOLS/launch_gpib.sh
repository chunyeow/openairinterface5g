modprobe ni_usb_gpib
fxload -D /proc/bus/usb/004/003 -I /usr/local/share/linux-gpib-3.2.11/gpib_firmware-2008-08-10/ni_gpib_usb_b/niusbb_firmware.hex -s /usr/local/share/linux-gpib-3.2.11/gpib_firmware-2008-08-10/ni_gpib_usb_b/niusbb_loader.hex
gpib_config
chmod a+rw /dev/gpib0
