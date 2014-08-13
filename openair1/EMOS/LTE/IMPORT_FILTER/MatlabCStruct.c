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
// file: MatlabCStruct.c - Computes the amount of storages that
//   would be required to represent a Matlab array in a C lngusage
//   struture. It also moves data between C structures and Matlab
//   arrays.
// This is particularly usful when reading/writing data from a
// byte stream, such as a file or socket interface, which contains
// formatted data, but needs to be re-represented in a Matlab array
// from its raw binary layout, or vice versa.
// 05/24/2004 jaj Replaced __int64 with INT64_T for portability
//--------------------------------------------------------------
#include <stdio.h>
#include <memory.h>

#include <mex.h>
#include "MatlabCStruct.h"

int MatlabCStruct_DebugFlag = 0;

#ifndef MAX
#define MAX(a,b) ((a)>=(b)?(a):(b))
#define MIN(a,b) ((a)<=(b)?(a):(b))
#endif

//--------------------------------------------------------------
// ClassIDText() - Simply return the name of the class
//--------------------------------------------------------------
char *ClassIDText(const mxArray *pArray)
{
  switch(mxGetClassID(pArray))
  {
  case mxUNKNOWN_CLASS : return "UNKNOWN";
  case mxCELL_CLASS    : return "CELL";
  case mxFUNCTION_CLASS: return "FUNCTION";
    //case mxOBJECT_CLASS  : return "OBJECT";
  case mxSTRUCT_CLASS  : return "STRUCT";
  case mxCHAR_CLASS    : return "CHAR";
  case mxLOGICAL_CLASS : return "LOGICAL";
  case mxDOUBLE_CLASS  : return "DOUBLE";
  case mxSINGLE_CLASS  : return "SINGLE";
  case mxINT8_CLASS    : return "INT8";
  case mxUINT8_CLASS   : return "UINT8";
  case mxINT16_CLASS   : return "INT16";
  case mxUINT16_CLASS  : return "UINT16";
  case mxINT32_CLASS   : return "INT32";
  case mxUINT32_CLASS  : return "UINT32";
  case mxINT64_CLASS   : return "INT64";
  default: return "INVALID";
  }
} // end ClassIDText()

//--------------------------------------------------------------
// SwapBytes() - size must be even.
//--------------------------------------------------------------
void SwapBytes(char *p0, int size, int count)
{
  char t; // Temporory register for byte swaps
  int inc0 = size/2; // pointer increments
  int inc1 = size+inc0;
  char *p1 = p0+(size-1);

  while (count-- > 0)
  {
    while (p1>p0)
    {
      t = *p0;
      *p0 = *p1;
      p0++;
      *p1 = t;
      p1--;
    }
    p0 += inc0;
    p1 += inc1;
  }
}
#define SWAP(x) SwapBytes(&(x), sizeof(x))

