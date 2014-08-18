/***************************************************************************
                          MIH_C_Link_Primitives.h  -  description
                             -------------------
    copyright            : (C) 2012 by Eurecom
    email                : davide.brizzolara@eurecom.fr; michelle.wetterwald@eurecom.fr
***************************************************************************
Type definition and structure for ODTONE interface
***************************************************************************/

/** \defgroup MIH_C_LINK_PRIMITIVES 802.21 Link Primitives
 * \ingroup MIH_C_INTERFACE
 *
 *  @{
 */

#ifndef __MIH_C_LINK_PRIMITIVES_H__
#define __MIH_C_LINK_PRIMITIVES_H__
#include <sys/types.h>
//-----------------------------------------------------------------------------
#include "MIH_C_Types.h"
//-----------------------------------------------------------------------------
/*! \struct  MIH_C_Link_Detected_indication
* \brief Structure defining the  Link_Detected.indication MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.1 Link_Detected.indication)
*/
typedef struct MIH_C_Link_Detected_indication
{
    MIH_C_LINK_DET_INFO_T      LinkDetectedInfo;    /*!< \brief  Information of a detected link. */
}__attribute__((__packed__))MIH_C_Link_Detected_indication_t;

                                                                               //-----------------------------------------------------------------------------
/*! \struct  MIH_C_Link_Up_indication
* \brief Structure defining the Link_Up.indication MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.2 Link_Up.indication)
*/
typedef struct MIH_C_Link_Up_indication
{
    MIH_C_LINK_TUPLE_ID_T      LinkIdentifier;           /*!< \brief  Identifier of the link associated with the event. */
    MIH_C_LINK_ADDR_T         *OldAccessRouter;          /*!< \brief  (Optional) Old Access Router link address.*/
    MIH_C_LINK_ADDR_T         *NewAccessRouter;          /*!< \brief  (Optional) New Access Router link address. */
    MIH_C_IP_RENEWAL_FLAG_T   *IPRenewalFlag;            /*!< \brief  (Optional) Indicates whether the MN needs to changeIP Address in the new PoA. */
    MIH_C_IP_MOB_MGMT_T       *MobilityManagementSupport;/*!< \brief  (Optional) Indicates the type of Mobility Management Protocol supported by the new PoA. */
}__attribute__((__packed__))MIH_C_Link_Up_indication_t;



/*! \struct  MIH_C_Link_Down_indication
* \brief Structure defining the Link_Down.indication MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.3 Link_Down.indication)
*/
typedef struct MIH_C_Link_Down_indication
{
    MIH_C_LINK_TUPLE_ID_T      LinkIdentifier;           /*!< \brief  Identifier of the link associated with the event. */
    MIH_C_LINK_ADDR_T         *OldAccessRouter;          /*!< \brief  (Optional) Old Access Router link address.*/
    MIH_C_LINK_DN_REASON_T     ReasonCode;               /*!< \brief  Reason why the link went down. */
}__attribute__((__packed__))MIH_C_Link_Down_indication_t;

TYPEDEF_LIST(MIH_C_LINK_PARAM_RPT, 16);
/*! \struct  MIH_C_Link_Parameters_Report_indication
* \brief Structure defining the Link_Parameters_Report.indication MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.4 Link_Parameters_Report.indication)
*/
typedef struct MIH_C_Link_Parameters_Report_indication
{
    MIH_C_LINK_TUPLE_ID_T      LinkIdentifier;                /*!< \brief  Identifier of the link associated with the event. */
    LIST(MIH_C_LINK_PARAM_RPT, LinkParametersReportList)      /*!< \brief  A list of Link Parameter Report.*/
}__attribute__((__packed__))MIH_C_Link_Parameters_Report_indication_t;


