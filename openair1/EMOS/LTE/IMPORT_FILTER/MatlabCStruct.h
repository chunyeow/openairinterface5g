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
// file: MatlabCStruct.h - Include file for MatlabCStruct.c
//--------------------------------------------------------------

//--------------------------------------------------------------
// ELEMENT_ATTRIBUTES
//--------------------------------------------------------------
typedef struct
{
  int ElementSize; // Number of bytes required for storage of a single element in a Matlab array
  int ElementCount; // Number of elements in array
  int StorageSize; // Number of bytes required for storage of a single element in a C Structure
  int Alignment; // 1, 2, 4, or 8 denoting required byte alignment for this structure.
} ELEMENT_ATTRIBUTES;

extern int MatlabCStruct_DebugFlag;

// Function prototypes
ELEMENT_ATTRIBUTES MatlabCStruct(const mxArray *pArray, int MemoryAlignment, int NestLevel,
                                     unsigned char *pDataDest, unsigned char *pDataSource,
                                     int SwapFlag);
char *ClassIDText(const mxArray *pArray);
void SwapBytes(char *p0, int size, int count);
#define SWAP(x) SwapBytes(&(x), sizeof(x))




