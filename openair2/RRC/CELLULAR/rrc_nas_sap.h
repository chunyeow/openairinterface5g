/***************************************************************************
                          rrc_nas_sap.h  -
                          -------------------
    copyright            : (C) 2001, 2010 by Eurecom
    created by	         : Lionel.Gauthier@eurecom.fr	
    modified by          : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Definition of RRC FIFOs and SAPs to interface NAS
 **************************************************************************
  FIFOs Operational mode
        FIFO      RRC         NAS
  UE    GC        Write       Read
        Nt        Write       Read
        DC_IN     Read        Write
        DC_OUT    Write       Read
  RG    GC        Read        Write
        Nt        Read        Write
        DC_IN     Read        Write
        DC_OUT    Write       Read
 ***************************************************************************/
#ifndef __RRC_SAP_H__
#define __RRC_SAP_H__

// #define RRC_BASE_STATION       0    // just for test of protocol on a single node (loopback on MAC)
// #define RRC_MOBILE_STATION     1    // just for test of protocol on a single node (loopback on MAC)

#define RRC_DEVICE_UE_GCSAP     "/dev/rtf40"
#define RRC_DEVICE_UE_NTSAP     "/dev/rtf41"
#define RRC_DEVICE_UE_DCSAP_IN  "/dev/rtf42"
#define RRC_DEVICE_UE_DCSAP_OUT "/dev/rtf43"
#define RRC_DEVICE_RG_GCSAP     "/dev/rtf44"
#define RRC_DEVICE_RG_NTSAP     "/dev/rtf45"
#define RRC_DEVICE_RG_DCSAP0_IN  "/dev/rtf46"
#define RRC_DEVICE_RG_DCSAP0_OUT "/dev/rtf47"
#define RRC_DEVICE_RG_DCSAP1_IN  "/dev/rtf48"
#define RRC_DEVICE_RG_DCSAP1_OUT "/dev/rtf49"
#define RRC_DEVICE_RG_DCSAP2_IN  "/dev/rtf50"
#define RRC_DEVICE_RG_DCSAP2_OUT "/dev/rtf51"

#ifndef USER_MODE

#define RRC_SAPI_UE_GCSAP           40
#define RRC_SAPI_UE_NTSAP           41
#define RRC_SAPI_UE_DCSAP_IN        42
#define RRC_SAPI_UE_DCSAP_OUT       43
#define RRC_SAPI_RG_GCSAP           44
#define RRC_SAPI_RG_NTSAP           45
#define RRC_SAPI_RG_DCSAP0_IN       46
#define RRC_SAPI_RG_DCSAP0_OUT      47
#define RRC_SAPI_RG_DCSAP1_IN       48
#define RRC_SAPI_RG_DCSAP1_OUT      49
#define RRC_SAPI_RG_DCSAP2_IN       50
#define RRC_SAPI_RG_DCSAP2_OUT      51

#else  //USER_MODE

#ifdef NODE_MT
#define RRC_SAPI_UE_GCSAP      "./RUE_GC"
#define RRC_SAPI_UE_NTSAP      "./RUE_NT"
#define RRC_SAPI_UE_DCSAP_IN   "./RUE_DCIN"
#define RRC_SAPI_UE_DCSAP_OUT  "./RUE_DCOUT"
#endif //NODE_MT
#ifdef NODE_RG
#define RRC_SAPI_RG_GCSAP       "./RRG_GC"
#define RRC_SAPI_RG_NTSAP       "./RRG_NT"
#define RRC_SAPI_RG_DCSAP0_IN   "./RRG_DCIN0"
#define RRC_SAPI_RG_DCSAP0_OUT  "./RRG_DCOUT0"
#define RRC_SAPI_RG_DCSAP1_IN   "./RRG_DCIN1"
#define RRC_SAPI_RG_DCSAP1_OUT  "./RRG_DCOUT1"
#define RRC_SAPI_RG_DCSAP2_IN   "./RRG_DCIN2"
#define RRC_SAPI_RG_DCSAP2_OUT  "./RRG_DCOUT2"
#endif  //NODE_RG
#endif  //USER_MODE

//#ifdef NAS_NETLINK
// For netlink, all SAPs are on the same socket
// GC-NT - OUT=UE, IN=RG
// DCx   - OUT/IN = UE + RG
#define RRC_NAS_GC_IN   0
#define RRC_NAS_GC_OUT  1
#define RRC_NAS_NT_IN   2
#define RRC_NAS_NT_OUT  3
#define RRC_NAS_DC0_IN  4
#define RRC_NAS_DC0_OUT 5
#define RRC_NAS_DC1_IN  6
#define RRC_NAS_DC1_OUT 7
#define RRC_NAS_DC2_IN  8
#define RRC_NAS_DC2_OUT 9
// ToDo : continue with parameterization of the value (up to 250 MTs for 1 short int)
//#endif //NAS_NETLINK

#define NAS_RRCNL_ID 30
#define NL_DEST_RRC_PID 1

#endif
