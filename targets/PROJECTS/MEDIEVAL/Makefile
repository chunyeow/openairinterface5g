all: oaisim naslite_netlink_ether

userclean: clean oaisim naslite_netlink_ether

oaisim:
	(cd $(OPENAIR_TARGETS)/SIMU/USER && $(MAKE) NAS=1 OAI_NW_DRIVER_TYPE_ETHERNET=1 Rel10=1)

naslite_netlink_ether:
	(cd $(OPENAIR2_DIR) && $(MAKE) naslite_netlink_ether.ko)
	(cd $(OPENAIR2_DIR)/NAS/DRIVER/LITE/RB_TOOL/ && $(MAKE))

clean:
	(cd $(OPENAIR_TARGETS)/SIMU/USER && $(MAKE) clean)
	(cd $(OPENAIR2_DIR)/NAS/DRIVER/LITE && $(MAKE) clean)
	(cd $(OPENAIR_TARGETS)/SIMU/USER && make clean)
	(cd $(OPENAIR_TARGETS)/SIMU/USER && make cleanasn1)

