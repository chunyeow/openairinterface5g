/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2012 Eurecom

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
  Forums       : http://forums.eurecom.fr/openairinterface
  Address      : EURECOM, Campus SophiaTech, 450 Route des Chappes
                 06410 Biot FRANCE

*******************************************************************************/

#ifndef SGW_LITE_DEFS_H_
#define SGW_LITE_DEFS_H_

#ifndef SGW_DEBUG
# define SGW_DEBUG(x, args...) do { fprintf(stdout, "[SGW] [D]"x, ##args); } \
    while(0)
#endif
#ifndef SGW_WARNING
# define SGW_WARNING(x, args...) do { fprintf(stdout, "[SGW] [W]"x, ##args); } \
    while(0)
#endif
#ifndef SGW_ERROR
# define SGW_ERROR(x, args...) do { fprintf(stderr, "[SGW] [E]"x, ##args); } \
    while(0)
#endif

int sgw_lite_init(const mme_config_t *mme_config);

#endif /* SGW_LITE_DEFS_H_ */
