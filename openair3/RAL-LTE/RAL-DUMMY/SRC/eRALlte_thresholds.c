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

