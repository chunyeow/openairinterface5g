export KERNEL_DIR=/lib/modules/$(shell uname -r)/
CC=gcc
CCC=gcc

ITTI_DIR = $(COMMON_UTILS_DIR)/itti

ITTI_OBJS =  $(ITTI_DIR)/intertask_interface.o
ITTI_OBJS += $(ITTI_DIR)/intertask_interface_dump.o
ITTI_OBJS += $(ITTI_DIR)/backtrace.o
ITTI_OBJS += $(ITTI_DIR)/memory_pools.o
ITTI_OBJS += $(ITTI_DIR)/signals.o
ITTI_OBJS += $(ITTI_DIR)/timer.o


HASHTABLE_DIR = $(COMMON_UTILS_DIR)/collection/hashtable

HASHTABLE_OBJS =  $(HASHTABLE_DIR)/hashtable.o
HASHTABLE_OBJS += $(HASHTABLE_DIR)/obj_hashtable.o

UTILS_OBJS = $(ITTI_OBJS) $(HASHTABLE_OBJS) 

UTILS_incl = 				\
    -I$(COMMON_UTILS_DIR)   \
    -I$(HASHTABLE_DIR)      \
    -I$(ITTI_DIR)
