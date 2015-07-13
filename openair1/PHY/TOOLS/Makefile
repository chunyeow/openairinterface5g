lte_dfts: lte_dfts.c
	gcc -O3 -mavx2 -o lte_dfts lte_dfts.c time_meas.c file_output.c ../../SIMULATION/TOOLS/taus.c -I$$OPENAIR1_DIR -I$$OPENAIR_TARGETS -I$$OPENAIR2_DIR/COMMON -DUSER_MODE -DMR_MAIN -DNB_ANTENNAS_RX=1 # -DD256STATS #-DD64STATS

dft_cycles: lte_dfts
	./lte_dfts | egrep cycles
