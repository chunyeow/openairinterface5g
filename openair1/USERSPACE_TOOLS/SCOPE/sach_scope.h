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
/** Header file generated with fdesign on Tue Oct 21 17:41:09 2008.**/

#ifndef FD_sach_scope_h_
#define FD_sach_scope_h_

/** Callbacks, globals and object handlers **/


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *sach_scope;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *sacch00;
	FL_OBJECT *sach00;
	FL_OBJECT *sacch01;
	FL_OBJECT *sach01;
	FL_OBJECT *sacch02;
	FL_OBJECT *sach02;
	FL_OBJECT *sacch03;
	FL_OBJECT *sach03;
	FL_OBJECT *sacch04;
	FL_OBJECT *sach04;
	FL_OBJECT *sacch10;
	FL_OBJECT *sach10;
	FL_OBJECT *sacch11;
	FL_OBJECT *sach11;
	FL_OBJECT *sacch12;
	FL_OBJECT *sach12;
	FL_OBJECT *sacch13;
	FL_OBJECT *sach13;
	FL_OBJECT *sacch14;
	FL_OBJECT *sach14;
	FL_OBJECT *sacch20;
	FL_OBJECT *sach20;
	FL_OBJECT *sacch21;
	FL_OBJECT *sach21;
	FL_OBJECT *sacch22;
	FL_OBJECT *sach22;
	FL_OBJECT *sacch23;
	FL_OBJECT *sach23;
	FL_OBJECT *sacch24;
	FL_OBJECT *sach24;
	FL_OBJECT *sacch30;
	FL_OBJECT *sach30;
	FL_OBJECT *sacch31;
	FL_OBJECT *sach31;
	FL_OBJECT *sacch32;
	FL_OBJECT *sach32;
	FL_OBJECT *sacch33;
	FL_OBJECT *sach33;
	FL_OBJECT *sacch34;
	FL_OBJECT *sach34;
} FD_sach_scope;

extern FD_sach_scope * create_form_sach_scope(void);

#endif /* FD_sach_scope_h_ */
