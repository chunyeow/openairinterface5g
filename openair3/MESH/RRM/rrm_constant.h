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

\file    	rrm_constant.h

\brief   	Fichier d'entete contenant les declarations des types, des defines ,
			et des fonctions relatives aux constantes du RRM 
			(Radio Resource Management ).

\author  	BURLOT Pascal

\date    	15/07/08

   
\par     Historique:
			$Author$  $Date$  $Revision$
			$Id$
			$Log$			

*******************************************************************************
*/

#ifndef RRM_CONSTANT_H
#define RRM_CONSTANT_H

#ifdef __cplusplus
extern "C" {
#endif

//! Index QoS pour le radio Bearer SRB0
#define QOS_SRB0 		0
//! Index QoS pour le radio Bearer SRB1
#define QOS_SRB1 		1
//! Index QoS pour le radio Bearer SRB2
#define QOS_SRB2 		2
//! Index QoS pour le radio Bearer DTCH_B
#define QOS_DTCH_B 		3
//! Index QoS pour le radio Bearer DTCH_D
#define QOS_DTCH_D		4
//! Index QoS pour le radio Bearer DTCH_USER1
#define QOS_DTCH_USER1	5
//! Index QoS pour le radio Bearer DTCH_USER2
#define QOS_DTCH_USER2	6
//! Index QoS pour le radio Bearer DTCH_USER3
#define QOS_DTCH_USER3	7
//! Index QoS pour le radio Bearer DTCH_USER4
#define QOS_DTCH_USER4	8
//! Index QoS pour le radio Bearer DTCH_USER5
#define QOS_DTCH_USER5	9
//! Nombre d'index de QoS
#define MAX_QOS_CLASS 	10 
 

extern const  LCHAN_DESC        Lchan_desc[MAX_QOS_CLASS] ;
extern const  MAC_RLC_MEAS_DESC Mac_rlc_meas_desc[MAX_QOS_CLASS] ;
extern const SENSING_MEAS_DESC  Sensing_meas_desc ;


#ifdef __cplusplus
}
#endif

#endif /* RRM_CONSTANT_H */
