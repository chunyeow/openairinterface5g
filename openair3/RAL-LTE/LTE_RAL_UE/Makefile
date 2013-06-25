# lines starting with the pound sign are comments.
#
# These things are options that you might need
# to tweak.



INTERFACE_DIR=../INTERFACE-802.21
INCLUDE_DIR=INCLUDE
SRC_DIR=SRC
# external directories for ioctl
RRC_DIR := $(OPENAIR2_DIR)/RRC/CELLULAR
NAS_DIR := $(OPENAIR2_DIR)/NAS/DRIVER/CELLULAR/NASMT

# SVN version
#SVNREV = -D'SVN_REV="$(shell svnversion -n .)"'
SVNREV = -D'SVN_REV="1"'

CC = gcc

#CFLAGS = -Wall  -g -I$(SRC_DIR)  -I$(INTERFACE_DIR)/INCLUDE  -I$(INCLUDE_DIR) \
#	-DMIH_C_MEDIEVAL_EXTENSIONS -DMSCGEN_PYTOOL -DMIH_USER_CONTROL $(SVNREV)

CFLAGS = -Wall  -g -I$(SRC_DIR)  -I$(INTERFACE_DIR)/INCLUDE  -I$(INCLUDE_DIR) -DMIH_C_MEDIEVAL_EXTENSIONS -DMIH_USER_CONTROL $(SVNREV)
CFLAGS += -I$(NAS_DIR) -I$(RRC_DIR)

LDFLAGS = -L$(INTERFACE_DIR)/LIB -lmih_c-802.21 -lrt

MRAL_OBJS   = $(SRC_DIR)/lteRALue_mih_msg.o
#MRAL_OBJS  += $(SRC_DIR)/lteRALue_thresholds.o
#MRAL_OBJS  += $(SRC_DIR)/lteRALue_parameters.o
#MRAL_OBJS  += $(SRC_DIR)/lteRALue_action.o
#MRAL_OBJS  += $(SRC_DIR)/lteRALue_get.o
#MRAL_OBJS  += $(SRC_DIR)/lteRALue_subscribe.o
MRAL_OBJS  += $(SRC_DIR)/lteRALue_mih_execute.o
MRAL_OBJS  += $(SRC_DIR)/lteRALue_main.o
MRAL_OBJS  += $(SRC_DIR)/lteRALue_ioctl.o
MRAL_OBJS  += $(SRC_DIR)/lteRALue_process.o


OBJS =   $(MRAL_OBJS)

M_RAL_EXE = LTE_RAL_UE

EXECUTABLE = interface $(M_RAL_EXE) 

# "all" is the default target. Simply make it point to myprogram.

all: $(EXECUTABLE)

runral: $(M_RAL_EXE)
	-rm -f /tmp/*.txt
	-./$(M_RAL_EXE) > /tmp/mral.txt

interface :
	-(cd $(INTERFACE_DIR) && make)
#	-(cd $(INTERFACE_DIR) && make clean && make)

cleaninterface :
	-(cd $(INTERFACE_DIR) && make clean)


$(M_RAL_EXE) : $(MRAL_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c Makefile
	@echo Compiling $<
	@$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS)  -o $@ $<

clean:
	-find . -name "*.o" -delete
	-find . -name "*.*~" -delete
	-find . -name "*~" -delete
#	-rm -f $(M_RAL_EXE) $(M_NAS_EXE)
#	-rm -f $(E_RAL_EXE) $(E_NAS_EXE)
