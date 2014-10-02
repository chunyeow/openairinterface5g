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
/*****************************************************************************

Source      mcc_mnc.h

Version     0.1

Date        {2014/10/02

Product

Subsystem

Author      Lionel GAUTHIER

Description Defines the MCC/MNC list delivered by the ITU

*****************************************************************************/
#ifndef __MCC_MNC_H__
#define __MCC_MNC_H__


typedef struct mcc_mnc_list_s {
    uint16_t mcc;
    char     mnc[4];
} mcc_mnc_list_t;

int find_mnc_length(const char mcc_digit1P,
        const char mcc_digit2P,
        const char mcc_digit3P,
        const char mnc_digit1P,
        const char mnc_digit2P,
        const char mnc_digit3P);
#endif
