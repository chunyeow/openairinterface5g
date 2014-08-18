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

\file    	rrm_util.c

\brief   	Quelques fonctions utiles

\author  	BURLOT Pascal

\date    	17/07/08

   
\par     Historique:
			$Author$  $Date$  $Revision$
			$Id$
			$Log$

*******************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>

//#include "extern_definition.h"
#include "L3_rrc_defs.h"
#include "rrm_util.h"

/*!
*******************************************************************************
\brief  Affichage de l'adresse MAC
*/

void print_L2_id( 
	L2_ID *id ///< Pointeur sur l'adresse MAC
	)
{
	int i ;
	for ( i=0;i<8;i++)
		fprintf(stderr,"%02X:", id->L2_id[i]);
}

/*!
*******************************************************************************
\brief  Affichage de l'adresse IP
*/

void print_L3_id( 
	L3_INFO_T type			, ///< type de l'info de niveau 3
	unsigned char *L3_info    ///< Pointeur sur l'identification de niveau 3
	)
{
	int i , n =0 ;
	
	if ( type==IPv4_ADDR)
		n=4;
		
	if (type==IPv6_ADDR) 
		n=16;
		
	for ( i=0;i<n;i++)
		fprintf(stderr,"%02X:", L3_info[i]);
}

/*!
*******************************************************************************
\brief  Affichage d'un message en hexa
*/

void printHex( 
	char *msg, ///< Message brute
	int len  , ///< Longueur a afficher
	int opt    ///< Option ASCII 
	)
{
	int i ;
	if (msg != NULL )
		for ( i=0;i<len;i++)
		{
			fprintf(stderr,"%02X", *msg&0xff);
			if (opt )
			{
				char c = *msg ;
				if ( c<' ' ) c=' ' ;
				if ( c>'Z')  c=' ' ;
				fprintf(stderr,"(%c)",  c );
			}

			if ( i%16 == 15 ) 
				fprintf(stderr,":\n");
			else
				fprintf(stderr,":");
				
			msg++ ;
		}
	fprintf(stderr,"\n");
	fflush( stderr) ;
}

/*!
*******************************************************************************
\brief  Lecture du registe de cycle du processeur

\return  retourne le cycle courant du processeur
*/

__inline__ unsigned long long int rdtsc()
 {
    unsigned long long int x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));     
    return x;  
 }

/*!
*******************************************************************************
\brief 	La fonction retourne l'horloge courante
		
\return  current clock
*/
double get_currentclock(  )
{
	/*
	struct timespec tp;
	clock_gettime(CLOCK_REALTIME, &tp );
	return ( ((double) tp.tv_sec ) + ( (double) tp.tv_nsec)*1.0E-9 ) ; 
	*/
#ifdef LINUX
	
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);
	
	return ( ((double) tv.tv_sec ) + ( (double) tv.tv_usec)*1e-6) ;
#else
	 unsigned long long int x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    
    return ( (double) x * 0.36e-9 ) ;
    
#endif
}
