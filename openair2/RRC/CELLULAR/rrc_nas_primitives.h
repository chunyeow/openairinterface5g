/*********************************************************************
                         rrc_nas_primitives.h  -  description
                            -------------------
   begin                : Jan 11, 2002
   copyright            : (C) 2001, 2010 by Eurecom
   email                : Michelle.Wetterwald@eurecom.fr
 *********************************************************************
 Define RRC external interface primitives
 ********************************************************************/
#ifndef __RRC_NASPRIM_H__
#define __RRC_NASPRIM_H__

//----------------------------------------------------------
// Primitives
//----------------------------------------------------------
#define INFO_BROADCAST_REQ  1
#define INFO_BROADCAST_IND  2
#define CONN_ESTABLISH_REQ  3
#define CONN_ESTABLISH_IND  4
#define CONN_ESTABLISH_RESP 5
#define CONN_ESTABLISH_CNF  6
#define CONN_RELEASE_REQ    7
#define CONN_RELEASE_IND    8
#define DATA_TRANSFER_REQ   9
#define DATA_TRANSFER_IND   10
#define RB_ESTABLISH_REQ    11
#define RB_ESTABLISH_IND    12
#define RB_ESTABLISH_CNF    14
#define RB_RELEASE_REQ      15
#define RB_RELEASE_IND      16
#define MEASUREMENT_IND     17
#define CONN_LOSS_IND       18
#define PAGING_REQ          19
#define NOTIFICATION_IND    20
#define MBMS_BEARER_ESTABLISH_REQ 21
#define MBMS_BEARER_ESTABLISH_CNF 22
#define MBMS_BEARER_RELEASE_REQ   23
#define MBMS_UE_NOTIFY_REQ        24
#define MBMS_UE_NOTIFY_IND        25
#define MBMS_UE_NOTIFY_CNF        26
//Added for Medieval demo 3 - MW
#define ENB_MEASUREMENT_REQ 27
#define ENB_MEASUREMENT_IND 29

//----------------------------------------------------------
// Constants
//----------------------------------------------------------
// Define max length authorized (to be updated later)
#define NAS_MAX_LENGTH 180  // maximum length of a NAS primitive
#define NAS_TL_SIZE     4  // size of the Type+Length fields of the primitive
#define NAS_DATA_MAX_LENGTH NAS_MAX_LENGTH - NAS_TL_SIZE //remaining bytes for the message
#define MAX_RABS 27   // = MAXURAB
#define MAX_MEASURE_NB  5
#define MAX_MBMS_SERVICES 4 //spec 128
#define MAX_MEASURE_UE  3 // if applicable, should be identical with maxUsers value

//Connection Establishment status
// UE
#define TERMINATED         0
#define ABORTED             1
#define ALREADY_CONNECTED  2
// RG + RB Establishment Confirm
#define ACCEPTED   0
#define FAILURE     1
//----------------------------------------------------------
// Parameter types
//----------------------------------------------------------
typedef u32 nasPeriod_t; // Broadcast repetition period
typedef u16  nasBroadcastCategory_t; // identifies the type of data to broadcast (IP signalling/ List of neighbors)
typedef u8  nasIMEI_t[14]; // 14 digits of IMEI
typedef u16 nasDataLength_t; // Length of the data in the primitive
typedef u16 nasCellID_t; // ID of the cell for connection
typedef u32 nasLocalConnectionRef_t; // local identifier
typedef u16  nasConnectionStatus_t; // connection establishment status
typedef u16  nasReleaseCause_t; // connection release cause
typedef u16  nasSignalingPriority_t; // priority to use srb3 or srb4
typedef u16 nasRadioBearerId_t;
typedef u16 nasQoSTrafficClass_t; //QoS traffic class requested
typedef u16  nasIPdscp_t; // DSCP code transported to service NAS
typedef struct nasRBDef {
  nasRadioBearerId_t rbId;
  nasQoSTrafficClass_t QoSclass;
  nasIPdscp_t dscp;
} nasrbParms_t;
typedef u32 nasSapId_t; // Id of the QoS SAP to use
typedef u16  nasRBEstablishStatus_t; // radio bearer establishment status
typedef u16  nasNumRBsInList_t; // number of RBs defined in the list
typedef nasrbParms_t nasRBList_t[MAX_RABS]; // List of Rbs for handover
typedef u16  nasNumRGsMeas_t; // number of RGs that could be measured
typedef u32 nasPagingUEId_t; // Cell_Id of the mobile, = Local Conn Ref
typedef u32 nasSigLevel_t; // Signal level measured
typedef struct nasMeasures {
  nasCellID_t cell_id;
  nasSigLevel_t level;
} nasMeasures_t;
typedef u32 nasMBMSServiceId_t; //3 bytes - to be completed by PLMN-Id
typedef u16 nasMBMSSessionId_t; //1 byte
typedef struct nasMBMSService {
  nasMBMSServiceId_t   mbms_serviceId;
} nasMBMSService_t;
typedef u32 nasMBMSDuration_t; //3 bytes
typedef u16 nasMBMSStatus_t;   // UE notification status
//Added for Medieval demo 3 - MW
typedef u16 nasNumConnUEs_t; // number of UEs that are connected
typedef u32 nasENbMeasure_t; // type definition to hold measures
typedef struct nasENbMeasures {
        nasENbMeasure_t rlcBufferOccupancy;
        nasENbMeasure_t scheduledPRB;
        nasENbMeasure_t totalDataVolume;
} nasENbMeasures_t;


