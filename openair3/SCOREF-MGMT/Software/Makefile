
CPP=g++
RM=rm
ECHO=echo
STRIP=strip
CPP_FILES:=$(shell find ./src/ -iname '*.cpp')
OBJ_FILES=$(CPP_FILES:.cpp=.o)
EXE_FILE=bin/SCOREF-MGMT
CPPFLAGS=-Wall
LDFLAGS=-lboost_system -lboost_filesystem -lboost_thread -lboost_program_options
INCLUDE=-Isrc/ -Isrc/packets/ -Isrc/util/
TEST_DIR=test/
TEST_EXE=TEST-SCOREF-MGMT

OPT_FLAGS=-O3
DEBUG_FLAGS=-O0 -g3
STATIC_FLAGS=-static

# Default is dynamic build
BUILD_FLAGS=$(OPT_FLAGS)

ifdef BUILD_DEBUG
	BUILD_FLAGS=$(DEBUG_FLAGS)
endif
ifdef BUILD_STATIC
	BUILD_FLAGS=$(STATIC_FLAGS)
	LDFLAGS=-static -lboost_system -lboost_filesystem -lboost_thread -pthread
endif
ifdef BUILD_DYNAMIC
	BUILD_FLAGS=$(OPT_FLAGS)
endif

all: $(CPP_FILES) binary

binary: $(CPP_FILES) $(OBJ_FILES)
	$(CPP) $(OBJ_FILES) -o $(EXE_FILE) $(LDFLAGS)

strip:
	$(STRIP) $(EXE_FILE) &> /dev/null

test: binary
	echo "Building test code..."
	cd $(TEST_DIR)
	make -f Makefile
	cd -
	echo "Unit tests are ready to be run under test/ directory"

clean:
	$(RM) -fv $(EXE_FILE)
	$(RM) -fv $(TEST_DIR)/$(TEST_EXE)
	find . -iname "*.o" -exec rm -fv {} \;

.cpp.o:
	$(CPP) -c $(BUILD_FLAGS) $(CPPFLAGS) $< -o $@
