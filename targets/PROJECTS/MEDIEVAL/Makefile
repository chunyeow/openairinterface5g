all: oaisim naslite_netlink_ether

userclean: clean oaisim naslite_netlink_ether

oaisim:
	(cd $(OPENAIR_TARGETS)/SIMU/USER && make NAS=1 OAI_NW_DRIVER_TYPE_ETHERNET=1 Rel10=1 -j8)

naslite_netlink_ether:
	(cd $(OPENAIR2_DIR) && make naslite_netlink_ether.ko)
	(cd $(OPENAIR2_DIR)/NAS/DRIVER/LITE/RB_TOOL/ && make)

clean:
	(cd $(OPENAIR2_DIR)/NAS/DRIVER/LITE && make clean)
	(cd $(OPENAIR_TARGETS)/SIMU/USER && make clean)
	(cd $(OPENAIR_TARGETS)/SIMU/USER && make cleanasn1)

