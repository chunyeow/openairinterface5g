# not a GNU package. You can remove this line, if
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign 1.4

SUBDIRS = src

install_cbmimo1_softmodem_emos:
	make fifos
	insmod device_driver/openair_rf_softmodem_emos.ko ;

remove_cbmimo1_emos:
	rmmod openair_rf

fifos:
	@for i in `seq 0 64`;\
	do \
	have_rtfX=`ls /dev/ |grep -c rtf$$i`;\
	if test "$$have_rtfX" == 0 ;then \
	mknod -m 666 /dev/rtf$$i c 150 $$i; \
	fi;\
	done


