TURBO_SRC    = 3gpplte.c 3gpplte_turbo_decoder_sse.c crc_byte.c
RATE13CC_SRC = ccoding_byte_lte.c viterbi_lte.c crc_byte.c
RATE12CC_SRC = ccoding_byte.c viterbi.c crc_byte.c

all: turbolte_test rate13cc_test rate12cc_test run_turbo run_rate13cc run_rate13ccdab run_rate12cc

turbolte_test: $(TURBO_SRC)
	gcc -o turbo_test $(TURBO_SRC)  -DTEST_DEBUG -DUSER_MODE -msse2 -mssse3 -Wall

rate13cc_test: $(RATE13CC_SRC)
	gcc -o rate13cc_test $(RATE13CC_SRC)  -DTEST_DEBUG -DUSER_MODE -msse2 -mssse3 -Wall

rate12cc_test: $(RATE12CC_SRC)
	gcc -o rate12cc_test $(RATE12CC_SRC)  -DTEST_DEBUG -DUSER_MODE -msse2 -mssse3 -Wall

run_turbo: turbolte_test
	./turbo_test

run_rate13cc: rate13cc_test
	./rate13cc_test

run_rate13ccdab: rate13cc_test
	./rate13cc_test -d

run_rate12cc: rate12cc_test
	./rate12cc_test

clean: 
	rm *.o