//----------------------------------------------------------
// Primitive definitions
//----------------------------------------------------------
// -- SAP-GC
struct NASInfoBroadcastReq {
 nasPeriod_t period;  // 0 = one-shot, otherwise in  x 10 ms
 nasBroadcastCategory_t category;
 nasDataLength_t nasDataLength;
};

struct NASMBMSBearerEstablishReq {
 nasMBMSServiceId_t   mbms_serviceId;
 nasMBMSSessionId_t   mbms_sessionId ;
 nasQoSTrafficClass_t mbms_QoSclass;
 nasSapId_t           mbms_sapId;
 nasRadioBearerId_t   mbms_rbId;
 nasMBMSDuration_t    mbms_duration;
};

struct NASMBMSBearerReleaseReq {
 nasMBMSServiceId_t   mbms_serviceId;
 nasMBMSSessionId_t   mbms_sessionId ;
 nasRadioBearerId_t   mbms_rbId;
};


struct NASInfoBroadcastInd {
 nasDataLength_t nasDataLength;
};

struct nas_ue_gc_element {
 u16  type;
 u16 length;
 union {
   struct NASInfoBroadcastInd broadcast_ind;
 } nasUEGCPrimitive;
};

struct nas_rg_gc_element {
 u16  type;
 u16 length;
 union {
   struct NASInfoBroadcastReq        broadcast_req;
   struct NASMBMSBearerEstablishReq  mbms_establish_req;
   struct NASMBMSBearerReleaseReq    mbms_release_req;
 } nasRGGCPrimitive;
};

// -- SAP-DC
struct NASConnEstablishReq {
 nasLocalConnectionRef_t  localConnectionRef;   //provided by NAS
 nasCellID_t cellId;
};

struct NASConnEstablishInd {
 nasLocalConnectionRef_t  localConnectionRef;
 nasIMEI_t InterfaceIMEI;
};

struct NASConnEstablishConf {
 nasLocalConnectionRef_t localConnectionRef;
 nasConnectionStatus_t    status;     // can be : Accepted, Failure
 nasNumRBsInList_t num_RBs;        // actual number of RBs in the list
 nasRBList_t RB_List;
};

struct NASConnEstablishResp {
 nasLocalConnectionRef_t  localConnectionRef;
 nasIMEI_t InterfaceIMEI;
 nasConnectionStatus_t    status;     // can be : Terminated, Aborted , Already_Connected
};

struct NASConnReleaseReq {
 nasLocalConnectionRef_t  localConnectionRef;
 nasReleaseCause_t releaseCause;
};

struct NASConnReleaseInd {
 nasLocalConnectionRef_t  localConnectionRef;
 nasReleaseCause_t releaseCause;
};

struct NASConnLossInd {
 nasLocalConnectionRef_t  localConnectionRef;
};

struct NASDataReq {
 nasLocalConnectionRef_t  localConnectionRef;
 nasSignalingPriority_t priority;
 nasDataLength_t nasDataLength;
};

struct NASDataInd {
 nasLocalConnectionRef_t  localConnectionRef;
 nasSignalingPriority_t priority;
 nasDataLength_t nasDataLength;
};

struct NASrbEstablishReq {
 nasLocalConnectionRef_t  localConnectionRef;
 nasRadioBearerId_t rbId;
 nasQoSTrafficClass_t  QoSclass;
 nasIPdscp_t dscp;
};

struct NASrbEstablishInd {
 nasLocalConnectionRef_t  localConnectionRef;
 nasRadioBearerId_t rbId;
 nasQoSTrafficClass_t  QoSclass;
 nasIPdscp_t dscp;
 nasSapId_t  sapId;
};

