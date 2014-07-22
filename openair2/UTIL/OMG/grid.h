
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
 * \file grid.h **/

#ifndef GRID_H_
#define GRID_H_

#include "omg.h"

int max_vertices_ongrid(omg_global_param omg_param);

int max_connecteddomains_ongrid(omg_global_param omg_param);


double vertice_xpos(int loc_num, omg_global_param omg_param);


double vertice_ypos(int loc_num, omg_global_param omg_param);


double area_minx(int block_num, omg_global_param omg_param);


double area_miny(int block_num, omg_global_param omg_param);

unsigned int next_block(int current_bn, omg_global_param omg_param);

unsigned int selected_blockn(int block_n,int type,int div);

#endif 

