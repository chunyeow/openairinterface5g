/*________________________mac_rrc_primitives.h________________________

 Authors : Hicham Anouar, Raymond Knopp
 Company : EURECOM
 Emails  : anouar@eurecom.fr,  knopp@eurecom.fr
________________________________________________________________*/


#ifndef __MAC_RRC_PRIMITIVES_H__
#    define __MAC_RRC_PRIMITIVES_H__



#ifndef OPENAIR2_IN
#include "LAYER2/RLC/rlc.h"
#endif
#include "COMMON/platform_types.h"
#include "COMMON/platform_constants.h"

#include "RadioResourceConfigCommonSIB.h"
#include "RadioResourceConfigDedicated.h"
#include "MeasGapConfig.h"
#ifdef Rel10
#include "MBSFN-AreaInfoList-r9.h"
#include "MBSFN-SubframeConfigList.h"
#endif
//#include "rrm_config_structs.h"
//#include "platform_types.h"
/** @defgroup _mac_rrc_primitives_ MAC Layer Primitives for Communications with RRC 
 * @ingroup _openair_mac_layer_specs_
 * @{
This subclause describes the primitives for communications between the RRC and MAC sub-layers.  


This subclause describes the primitives for communications between the RRC and MAC sub-layers.
The RRC sequence diagrams are shown below.

The primitives responsible for relaying of radio and traffic measurements to the radio resource manager via
RRC signaling are :
-# MAC_MEAS_REQ: enables RRC to request a measurement process in the MAC (one-shot, periodic or event-driven). This configures
a measurement process in the MAC.
-# MAC_MEAS_IND: indicates to RRC that a (periodic) measurement request is available. 
-# MAC_MEAS_CONFIRM: enables RRC to acknowledge a MAC-MEAS-Ind.

These associate a set of measurements (described by the MAC_MEAS_t structure) to a particular logical channel.  Each measurement has
an associated period (which can be infinite in the case of a one-shot measurement) and a set of triggers for generating event-driven
instantaneous measurement reports. The latter is typically used to signal events that correspond to degradation or loss of links with respect to
negotiated QoS values.  These consequently trigger reconfiguration of services by the network. The averaging parameters used by the
MAC measurement process is described by the structure MAC_AVG_t. Note that it is the responsibility
of the MAC-layer to relay PHY measurements which are directly available to the scheduling entity in the MAC-layer.
RRC does not directly act on the PHY measurement sub-system. 

The primitives responsible for logical channel MAC configuration by the radio resource manager via RRC signaling are

-# MAC_CONFIG_REQ: enables RRC to configure a new logical channel description at the MAC layer.  The associated QoS
descriptor (MAC_LCHAN_DESC contains the parameters defining the logical channel resource scheduling. 
-# MAC_CONFIG_CONFIRM: indicates to RRC that a given logical channel has or has not been configured.

The primitives responsible for indicating data synchronization (or loss of) to RRC are
-# MAC_SYNC : indicates to RRC that a physical link for a newly established logical channel has been used (reception) with
success.  The parameter passed indicates the logical channel id.  This represents the event of successfully decoding a particular logical
channel for the first time.  In the event that several logical channels share the same physical resources (e.g. BCCH and CCCH), it 
shall only be sent for the logical channel with the lowest id.
-# MAC_OUT_OF_SYNC : UE primitive indicating to RRC that a physical link for a particular Node-B has been lossed.  The parameter
passed indicates the id of the Node-B.  This represents the event of detecting an unacceptable error rate on the CHBCH of the Node-B in question.
and can be used in the case of handover between Node-B's.  Note that this message is redundant since an equivalent way of obtaining the same 
result could be based on an event-driven measurement report.
*/

#        define NB_SIG_CNX_CH 1
#        define NB_CNX_CH MAX_MOBILES_PER_RG  

#        define NB_SIG_CNX_UE 2 //MAX_MANAGED_RG_PER_MOBILE
#        define NB_CNX_UE 2//MAX_MANAGED_RG_PER_MOBILE  

#define NUMBER_OF_MEASUREMENT_SUBBANDS 16

#define RRC_RACH_ASS_REQ 0
#define MAC_RACH_BW_REQ 1

typedef enum {
  RRC_OK=0,
  RRC_ConnSetup_failed,
  RRC_PHY_RESYNCH
} RRC_status_t;


/*! MAC/PHY Measurement Structure*/
/*! \brief Logical Channel ID structure */
typedef struct{
  unsigned short    Index __attribute__ ((packed));       //QOS CLASS?          /*!< \brief Index of logical channel*/
}LCHAN_ID;


