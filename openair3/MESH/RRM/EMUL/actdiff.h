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

\file       actdiff.h

\brief      Fichier d'entête 

            Il contient les declarations de type, des defines relatif aux 
            fonctions à la gestion des actions differees.

\author     BURLOT Pascal

\date       13/08/08

   
\par     Historique:
            $Author$  $Date$  $Revision$
            $Id$
            $Log$

*******************************************************************************
*/

#ifndef ACTDIFF_H
#define ACTDIFF_H

#ifdef __cplusplus
extern "C" {
#endif

///< \brief action differee ( a traiter ulterieurement )        
typedef struct actdiff_s {
    double            tv   ; ///< date du traitement
    int               id   ; ///< ID de l'action differee
    sock_rrm_t       *sock ; ///< socket associe 
    msg_t            *msg  ; ///< message a envoyer
    struct actdiff_s *next ; ///< pointeur sur la prochaine action differee
} actdiff_t ;

actdiff_t *add_actdiff( 
                    actdiff_t **pEntry , 
                    double delai,
                    int   id,
                    sock_rrm_t *sock,
                    msg_t  *msg
                    );

void processing_actdiff( actdiff_t **pEntry ) ;

#ifdef __cplusplus
}
#endif

#endif /* ACTDIFF_H */
