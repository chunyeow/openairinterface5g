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

\file    	rrm_util.h

\brief   	Fichier d'entete contenant les declarations des types, des defines ,
			et des fonctions relatives a des routines utilitaires du RRM .

\author  	BURLOT Pascal

\date    	17/07/08


\par     Historique:
        P.BURLOT 2009-01-20 
            + ajout de la macro msg_fifo() pour debug
            + ajout de la macro MSG_L2ID() pour l'affichage du L2_ID (debug)
            + ajout de la macro RRM_CALLOC2() (utilise par la fonction msg_rrc_sensing_meas_ind())
            + ajout d'un #ifdef USE_CALLOC si on veut utiliser calloc() ou malloc().

*******************************************************************************
*/

#ifndef RRM_UTIL_H
#define RRM_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#if 1
//! Macro permettant l'affichage pour le debug
	#define msg( ... ) printf(__VA_ARGS__)
//! Macro permettant l'affichage pour le debug
	#define msg_fct( ... ) printf(__VA_ARGS__)
#else
//! Macro inactive
	#define msg( ... )
//! Macro inactive
	#define msg_fct( ... )
#endif

#if 0
//! Macro permettant l'affichage pour le debug
	#define msg_fifo( ... ) printf(__VA_ARGS__)
#else
//! Macro inactive
	#define msg_fifo( ... )
#endif

#define MSG_L2ID(p) msg("[INFO] L2ID=%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:\n",\
	                (p).L2_id[0],(p).L2_id[1], (p).L2_id[2],(p).L2_id[3],\
	                (p).L2_id[4],(p).L2_id[5], (p).L2_id[6],(p).L2_id[7] );

#define RRM_FREE(p)       if ( (p) != NULL) { free(p) ; p=NULL ; } if ( (p) != NULL) { fprintf( stderr, "(%d) [%s]\n",__LINE__, __FILE__); fflush(stderr) ; }
#define RRM_MALLOC(t,n)   (t *) malloc( sizeof(t) * n )

#ifdef USE_CALLOC
// calloc version : la memoire est intialisee a zero
#define RRM_CALLOC(t,n)    (t *) calloc( n  , sizeof(t))
#define RRM_CALLOC2(t,s)   (t *) calloc( 1  , s )
#else
// malloc version : la memoire n'est pas intialisee a zero
#define RRM_CALLOC(t,n)    (t *) malloc( sizeof(t) * n )
#define RRM_CALLOC2(t,s)   (t *) malloc( s )
#endif /* USE_CALLOC */

#define RRM_MALLOC_STR(n) RRM_MALLOC(char,n+1)

void print_L2_id(	L2_ID *id );
void print_L3_id(L3_INFO_T type,unsigned char *L3_info);
void printHex( char *msg, int len , int opt);
double get_currentclock(  ) ;


#ifdef __cplusplus
}
#endif

#endif /* RRM_UTIL_H */
