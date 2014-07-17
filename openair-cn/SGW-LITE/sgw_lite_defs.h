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
/*! \file sgw_lite_defs.h
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/

#ifndef SGW_LITE_DEFS_H_
#define SGW_LITE_DEFS_H_



#if defined(ENB_MODE)
# include "UTIL/LOG/log.h"
# define SPGW_APP_ERROR(x, args...) LOG_E(SPGW, x, ##args)
# define SPGW_APP_WARN(x, args...)  LOG_W(SPGW, x, ##args)
# define SPGW_APP_TRACE(x, args...)  LOG_T(SPGW, x, ##args)
# define SPGW_APP_INFO(x, args...) LOG_I(SPGW, x, ##args)
# define SPGW_APP_DEBUG(x, args...) LOG_I(SPGW, x, ##args)
#else
# define SPGW_APP_ERROR(x, args...) do { fprintf(stdout, "[SPGW-APP][E]"x, ##args); } while(0)
# define SPGW_APP_WARN(x, args...)  do { fprintf(stdout, "[SPGW-APP][W]"x, ##args); } while(0)
# define SPGW_APP_TRACE(x, args...)  do { fprintf(stdout, "[SPGW-APP][T]"x, ##args); } while(0)
# define SPGW_APP_INFO(x, args...) do { fprintf(stdout, "[SPGW-APP][I]"x, ##args); } while(0)
# define SPGW_APP_DEBUG(x, args...) do { fprintf(stdout, "[SPGW-APP][D]"x, ##args); } while(0)
#endif


int sgw_lite_init(char* config_file_name_pP);

#endif /* SGW_LITE_DEFS_H_ */