/*! \struct  MIH_C_Link_Going_Down_indication
* \brief Structure defining the Link_Going_Down.indication MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.5 Link_Going_Down.indication)
*/
typedef struct MIH_C_Link_Going_Down_indication
{
    MIH_C_LINK_TUPLE_ID_T      LinkIdentifier;                /*!< \brief  Identifier of the link associated with the event. */
    UNSIGNED_INT2(TimeInterval)                               /*!< \brief  Time Interval (in milliseconds) specifies the time interval at which the link is expected to go down. A value of "0" is specified if the time interval is unknown.*/
    MIH_C_LINK_GD_REASON_T     LinkGoingDownReason;           /*!< \brief  The reason why the link is going to be down. */

}__attribute__((__packed__))MIH_C_Link_Going_Down_indication_t;



/*! \struct  MIH_C_Link_Handover_Imminent_indication
* \brief Structure defining the Link_Handover_Imminent.indication MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.6 Link_Handover_Imminent.indication)
*/
typedef struct MIH_C_Link_Handover_Imminent_indication
{
    MIH_C_LINK_TUPLE_ID_T      OldLinkIdentifier;             /*!< \brief  Identifier of the old link. */
    MIH_C_LINK_TUPLE_ID_T      NewLinkIdentifier;             /*!< \brief  Identifier of the new link. */
    MIH_C_LINK_ADDR_T         *OldAccessRouter;               /*!< \brief  (Optional) Link address of old Access Router.*/
    MIH_C_LINK_ADDR_T         *NewAccessRouter;               /*!< \brief  (Optional) Link address of new Access Router.*/
}__attribute__((__packed__))MIH_C_Link_Handover_Imminent_indication_t;


/*! \struct  MIH_C_Link_Handover_Complete_indication
* \brief Structure defining the Link_Handover_Complete.indication MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.7 Link_Handover_Complete.indication)
*/
typedef struct MIH_C_Link_Handover_Complete_indication
{
    MIH_C_LINK_TUPLE_ID_T      OldLinkIdentifier;             /*!< \brief  Identifier of the old link. */
    MIH_C_LINK_TUPLE_ID_T      NewLinkIdentifier;             /*!< \brief  Identifier of the new link. */
    MIH_C_LINK_ADDR_T         *OldAccessRouter;               /*!< \brief  (Optional) Link address of old Access Router.*/
    MIH_C_LINK_ADDR_T         *NewAccessRouter;               /*!< \brief  (Optional) Link address of new Access Router.*/
    MIH_C_STATUS_T             LinkHandoverStatus;            /*!< \brief  Status of the link handover.*/
}__attribute__((__packed__))MIH_C_Link_Handover_Complete_indication_t;


/*! \struct  MIH_C_Link_PDU_Transmit_Status_indication
* \brief Structure defining the Link_PDU_Transmit_Status.indication MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.8 Link_PDU_Transmit_Status.indication)
*/
typedef struct MIH_C_Link_PDU_Transmit_Status_indication
{
    MIH_C_LINK_TUPLE_ID_T      LinkIdentifier;                /*!< \brief  Identifier of the link associated with the event. */
    UNSIGNED_INT2(PacketIdentifier)                           /*!< \brief  Identifier for higher layer PDU on which this notification is generated. */
    BOOLEAN(TransmissionStatus)                               /*!< \brief  Status of the transmitted packet. True: Success, False: Failure*/
}__attribute__((__packed__))MIH_C_Link_PDU_Transmit_Status_indication_t;


/*! \struct  MIH_C_Link_Capability_Discover_request
* \brief Structure defining the Link_Capability_Discover.request MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.9.1 Link_Capability_Discover.request)
*/
typedef struct MIH_C_Link_Capability_Discover_request
{
}__attribute__((__packed__))MIH_C_Link_Capability_Discover_request_t;


/*! \struct  MIH_C_Link_Capability_Discover_confirm
* \brief Structure defining the Link_Capability_Discover.confirm MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.9.2 Link_Capability_Discover.confirm)
*/
typedef struct MIH_C_Link_Capability_Discover_confirm
{
    MIH_C_STATUS_T             Status;                        /*!< \brief  Status of operation. Code 3 (Authorization Failure) is not applicable.*/
    MIH_C_LINK_EVENT_LIST_T    *SupportedLinkEventList;       /*!< \brief  List of link-layer events supported by the link layer. This parameter is not included if Status does not indicate “Success.” */
    MIH_C_LINK_CMD_LIST_T      *SupportedLinkCommandList;     /*!< \brief  List of link-layer commands supported by the link layer. This parameter is not included if Status does not indicate “Success.” */
}__attribute__((__packed__))MIH_C_Link_Capability_Discover_confirm_t;


