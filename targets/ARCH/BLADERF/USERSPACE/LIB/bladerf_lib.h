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
  
   Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

/** bladerf_lib.h
 *
 * Author: navid nikaein
 */

#include <libbladeRF.h>
/*
enum brf_err_num{
  BLADERF_ERR_MIN=0,
  BLADERF_ERR_UNEXPECTED=-1,
  BLADERF_ERR_RANGE=-2,
  BLADERF_ERR_INVAL=-3,
  BLADERF_ERR_MEM=-4,
  BLADERF_ERR_IO=-5,
  BLADERF_ERR_TIMEOUT=-6,
  BLADERF_ERR_NODEV=-7,
  BLADERF_ERR_UNSUPPORTED=-8,
  BLADERF_ERR_MISALIGNED=-9,
  BLADERF_ERR_CHECKSUM=-10,
  BLADERF_ERR_NO_FILE=-11,
  BLADERF_ERR_UPDATE_FPGA=-12,
  BLADERF_ERR_UPDATE_FW=-13,
  BLADERF_ERR_TIME_PAST=-14,
  BLADERF_ERR_MAX=-15,
}
mapping brf_err_names[] = {
  {"BLADERF_OK", BLADERF_ERR_MIN},
  {"BLADERF_ERR_UNEXPECTED",BLADERF_ERR_UNEXPECTED},
  {"BLADERF_ERR_RANGE",BLADERF_ERR_RANGE},
  {"BLADERF_ERR_INVAL",BLADERF_ERR_INVAL},
  {"BLADERF_ERR_MEM",BLADERF_ERR_MEM},
  {"BLADERF_ERR_IO",BLADERF_ERR_IO},
  {"BLADERF_ERR_TIMEOUT",BLADERF_ERR_TIMEOUT},
  {"BLADERF_ERR_NODEV",BLADERF_ERR_NODEV},
  {"BLADERF_ERR_UNSUPPORTED",BLADERF_ERR_UNSUPPORTED},
  {"BLADERF_ERR_MISALIGNED",BLADERF_ERR_MISALIGNED},
  {"BLADERF_ERR_CHECKSUM",BLADERF_ERR_CHECKSUM},
  {"BLADERF_ERR_NO_FILE",BLADERF_ERR_NO_FILE},
  {"BLADERF_ERR_UPDATE_FPGA",BLADERF_ERR_UPDATE_FPGA},
  {"BLADERF_ERR_UPDATE_FW",BLADERF_ERR_UPDATE_FW},
  {"BLADERF_ERR_TIME_PAST",BLADERF_ERR_TIME_PAST},
  {NULL, BLADERF_ERR_MAX}
};
*/
/*
 * func prototypes 
 */

void brf_error(int status);
