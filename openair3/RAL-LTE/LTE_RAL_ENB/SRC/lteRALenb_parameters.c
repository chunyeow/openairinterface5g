/*****************************************************************************
 *   Eurecom OpenAirInterface 3
 *    Copyright(c) 2012 Eurecom
 *
 * Source eRALlte_parameters.c
 *
 * Version 0.1
 *
 * Date  07/03/2012
 *
 * Product MIH RAL LTE
 *
 * Subsystem 
 *
 * Authors Michelle Wetterwald, Lionel Gauthier, Frederic Maurel
 *
 * Description 
 *
 *****************************************************************************/

#include "lteRALenb_parameters.h"

#include "lteRALenb_mih_msg.h"

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:  eRALlte_get_parameters_request()                          **
 **                                                                        **
 ** Description: Processes the Link_Get_Parameters.request message and     **
 **   sends a LinK_Get_Parameters.confirm message to the MIHF.  **
 **                                                                        **
 ** Inputs:  msgP:  Pointer to the received message            **
 **     Others: None                                       **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: None                                       **
 **     Others: None                                       **
 **                                                                        **
 ***************************************************************************/
void eRALlte_get_parameters_request(MIH_C_Message_Link_Get_Parameters_request_t* msgP)
{
    MIH_C_STATUS_T status = MIH_C_STATUS_SUCCESS;

    /* Get parameters link command is not supported at the network side */
    DEBUG(" Get Parameters request is not supported by the network\n");
    eRALlte_send_get_parameters_confirm(&msgP->header.transaction_id,
     &status, NULL, NULL, NULL);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/

