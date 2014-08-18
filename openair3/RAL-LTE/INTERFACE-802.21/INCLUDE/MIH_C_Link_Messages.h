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
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

/*! \file MIH_C_Link_Messages.h
 * \brief This file defines the messages of the MIH Link SAP.
 * \author BRIZZOLARA Davide, GAUTHIER Lionel, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */
/** \defgroup MIH_C_LINK_MESSAGES 802.21 Link Messages
 * \ingroup MIH_C_INTERFACE
 *
 *  @{
 */

#ifndef __MIH_C_LINK_MESSAGES_H__
#define __MIH_C_LINK_MESSAGES_H__
#include <sys/types.h>
//-----------------------------------------------------------------------------
#include "MIH_C.h"
//-----------------------------------------------------------------------------
#define MIH_C_PROTOCOL_VERSION    1
#define MIH_C_MESSAGE_ID(SID, OC, AID) ((u_int16_t)SID << 12) | ((u_int16_t)OC << 10) | ((u_int16_t)AID)
#ifdef MIH_C_MEDIEVAL_EXTENSIONS
#define MIH_C_MESSAGE_LINK_REGISTER_INDICATION_ID               MIH_C_MESSAGE_ID(1, 3, 6)
#endif
#define MIH_C_MESSAGE_LINK_DETECTED_INDICATION_ID               MIH_C_MESSAGE_ID(2, 3, 1)
#define MIH_C_MESSAGE_LINK_UP_INDICATION_ID                     MIH_C_MESSAGE_ID(2, 3, 2)
#define MIH_C_MESSAGE_LINK_DOWN_INDICATION_ID                   MIH_C_MESSAGE_ID(2, 3, 3)
#define MIH_C_MESSAGE_LINK_PARAMETERS_REPORT_INDICATION_ID      MIH_C_MESSAGE_ID(2, 3, 5)
#define MIH_C_MESSAGE_LINK_GOING_DOWN_INDICATION_ID             MIH_C_MESSAGE_ID(2, 3, 6)
#define MIH_C_MESSAGE_LINK_HANDOVER_IMMINENT_INDICATION_ID      MIH_C_MESSAGE_ID(2, 3, 7)
#define MIH_C_MESSAGE_LINK_HANDOVER_COMPLETE_INDICATION_ID      MIH_C_MESSAGE_ID(2, 3, 8)
#define MIH_C_MESSAGE_LINK_PDU_TRANSMIT_STATUS_INDICATION_ID    MIH_C_MESSAGE_ID(2, 3, 9)
#define MIH_C_MESSAGE_LINK_CAPABILITY_DISCOVER_REQUEST_ID       MIH_C_MESSAGE_ID(1, 1, 1)
#define MIH_C_MESSAGE_LINK_CAPABILITY_DISCOVER_CONFIRM_ID       MIH_C_MESSAGE_ID(1, 0, 1)
#define MIH_C_MESSAGE_LINK_EVENT_SUBSCRIBE_REQUEST_ID           MIH_C_MESSAGE_ID(1, 1, 4)
#define MIH_C_MESSAGE_LINK_EVENT_SUBSCRIBE_CONFIRM_ID           MIH_C_MESSAGE_ID(1, 0, 4)
#define MIH_C_MESSAGE_LINK_EVENT_UNSUBSCRIBE_REQUEST_ID         MIH_C_MESSAGE_ID(1, 1, 5)
#define MIH_C_MESSAGE_LINK_EVENT_UNSUBSCRIBE_CONFIRM_ID         MIH_C_MESSAGE_ID(1, 0, 5)
#define MIH_C_MESSAGE_LINK_GET_PARAMETERS_REQUEST_ID            MIH_C_MESSAGE_ID(3, 1, 1)
#define MIH_C_MESSAGE_LINK_GET_PARAMETERS_CONFIRM_ID            MIH_C_MESSAGE_ID(3, 0, 1)
#define MIH_C_MESSAGE_LINK_CONFIGURE_THRESHOLDS_REQUEST_ID      MIH_C_MESSAGE_ID(3, 1, 2)
#define MIH_C_MESSAGE_LINK_CONFIGURE_THRESHOLDS_CONFIRM_ID      MIH_C_MESSAGE_ID(3, 0, 2)
#define MIH_C_MESSAGE_LINK_ACTION_REQUEST_ID                    MIH_C_MESSAGE_ID(3, 1, 3)
#define MIH_C_MESSAGE_LINK_ACTION_CONFIRM_ID                    MIH_C_MESSAGE_ID(3, 0, 3)
//-----------------------------------------------------------------------------


