# lines starting with the pound sign are comments.
#
# These things are options that you might need
# to tweak.


# You can modify the below as well, but probably
# won't need to.
INTERFACE_DIR=../INTERFACE-802.21
INCLUDE_DIR=INCLUDE
SRC_DIR=SRC

# SVN version
SVNREV = -D'SVN_REV="$(shell svnversion -n .)"'

CC = gcc

CFLAGS = -Wall  -g -I$(SRC_DIR)  -I$(INTERFACE_DIR)/INCLUDE  -I$(INCLUDE_DIR) \
	-DMIH_C_MEDIEVAL_EXTENSIONS -DMSCGEN_PYTOOL -DMIH_USER_CONTROL $(SVNREV)

LDFLAGS = -L$(INTERFACE_DIR)/LIB -lmih_c-802.21  -lrt

MRAL_OBJS   = $(SRC_DIR)/mRALlte_mih_msg.o
MRAL_OBJS  += $(SRC_DIR)/mRALlte_thresholds.o
MRAL_OBJS  += $(SRC_DIR)/mRALlte_parameters.o
MRAL_OBJS  += $(SRC_DIR)/mRALlte_action.o
MRAL_OBJS  += $(SRC_DIR)/mRALlte_get.o
MRAL_OBJS  += $(SRC_DIR)/mRALlte_subscribe.o
MRAL_OBJS  += $(SRC_DIR)/mRALlte_main.o
MRAL_OBJS  += $(SRC_DIR)/mRALlte_NAS.o
MRAL_OBJS  += $(SRC_DIR)/mRALlte_process.o

ERAL_OBJS  += $(SRC_DIR)/eRALlte_main.o
ERAL_OBJS  += $(SRC_DIR)/eRALlte_mih_msg.o
ERAL_OBJS  += $(SRC_DIR)/eRALlte_thresholds.o
ERAL_OBJS  += $(SRC_DIR)/eRALlte_parameters.o
ERAL_OBJS  += $(SRC_DIR)/eRALlte_action.o
ERAL_OBJS  += $(SRC_DIR)/eRALlte_subscribe.o
ERAL_OBJS  += $(SRC_DIR)/eRALlte_NAS.o
ERAL_OBJS  += $(SRC_DIR)/eRALlte_process.o

OBJS =   $(MRAL_OBJS) $(ERAL_OBJS)

M_RAL_EXE = mRALlteDummy
M_NAS_EXE = NASUEdummy

E_RAL_EXE = eRALlteDummy
E_NAS_EXE = NASRGdummy

EXECUTABLE = interface $(M_RAL_EXE) $(M_NAS_EXE) $(E_RAL_EXE) $(E_NAS_EXE)

# "all" is the default target. Simply make it point to myprogram.

all: $(EXECUTABLE)

ral: $(M_RAL_EXE) $(E_RAL_EXE)

runralwithvalgrind: $(M_RAL_EXE)
	-rm -f /tmp/*.txt
	-valgrind --leak-check=full --show-reachable=yes --track-origins=yes  ./$(M_RAL_EXE) > /tmp/mral.txt

runral: $(M_RAL_EXE)
	-rm -f /tmp/*.txt
	-./$(M_RAL_EXE) > /tmp/mral.txt

interface :
	-(cd $(INTERFACE_DIR) && make clean && make)


$(M_RAL_EXE) : $(MRAL_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(M_NAS_EXE) : $(SRC_DIR)/nasUEdummy.o
	$(CC) -o $@ $^

$(E_RAL_EXE) : $(ERAL_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(E_NAS_EXE) : $(SRC_DIR)/nasRGdummy.o
	$(CC) -o $@ $^

%.o: %.c Makefile
	@echo Compiling $<
	@$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS)  -o $@ $<

clean:
	-find . -name "*.o" -delete
	-find . -name "*.*~" -delete
	-find . -name "*~" -delete
	-rm -f $(M_RAL_EXE) $(M_NAS_EXE)
	-rm -f $(E_RAL_EXE) $(E_NAS_EXE)
