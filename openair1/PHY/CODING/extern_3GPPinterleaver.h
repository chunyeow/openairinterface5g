/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2014 Eurecom

  This program is free software; you can redistribute it and/or modify it under 
  the terms of the GNU General Public License as published by the Free Software 
  Foundation; either version 2 of the License, or (at your option) any later version

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

typedef struct interleaver_codebook { unsigned long nb_bits; unsigned short f1; unsigned short f2; unsigned int beg_index; 
} t_interleaver_codebook;
#ifndef INCL_INTERLEAVE
extern t_interleaver_codebook f1f2mat[];
extern short il_tb[];
extern short reverse_il_tl[];
#endif
