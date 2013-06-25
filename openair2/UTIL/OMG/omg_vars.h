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

/**
 * \file omg_vars.h
 * \brief Global variables 
 * 
 */

#ifndef __OMG_VARS_H__
#define __OMG_VARS_H__

#include "omg.h"

/*!A global variable used to store all the nodes information. It is an array in which every cell stocks the nodes information for a given mobility type. Its length is equal to the maximum number of mobility models that can exist in a single simulation scenario */
Node_list Node_Vector[MAX_NUM_MOB_TYPES];

/*! A global variable which represents the length of the Node_Vector */
int Node_Vector_len[MAX_NUM_MOB_TYPES];

/*!A global variable used to store the scheduled jobs, i.e (node, job_time) peers   */
Job_list Job_Vector;

/*! A global variable which represents the length of the Job_Vector */
int Job_Vector_len;

/*! A global variable used gather the fondamental parameters needed to launch a simulation scenario*/
omg_global_param omg_param_list;

//double m_time;


#endif /*  __OMG_VARS_H__ */
