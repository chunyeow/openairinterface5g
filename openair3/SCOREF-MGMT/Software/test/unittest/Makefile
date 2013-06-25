
CPP=g++
RM=rm
ECHO=echo
STRIP=strip
CPP_FILES:=$(shell find . -iname '*.cpp')
OBJ_FILES=$(CPP_FILES:.cpp=.o)
# We define the same set of files except 'main()' 
# to link test code against SCOREF-MGMT 
CPP_SCOREF_FILES:=$(shell find ../../src/ -iname '*.cpp' ! -iname 'main.cpp')
OBJ_SCOREF_FILES=$(CPP_SCOREF_FILES:.cpp=.o)
EXE_FILE=TEST-SCOREF-MGMT
CPPFLAGS=-Wall
LDFLAGS=-lboost_system -lboost_filesystem -lboost_thread -lboost_program_options -lgtest
INCLUDE=-I../../src/ -I../../src/packets/ -I../../src/util/

BUILD_FLAGS=-O3

all: $(CPP_FILES) binary

binary: $(CPP_FILES) $(OBJ_FILES)
	cd ../../ && make -f Makefile && cd -
	$(CPP) $(OBJ_FILES) $(OBJ_SCOREF_FILES) -o $(EXE_FILE) $(LDFLAGS)

strip:
	$(STRIP) $(EXE_FILE) &> /dev/null

clean:
	$(RM) -fv $(EXE_FILE)
	find . -iname "*.o" -exec rm -fv {} \;

.cpp.o:
	$(CPP) -c $(BUILD_FLAGS) $(CPPFLAGS) $< -o $@
