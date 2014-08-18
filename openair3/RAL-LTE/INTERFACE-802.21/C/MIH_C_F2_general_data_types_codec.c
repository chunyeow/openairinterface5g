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

#define MIH_C_INTERFACE
#define MIH_C_F2_GENERAL_DATA_TYPES_CODEC_C
#include "MIH_C_F2_general_data_types_codec.h"
//-----------------------------------------------------------------------------
unsigned int MIH_C_BOOLEAN2String(MIH_C_BOOLEAN_T* dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    if (*dataP != MIH_C_BOOLEAN_FALSE) {
        buffer_index += sprintf(&bufP[buffer_index], "TRUE");
    } else {
        buffer_index += sprintf(&bufP[buffer_index], "FALSE");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_STATUS2String(MIH_C_STATUS_T *statusP, char* bufP){
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    switch (*statusP) {
        case MIH_C_STATUS_SUCCESS:                 buffer_index += sprintf(&bufP[buffer_index], "STATUS_SUCCESS");break;
        case MIH_C_STATUS_UNSPECIFIED_FAILURE:     buffer_index += sprintf(&bufP[buffer_index], "STATUS_UNSPECIFIED_FAILURE");break;
        case MIH_C_STATUS_REJECTED:                buffer_index += sprintf(&bufP[buffer_index], "STATUS_REJECTED");break;
        case MIH_C_STATUS_AUTHORIZATION_FAILURE:   buffer_index += sprintf(&bufP[buffer_index], "STATUS_AUTHORIZATION_FAILURE");break;
        case MIH_C_STATUS_NETWORK_ERROR:           buffer_index += sprintf(&bufP[buffer_index], "STATUS_NETWORK_ERROR");break;
        default:                                   buffer_index += sprintf(&bufP[buffer_index], "UNKNOWN_STATUS");
    }
    return buffer_index;
}

