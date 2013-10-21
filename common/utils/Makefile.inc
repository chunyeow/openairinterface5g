ITTI_DIR = $(COMMON_UTILS_DIR)/itti

ITTI_OBJS =  $(ITTI_DIR)/intertask_interface.o
ITTI_OBJS += $(ITTI_DIR)/intertask_interface_dump.o
ITTI_OBJS += $(ITTI_DIR)/timer.o

UTILS_OBJ = $(ITTI_OBJS)

UTILS_incl = \
    -I$(ITTI_DIR)
