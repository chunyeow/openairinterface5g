/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2011 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/

/*! \file otg_models.h
* \brief Data structure and functions for OTG 
* \author M. Laner
* \date 2013
* \version 1.0
* \company Eurecom
* \email: openair_tech@eurecom.fr
* \note
* \warning
*/

#ifndef __OTG_MODELS_H__
#	define __OTG_MODELS_H__



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "UTIL/MATH/oml.h"



/*function declarations*/


/*tarma*/
double tarmaCalculateSample( double inputSamples[], tarmaProcess_t *proc);
void tarmaUpdateInputSample (tarmaStream_t *stream);
tarmaStream_t *tarmaInitStream(tarmaStream_t *stream);
void tarmaSetupOpenarenaDownlink(tarmaStream_t *stream);
void tarmaPrintProc(tarmaProcess_t *proc);
void tarmaPrintStreamInit(tarmaStream_t *stream);

/*tarma video*/
void tarmaPrintVideoInit(tarmaVideo_t *video);
tarmaVideo_t *tarmaInitVideo(tarmaVideo_t *video);
double tarmaCalculateVideoSample(tarmaVideo_t *video);
void tarmaSetupVideoGop12(tarmaVideo_t *video, double compression);

/*background*/
backgroundStream_t *backgroundStreamInit(backgroundStream_t *stream, double lambda_n);
void backgroundUpdateStream(backgroundStream_t *stream, int ctime);
double backgroundCalculateSize(backgroundStream_t *stream, int ctime, int idt);
void backgroundPrintStream(backgroundStream_t *stream);

#endif