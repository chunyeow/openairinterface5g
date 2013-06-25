obj-m 		+= kemul_nl.o

all:
	make -C .. 	

modules:
	@echo "**** module $(obj-m) ******"
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	
cleank:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
