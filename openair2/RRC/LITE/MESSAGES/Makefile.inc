
export IS_REL10_CBA=$(shell if [ -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel10-cba ] ; then  echo "1" ; else  echo "0" ; fi)
export IS_REL10=$(shell if [ -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel10 ] ; then  echo "1" ; else  echo "0" ; fi)
export IS_REL8=$(shell if [ -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel8 ] ; then  echo "1" ; else  echo "0" ; fi)

ifdef CBA
CFLAGS += -DRel10
CFLAGS += -DCBA
@echo "Rel10=1 CBA=1"
ifeq ($(IS_REL10_CBA), 0)
tmp3:=$(shell cd $(OPENAIR2_DIR)/RRC/LITE/MESSAGES ; rm -f ${ASN_MODULE_SOURCES} $(ASN_MODULE_HEADERS) *.o *.d  )
tmp2:=$(shell cd $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/ ; rm EUTRA-RRC-Definitions.asn ; ln -s EUTRA-RRC-Definitions-a20-lola.asn EUTRA-RRC-Definitions.asn ; cd $(OPENAIR_TARGETS)/SIMU/USER/ ; rm -f $(ASN1_MSG_OBJS1) ; rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.inc.generated ; rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.am.sample ; rm -f $(L2_OBJS) ; rm -f *.o ; rm -f oaisim ; touch $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel10-cba ; rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel8 ; rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel10 ; /bin/bash $(OPENAIR_TARGETS)/SCRIPTS/asn1_generate_rrc_messaging_c_code.bash )
tmp:=$(shell cd $(OPENAIR2_DIR)/RRC/LITE/MESSAGES ;  awk '/converter-sample.c/ {exit} // {print}' Makefile.am.sample > Makefile.inc.generated  )
endif

else ifdef Rel10

CFLAGS += -DRel10
@echo "Rel10=1"
ifeq ($(IS_REL10), 0)
tmp3:=$(shell cd $(OPENAIR2_DIR)/RRC/LITE/MESSAGES ; rm -f ${ASN_MODULE_SOURCES} $(ASN_MODULE_HEADERS) *.o *.d  )
tmp2:=$(shell cd $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/ ; rm EUTRA-RRC-Definitions.asn ; ln -s EUTRA-RRC-Definitions-a20.asn EUTRA-RRC-Definitions.asn ; cd $(OPENAIR_TARGETS)/SIMU/USER/ ; rm -f $(ASN1_MSG_OBJS1) ; rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.inc.generated ; rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.am.sample ; rm -f $(L2_OBJS) ; rm -f *.o ; rm -f oaisim ; touch $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel10 ; rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel8 ; rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel10-cba ; /bin/bash $(OPENAIR_TARGETS)/SCRIPTS/asn1_generate_rrc_messaging_c_code.bash )
tmp:=$(shell cd $(OPENAIR2_DIR)/RRC/LITE/MESSAGES ;  awk '/converter-sample.c/ {exit} // {print}' Makefile.am.sample > Makefile.inc.generated  )
endif

else # default is rel 8

@echo "Rel8=1"
CFLAGS += -DRel8
ifeq ($(IS_REL8), 0)
tmp3:=$(shell cd $(OPENAIR2_DIR)/RRC/LITE/MESSAGES ; rm -f ${ASN_MODULE_SOURCES} $(ASN_MODULE_HEADERS) *.o *.d  )
tmp2:=$(shell cd $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/ ; rm EUTRA-RRC-Definitions.asn ; ln -s EUTRA-RRC-Definitions-86.asn EUTRA-RRC-Definitions.asn ;  cd $(OPENAIR_TARGETS)/SIMU/USER/ ; rm -f $(ASN1_MSG_OBJS1) ; rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.inc.generated ; rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.am.sample ; rm -f $(L2_OBJS); rm -f *.o ; rm -f oaisim ; touch $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel8 ; rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel10 ; rm -f $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/asn1c/ASN1_files/.lock-rel10-cba ; /bin/bash $(OPENAIR_TARGETS)/SCRIPTS/asn1_generate_rrc_messaging_c_code.bash )
tmp:=$(shell cd $(OPENAIR2_DIR)/RRC/LITE/MESSAGES ;  awk '/converter-sample.c/ {exit} // {print}' Makefile.am.sample > Makefile.inc.generated  )
endif

endif

tmp:=$(shell cd $(OPENAIR2_DIR)/RRC/LITE/MESSAGES ; if [ ! -s Makefile.am.sample ] ; then /bin/bash $(OPENAIR_TARGETS)/SCRIPTS/asn1_generate_rrc_messaging_c_code.bash ; fi; if [ ! -s Makefile.inc.generated ] ; then  awk '/converter-sample.c/ {exit} // {print}' Makefile.am.sample > Makefile.inc.generated ; fi )

include $(OPENAIR2_DIR)/RRC/LITE/MESSAGES/Makefile.inc.generated

ASN_CONVERTER_SOURCES+=asn1_msg.c
 

EXTRA_CFLAGS += -I. -I$(OPENAIR2_DIR)/RRC/LITE/MESSAGES 

ASN1_MODULE_OBJS=${ASN_MODULE_SOURCES:.c=.o}
ASN1_CONVERTER_OBJS=${ASN_CONVERTER_SOURCES:.c=.o} 
ASN1_MSG_OBJS=$(ASN1_MODULE_OBJS) $(ASN1_CONVERTER_OBJS)