typedef struct{
  char Rssi;                        /*!< \brief RSSI (dBm) on physical resources corresponding to logical channel*/
  char Sinr[NUMBER_OF_MEASUREMENT_SUBBANDS];                        /*!< \brief Average SINR (dB) on physical resources corresponding to logical channel*/
  u8 Harq_delay;         /*!< \brief Average number of transmission rounds (times 10) on transport channel associated with 
				      logical channel*/
  unsigned short Bler;              /*!< \brief Average block error rate (times 1000) on transport channel associated with logical channel*/
  u8 Spec_eff;           /*!< \brief Actual Spectral efficiency (bits/symbol times 10) of transport channel associated with logical channel*/
 u8 Qdepth;
 //u8 Tx_rate;
 //u8 Rx_rate;
}__attribute__ ((__packed__)) MAC_MEAS_T;
#define MAC_MEAS_T_SIZE sizeof(MAC_MEAS_T)

/*!\brief MAC/RLC Measurement Information
 */
typedef struct{
  char Rssi;                        /*!< \brief RSSI (dBm) on physical resources corresponding to logical channel*/
  char Sinr[NUMBER_OF_MEASUREMENT_SUBBANDS];                        /*!< \brief Average SINR (dB) on physical resources corresponding to logical channel*/
  u8 Harq_delay;         /*!< \brief Average number of transmission rounds (times 10) on transport channel associated with 
				      logical channel*/
  unsigned short Bler;              /*!< \brief Average block error rate (times 1000) on transport channel associated with logical channel*/
  u8 Spec_eff;           /*!< \brief Actual Spectral efficiency (bits/symbol times 10) of transport channel associated with logical channel*/
  u8  rlc_sdu_buffer_occ;    /*!< \brief RLC SDU buffer occupancy */
  unsigned short rlc_sdu_loss_indicator; /*!< \brief RLC SDU Loss indicator */
}__attribute__ ((__packed__)) MAC_RLC_MEAS_T;
#define MAC_RLC_MEAS_T_SIZE sizeof(MAC_RLC_MEAS_T)





/*! MAC/PHY Measurement Averaging Parameters.  These are fixed-point 16-bit representations of the interval [0,1)*/

typedef struct {
unsigned short Rssi_forgetting_factor;                        /*!< \brief Forgetting factor for RSSI averaging*/
unsigned short Sinr_forgetting_factor;                        /*!< \brief Forgetting factor for SINR averaging*/
unsigned short Harq_delay_forgetting_factor;         /*!< \brief Forgetting factor for HARQ delay averaging*/
unsigned short Bler_forgetting_factor;              /*!< \brief Forgetting factor for block error rate (times 1000) averaging*/
unsigned short Spec_eff_forgetting_factor;           /*!< \brief Forgetting factor for Spectral efficiency averaging*/
}__attribute__ ((__packed__))  MAC_AVG_T;

/*!\brief This primitive indicates to RRC that a measurement request is available */

typedef struct {
  LCHAN_ID  Lchan_id;           /*!< \brief Logical Channel ID*/
  unsigned short Process_id;         /*!< \brief Id of MAC measurement process*/
  MAC_MEAS_T Meas;
  char Meas_status;              /*!< \brief Latest set of MAC measurements performed in the current process*/
}__attribute__ ((__packed__))  MAC_MEAS_IND;

#define MAC_MEAS_IND_SIZE sizeof(MAC_MEAS_IND)

/*!\brief This primitive indicates to MAC that a measurement request was received
 */
typedef struct {
  //LCHAN_ID  Lchan_id;           /*!< \brief Logical Channel ID*/
  //unsigned short       Process_id;         /*!< \brief Id of new MAC measurement process*/
  char     Meas_status;           /*!< \brief Status (0-positive, 1-negative)*/       
}__attribute__ ((__packed__))  MAC_MEAS_CONFIRM;

/*!\brief This primitive parametrizes a MAC measurement process
 */
typedef struct {
  MAC_MEAS_T Meas_trigger;      /*!< \brief Thresholds to trigger event driven measurement reports*/
  MAC_AVG_T Mac_avg;            /*!< \brief Set of forgetting factors for the averaging of the MAC measurement process*/
  unsigned int bo_forgetting_factor; /*!< \brief Forgetting factor for RLC buffer occupancy averaging*/
  unsigned int sdu_loss_trigger; /*!< \brief Trigger for RLC SDU losses*/
  unsigned short Rep_amount;    /*!< \brief Number of Measurements for this process, 0 means infinite*/
  unsigned short Rep_interval;  /*!< \brief Reporting interval between successive measurement reports in this process*/
}__attribute__ ((__packed__))  MAC_RLC_MEAS_DESC;



