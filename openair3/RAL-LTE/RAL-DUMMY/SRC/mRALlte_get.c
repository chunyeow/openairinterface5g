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
#define MRAL_MODULE
#define MRALLTE_GET_C
#include "mRALlte_get.h"

//-----------------------------------------------------------------------------
void MIH_C_3GPP_ADDR_load_3gpp_str_address(MIH_C_3GPP_ADDR_T* _3gpp_addrP, u_int8_t* strP)
//-----------------------------------------------------------------------------
{
    int           i, l;
    u_int8_t      val_temp;
    unsigned char address_3gpp[32];
    unsigned char buf[3];
    u_int8_t _3gpp_byte_address[8];

    strcpy((char *)address_3gpp, (char *)strP);
    for(l=0; l<8; l++)
    {
        i=l*2;
        buf[0]= address_3gpp[i];
        buf[1]= address_3gpp[i+1];
        buf[2]= '\0';
        //sscanf((const char *)buf,"%02x", &val_temp);
        sscanf((const char *)buf,"%hhx", &val_temp);
        _3gpp_byte_address[l] = val_temp;
    }
    MIH_C_3GPP_ADDR_set(_3gpp_addrP, _3gpp_byte_address, 8);
}