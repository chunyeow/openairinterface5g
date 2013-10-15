# compiles the openair_usrp

#OPENAIRTARGETS_DIR ?=../../../..

#OPENAIROBJS += $(OPENAIRTARGETS_DIR)/ARCH/USRP/USERSPACE/LIB/openair0_lib.o
#CFLAGS += -I$(OPENAIRTARGETS_DIR)/ARCH/USRP/USERSPACE/LIB -I$(OPENAIRTARGETS_DIR)/ARCH/USRP/DEFS


#example: example.o $(OPENAIROBJS) 
#	gcc -o $@ $(CFLAGS) -lm $(OPENAIROBJS) $<

#openair_usrp.o:
#	$(CXX) -c openair_usrp.cpp -o openair_usrp.o

#clean:
#	rm -f *.o *~
#a	rm -f example
USRP_OBJ += $(OPENAIR_TARGETS)/ARCH/USRP/USERSPACE/LIB/openair_usrp.o
USRP_FILE_OBJ += $(OPENAIR_TARGETS)/ARCH/USRP/USERSPACE/LIB/openair_usrp.cpp
USRP_CFLAGS += -I$(OPENAIR_TARGETS)/ARCH/USRP/USERSPACE/LIB/