/*!\brief This primitive indicates to MAC that a (periodic) measurement request is requested for a particular logical channel
 */
typedef struct {
  LCHAN_ID Lchan_id;            /*!< \brief Logical Channel ID*/
  unsigned short  UE_eNB_index;
  unsigned short Process_id;         /*!< \brief Id of new MAC measurement process*/
  //  MAC_MEAS_DESC Meas_desc;/*!< \brief Measurement descriptor associated with Logical channel*/
  MAC_MEAS_T Meas_trigger;      /*!< \brief Thresholds to trigger event driven measurement reports*/
  MAC_AVG_T Mac_avg;            /*!< \brief Set of forgetting factors for the averaging of the MAC measurement process*/
  unsigned short Rep_amount;    /*!< \brief Number of Measurements for this process, 0 means infinite*/
  unsigned short Rep_interval;  /*!< \brief Reporting interval between successive measurement reports in this process*/
  MAC_MEAS_T Mac_meas;      /*!< \brief Measure structure to be filled by the MAC  */
}__attribute__ ((__packed__))  MAC_MEAS_REQ;
#define MAC_MEAS_REQ_SIZE sizeof(MAC_MEAS_REQ)

typedef struct MEAS_REQ_TABLE_ENTRY{
  MAC_MEAS_REQ Mac_meas_req;
  unsigned int Last_report_frame;
  unsigned int Next_check_frame;
  u8 Status;
  u8 Meas_req_status;
  u8 Rx_activity;
  //u8 Meas_Direction;//???
  struct MEAS_REQ_TABLE_ENTRY *Next_entry;
  struct MEAS_REQ_TABLE_ENTRY *Prev_entry;
}__attribute__ ((__packed__)) MAC_MEAS_REQ_ENTRY;


/*!\brief This primitive parametrizes an RLC measurement process
 */
typedef struct{
  unsigned int bo_forgetting_factor; /*!< \brief Forgetting factor for buffer occupancy averaging*/
  unsigned int sdu_loss_trigger; /*!< \brief Trigger for RLC sdu losses*/
}__attribute__ ((__packed__))  RLC_MEAS_DESC;

//#define NUM_MEAS_REQ_MAX 100 //from rrm






/*! \brief MAC Logical Channel Descriptor
 */
typedef struct {
  unsigned short transport_block_size;                  /*!< \brief Minimum PDU size in bytes provided by RLC to MAC layer interface */
  unsigned short max_transport_blocks;                  /*!< \brief Maximum PDU size in bytes provided by RLC to MAC layer interface */
  unsigned long  Guaranteed_bit_rate;           /*!< \brief Guaranteed Bit Rate (average) to be offered by MAC layer scheduling*/
  unsigned long  Max_bit_rate;                  /*!< \brief Maximum Bit Rate that can be offered by MAC layer scheduling*/
  u8  Delay_class;                  /*!< \brief Delay class offered by MAC layer scheduling*/
  u8  Target_bler;                  /*!< \brief Target Average Transport Block Error rate*/
  u8  Lchan_t;                      /*!< \brief Logical Channel Type (BCCH,CCCH,DCCH,DTCH_B,DTCH,MRBCH)*/
}__attribute__ ((__packed__))  LCHAN_DESC;

#define LCHAN_DESC_SIZE sizeof(LCHAN_DESC) 

/*!\brief This primitive indicates to RRC that a particular logical channel has been established and that successful transmission has
been received.  The parameter passed is the logical channel id.
 */
typedef struct MAC_SYNC {
  LCHAN_ID Lchan_id;       /*!< \brief Logical Channel ID*/
} MAC_SYNC;

/*!\brief This primitive indicates to RRC (UE) that a particular Node-B has been lossed and passes CHBCH logical channel
node_id as parameter
 */
typedef struct MAC_OUT_OF_SYNC {
  unsigned short  Node_id;       /*!< \brief Logical Channel ID*/
} MAC_OUT_OF_SYNC;


/*!\brief This primitive enables RRC to configure a new logical channel description at the MAC layer.
 */
