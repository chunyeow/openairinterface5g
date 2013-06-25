TOP_DIR = $(OPENAIR1_DIR)
bin_PROGRAMS = emos_gui
emos_gui_SOURCES = emos_gui.c emos_gui_forms.c emos_callbacks.c \
$(TOP_DIR)/PHY/CONFIG/openair_configure.c $(TOP_DIR)/PHY/CONFIG/openair_readconfigfile.c FFT.c matrix_util.c

# set the include path found by configure
INCLUDES = -I$(TOP_DIR) -I/usr/realtime/include -I/usr/include/X11 $(all_includes)

# the library search path.
emos_gui_LDFLAGS = $(all_libraries) 
noinst_HEADERS = emos_gui.h emos_gui_forms.h matrix_util.h
AM_CFLAGS = -DUSER_MODE -DPC_TARGET -DPC_DSP -DNB_ANTENNAS_TXRX=2 -DNB_ANTENNAS_RX=2 -DNB_ANTENNAS_TX=2 -DEMOS -DNODE_RG -DCBMIMO1 -DPHY_CONTEXT -msse2 -mmmx -msse
emos_gui_LDADD = -L/usr/lib -L/usr/local/lib -L/usr/realtime/lib\
	-L/usr/X11R6/lib -lm -lforms -llxrt -lpthread -lX11 -lXpm -lgps

