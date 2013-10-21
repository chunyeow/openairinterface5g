/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2013 Eurecom

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

#ifndef S11_CAUSES_H_
#define S11_CAUSES_H_

typedef struct SGWCauseMapping_e {
    uint8_t value;
    /* Displayable cause name */
    char    *name;
    /* Possible cause in message? */
    unsigned create_session_response:1;
    unsigned create_bearer_response:1;
    unsigned modify_bearer_response:1;
    unsigned delete_session_response:1;
} SGWCauseMapping_t;

char *sgw_cause_2_string(uint8_t cause_value);

#endif /* S11_CAUSES_H_ */
