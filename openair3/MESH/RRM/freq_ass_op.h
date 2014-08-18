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

\file    	freq_ass_op.h

\brief   	Fichier d'entete contenant les declarations des types, des defines ,
			et des fonctions relatives a la gestion des frequences.

\author  	IACOBELLI Lorenzo

\date    	21/10/09

   
\par     Historique:
			$Author$  $Date$  $Revision$
			$Id$
			$Log$

*******************************************************************************
*/

#ifndef FREQ_ASS_OP_H
#define FREQ_ASS_OP_H

#ifdef __cplusplus
extern "C" {
#endif

//mod_lor_10_05_28: declaration of function for sensing and frequency allocation    
unsigned int evalaute_sens_info(
    Sens_node_t *Sens_db,
    unsigned int Start_f,
    unsigned int Final_f
    );

void take_decision( 
	Sens_node_t *SensDB     , //!< pointer to the sensing database
	unsigned int Ch_id      , //!< channel ID
	unsigned int *is_free  //mod_lor_10_05_28 ->char instead of int
	
	);
	
unsigned int take_decision_sens( 
	Sens_node_t *SensDB     , //!< pointer to the sensing database
	Sens_ch_t *finalSensChann, //!< info that will be returned after decision
	unsigned int Ch_id       //!< channel ID
	
	);
	
unsigned int find_available_channels(
    Sens_node_t *Sens_db,
    CHANNELS_DB_T **channels_db
    );
    
unsigned int check_SN_channels(
    Sens_node_t *Sens_db,
    CHANNELS_DB_T *channels_db,
    unsigned int *used_channels,
    unsigned int nb_used_ch
    );
    
void take_local_decision(
    Sens_ch_t *Sens_info
    );
    
unsigned int take_ch_coll_decision( 
	Sens_node_t *SensDB     , //!< pointer to the sensing database
	Sens_ch_t *finalSensChann, //!< info that will be returned after decision
	unsigned int Ch_id       //!< channel ID
	);
	
void disconnect_user( 
    Instance_t    inst     , //!< identification de l'instance
    L2_ID         L2_id      //!< L2_id of the SU 
    );


// ---------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif

#endif /* FREQ_ASS_OP_H */
