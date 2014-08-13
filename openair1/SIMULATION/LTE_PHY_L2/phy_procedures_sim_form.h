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
/** Header file generated with fdesign on Tue Jul 27 16:31:42 2010.**/

#ifndef FD_phy_procedures_sim_h_
#define FD_phy_procedures_sim_h_

/** Callbacks, globals and object handlers **/


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *phy_procedures_sim;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *pusch_constellation;
	FL_OBJECT *pdsch_constellation;
	FL_OBJECT *ch00;
} FD_phy_procedures_sim;

extern FD_phy_procedures_sim * create_form_phy_procedures_sim(void);

#endif /* FD_phy_procedures_sim_h_ */
