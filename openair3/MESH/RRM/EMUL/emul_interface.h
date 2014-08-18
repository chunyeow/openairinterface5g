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

\file       emul_interface.h

\brief      Fichier d'entête 

            Il contient les declarations de type, des defines relatif aux 
            fonctions d'emulation  des interfaces du RRM (Radio Resource 
            Management ).

\author     BURLOT Pascal

\date       15/07/08

   
\par     Historique:
            $Author$  $Date$  $Revision$
            $Id$
            $Log$

*******************************************************************************
*/


#ifndef EMUL_INTERFACE_H
#define EMUL_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

extern pthread_mutex_t   cmm_transact_exclu;
extern pthread_mutex_t   rrc_transact_exclu;

extern unsigned int      cmm_transaction; 
extern unsigned int      rrc_transaction;

extern pthread_mutex_t   actdiff_exclu;
extern actdiff_t        *list_actdiff;
extern unsigned int      cnt_actdiff;

extern transact_t       *cmm_transact_list; 
extern transact_t       *rrc_transact_list;

#ifdef __cplusplus
}
#endif

#endif /* EMUL_INTERFACE_H */
