/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
   included in this distribution in the file called "COPYING". If not, 
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06410 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

/** @addtogroup _mesh_layer3_ 
  * @{ This page describes the interface between the RRC and RRM/RRCI modules for OpenAirInterface Mesh.
  */


#ifndef __L3_RRC_DEFS_H__
#define __L3_RRC_DEFS_H__

#define MAX_L3_INFO    16
#define LENGTH_L2_ID   8
#define IEEE_ADDR_LEN 6


#include "COMMON/mac_rrc_primitives.h"

/*! \brief SENDORA scenario active: flags to set at the beginning of the simulation
*/
#define WSN            1  ///if wsn = 0 -> secondary network, else sensor network 
#define SCEN_1         1
#define SCEN_2_CENTR   0
#define SCEN_2_DISTR   0
#define COLL_CLUST     0  //mod_lor_10_11_04

//mod_lor_10_05_05++
/*!
*******************************************************************************
\brief Id of different CRRM entities in case of multiple entities on same machine       
  */
#define BTS_ID -1
#define FC_ID -1
#define CH_COLL_ID -1
#define FIRST_SENSOR_ID 0
#define FIRST_SECOND_CLUSTER_USER_ID 5 //add_lor_11_01_06 AAA: if two clusters on two pcs this parameter should be greater than the number of rrm instances
#define FIRST_MR_ID 8 //mod_lor_11_02_18: if cluster on more than 1 pc, this is the ID of first MR
//mod_lor_10_05_05--

/*! 
 *******************************************************************************
 \brief Parameters about channels:
 * 
*/
#define NB_OF_SENSORS 3 //!< Number of units that can perform sensing (if scen2_centr it includes secondary users of both clusters)
#define CH_NEEDED_FOR_SN 1 //!< Number of channels needed by secondary network//mod_lor_10_05_17
#define SB_NEEDED_FOR_SN 25 //mod_lor_10_05_26: 
#define NB_SENS_MAX    4  //!< Maximum number of channels accepted by the system; AAA: should be low (4) in SCEN_2 otherwise overflow at rrc level
#define MAX_NUM_SB 50//mod_eure_lor
#define SB_BANDWIDTH   180  //! in khz, bandwidth of each sub-band; AAA -> modify only in relation with NUM_SB and sensing parameters in emul_interface!  //mod_lor_10_05_26
#define NUM_SB 50//mod_eure_lor
#define LAMBDA0 -90   //mod_lor_10_05_26: for mu0 that is the averaged value
#define LAMBDA1 -75   //mod_lor_10_05_26: for mu1 that is the maximum value
#define MIN_NB_SB_CH 25   //mod_lor_10_05_26: for mu1 that is the maximum value
#define BG 8 //! sub-bands of protection between channels
#define MAX_USER_NB 5 //! max number of users //add_lor_10_11_03

/*! \brief Transaction ID descriptor
*/
typedef unsigned int Transaction_t;

/*! \brief Instance ID descriptor
 */
typedef unsigned char Instance_t;

/*!\brief Radio Bearer ID descriptor
 */
typedef unsigned short RB_ID;


typedef unsigned short QOS_CLASS_T;

#define LCHAN_BCCH      0
#define LCHAN_CCCH      1
#define LCHAN_DCCH      2
#define LCHAN_DTCH_B    3
#define LCHAN_DTCH      4
#define LCHAN_MRBCH     5

/*!\brief Layer 2 Identifier
 */

typedef struct {
  unsigned char L2_id[LENGTH_L2_ID];
} L2_ID;



/*!\brief Measurement Mode
 */
typedef enum {
  PERIODIC=0,   /*!< Periodic measurement*/
  EVENT_DRIVEN  /*!< Event-driven measurement*/
} MEAS_MODE;

/*!\brief Sensing measurement descriptor
 */