#ifdef MIH_C_MEDIEVAL_EXTENSIONS
/*! \struct  MIH_C_Message_Link_Register_indication
* \brief Structure defining the message Link_Register.indication, ODTONE specific.
*/
typedef struct MIH_C_Message_Link_Register_indication
{
    MIH_C_HEADER_T                        header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                       source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                       destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Register_indication_t      primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Register_indication_t;

#endif

/*! \struct  MIH_C_Message_Link_Detected_indication
* \brief Structure defining the message Link_Detected.indication
*/
typedef struct MIH_C_Message_Link_Detected_indication
{
    MIH_C_HEADER_T                        header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                       source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                       destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Detected_indication_t      primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Detected_indication_t;


/*! \struct  MIH_C_Message_Link_Up_indication
* \brief Structure defining the message Link_Up.indication.
*/
typedef struct MIH_C_Message_Link_Up_indication
{
    MIH_C_HEADER_T                        header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                       source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                       destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Up_indication_t            primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Up_indication_t;



/*! \struct  MIH_C_Message_Link_Down_indication
* \brief Structure defining the message Link_Down.indication.
*/
typedef struct MIH_C_Message_Link_Down_indication
{
    MIH_C_HEADER_T                        header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                       source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                       destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Down_indication_t          primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Down_indication_t;


/*! \struct  MIH_C_Message_Link_Parameters_Report_indication
* \brief Structure defining the message Link_Parameters_Report.indication.
*/
typedef struct MIH_C_Message_Link_Parameters_Report_indication
{
    MIH_C_HEADER_T                              header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                             source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                             destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Parameters_Report_indication_t   primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Parameters_Report_indication_t;


/*! \struct  MIH_C_Message_Link_Going_Down_indication
* \brief Structure defining the message Link_Going_Down.indication.
*/
typedef struct MIH_C_Message_Link_Going_Down_indication
{
    MIH_C_HEADER_T                        header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                       source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                       destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Going_Down_indication_t    primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Going_Down_indication_t;



/*! \struct  MIH_C_Message_Link_Handover_Imminent_indication
* \brief Structure defining the message Link_Handover_Imminent.indication.
*/
typedef struct MIH_C_Message_Link_Handover_Imminent_indication
{
    MIH_C_HEADER_T                            header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                           source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                           destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Handover_Imminent_indication_t primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Handover_Imminent_indication_t;


/*! \struct  MIH_C_Message_Link_Handover_Complete_indication
* \brief Structure defining the message Link_Handover_Complete.indication.
*/
typedef struct MIH_C_Message_Link_Handover_Complete_indication
{
    MIH_C_HEADER_T                             header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                            source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                            destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Handover_Complete_indication_t  primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Handover_Complete_indication_t;


/*! \struct  MIH_C_Message_Link_PDU_Transmit_Status_indication
* \brief Structure defining the message Link_PDU_Transmit_Status.indication.
*/
typedef struct MIH_C_Message_Link_PDU_Transmit_Status_indication
{
    MIH_C_HEADER_T                              header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                             source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                             destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_PDU_Transmit_Status_indication_t primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_PDU_Transmit_Status_indication_t;


/*! \struct  MIH_C_Message_Link_Capability_Discover_request
* \brief Structure defining the message Link_Capability_Discover.request.
*/
typedef struct MIH_C_Message_Link_Capability_Discover_request
{
    MIH_C_HEADER_T                           header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                          source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                          destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Capability_Discover_request_t primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Capability_Discover_request_t;


/*! \struct  MIH_C_Message_Link_Capability_Discover_confirm
* \brief Structure defining the message Link_Capability_Discover.confirm.
*/
typedef struct MIH_C_Message_Link_Capability_Discover_confirm
{
    MIH_C_HEADER_T                           header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                          source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                          destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Capability_Discover_confirm_t primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Capability_Discover_confirm_t;


/*! \struct  MIH_C_Message_Link_Event_Subscribe_request
* \brief Structure defining the message Link_Event_Subscribe.request.
*/
typedef struct MIH_C_Message_Link_Event_Subscribe_request
{
    MIH_C_HEADER_T                        header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                       source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                       destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Event_Subscribe_request_t  primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Event_Subscribe_request_t;


/*! \struct  MIH_C_Message_Link_Event_Subscribe_confirm
* \brief Structure defining the message Link_Event_Subscribe.confirm.
*/
typedef struct MIH_C_Message_Link_Event_Subscribe_confirm
{
    MIH_C_HEADER_T                        header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                       source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                       destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Event_Subscribe_confirm_t  primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Event_Subscribe_confirm_t;


/*! \struct  MIH_C_Message_Link_Event_Unsubscribe_request
* \brief Structure defining the message Link_Event_Unsubscribe.request.
*/
typedef struct MIH_C_Message_Link_Event_Unsubscribe_request
{
    MIH_C_HEADER_T                         header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                        source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                        destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Event_Unsubscribe_request_t primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Event_Unsubscribe_request_t;


/*! \struct  MIH_C_Message_Link_Event_Unsubscribe_confirm
* \brief Structure defining the message Link_Event_Unsubscribe.confirm.
*/
typedef struct MIH_C_Message_Link_Event_Unsubscribe_confirm
{
    MIH_C_HEADER_T                         header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                        source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                        destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Event_Unsubscribe_confirm_t primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Event_Unsubscribe_confirm_t;


/*! \struct  MIH_C_Message_Link_Get_Parameters_request
* \brief Structure defining the message Link_Get_Parameters.request.
*/
typedef struct MIH_C_Message_Link_Get_Parameters_request
{
    MIH_C_HEADER_T                        header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                       source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                       destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Get_Parameters_request_t   primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Get_Parameters_request_t;


/*! \struct  MIH_C_Message_Link_Get_Parameters_confirm
* \brief Structure defining the message Link_Get_Parameters.confirm.
*/
typedef struct MIH_C_Message_Link_Get_Parameters_confirm
{
    MIH_C_HEADER_T                        header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                       source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                       destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Get_Parameters_confirm_t   primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Get_Parameters_confirm_t;


/*! \struct  MIH_C_Message_Link_Configure_Thresholds_request
* \brief Structure defining the message Link_Configure_Thresholds.request.
*/
typedef struct MIH_C_Message_Link_Configure_Thresholds_request
{
    MIH_C_HEADER_T                            header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                           source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                           destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Configure_Thresholds_request_t primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Configure_Thresholds_request_t;


/*! \struct  MIH_C_Message_Link_Configure_Thresholds_confirm
* \brief Structure defining the message Link_Configure_Thresholds.confirm.
*/
typedef struct MIH_C_Message_Link_Configure_Thresholds_confirm
{
    MIH_C_HEADER_T                             header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                            source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                            destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Configure_Thresholds_confirm_t  primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Configure_Thresholds_confirm_t;


/*! \struct  MIH_C_Message_Link_Action_request
* \brief Structure defining the message Link_Action.request.
*/
typedef struct MIH_C_Message_Link_Action_request
{
    MIH_C_HEADER_T                        header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                       source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                       destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Action_request_t           primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Action_request_t;


/*! \struct  MIH_C_Message_Link_Action_confirm
* \brief Structure defining the message Link_Action.confirm.
*/
typedef struct MIH_C_Message_Link_Action_confirm
{
    MIH_C_HEADER_T                        header;         /*!< \brief  Header of the message. */
    MIH_C_MIHF_ID_T                       source;         /*!< \brief  Source of the message. */
    MIH_C_MIHF_ID_T                       destination;    /*!< \brief  Destination of the message. */
    MIH_C_Link_Action_confirm_t           primitive;      /*!< \brief  Primitive. */
}__attribute__((__packed__))MIH_C_Message_Link_Action_confirm_t;


#endif
/** @}*/