typedef struct{ 
  LCHAN_ID   Lchan_id;              /*!< \brief Logical Channel ID*/
  //   unsigned short UE_eNB_index; //ID of CH who configued the LC (Need this to identify LC in the UE side)
  u8 Lchan_type; // DCCH_LCHAN, DTCH_LCHAN
   LCHAN_DESC  Lchan_desc[2];  /*!< \brief Logical Channel QoS Descriptor (MAC component) */
   u8 L3_info_type;
   u8 L3_info[16]; 
//#ifndef CELLULAR
   unsigned short UE_eNB_index;
//#endif
}__attribute__ ((__packed__))  MAC_CONFIG_REQ; 
#define MAC_CONFIG_REQ_SIZE sizeof(MAC_CONFIG_REQ)


/*!\brief This primitive indicates to RRC that a given logical channel has or has not been configured.
*/
typedef struct {
  LCHAN_ID Lchan_id;        /*!< \brief Logical Channel ID*/
  u8    Config_status;   /*!< \brief Configuration status (0-positive, 1-negative)*/
}__attribute__ ((__packed__))  MAC_CONFIG_CONFIRM;

/**@}*/


/*
** @addtogroup _mac_impl_ MAC Layer (MAC) Reference Implementation
 *@{
 */

//#include "LAYER2/MAC/defs.h"



typedef struct {
  char Sinr_meas[NB_CNX_CH+1][NUMBER_OF_MEASUREMENT_SUBBANDS];
  char Wideband_sinr;
  u8 Forg_fact;
  unsigned short Rep_interval;
  u8 Status;
  unsigned int Last_report_frame; 
  unsigned int Next_check_frame;
  u8 Active;
}__attribute__ ((__packed__)) DEFAULT_UE_MEAS;
#define DEFAULT_UE_MEAS_SIZE sizeof(DEFAULT_UE_MEAS)

typedef struct {  //From RRC
  unsigned short UE_index;
  u8 Forg_fact;
  unsigned short Rep_interval;;
}__attribute__ ((__packed__)) DEFAULT_MEAS_REQ;
#define DEFAULT_MEAS_REQ_SIZE sizeof(DEFAULT_MEAS_REQ)

typedef struct {  //To RRC
  unsigned short UE_index __attribute__ ((packed));
  char Sinr_meas[NB_CNX_CH+1];
  u8 Rb_active[NB_RAB_MAX];	
}__attribute__ ((__packed__)) DEFAULT_MEAS_IND;
#define DEFAULT_MEAS_IND_SIZE sizeof(DEFAULT_MEAS_IND)


typedef struct {
  char Sinr_meas[NB_CNX_CH+1][NUMBER_OF_MEASUREMENT_SUBBANDS];
  char Wideband_sinr;
  unsigned int cqi;
  u8 Forg_fact;
  unsigned short Rep_interval;
  unsigned int Last_report_frame;
  unsigned int Next_check_frame; 
  u8 Status; //IDLE,NEED_rADIO_CONFIG, RADIO_CONFIG_TX, RADIO_CONFIG_ok
  u8 Active;
}__attribute__ ((__packed__)) DEFAULT_CH_MEAS;
#define DEFAULT_eNB_MEAS_SIZE sizeof(DEFAULT_eNB_MEAS)



#ifndef OPENAIR2_IN

#ifndef CELLULAR
//#include "L3_rrc_defs.h"
#endif

typedef struct{   //RRC_INTERFACE_FUNCTIONS
  unsigned int Frame_index;
  unsigned short UE_index[NB_MODULES_MAX][NB_SIG_CNX_UE];
  u8  eNB_id[NB_MODULES_MAX][NB_CNX_UE];
#ifndef CELLULAR
  //  L2_ID UE_id[NB_MODULES_MAX][NB_CNX_CH];
  u8 UE_id[NB_MODULES_MAX][NB_CNX_CH][5];
#endif
  void (*openair_rrc_top_init)(void); 
  char (*openair_rrc_eNB_init)(u8 ); 
  char (*openair_rrc_UE_init)(u8, u8); 
  RRC_status_t (*rrc_rx_tx)(u8,u32,u8,u8); 
  u8 (*mac_rrc_data_ind)(u8,u32,unsigned short,char *,unsigned short,u8 eNB_flag, u8 eNB_index);
  u8 (*mac_rrc_data_req)(u8,u32,unsigned short,u8,char *,u8 eNB_flag, u8 eNB_index);
  void (*mac_rrc_meas_ind)(u8,MAC_MEAS_REQ_ENTRY*);
  void  (*def_meas_ind)(u8, u8);
  void (*rrc_data_indP)  (module_id_t , rb_id_t , sdu_size_t , char*);
  void (*fn_rrc)  (void);
  u8 (*get_rrc_status)(u8 Mod_id,u8 eNB_flag,u8 eNB_index);  
}RRC_XFACE;


