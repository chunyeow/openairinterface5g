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
/** Header file generated with fdesign on Tue Nov  4 16:26:49 2008.**/

#ifndef FD_chbch_scope_h_
#define FD_chbch_scope_h_

/** Callbacks, globals and object handlers **/


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *chbch_scope;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *control_miniframe;
	FL_OBJECT *channel_t_re;
	FL_OBJECT *scatter_plot;
	FL_OBJECT *demod_out;
	FL_OBJECT *channel_f;
	FL_OBJECT *channel_t_im;
	FL_OBJECT *decoder_input;
	FL_OBJECT *determ;
	FL_OBJECT *ideterm;
} FD_chbch_scope;

extern FD_chbch_scope * create_form_chbch_scope(void);

#endif /* FD_chbch_scope_h_ */
