# lines starting with the pound sign are comments.
#
# These things are options that you might need
# to tweak.


# You can modify the below as well, but probably
# won't need to.
INTERFACE_DIR=../INTERFACE-802.21
INCLUDE_DIR=INCLUDE
SRC_DIR=SRC
# external directories for ioctl
RRC_DIR := $(OPENAIR2_DIR)/RRC/CELLULAR
NAS_DIR := $(OPENAIR2_DIR)/NAS/DRIVER/CELLULAR/NASRG

# SVN version
#SVNREV = -D'SVN_REV="$(shell svnversion -n .)"'
SVNREV = -D'SVN_REV="1"'

CC = gcc

#CFLAGS = -Wall  -g -I$(SRC_DIR)  -I$(INTERFACE_DIR)/INCLUDE  -I$(INCLUDE_DIR) \
#	-DMIH_C_MEDIEVAL_EXTENSIONS -DMSCGEN_PYTOOL -DMIH_USER_CONTROL $(SVNREV)

CFLAGS = -Wall  -g -I$(SRC_DIR)  -I$(INTERFACE_DIR)/INCLUDE  -I$(INCLUDE_DIR) -DMIH_C_MEDIEVAL_EXTENSIONS -DMIH_USER_CONTROL $(SVNREV)
CFLAGS += -I$(NAS_DIR) -I$(RRC_DIR)

LDFLAGS = -L$(INTERFACE_DIR)/LIB -lmih_c-802.21 -lrt

ERAL_OBJS  += $(SRC_DIR)/lteRALenb_mih_msg.o
ERAL_OBJS  += $(SRC_DIR)/lteRALenb_rrc_msg.o
ERAL_OBJS  += $(SRC_DIR)/lteRALenb_thresholds.o
ERAL_OBJS  += $(SRC_DIR)/lteRALenb_parameters.o
ERAL_OBJS  += $(SRC_DIR)/lteRALenb_action.o
ERAL_OBJS  += $(SRC_DIR)/lteRALenb_subscribe.o
ERAL_OBJS  += $(SRC_DIR)/lteRALenb_process.o
ERAL_OBJS  += $(SRC_DIR)/lteRALenb_main.o

OBJS = $(ERAL_OBJS)


E_RAL_EXE = LTE_RAL_ENB

EXECUTABLE = interface $(E_RAL_EXE)

# "all" is the default target. Simply make it point to myprogram.

all: $(EXECUTABLE)

interface :
#	-(cd $(INTERFACE_DIR) && make clean && make)
	-(cd $(INTERFACE_DIR) && make)

cleaninterface :
	-(cd $(INTERFACE_DIR) && make clean)

$(E_RAL_EXE) : $(ERAL_OBJS)
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
