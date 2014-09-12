
build: 
	./build_oai.bash 1	

rebuild:
	./build_oai.bash 0


help:
	@echo make check1 performs a simple sanity check
	@echo make check2 performs another simple sanity check
	@echo make check does both the above checks
 
all: help

check1:
	@echo Doing 1 eNB 1 UE test
	(cd SIMU/EXAMPLES/VIRT_EMUL_1eNB && make one_eNB_one_UE )

check2:
	@echo Doing 1 eNB 4 UE test
	(cd SIMU/EXAMPLES/VIRT_EMUL_1eNB && make one_eNB_four_UE )

install:
	@echo Generating ASN1 files
	@sh SCRIPTS/install_asn1.sh 

check: check1 check2

clean:
	(cd SIMU/USER && make clean)
