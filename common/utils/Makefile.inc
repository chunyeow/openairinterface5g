ITTI_DIR = $(COMMON_UTILS_DIR)/itti

ITTI_OBJS =  $(ITTI_DIR)/intertask_interface.o
ITTI_OBJS += $(ITTI_DIR)/intertask_interface_dump.o
ITTI_OBJS += $(ITTI_DIR)/backtrace.o
ITTI_OBJS += $(ITTI_DIR)/signals.o
ITTI_OBJS += $(ITTI_DIR)/timer.o

UTILS_OBJS = $(ITTI_OBJS)

UTILS_incl = 				\
	-I$(COMMON_UTILS_DIR) 	\
    -I$(ITTI_DIR)
