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

/*!
*******************************************************************************

\file       rrc_2_rrm_msg.c

\brief      Fonctions permettant le formattage des donnees pour l'envoi d'un
            message sur le socket entre le  SENSING et le RRM -> to use in emulation of sensing unit

\author     IACOBELLI Lorenzo

\date       15/04/10

   
\par     Historique:
        

*******************************************************************************
*/

#ifdef SNS_EMUL

#include <stdio.h> 
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "L3_rrc_defs.h"
#include "rrm_sock.h"
#include "L3_rrc_interface.h"
#include "rrc_rrm_msg.h"
#include "rrm_util.h"

#else

#include "defs.h"

#endif

#include "L3_rrc_defs.h"

#include "rrm_sock.h"
#include "sensing_rrm_msg.h"
#include "rrm_util.h"

msg_t mesg;

#ifdef TRACE
//! Macro creant la chaine a partir du nom de la variable
#define STRINGIZER(x) #x
//! Tableau pour le mode trace faisant la translation entre le numero et le nom du message
const char *Str_msg_sns_rrm[NB_MSG_SNS_RRM] = { 
    STRINGIZER(SNS_UPDATE_SENS          ),
    STRINGIZER(RRM_SCAN_ORD             ),
    STRINGIZER(RRM_END_SCAN_ORD         ),
    STRINGIZER(SNS_END_SCAN_CONF        )
} ;

#endif






/*!
*******************************************************************************
\brief  This function initialize the message header 
\return any return value 
*/
static void init_sensing_msg_head( 
    msg_head_t    *msg_head , //!< message header to initialize
    Instance_t     inst     , //!< Instance ID
    MSG_SENSING_RRM_T  msg_type , //!< type of message to initialize
    unsigned int   size     , //!< size of message
    Transaction_t  Trans_id   //!< transaction id associated to this message
    ) 
{
    if ( msg_head != NULL )
    {
        msg_head->start    = START_MSG ; 
        msg_head->msg_type = 0xFF & msg_type ;
        msg_head->inst     = inst  ;
        msg_head->Trans_id = Trans_id  ;
        msg_head->size     = size;
    }
}

/*****************************************************************************/

rrc_update_sens_t P_update_sens;
msg_t * msg_sensing_update_sens(
    Instance_t      inst         , //!< Instance ID
    Sens_ch_t *Sens,
    unsigned char NB_info
    )
{
 
  init_sensing_msg_head(&(mesg.head),inst, SNS_UPDATE_SENS, sizeof( rrc_update_sens_t ) ,0);
  memcpy( &P_update_sens.Sens_meas[0], (rrc_update_sens_t *)Sens, NB_info*sizeof(Sens_ch_t) )  ;
  P_update_sens.NB_info=NB_info;
  mesg.data = (char *) &P_update_sens;	
  return &mesg ;

}

/*****************************************************************************/

msg_t * msg_sensing_end_scan_conf(
    Instance_t      inst          //!< Instance ID
    )
{
  
  init_sensing_msg_head(&(mesg.head),inst, SNS_END_SCAN_CONF, 0 ,0);
  
  mesg.data = NULL;	
  return &mesg ;

}




 

