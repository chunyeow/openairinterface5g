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

  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.

 *******************************************************************************/
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "log.h"

/* mme log */
int log_enabled = 0;

int log_init(const mme_config_t *mme_config_p,
             log_specific_init_t specific_init)
{
    if (mme_config_p->verbosity_level == 1) {
        log_enabled = 1;
    } else if (mme_config_p->verbosity_level == 2) {
        log_enabled = 1;
    } else {
        log_enabled = 0;
    }
    return specific_init(mme_config_p->verbosity_level);
}
