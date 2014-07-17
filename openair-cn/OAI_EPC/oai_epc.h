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
/** @mainpage

  @section intro Introduction

  openair-mme project tends to provide an implementation of LTE core network.

  @section scope Scope


  @section design Design Philosophy

  Included protocol stacks:
  - SCTP RFC####
  - S1AP 3GPP TS 36.413 R10.5
  - S11 abstraction between MME and S-GW
  - 3GPP TS 23.401 R10.5
  - nw-gtpv1u for s1-u (http://amitchawre.net/)
  - freeDiameter project (http://www.freediameter.net/) 3GPP TS 29.272 R10.5

  @section applications Applications and Usage

  Please use the script to start LTE epc in root src directory

 */

#ifndef OAI_EPC_H_
#define OAI_EPC_H_

int oai_epc_log_specific(int log_level);

int main(int argc, char *argv[]);

#endif /* OAI_EPC_H_ */