typedef struct{
  //MAC_INTERFACE_FUNCTIONS
  unsigned int frame;
  unsigned short Node_id[NB_MODULES_MAX];
  char Is_cluster_head[NB_MODULES_MAX];
  void (*macphy_exit)(const char *);          /*  Pointer function that stops the low-level scheduler due an exit condition */
  unsigned short (*mac_config_req)(u8,u8,MAC_CONFIG_REQ*);
  MAC_MEAS_REQ_ENTRY* (*mac_meas_req)(u8 ,  MAC_MEAS_REQ*);
  void (*mac_out_of_sync_ind)(u8,u32,unsigned short);
  //u8 (*mac_rrc_data_ind)(u8,unsigned short,char *,u8);
  //u8 (*mac_rrc_data_req)( u8, unsigned short, u8,char *);
  //void (*mac_switch_node_function)(u8);
  //  void (*mac_rlc_exit)(void);
  //RLC_INTERFACE_FUNCTIONS
  //  void (*pdcp_run)(unsigned int);
  void (*pdcp_run)(void);
  void (*pdcp_data_req)(module_id_t, rb_id_t, sdu_size_t, char*);	
  signed int (*rrc_rlc_config_req)(unsigned int, unsigned int, unsigned int, unsigned int, rlc_info_t );
  int (*rrc_mac_config_req)(u8 Mod_id,u8 eNB_flag,u8 UE_id,u8 eNB_index,
			    RadioResourceConfigCommonSIB_t *radioResourceConfigCommon,
			    struct PhysicalConfigDedicated *physicalConfigDedicated,
			    MAC_MainConfig_t *mac_MainConfig,
			    long logicalChannelIdentity,
			    LogicalChannelConfig_t *logicalChannelConfig,
			    MeasGapConfig_t *measGapConfig, 
			    TDD_Config_t *tdd_Config,
			    u8 *SIwindowsize,
			    u16 *SIperiod
 #ifdef Rel10
			    ,
			    u8 MBMS_Flag,
			    struct MBSFN_SubframeConfigList *mbsfn_SubframeConfigList,
			    MBSFN_AreaInfoList_r9_t *mbsfn_AreaInfoList,
			    struct PMCH_InfoList_r9 *pmch_InfoList
#endif
			    );
  unsigned int (*mac_rlc_data_req)(module_id_t, unsigned int, char*);
  void (*mac_rlc_data_ind)(module_id_t, chan_id_t, char*, tb_size_t, num_tb_t, crc_t* );
  mac_rlc_status_resp_t (*mac_rlc_status_ind)   (module_id_t, chan_id_t, tb_size_t, num_tb_t);
  signed int (*rrc_rlc_data_req)(module_id_t, rb_id_t, mui_t, confirm_t, sdu_size_t, char *);
  void (*rrc_rlc_register_rrc) (void (*rrc_data_indP)(module_id_t , rb_id_t , sdu_size_t , char* ),
				void  (*rrc_data_confP) (module_id_t , rb_id_t , mui_t ) ) ;
  //rlc_op_status_t rrc_rlc_config_req   (module_id_t, rb_id_t, rb_type_t, rlc_info_t );
  //rlc_op_status_t rrc_rlc_data_req     (module_id_t, rb_id_t, mui_t, confirm_t, sdu_size_t, mem_block_t*);
  //void   rrc_rlc_register_rrc ( void(*rrc_data_indP)  (module_id_t , rb_id_t , sdu_size_t , mem_block_t*),void(*rrc_data_conf) (module_id_t , rb_id_t , mui_t) );
  void (*mrbch_phy_sync_failure) (u8 Mod_id, u32 frame, u8 Free_ch_index);
  void (*dl_phy_sync_success) (u8 Mod_id, u32 frame, u8 eNB_index);
}MAC_RLC_XFACE;


#endif



#define IDLE 0
#define NEED_RADIO_CONFIG 3
#define RADIO_CONFIG_TX 2
#define RADIO_CONFIG_OK 1

#define MEAS_CONFIGURED 4
#define MEAS_OK 5
#define MEAS_TRIGGER 6
#define RRC_MEAS_REPORT_REQ 7
#define MEAS_REPORT 8 

#define RAB_MEAS_IND 0
#define DEF_MEAS_IND 1

#define ADD_LC 0
#define REMOVE_LC 1

#endif


