ifdef eNB_flag
EXTRA_CFLAGS += -DNODE_RG -DCELLULAR
endif

ifdef UE_flag
EXTRA_CFLAGS += -DNODE_MT -DCELLULAR
endif


RRC_CELLULAR_OBJS = $(RRC_CELL_DIR)/rrc_utilities.o 
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_bch_per_basic.o $(RRC_CELL_DIR)/rrc_bch_ies.o $(RRC_CELL_DIR)/rrc_bch_mib.o
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_msg_pdus.o $(RRC_CELL_DIR)/rrc_mbms_pdus.o

ifdef eNB_flag
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_rg_init.o  $(RRC_CELL_DIR)/rrc_rg_main.o $(RRC_CELL_DIR)/rrc_rg_srb_bch.o
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_rg_dummies.o $(RRC_CELL_DIR)/rrc_rg_L2_interface.o $(RRC_CELL_DIR)/rrc_rg_L2_frontend.o
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_rg_bch_init.o $(RRC_CELL_DIR)/rrc_rg_bch_write.o
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_rg_nas_intf.o $(RRC_CELL_DIR)/rrc_rg_msg_decode.o $(RRC_CELL_DIR)/rrc_rg_outputs.o
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_rg.o $(RRC_CELL_DIR)/rrc_rg_esterfsm.o $(RRC_CELL_DIR)/rrc_rg_data.o
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_rg_process.o $(RRC_CELL_DIR)/rrc_rg_control.o $(RRC_CELL_DIR)/rrc_rg_msg_peer.o
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_rg_mbms_encode.o $(RRC_CELL_DIR)/rrc_rg_mbms_nas_if.o 
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_rg_mbms_outputs.o $(RRC_CELL_DIR)/rrc_rg_mbms_process.o 
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_rg_rrm_process.o $(RRC_CELL_DIR)/rrc_rg_rrm_decode.o $(RRC_CELL_DIR)/rrc_rg_rrm_outputs.o
endif

ifdef UE_flag
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_ue_init.o  $(RRC_CELL_DIR)/rrc_ue_main.o $(RRC_CELL_DIR)/rrc_ue_srb_bch.o
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_ue_dummies.o $(RRC_CELL_DIR)/rrc_ue_L2_interface.o $(RRC_CELL_DIR)/rrc_ue_L2_frontend.o
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_ue_bch_read.o $(RRC_CELL_DIR)/rrc_ue_bch_process.o
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_ue_nas_intf.o $(RRC_CELL_DIR)/rrc_ue_msg_decode.o $(RRC_CELL_DIR)/rrc_ue_outputs.o
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_rb_ue.o $(RRC_CELL_DIR)/rrc_rb_ue_data.o
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_ue.o  $(RRC_CELL_DIR)/rrc_ue_data.o $(RRC_CELL_DIR)/rrc_ue_control.o
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_ue_esterfsm.o $(RRC_CELL_DIR)/rrc_ue_process.o  $(RRC_CELL_DIR)/rrc_ue_msg_peer.o
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_ue_mbms_decode.o $(RRC_CELL_DIR)/rrc_ue_mbms_fsm.o
RRC_CELLULAR_OBJS += $(RRC_CELL_DIR)/rrc_ue_mbms_outputs.o $(RRC_CELL_DIR)/rrc_ue_mbms_process.o
endif

rrc_test:
	@echo RRC makefile is in
	@echo RRC cellular variable $(rrc_cellular)
	@echo $(L2_incl)