# lines starting with the pound sign are comments.
#
# These things are options that you might need
# to tweak.


# You can modify the below as well, but probably
# won't need to.
CODE_DIR=C
INCLUDE_DIR=INCLUDE
LIB_DIR=LIB

CC = gcc

# include directories
INCLUDES = -I$(INCLUDE_DIR)

# C compiler flags (-g -O2 -Wall)
CFLAGS = -g -Wall -DMIH_C_MEDIEVAL_EXTENSIONS




SRC  = $(CODE_DIR)/MIH_C_header_codec.c
SRC += $(CODE_DIR)/MIH_C_msg_codec.c
SRC += $(CODE_DIR)/MIH_C_primitive_codec.c
SRC += $(CODE_DIR)/MIH_C_F1_basic_data_types_codec.c
SRC += $(CODE_DIR)/MIH_C_F2_general_data_types_codec.c
SRC += $(CODE_DIR)/MIH_C_F3_data_types_for_address_codec.c
SRC += $(CODE_DIR)/MIH_C_F4_data_types_for_links_codec.c
SRC += $(CODE_DIR)/MIH_C_F9_data_types_for_qos_codec.c
SRC += $(CODE_DIR)/MIH_C_F13_data_types_for_information_elements_codec.c
SRC += $(CODE_DIR)/MIH_C_L2_type_values_for_tlv_encoding.c
SRC += $(CODE_DIR)/MIH_C_Medieval_extensions.c
SRC += $(CODE_DIR)/MIH_C_log.c
SRC += $(CODE_DIR)/MIH_C_bit_buffer.c
SRC += $(CODE_DIR)/MIH_C.c

OBJ = $(SRC:.c=.o)

LIBS =  -L/usr/local/lib -lrt

OUT = $(LIB_DIR)/libmih_c-802.21.a



LDFLAGS = -g

.SUFFIXES: .c

default: $(OUT)

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)


clean:
	-find . -name "*.o" -delete
	-find . -name "*.*~" -delete
	-find . -name "*~" -delete
	-rm -Rf DOXYGEN/html
	-rm -f $(OUT)



