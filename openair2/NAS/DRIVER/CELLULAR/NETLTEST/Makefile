# Lines starting with the pound sign are comments.
#
# These things are options that you might need
# to tweak.

# You can modify the below as well, but probably
# won't need to.

#CC = egcs

#UPDIR	:= $(shell /bin/pwd)
#GRAAL_DIR  =$(UPDIR)/..
#RRC_DIR =$(UPDIR)/../../../AS/L3/RRC

CELL_UPDIR := $(shell /bin/pwd)
NAS_DIR := $(OPENAIR2_DIR)/NAS/DRIVER/CELLULAR/NASMT
RRC_DIR := $(OPENAIR2_DIR)/RRC/CELLULAR
EXTRA_CFLAGS += -I$(RRC_DIR) -DNODE_MT

CFLAGS = -Wall -g -I$(NAS_DIR) -I$(RRC_DIR)

COMPILE = $(CC) $(CFLAGS) -c

OBJS = netltest.o

EXECUTABLE = netltest

# "all" is the default target. Simply make it point to myprogram.

all: $(EXECUTABLE)

# Define the components of the program, and how to link them together.
# These components are defined as dependencies; that is, they must be
# made up-to-date before the code is linked.

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(OBJS)

# Add any special rules here.

# Specify that all .o files depend on .c files, and indicate how
# the .c files are converted (compiled) to the .o files.

%.o: %.c
	$(COMPILE) -o $@ $<

clean:
	-rm $(OBJS) $(EXECUTABLE)
