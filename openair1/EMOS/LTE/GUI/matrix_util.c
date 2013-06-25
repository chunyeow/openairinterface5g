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
