/*****************************************************************************
 *			Eurecom OpenAirInterface 3
 * 			Copyright(c) 2012 Eurecom
 *
 * Source	eRALlte_thresholds.c
 *
 * Version	0.1
 *
 * Date		07/03/2012
 *
 * Product	MIH RAL LTE
 *
 * Subsystem	
 *
 * Authors	Michelle Wetterwald, Lionel Gauthier, Frederic Maurel
 *
 * Description	
 *
 *****************************************************************************/

#include "eRALlte_thresholds.h"

#include "eRALlte_mih_msg.h"

/****************************************************************************/
/*******************  G L O C A L    D E F I N I T I O N S  *****************/
/****************************************************************************/

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:	 eRALlte_configure_thresholds_request()                    **
 **                                                                        **
 ** Description: Processes the Link_Configure_Thresholds.request message   **
 **		 and sends a Link_Configure_Thresholds.confirm message to  **
 **		 the MIHF.                                                 **
 **                                                                        **
 ** Inputs:	 msgP:		Pointer to the received message            **
 ** 	 	 Others:	ralpriv                                    **
 **                                                                        **
 ** Outputs:	 None                                                      **
 **		 Return:	None                                       **
 ** 	 	 Others:	None                                       **
 **                                                                        **
 ***************************************************************************/
void eRALlte_configure_thresholds_request(MIH_C_Message_Link_Configure_Thresholds_request_t* msgP)
{
    MIH_C_STATUS_T status = MIH_C_STATUS_REJECTED;

    /* Configure thresholds link command is not supported at the network side */
    DEBUG(" Configure thresholds request is not supported by the network\n");
    eRALlte_send_configure_thresholds_confirm(&msgP->header.transaction_id,
					      &status, NULL);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/