//--------------------------------------------------------------
// funciton: MatlabCStruct() - Traverse the structure tree
//   to determine the storage requirements when converted to a 
//   C structure.
// INPUTS:
//   pArray - Pointer to Matlab array to be analyzed
//   MemoryAlignment - 1, 2, 4, or 8, indicating desired (maximum) element
//     alignment in the C structure.
//   NestLevel - For recursing strucutres containing structures.
//   pDataDest - NULL, or a pointer to the beginning of a C structure/
//     memory area to which to move Matlab array data.
//   pDataSource - NULL, or a pointer to the beginning of a C structure/
//     memory area from which to move Matlab array data.
// RETURNS:
//   ELEMENT_ATTRIBUTES structure, containing information about the
//     Matlab array being analyzed.
// NOTES:
//   Call with both pDataDest and pDataSource equal to NULL to simply
//     determine how much C storage is required for a given Matlab
//     array. That amount can then be allocated, or compared to
//     expected.
//--------------------------------------------------------------
ELEMENT_ATTRIBUTES MatlabCStruct(const mxArray *pArray, int MemoryAlignment, int NestLevel,
                                     unsigned char *pDataDest, unsigned char *pDataSource, int SwapFlag)
{
#define SPACES {int k;for (k=0; k<NestLevel; k++) printf("  "); }
  ELEMENT_ATTRIBUTES Attributes;
  int i, j;

  // Initialize attributes
  Attributes.ElementCount;
  Attributes.StorageSize = 0;
  Attributes.Alignment = 0;
  Attributes.ElementSize = 0; // This field meaningless for a structure

  if (pArray == NULL)
    return Attributes; // Uninitialized array

  Attributes.ElementCount = mxGetNumberOfElements(pArray);

  switch(mxGetClassID(pArray))
  {
  case mxUNKNOWN_CLASS :
  case mxFUNCTION_CLASS: 
    //case mxOBJECT_CLASS  : Attributes.StorageSize = 0; Attributes.Alignment = 1; return Attributes;

  case mxCELL_CLASS    : break; // Fall down to code below
  case mxSTRUCT_CLASS  : break; // Fall down to code below
  case mxCHAR_CLASS    : Attributes.ElementSize = sizeof(mxChar); break;
  case mxLOGICAL_CLASS : Attributes.ElementSize = sizeof(mxLogical); break;
  case mxDOUBLE_CLASS  : Attributes.ElementSize = sizeof(double); break;
  case mxSINGLE_CLASS  : Attributes.ElementSize = sizeof(float); break;
  case mxINT8_CLASS    : Attributes.ElementSize = sizeof(char); break;
  case mxUINT8_CLASS   : Attributes.ElementSize = sizeof(unsigned char); break;
  case mxINT16_CLASS   : Attributes.ElementSize = sizeof(short); break;
  case mxUINT16_CLASS  : Attributes.ElementSize = sizeof(unsigned short); break;
  case mxINT32_CLASS   : Attributes.ElementSize = sizeof(long); break;
  case mxUINT32_CLASS  : Attributes.ElementSize = sizeof(unsigned long); break;
//  case mxINT64_CLASS   : Attributes.ElementSize = sizeof(__int64); break;
  case mxINT64_CLASS   : Attributes.ElementSize = sizeof(INT64_T); break; // jaj 05/24/2004
  default: mexErrMsgTxt("Unknown MX class");
  }

  if (mxIsCell(pArray)) // Array is a cell array
  {
    int nElem;
    mxArray *pCell; // One field in the structure

    
    nElem = mxGetNumberOfElements(pArray);
    for (i=0; i<nElem; i++)
    {
      ELEMENT_ATTRIBUTES CellAttributes;
      
      pCell = mxGetCell(pArray, i);
      
      CellAttributes = MatlabCStruct(pCell, MemoryAlignment, NestLevel+1, NULL, NULL, SwapFlag);
      
      // See if padding needs to be added before the element
      if (Attributes.StorageSize % CellAttributes.Alignment > 0)
      {
        int Padding;
        Padding = CellAttributes.Alignment - (Attributes.StorageSize % CellAttributes.Alignment);
        if (MatlabCStruct_DebugFlag && (pDataDest || pDataSource))
        {
          SPACES printf("%3d: [%d bytes of padding]\n", Attributes.StorageSize, Padding);
        }
        Attributes.StorageSize += Padding;
      }
      
      if (MatlabCStruct_DebugFlag && (pDataDest || pDataSource))
      {
        SPACES printf("%3d: [cell %d of %d]: %dx%d bytes of %s data\n", Attributes.StorageSize, i, nElem, CellAttributes.StorageSize, CellAttributes.ElementCount, ClassIDText(pCell));
      }
      
      if (pDataDest)
        if ((long)pDataDest == -1)
          CellAttributes = MatlabCStruct(pCell, MemoryAlignment, NestLevel+1, pDataDest, NULL, SwapFlag);
        else
          CellAttributes = MatlabCStruct(pCell, MemoryAlignment, NestLevel+1, pDataDest+Attributes.StorageSize, NULL, SwapFlag);
      if (pDataSource)
        if ((long)pDataSource == -1)
          CellAttributes = MatlabCStruct(pCell, MemoryAlignment, NestLevel+1, NULL, pDataSource, SwapFlag);
        else
          CellAttributes = MatlabCStruct(pCell, MemoryAlignment, NestLevel+1, NULL, pDataSource+Attributes.StorageSize, SwapFlag);
      
      Attributes.StorageSize += CellAttributes.StorageSize * CellAttributes.ElementCount;
      Attributes.Alignment = MAX(Attributes.Alignment, CellAttributes.Alignment);
    }
    
    // See if padding needs to be added at the end of the structure
    if (Attributes.StorageSize % Attributes.Alignment > 0)
    {
      int Padding;
      Padding = Attributes.Alignment - (Attributes.StorageSize % Attributes.Alignment);
      if (MatlabCStruct_DebugFlag && (pDataDest || pDataSource))
      {
        SPACES printf("%3d: [%d bytes of padding]\n", Attributes.StorageSize, Padding);
      }
      Attributes.StorageSize += Padding;
    }
    
    if (MatlabCStruct_DebugFlag && (pDataDest || pDataSource))
    {
      SPACES printf("%3d: [end of cell array]\n", Attributes.StorageSize);
    }

    // 08/14/03 jaj Make the element count 1 for cell arrays
    Attributes.ElementCount = 1;

  } // end array is Cell array


  else if (mxIsChar(pArray))
  {
    mxChar *pChar;
    Attributes.StorageSize = sizeof(char); // special case
    Attributes.Alignment = MIN(Attributes.StorageSize, MemoryAlignment);
    if (pDataDest && (long)pDataDest != -1)
    {
      pChar = (mxChar *)mxGetData(pArray);
      for (i=0; i<Attributes.ElementCount; i++)
        pDataDest[i] = (unsigned char)pChar[i];
    }
    if (pDataSource && (long)pDataSource != -1)
    {
      pChar = (mxChar *)mxGetData(pArray);
      for (i=0; i<Attributes.ElementCount; i++)
        pChar[i] = (mxChar)pDataSource[i];
    }
    
  }
  
  
  else if (mxIsStruct(pArray)) // Array is a Structure
  {
    int nElem; // 08/14/03 jaj
    int nField;
    mxArray *pField; // One field in the structure
    
    // Initialize attributes (ElementCount has already been assigned)
    Attributes.StorageSize = 0;
    Attributes.Alignment = 0;
    Attributes.ElementSize = 0; // This field meaningless for a structure
    
    nField = mxGetNumberOfFields(pArray);
    nElem = mxGetNumberOfElements(pArray);

    for (j=0; j<nElem; j++) // 08/14/03 jaj
    {
      ELEMENT_ATTRIBUTES LocalAttributes;
      LocalAttributes.StorageSize = 0;
      LocalAttributes.Alignment = 0;
      LocalAttributes.ElementSize = 0; // This field meaningless for a structure

      if (nElem > 1 && MatlabCStruct_DebugFlag && (pDataDest || pDataSource))
      { 
        SPACES printf("%3d: [index %d of %d]\n", Attributes.StorageSize, j, nElem);
        NestLevel++;
      }


      for (i=0; i<nField; i++)
      {
        const char *FieldName; // For debug purposes
        ELEMENT_ATTRIBUTES FieldAttributes;
        
        FieldName = mxGetFieldNameByNumber(pArray, i);
        pField = mxGetFieldByNumber(pArray, j, i);
        
        FieldAttributes = MatlabCStruct(pField, MemoryAlignment, NestLevel+1, NULL, NULL, SwapFlag);
        
        // See if padding needs to be added before the element
        if (LocalAttributes.StorageSize % FieldAttributes.Alignment > 0)
        {
          int Padding;
          Padding = FieldAttributes.Alignment - (LocalAttributes.StorageSize % FieldAttributes.Alignment);
          if (MatlabCStruct_DebugFlag && (pDataDest || pDataSource))
          {
            SPACES printf("%3d: [%d bytes of padding]\n", LocalAttributes.StorageSize, Padding);
          }
          LocalAttributes.StorageSize += Padding;
        }
        
        if (MatlabCStruct_DebugFlag && (pDataDest || pDataSource))
        {
          SPACES printf("%3d: [%s] %dx%d bytes of %s data\n", LocalAttributes.StorageSize, FieldName, FieldAttributes.StorageSize, FieldAttributes.ElementCount, ClassIDText(pField));
        }
        
        if (pDataDest)
        {
          if ((long)pDataDest == -1)
            FieldAttributes = MatlabCStruct(pField, MemoryAlignment, NestLevel+1, pDataDest, NULL, SwapFlag);
          else
            FieldAttributes = MatlabCStruct(pField, MemoryAlignment, NestLevel+1, pDataDest+LocalAttributes.StorageSize+Attributes.StorageSize, NULL, SwapFlag);
        }
        if (pDataSource)
        {
          if ((long)pDataSource == -1)
            FieldAttributes = MatlabCStruct(pField, MemoryAlignment, NestLevel+1, NULL, pDataSource, SwapFlag);
          else
            FieldAttributes = MatlabCStruct(pField, MemoryAlignment, NestLevel+1, NULL, pDataSource+LocalAttributes.StorageSize+Attributes.StorageSize, SwapFlag);
        }
        
        LocalAttributes.StorageSize += FieldAttributes.StorageSize * FieldAttributes.ElementCount;
        LocalAttributes.Alignment = MAX(LocalAttributes.Alignment, FieldAttributes.Alignment);
        LocalAttributes.ElementCount = 1;
      }

      // See if padding needs to be added at the end of the structure
      if (LocalAttributes.StorageSize % LocalAttributes.Alignment > 0)
      {
        int Padding;
        Padding = LocalAttributes.Alignment - (LocalAttributes.StorageSize % LocalAttributes.Alignment);
        if (MatlabCStruct_DebugFlag && (pDataDest || pDataSource))
        {
          SPACES printf("%3d: [%d bytes of padding]\n", LocalAttributes.StorageSize, Padding);
        }
        LocalAttributes.StorageSize += Padding;
      }
    
      Attributes.StorageSize += LocalAttributes.StorageSize * LocalAttributes.ElementCount;
      Attributes.Alignment = MAX(Attributes.Alignment, LocalAttributes.Alignment);

      if (nElem > 1 && MatlabCStruct_DebugFlag && (pDataDest || pDataSource))
      { 
        NestLevel--;
      }
    } // end for(j)
    
    // See if padding needs to be added at the end of the structure
    if (Attributes.StorageSize % Attributes.Alignment > 0)
    {
      int Padding;
      Padding = Attributes.Alignment - (Attributes.StorageSize % Attributes.Alignment);
      if (MatlabCStruct_DebugFlag && (pDataDest || pDataSource))
      {
        SPACES printf("%3d: [%d bytes of padding]\n", Attributes.StorageSize, Padding);
      }
      Attributes.StorageSize += Padding;
    }
    
    if (MatlabCStruct_DebugFlag && (pDataDest || pDataSource))
    {
      SPACES printf("%3d: [end of structure]\n", Attributes.StorageSize);
    }

    // 08/14/03 jaj Make the element count 1 for structure arrays
    Attributes.ElementCount = 1;

  } // end array is Structure

  
  else
  {
    // Array is any of the basic, non-aggregate types, except mxChar
    // Handle byte-swapping on Windows WIN32 platform

    Attributes.StorageSize = Attributes.ElementSize;
    Attributes.Alignment = MIN(Attributes.StorageSize, MemoryAlignment);
    if (pDataDest && (long)pDataDest != -1)
    {
      memcpy(pDataDest, mxGetData(pArray), Attributes.ElementSize * Attributes.ElementCount);
      if (SwapFlag)
        SwapBytes((char *)pDataDest, Attributes.ElementSize, Attributes.ElementCount);
    }
    if (pDataSource && (long)pDataSource != -1)
    {
      if (SwapFlag)
        SwapBytes((char *)pDataSource, Attributes.ElementSize, Attributes.ElementCount);
      memcpy(mxGetData(pArray), pDataSource, Attributes.ElementSize * Attributes.ElementCount);
      // and un-swap
      if (SwapFlag)
        SwapBytes((char *)pDataSource, Attributes.ElementSize, Attributes.ElementCount);
    }
  } // end basic type (except mxChar)
  
  
  return Attributes;
} // end MatlabCStruct()