/*! \struct  MIH_C_Link_Event_Subscribe_request
* \brief Structure defining the Link_Event_Subscribe.request MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.10.1 Link_Event_Subscribe.request)
*/
typedef struct MIH_C_Link_Event_Subscribe_request
{
    MIH_C_LINK_EVENT_LIST_T    RequestedLinkEventList;        /*!< \brief  List of link-layer events that for which the subscriber would like to receive indications. */
}__attribute__((__packed__))MIH_C_Link_Event_Subscribe_request_t;


/*! \struct  MIH_C_Link_Event_Subscribe_confirm
* \brief Structure defining the Link_Event_Subscribe.confirm MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.10.2 Link_Event_Subscribe.confirm)
*/
typedef struct MIH_C_Link_Event_Subscribe_confirm
{
    MIH_C_STATUS_T             Status;                        /*!< \brief  Status of operation. Code 3 (Authorization Failure) is not applicable.*/
    MIH_C_LINK_EVENT_LIST_T    *ResponseLinkEventList;        /*!< \brief  List of successfully subscribed link events. This parameter is not included if Status does not indicate "Success".*/
}__attribute__((__packed__))MIH_C_Link_Event_Subscribe_confirm_t;


/*! \struct  MIH_C_Link_Event_Unsubscribe_request
* \brief Structure defining the Link_Event_Unsubscribe.request MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.11.1 Link_Event_Unsubscribe.request)
*/
typedef struct MIH_C_Link_Event_Unsubscribe_request
{
    MIH_C_LINK_EVENT_LIST_T    RequestedLinkEventList;        /*!< \brief  List of link-layer events for which indications need to be unsubscribed from the Event Source.*/
}__attribute__((__packed__))MIH_C_Link_Event_Unsubscribe_request_t;


/*! \struct  MIH_C_Link_Event_Unsubscribe_confirm
* \brief Structure defining the Link_Event_Unsubscribe.confirm MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.11.2 Link_Event_Unsubscribe.confirm)
*/
typedef struct MIH_C_Link_Event_Unsubscribe_confirm
{
    MIH_C_STATUS_T             Status;                        /*!< \brief  Status of operation. Code 3 (Authorization Failure) is not applicable.*/
    MIH_C_LINK_EVENT_LIST_T    *ResponseLinkEventList;        /*!< \brief  List of successfully unsubscribed link events. This parameter is not included if Status does not indicate “Success.” */
}__attribute__((__packed__))MIH_C_Link_Event_Unsubscribe_confirm_t;


/*! \struct  MIH_C_Link_Get_Parameters_request
* \brief Structure defining the Link_Get_Parameters.request MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.12.1 Link_Get_Parameters.request)
*/
typedef struct MIH_C_Link_Get_Parameters_request
{
    LIST(MIH_C_LINK_PARAM_TYPE, LinkParametersRequest)        /*!< \brief  A list of link parameters for which status is requested.*/
    MIH_C_LINK_STATES_REQ_T    LinkStatesRequest;             /*!< \brief  The link states to be requested.*/
    MIH_C_LINK_DESC_REQ_T      LinkDescriptorsRequest;        /*!< \brief  The link descriptors to be requested. */
}__attribute__((__packed__))MIH_C_Link_Get_Parameters_request_t;


/*! \struct  MIH_C_Link_Get_Parameters_confirm
* \brief Structure defining the Link_Get_Parameters.confirm MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.12.1 Link_Get_Parameters.confirm)
*/
typedef struct MIH_C_Link_Get_Parameters_confirm
{
    MIH_C_STATUS_T             Status;                        /*!< \brief  Status of operation. Code 3 (Authorization Failure) is not applicable.*/
    LIST(MIH_C_LINK_PARAM,      *LinkParametersStatusList)          /*!< \brief  A list of measurable link parameters and their current values.*/
    LIST(MIH_C_LINK_STATES_RSP, *LinkStatesResponse)           /*!< \brief  The current link state information.*/
    LIST(MIH_C_LINK_DESC_RSP,   *LinkDescriptorsResponse)        /*!< \brief  The descriptors of a link.*/
}__attribute__((__packed__))MIH_C_Link_Get_Parameters_confirm_t;

