/*****************************************************************************
 *   Eurecom OpenAirInterface 3
 *    Copyright(c) 2012 Eurecom
 *
 * Source eRALlte_variables.h
 *
 * Version 0.1
 *
 * Date  06/26/2012
 *
 * Product MIH RAL LTE
 *
 * Subsystem RAL-LTE internal data structure
 *
 * Authors Michelle Wetterwald, Lionel Gauthier, Frederic Maurel
 *
 * Description Defines the data structure managed by the RAL-LTE process at
 *  the network side and the access router dummy configuration.
 *
 *****************************************************************************/
#ifndef __LTE_RAL_ENB_VARIABLES_H__
#define __LTE_RAL_ENB_VARIABLES_H__

// Define working mode : Dummy or Realtime
//#define RAL_DUMMY
#define RAL_REALTIME

//#define ENABLE_MEDIEVAL_DEMO3
//#define MUSER_CONTROL // in demo3, for triggering the congestion report manually

//flag to reduce the logs
#define DEBUG_RAL_DETAILS



/****************************************************************************/
/*********************  G L O B A L    C O N S T A N T S  *******************/
/****************************************************************************/

#ifdef RAL_REALTIME
/*Arguments ioctl command
 */
//arg[0]
//LG#define IO_OBJ_STATS 0
//LG#define IO_OBJ_CNX   1
//LG#define IO_OBJ_RB    2
//LG#define IO_OBJ_MEAS  3
//LG#define IO_OBJ_MC    4  // multicast

//arg[1]
//LG#define IO_CMD_ADD   0
//LG#define IO_CMD_DEL   1
//LG#define IO_CMD_LIST  2

#define NAS_CONNECTED     1  //same as NAS interface
#define NAS_DISCONNECTED  0
#endif // RAL_REALTIME

/* Radio Bearer attachment status; must be the same as NAS interface     */
#define RB_CONNECTED  NAS_CONNECTED
#define RB_DISCONNECTED  NAS_DISCONNECTED

#define RAL_TRUE 1
#define RAL_FALSE 0


/*Access Router configuration
 */
#ifdef RAL_REALTIME
//#define NAS_RG_NETL_MAX_RABS 27 //spec value
#define NAS_RG_NETL_MAX_RABS 5 //test value
#define NAS_RG_NETL_MAX_MEASURE_NB  5
#define NAS_RG_NETL_MAX_MTs 3
#endif

/* Maximum number of supported Radio Bearers */
#define RAL_MAX_RB  NAS_RG_NETL_MAX_RABS
/* Maximum number of supported Mobile Terminals */
#define RAL_MAX_MT NAS_RG_NETL_MAX_MTs
/* Maximum number of Radio Bearers per User Equipment */
#define RAL_MAX_RB_PER_UE 32
/* Default Radio Bearer identifier */
#define RAL_DEFAULT_MC_RAB_ID 5
#define RAL_DEFAULT_RAB_ID  6 // RBID 5 => MBMS, 6 => DEFAULTRAB, 7+ => others
/* Default Radio Bearer QoS value */
#define RAL_DEFAULT_RAB_QoS 2 // RRC_QOS_CONV_64_64
/* Default current cell identifier */
#define RAL_DEFAULT_CELL_ID 5
// Constants for Measures
#define RAL_DEFAULT_MEAS_POLLING_INTERVAL 51
#define RAL_DEFAULT_CONGESTION_THRESHOLD 80

/* Default bit rates */
#define RAL_BITRATE_32k   32000
#define RAL_BITRATE_64k   64000
#define RAL_BITRATE_128k 128000
#define RAL_BITRATE_256k 256000
#define RAL_BITRATE_320k 320000
#define RAL_BITRATE_384k 384000
#define RAL_BITRATE_440k 440000

/* Public Land Mobile Network */
#define DEFAULT_PLMN_SIZE   3
#ifdef DEFINE_GLOBAL_CONSTANTS
const u_int8_t DefaultPLMN[DEFAULT_PLMN_SIZE] = {0x20, 0x80, 0x20};
#else
extern const u_int8_t DefaultPLMN[DEFAULT_PLMN_SIZE];
#endif

/*Destination addresses
 */
enum {
    ADDR_MT1 = 0,
    ADDR_MT2,
    ADDR_MBMS,
    ADDR_MT3,
    ADDR_MAX
};

#ifdef DEFINE_GLOBAL_CONSTANTS  // Modified MW 23/05/13
const char DestIpv6Addr[ADDR_MAX][16] = { // DUMMY
    // MT1 - 2001:660:382:14:335:600:8014:9150
    {0x20,0x01,0x06,0x60,0x03,0x82,0x00,0x14,0x03,0x35,0x06,0x00,0x80,0x14,0x91,0x50},
    // MT2 - 2001:660:382:14:335:600:8014:9151
    {0x20,0x01,0x06,0x60,0x03,0x82,0x00,0x14,0x03,0x35,0x06,0x00,0x80,0x14,0x91,0x51},
    // Multicast - FF3E:20:2001:DB8::43
    {0xFF,0x3E,0x00,0x20,0x20,0x01,0x0D,0xB8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x43},
    // MT3 - 2001:660:382:14:335:600:8014:9153
    {0x20,0x01,0x06,0x60,0x03,0x82,0x00,0x14,0x03,0x35,0x06,0x00,0x80,0x14,0x91,0x53}
};
#else
extern const char DestIpv6Addr[ADDR_MAX][16];
#endif

typedef int ral_enb_instance_t;

/****************************************************************************/
/************************  G L O B A L    T Y P E S  ************************/
/****************************************************************************/

/* List of link action types */
TYPEDEF_BITMAP8(MIH_C_LINK_AC_TYPE_LIST);




/****************************************************************************/
/********************  G L O B A L    V A R I A B L E S  ********************/
/****************************************************************************/

//extern struct ral_lte_priv *ralpriv;

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

#endif

