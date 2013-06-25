OSTYPE		= $(shell uname -o )

#AAA: if rrc emulated remove comment from next line
#EMUL_RRC        = 1
#AAA: if sns emulated remove comment from next line
#EMUL_SNS        = 1  
RRC_KERNEL_MODE = 0 


#VPATH		=.:./deps:./obj
# en ABSOLU pour utiliser les tags avec nedit de n'importe ou
CHEMIN		= $(shell pwd)
OPENAIR1_DIR= $(CHEMIN)/../../../openair1
OPENAIR2_DIR= $(CHEMIN)/../../../openair2
OPENAIR2_RRC= $(OPENAIR2_DIR)/RRC/MESH/ 
OPENAIR1_SENSING= $(OPENAIR1_DIR)/USERSPACE_TOOLS/SENDORA
OPENAIR3_RRM= $(CHEMIN)
INTERFACE   = $(CHEMIN)/..
EMUL        = $(CHEMIN)/EMUL
VPATH		= $(CHEMIN):$(EMUL):$(CHEMIN)/deps:$(CHEMIN)/obj

INCLUDE     = -I.  -I$(OPENAIR2_RRC) -I$(OPENAIR3_RRM) -I$(INTERFACE) -I$(OPENAIR2_DIR) -I$(OPENAIR1_SENSING)
CFLAGS		+= -Wall -g  $(INCLUDE) -DDEBUG -mmmx -msse -msse2 -std=gnu99 -DNOTRACE -DOPENAIR2_IN
LDFLAGS		= -lm -lforms -lX11 -L/usr/X11R6/lib -lXpm -lpthread 

ifeq ($(OSTYPE),Cygwin)

LDFLAGS		+= -lpthread 
CFLAGS		+= -DCYGWIN

else

LDFLAGS		+= -lpthread -lrt
CFLAGS		+= -DLINUX

endif 

# ifdef RRC_KERNEL_MODE
ifeq ($(RRC_KERNEL_MODE),1)
CFLAGS		+= -DRRC_KERNEL_MODE
endif

TARGET	    = emul_int rrm

#put rrm_graph.c instead of rrm.c to connect the graphic interface block to the rrm
SRC	    = rrm_graph.c    
SRC	   += rrm_sock.c
SRC	   += rrm_util.c
SRC	   += rrm_2_rrc_msg.c
SRC	   += cmm_msg.c
SRC	   += pusu_msg.c
SRC	   += transact.c
SRC	   += rrm_constant.c
SRC	   += ch_init.c
SRC	   += mr_attach.c
SRC	   += rb_mngt.c
SRC	   += neighbor_db.c
SRC	   += rb_db.c
SRC	   += sens_db.c
SRC	   += channels_db.c
SRC	   += msg_mngt.c
SRC	   += sens_op.c
SRC	   += freq_ass_op.c
SRC    += rrm_2_sensing_msg.c 
SRC    += ip_msg.c 
SRC    += sensing_form.c 
SRC    += SN_freq_form.c 
SRC    += sens_scen_2_form.c 
SRC    += all_freq_to_users_form.c
SRC    += sens_CH1_scen_2.c
SRC    += sens_sensor.c  

SRC_EMUL    = emul_interface.c
SRC_EMUL   += rrm_sock.c
SRC_EMUL   += rrm_util.c
SRC_EMUL   += cmm_msg.c
SRC_EMUL   += pusu_msg.c
SRC_EMUL   += transact.c
SRC_EMUL   += actdiff.c

#ifeq ($(EMUL_RRC),0) 
ifndef EMUL_RRC
CFLAGS     += -DPHY_EMUL
else
CFLAGS	   += -DRRC_EMUL
SRC_EMUL   += rrc_2_rrm_msg.c
SRC_EMUL   += scenario.c
endif

ifdef EMUL_SNS
CFLAGS	   += -DSNS_EMUL
SRC_EMUL   += sensing_2_rrm_msg.c
endif

OBJ		= $(SRC:%.c=./obj/%.o)
OBJ_EMUL	= $(SRC_EMUL:%.c=./obj/%.o)

DEP		= $(SRC:%.c=./deps/%.d) $(SRC_EMUL:%.c=./deps/%.d) 

all : Makefile deps $(TARGET)

emul_int: $(OBJ_EMUL)
	@echo '=============================='
	@echo "Linkage $@  ($(OSTYPE))"
	@echo '------------------------------'
	$(CC) -o $@ $^ $(LDFLAGS)

rrm: $(OBJ)
	@echo '=============================='
	@echo "Linkage $@  ($(OSTYPE))"
	@echo '------------------------------'
	$(CC) -o $@ $^ $(LDFLAGS)

deps/%.d : %.c
	@echo '------------------------------'
	@echo Dependance $@
	@echo '------------------------------'
	$(CC) $(CFLAGS)  -M -MM  $< | sed -e "1s/^/obj\//" -e "s/: /: Makefile /" > $@

obj/%.o:%.c
	@echo '------------------------------'
	@echo Compilation $@
	@echo '------------------------------'
	@echo Dependance $^
	@echo Dependance plus recente $?
	$(CC) $(CFLAGS) -c -o $@ $<


-include  deps/*.d

.PHONY: clean  cleanexe deps

deps: $(DEP)

clean:
	rm -f  $(TARGET) $(OBJ) *.o  obj/*.o tags deps/*.d *.stackdump

cleanexe:
	rm -f *.exe