TYPEDEF_LIST(MIH_C_LINK_CFG_PARAM, MIH_C_LINK_CFG_PARAM_LIST_LENGTH);
/*! \struct  MIH_C_Link_Configure_Thresholds_request
* \brief Structure defining the Link_Configure_Thresholds.request MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.13.1 Link_Configure_Thresholds.request)
*/
typedef struct MIH_C_Link_Configure_Thresholds_request
{
    LIST(MIH_C_LINK_CFG_PARAM, LinkConfigureParameterList)   /*!< \brief  A list of link threshold parameters.*/
}__attribute__((__packed__))MIH_C_Link_Configure_Thresholds_request_t;

#define MIH_C_LINK_CFG_STATUS_LIST_LENGTH 16
TYPEDEF_LIST(MIH_C_LINK_CFG_STATUS, MIH_C_LINK_CFG_STATUS_LIST_LENGTH);
/*! \struct  MIH_C_Link_Configure_Thresholds_confirm
* \brief Structure defining the Link_Configure_Thresholds.confirm MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.13.2 Link_Configure_Thresholds.confirm)
*/
typedef struct MIH_C_Link_Configure_Thresholds_confirm
{
    MIH_C_STATUS_T             Status;                           /*!< \brief  Status of operation. Code 3 (Authorization Failure) is not applicable.*/
    LIST(MIH_C_LINK_CFG_STATUS, *LinkConfigureStatusList)        /*!< \brief  A list of Link Configure Status. This parameter is not included if Status does not indicate “Success.”*/
}__attribute__((__packed__))MIH_C_Link_Configure_Thresholds_confirm_t;


/*! \struct  MIH_C_Link_Action_request
* \brief Structure defining the Link_Action.request MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.14.1 Link_Action.request)
*/
typedef struct MIH_C_Link_Action_request
{
//     MIH_C_LINK_ACTION_REQ_T
    MIH_C_LINK_ACTION_T        LinkAction;                       /*!< \brief  Specifies the action to perform. */
    UNSIGNED_INT2(ExecutionDelay)                                /*!< \brief  Time (in ms) to elapse before the action needs to be taken. A value of 0 indicates that the action is taken immediately. Time elapsed is calculated from the instance the request arrives until the time when the execution of the action is carried out. */
    MIH_C_LINK_ADDR_T          *PoALinkAddress;                  /*!< \brief  (Optional) The PoA link address to forward data to. This parameter is used when DATA_FWD_REQ action is requested. */
}__attribute__((__packed__))MIH_C_Link_Action_request_t;


/*! \struct  MIH_C_Link_Action_confirm
* \brief Structure defining the Link_Action.confirm MIH_LINK_SAP primitive (see IEEE Std 802.21-2008 7.3.14.2 Link_Action.confirm)
*/
typedef struct MIH_C_Link_Action_confirm
{
    MIH_C_STATUS_T             Status;                           /*!< \brief  Status of operation. Code 3 (Authorization Failure) is not applicable.*/
    LIST(MIH_C_LINK_SCAN_RSP, *ScanResponseSet)                  /*!< \brief  (Optional) A list of discovered links and related information. This parameter is not included if Status does not indicate “Success.” */
    MIH_C_LINK_AC_RESULT_T    *LinkActionResult;                 /*!< \brief  Specifies whether the link action was successful. This parameter is not included if Status does not indicate “Success.” */
}__attribute__((__packed__))MIH_C_Link_Action_confirm_t;

/*! \struct  MIH_C_Link_Register_indication
* \brief Structure defining the MIH_C_Link_Register.indication MIH_LINK_SAP primitive (ODTONE specific Link_Register - 101)
*/
typedef struct {
    MIH_C_LINK_ID_T            Link_Id;
}__attribute__((__packed__))MIH_C_Link_Register_indication_t;


#endif
/** @}*/
