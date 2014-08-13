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
// Matrix Utilities for Capacity Calculations
// by Florian Kaltenberger
// Created 10.10.2007

# include "matrix_util.h"
#include <stdio.h>
#include <stdlib.h>

// This function calculates HxH^H for a complex matrix H
// HH must be a pre-allocated matrix of size rows x rows
// The function returns NULL on error and a pointer to the 
// calculated matrix otherwise
struct complexf **square_mat(struct complexf **H, int rows, int cols, struct complexf **HH)
{
	int r,r2,c;
	
	if (H==NULL || HH==NULL || rows == 0 || cols == 0)
		return NULL;
		
	for (r=0; r<rows; r++)
	{
		for (r2=0; r2<rows; r2++)
		{
			HH[r][r2].r = 0;
			HH[r][r2].i = 0;
			for (c=0; c<cols; c++)
			{
				HH[r][r2].r += H[r][c].r * H[r2][c].r + H[r][c].i * H[r2][c].i;
				HH[r][r2].i += H[r][c].i * H[r2][c].r - H[r][c].r * H[r2][c].i;
			}
		}
	}
	return HH;
}
	
// This function scales the matrix H with the factor scale and adds I 
// The original matrix is overwritten	
struct complexf **addI_scale_mat(struct complexf **H, int rows, float scale)
{
	int r,r2;
	
	if (H==NULL || rows == 0)
		return NULL;
		
	for (r=0; r<rows; r++)
	{
		for (r2=0; r2<rows; r2++)
		{
			H[r][r2].r *= scale;
			H[r][r2].i *= scale;
			if (r==r2) 
			{
				H[r][r2].r += 1;
				H[r][r2].i += 1;
			}
		}
	}
	return H;
}
				
// This function calculates the determinant of a 2x2 Hermitian matrix H 
// The result will be real 	
float det(struct complexf **H)
{
	return H[0][0].r * H[1][1].r - (H[0][1].r * H[0][1].r + H[0][1].i * H[0][1].i);
}

float norm_fro_sq(struct complexf**H, int rows, int cols)
{
	float norm=0;
	int ar,ac;
	
	for (ar=0; ar<rows; ar++)
	{
		for (ac=0; ac<cols; ac++)
		{
			norm += H[ar][ac].r * H[ar][ac].r + H[ar][ac].i * H[ar][ac].i;
		}
	}
	return norm;
}
