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
//--------------------------------------------------------------
// file: cstruct.c - Analyze a Matlab array, to determine how it
//   would fit into C structure.
// 05/25/2004 jaj Removed <windows.h> include; stricter type
//   type compatibility when calling mxCreateStructMatrix();
//--------------------------------------------------------------
#include "mex.h"
#include <math.h>
//#include <windows.h> jaj 05/25/2004 removed

#include "MatlabCStruct.h"

// TODO: Add you supporting functions here


//--------------------------------------------------------------
// function: mexFunction - Entry point from Matlab environment
// INPUTS:
//   nlhs - number of left hand side arguments (outputs)
//   plhs[] - pointer to table where created matrix pointers are
//            to be placed
//   nrhs - number of right hand side arguments (inputs)
//   prhs[] - pointer to table of input matrices
//--------------------------------------------------------------
void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray  *prhs[] )
{
  ELEMENT_ATTRIBUTES Attributes;
  int ByteCount; // Total number of bytes required for C structure
  int Alignment = 8;
  int SwapFlag; // Byte Swapping Flag
  int LittleEndian = 0; // 0/1=local machine is big/litle Endian
  int ii;

  // Determine local byte ordering
  {
    unsigned char pattern[2] = {0x12, 0x34};
    unsigned short word1 = 0x1234;
    unsigned short word2 = *(unsigned short *)&pattern;
    LittleEndian = (word1 == word2) ? 0 : 1;
  }
  SwapFlag = LittleEndian; // Default = assume data is big endian

  if (nlhs == 0)
    MatlabCStruct_DebugFlag = 1;
  else
    MatlabCStruct_DebugFlag = 0;

  if (nrhs < 1)
    mexErrMsgTxt("Not enough input arguments.");

  if (nrhs >= 3)
  {
    for (ii=2; ii<nrhs; ii++)
    {
      // Double = alignment specifier
      if (mxIsDouble(prhs[ii]))
      {
        if (mxGetNumberOfElements(prhs[ii]) != 1)
          mexErrMsgTxt("Alignment specifier must be single element (1, 2, 4, or 8)");
        Alignment = (int)*mxGetPr(prhs[ii]);
        if (Alignment != 1 && Alignment != 2 && Alignment != 4 && Alignment != 8)
          mexWarnMsgTxt("Alignment specifier normally has of value of 1, 2, 4, or 8");
      }
      else if (mxIsChar(prhs[ii]))
      {
        char ch;
        if (mxGetNumberOfElements(prhs[ii]) != 1)
          mexErrMsgTxt("Byte-ordering specifier must be single element ('n','l','b' or 'r')");
        ch = *(char *)mxGetData(prhs[ii]);
        switch(ch)
        {
        case 'n': SwapFlag = 0; break; // Native byte ordering requested
        case 'b': SwapFlag = LittleEndian; break; // Big Endian data
        case 'l': SwapFlag = !LittleEndian; break; // Big Endian data
        case 'r': SwapFlag = 1; break; // Reverse byte ordering
        default: mexErrMsgTxt("Byte-ordering specifier must be one if 'n','l','b' or 'r'");
        }
      }
      else
        mexErrMsgTxt("Optional Arguments must be either Aligmnent Specifier (1,2,4,8), or Byte-orderin Specifier ('n','l','b','r')");

    }
  }

  Attributes = MatlabCStruct(prhs[0], Alignment, 0, (unsigned char *)-1, NULL, SwapFlag);
  ByteCount = Attributes.ElementCount*Attributes.StorageSize;

  if (nrhs >= 2 && !mxIsEmpty(prhs[1]))
  {
    mxClassID ClassID;
    ClassID = mxGetClassID(prhs[1]);
    if (ClassID != mxUINT8_CLASS && ClassID != mxINT8_CLASS)
      mexErrMsgTxt("Byte data vector must be of type INT8 or UINT8");
    if (ByteCount != mxGetNumberOfElements(prhs[1]))
    {
      char msg[100];
      sprintf(msg, "Number of elements in Byte data vector (%d) does not match structure size (%d)",
        mxGetNumberOfElements(prhs[1]), ByteCount);
      mexErrMsgTxt(msg);
    }
    plhs[0] = mxDuplicateArray(prhs[0]);
    MatlabCStruct(plhs[0], Alignment, 0, NULL, mxGetData(prhs[1]), SwapFlag);
  }
  else
  // Create output structure for Attributes
  {
    const char *FieldNames[] = {"size","align"}; // jaj 05/24/2004 added const
    plhs[0] = mxCreateStructMatrix(1, 1, 2, &FieldNames[0]);// jaj 05/24/2004 slightly altered
    mxSetField(plhs[0], 0, "size", mxCreateDoubleScalar(ByteCount));
    mxSetField(plhs[0], 0, "align", mxCreateDoubleScalar(Attributes.Alignment));

    if (nlhs >= 2)
    {
      // Extract C Data from array
      plhs[1] = mxCreateNumericMatrix(1, ByteCount, mxUINT8_CLASS, mxREAL);
      MatlabCStruct(prhs[0], Alignment, 0, mxGetData(plhs[1]), NULL, SwapFlag);
    }
  }

} // end mexFunction()



