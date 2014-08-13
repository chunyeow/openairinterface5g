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
double s_coeffs_eNB[11] = {-0.0031, 0.0131, -0.0101, -0.0843, 0.2630, 0.6428, 0.2630, -0.0843, -0.0101, 0.0131, -0.0031};
int s_ord_fir_eNB = 11;

double r_coeffs_eNB[6] = {-0.0098, 0.0104, 0.4995, 0.4995, 0.0104, -0.0098};
int r_ord_fir_eNB = 6;

double s_coeffs_UE[9] = {0.0062, -0.0054, -0.0661, 0.2471, 0.6365, 0.2471, -0.0661, -0.0054, 0.0062};
int s_ord_fir_UE = 9;

double r_coeffs_UE[8] = {0.0050, -0.0303, 0.0163, 0.5090, 0.5090, 0.0163, -0.0303, 0.0050};
int r_ord_fir_UE = 8;/*

double s_coeffs_eNB[2] = {1, 0}; 
int s_ord_fir_eNB = 2;

double r_coeffs_eNB[2] = {1, 0};
int r_ord_fir_eNB = 2;

double s_coeffs_UE[2] = {1, 0};
int s_ord_fir_UE = 2;

double r_coeffs_UE[2] = {1, 0};
int r_ord_fir_UE = 2;*/