struct NASrbEstablishConf {
 nasLocalConnectionRef_t  localConnectionRef;
 nasRadioBearerId_t  rbId;
 nasSapId_t  sapId;
 nasRBEstablishStatus_t status;     // can be : Accepted, Failure
 nasRBEstablishStatus_t fail_code;  // gives more details on failure cause
};

struct NASrbReleaseReq {
 nasLocalConnectionRef_t  localConnectionRef;
 nasRadioBearerId_t rbId;
};

struct NASrbReleaseInd {
 nasLocalConnectionRef_t  localConnectionRef;
 nasRadioBearerId_t rbId;
};

struct NASMeasureReq {
 nasLocalConnectionRef_t  localConnectionRef;
};

struct NASMeasureInd {
 nasLocalConnectionRef_t  localConnectionRef;
 nasNumRGsMeas_t nb_rg;
 nasMeasures_t measures[MAX_MEASURE_NB];
};

struct NASMBMSUENotifyReq {
 nasLocalConnectionRef_t  localConnectionRef;
 nasMBMSService_t joined_services[MAX_MBMS_SERVICES];
 nasMBMSService_t left_services[MAX_MBMS_SERVICES];
};

struct NASMBMSUENotifyInd {
 nasLocalConnectionRef_t  localConnectionRef;
 nasMBMSService_t joined_services[MAX_MBMS_SERVICES];
 nasMBMSService_t left_services[MAX_MBMS_SERVICES];
};

struct NASMBMSUENotifyCnf{
 nasLocalConnectionRef_t  localConnectionRef;
 nasMBMSStatus_t mbmsStatus;
};


// Temp - In the DC-SAP as there is no upwards GC-SAP in the RG (to be added)
struct NASMBMSBearerEstablishConf {
 nasRadioBearerId_t  rbId;
 nasSapId_t  sapId;
 nasRBEstablishStatus_t status;     // can be : Accepted, Failure
};

//Added for Medieval demo 3 - MW

struct NASENbMeasureReq {
  nasCellID_t cell_id;
};

struct NASEnbMeasureInd {
 nasCellID_t cell_id;
 nasNumConnUEs_t num_UEs;
 nasENbMeasures_t measures[MAX_MEASURE_UE];
 nasENbMeasure_t totalNumPRBs;
};

/*****
 * UE Primitives
 *****/
struct nas_ue_dc_element {
 u16  type;
 u16 length;
 union {
   struct NASConnEstablishReq conn_establish_req;
   struct NASConnEstablishResp conn_establish_resp;
//    struct NASConnReleaseInd conn_release_ind;
   struct NASConnReleaseReq conn_release_req;
   struct NASConnLossInd conn_loss_ind;
   struct NASDataReq data_transfer_req;
   struct NASDataInd data_transfer_ind;
   struct NASrbEstablishInd rb_establish_ind;
   struct NASrbReleaseInd rb_release_ind;
   struct NASMeasureInd measurement_ind;
   struct NASMeasureReq measurement_req;
   struct NASMBMSUENotifyInd mbms_ue_notify_ind;
 } nasUEDCPrimitive;
};

/*****
 * RG Primitives
 *****/
struct nas_rg_dc_element {
 u16  type;
 u16 length;
 union {
   struct NASConnEstablishInd conn_establish_ind;
   struct NASConnEstablishConf conn_establish_conf;
   struct NASConnReleaseInd conn_release_ind;
//    struct NASConnReleaseReq conn_release_req;
   struct NASConnLossInd conn_loss_ind;
   struct NASDataReq data_transfer_req;
   struct NASDataInd data_transfer_ind;
   struct NASrbEstablishReq rb_establish_req;
   struct NASrbEstablishConf rb_establish_conf;
   struct NASrbReleaseReq rb_release_req;
   struct NASMBMSUENotifyReq mbms_ue_notify_req;
   struct NASMBMSUENotifyCnf mbms_ue_notify_cnf;
   struct NASMBMSBearerEstablishConf mbms_establish_cnf;  //TEMP - should be in GC-SAP upwards
   struct NASENbMeasureReq eNBmeasurement_req;
   struct NASEnbMeasureInd eNBmeasurement_ind;
 } nasRGDCPrimitive;
};

// -- SAP-NT
struct NASPagingReq {
 nasPagingUEId_t UeId;
 nasDataLength_t nasDataLength;
};

struct NASNotificationInd {
 nasDataLength_t nasDataLength;
};

struct nas_ue_nt_element {
 u16  type;
 u16 length;
 union {
   struct NASNotificationInd notification_ind;
 } nasUENTPrimitive;
};

struct nas_rg_nt_element {
 u16  type;
 u16 length;
 union {
   struct NASPagingReq paging_req;
 } nasRGNTPrimitive;
};

#endif