typedef struct {
  unsigned char RSSI_Threshold;    /*!< Threshold (minus in dBm) for neighbour RSSI measurement*/
  unsigned char RSSI_F_Factor;    /*!< Forgetting factor for RSSI averaging*/
  unsigned short Rep_interval;  /*!< \brief Reporting interval between successive measurement reports in this process*/
} SENSING_MEAS_DESC;

/*!\brief Sensing measurement information
 */
typedef struct {
  unsigned char Rssi;    /*!< RSSI (minus in dBm) for neighbour*/
  L2_ID L2_id;           /*!< Layer 2 ID for neighbour*/
} SENSING_MEAS_T;

/*!
*******************************************************************************
\brief Structure of sensing information database        
*/
typedef struct  Sens_ch_s { 
    unsigned int        Start_f    ; ///< frequence initial du canal //mod_lor_10_03_17: intxflot
    unsigned int        Final_f    ; ///< frequence final du canal   //mod_lor_10_03_17: intxflot
    unsigned int        Ch_id      ; ///< ID du canal
    //mod_eure_lor++
    char I0[MAX_NUM_SB] ;
    char mu0[MAX_NUM_SB];
    char mu1[MAX_NUM_SB];
    
    //float               meas       ; ///< Sensing results 
    unsigned int        is_free  [MAX_NUM_SB]  ; ///< Decision about the channel //mod_lor_10_05_28 ->char instead of int
    //mod_eure_lor--
    struct  Sens_ch_s   *next      ; ///< pointeur sur le prochain canal 
} Sens_ch_t ;

/*!
*******************************************************************************
\brief Structure that describes the channels        
*/
typedef struct { 
    unsigned int        Start_f    ; ///< frequence initial du canal //mod_lor_10_03_17: intxflot
    unsigned int        Final_f    ; ///< frequence final du canal   //mod_lor_10_03_17: intxflot
    unsigned int        Ch_id      ; ///< ID du canal               //mod_lor_10_03_17: intxflot
    QOS_CLASS_T         QoS        ; ///< Max QoS possible on the channel
} CHANNEL_T ;

/*!\brief cooperation type between CHs
 */
typedef enum {
  NO_COOP     = 0, //!< No cooperation
  AMPL_FORW   = 1, //!< amplify and forward collaboration
  DECO_FORW   = 2, //!< decode and forward collaboration
}COOPERATION_T;
/*!
*******************************************************************************
\brief Structure that describes the channels        
*/
typedef struct { 
    //float               Start_f    ; ///< frequence initial du canal
    //float               Final_f    ; ///< frequence final du canal
    int                 Ch_id      ; ///< ID du canal
    //QOS_CLASS_T         QoS        ; ///< Max QoS possible on the channel
} MAC_INFO_T ;


/*!\brief Layer 3 Info types for RRC messages
 */
typedef enum {
  NONE_L3     = 0, //!< No information
  IPv4_ADDR   = 4, //!< IPv4 Address = size Info
  IPv6_ADDR   =16, //!< IPv6 Address = size Info
  MAC_ADDR    = 8  //!< MAC Id       = size Info
} L3_INFO_T;

/*!\brief Layer 3 Info types for RRC messages
 */
typedef enum {
  BROADCAST=0,        /*!< Broadcast bearer*/
  UNICAST,            /*!< Unicast bearer*/
  MULTICAST           /*!< Multicast bearer*/
} RB_TYPE;


/*!
*******************************************************************************
\brief Entete de la file des messages reçus ou a envoyer		
*/
typedef struct channels_db_s { 
	double               info_time ; ///< information age 
	unsigned int         is_free   ; ///< channel availability   //mod_lor_10_05_28 ->char instead of int
	unsigned int         priority  ; ///< channel priority
	unsigned int         is_ass    ; ///< channel used by secondary network
	L2_ID                source_id ; ///< SU using channel (source)
    L2_ID                dest_id   ; ///< SU using channel (dest)
	CHANNEL_T            channel   ; ///< channel description
	struct channels_db_s *next     ; ///< next node pointer
} CHANNELS_DB_T ;

#endif //__L3_RRC_DEFS_H__
/** @} */
