/***************************************************************************
                          rrc_bch_per_basic.c  -  description
                             -------------------
    copyright            : (C) 2002, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Procedures to perform PER encoding/decoding of basic entities
  Currently, following entities are supported:
  - constrained integer
  - bit string
  - constrained unsigned integer
 ***************************************************************************/
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
//#include "UTIL/MEM/mem_block.h"

#ifdef NODE_RG
#include "rrc_rg_vars_extern.h"
#endif
#ifdef NODE_MT
#include "rrc_ue_vars_extern.h"
#endif

/********************
// RRC definitions
 ********************/
//#include "rrc_constant.h"
//#include "rrc_bch_mib.h"
//-----------------------------------------------------------------------------
//#include "rrc_proto_int.h"
void rrc_print_buffer (char *buffer, int length);


//-----------------------------------------------------------------------------
void rrc_new_per_parms (PERParms * pParms, ENCODEDBLOCK * pBuffer){
//-----------------------------------------------------------------------------
  pParms->buffer = pBuffer;
  pParms->buff_index = 1;       //start +1 = placeholder for buffer total length
  pParms->buff_size = 1;        // idem
  pParms->bitoffset = 0;
  pParms->errInfo = P_SUCCESS;
}

//-----------------------------------------------------------------------------
void rrc_set_per_length (PERParms * pParms){
//-----------------------------------------------------------------------------
  char *pwrite;

  pwrite = ((char *) pParms->buffer);
  *pwrite = pParms->buff_size;
}

//-----------------------------------------------------------------------------
int rrc_get_per_length (ENCODEDBLOCK * pBlock){
//-----------------------------------------------------------------------------
  u8 length;

  length = *((char *) pBlock);
  return length;
}

//-----------------------------------------------------------------------------
void rrc_print_per_parms (PERParms * pParms){
//-----------------------------------------------------------------------------
#ifdef DEBUG_RRC_BROADCAST
  msg ("[RRC_BCH] PER Control block values \t- index : %d\t- size : %d\t- error info : %d", pParms->buff_index, pParms->buff_size, pParms->errInfo);
  rrc_print_buffer ((char *) pParms->buffer, pParms->buff_size);
#endif
}

//-----------------------------------------------------------------------------
int rrc_PEREnc_padding (PERParms * pParms, int length){
//-----------------------------------------------------------------------------
  int  status = P_SUCCESS;
  char *pwrite;
  int  i;

  pwrite = ((char *) pParms->buffer) + pParms->buff_index;

  for (i = 0; i < length; i++) {
    *pwrite = 0;
    pwrite++;
  }
  pParms->buff_index += length;
  pParms->buff_size += length;
  return status;
}

/**************************************************************/
/*  Constrained Integer                                       */
/**************************************************************/
//-----------------------------------------------------------------------------
int rrc_PEREnc_ConstrInteger (PERParms * pParms, int pvalue, int lower, int upper){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  unsigned char *pwrite = NULL;
  unsigned short int *pshort = NULL;

  pwrite = ((unsigned char *) pParms->buffer) + pParms->buff_index;

  if (pvalue >= lower && pvalue <= upper) {
    if ((lower >= -128) && (upper <= 127)) {
//                *pwrite = (unsigned char)pvalue;
      *pwrite = (char) pvalue;
      pParms->buff_index += 1;
      pParms->buff_size += 1;
    } else {
      if ((lower >= -32768) && (upper <= 32767)) {
//          pshort = (unsigned short int *)pwrite;
//                *pshort = (unsigned short int)pvalue;
        pshort = (unsigned short int *) pwrite;
        *pshort = (short int) pvalue;
        pParms->buff_index += 2;
        pParms->buff_size += 2;
      } else {
#ifdef DEBUG_RRC_BROADCAST
        msg ("[RRC_BCH] Error, Value larger than 2 bytes, Code to be written : %d\n", pvalue);
#endif
        status = P_MISCLERROR;
      }
    }
  } else {
    status = P_OUTOFBOUNDS;
  }
  return status;
}

//-----------------------------------------------------------------------------
int rrc_PERDec_ConstrInteger (PERParms * pParms, int *pvalue, int lower, int upper){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
//   unsigned char * pRead;
//   unsigned short int * pshort;
  char *pRead = NULL;
  short int *pshort = NULL;

  char value_read_1 = 0;
  short int value_read_2 = 0;

  pRead = ((char *) pParms->buffer) + pParms->buff_index;
  if ((lower >= -128) && (upper <= 127)) {
    value_read_1 = *(pRead);
    if (value_read_1 >= lower && value_read_1 <= upper) {
      *pvalue = value_read_1;
      pParms->buff_index += 1;
      pParms->buff_size -= 1;
    } else {
      status = P_OUTOFBOUNDS;
    }
  } else {
    if ((lower >= -32768) && (upper <= 32767)) {
      pshort = (short int *) pRead;
//       pshort = (unsigned short int *)pRead;
      value_read_2 = *(pshort);
      if (value_read_2 >= lower && value_read_2 <= upper) {
        *pvalue = value_read_2;
        pParms->buff_index += 2;
        pParms->buff_size -= 2;
      } else {
        status = P_OUTOFBOUNDS;
      }
    } else {
#ifdef DEBUG_RRC_BROADCAST
      msg ("[RRC_BCH] Error, Value>65535, Code to be written : %d\n", *pRead);
#endif
      status = P_MISCLERROR;
    }
#ifdef DEBUG_RRC_BROADCAST
    //      msg("[RRC_BCH] Value read : %d\n", *pvalue);
#endif

  }
  return status;
}

/**************************************************************/
/*  Constrained Unsigned Integer                              */
/**************************************************************/
//-----------------------------------------------------------------------------
int rrc_PEREnc_ConsUnsigned (PERParms * pParms, unsigned int pvalue, unsigned int lower, unsigned int upper){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  unsigned char  *pwrite;
  unsigned short int *pshort;

  pwrite = ((unsigned char *) pParms->buffer) + pParms->buff_index;

  if (pvalue >= lower && pvalue <= upper) {
    if (upper <= 255) {
      *pwrite = (unsigned char) pvalue;
      pParms->buff_index += 1;
      pParms->buff_size += 1;
    } else {
      if (upper <= 65535) {
        pshort = (unsigned short int *) pwrite;
        *pshort = (unsigned short int) pvalue;
        pParms->buff_index += 2;
        pParms->buff_size += 2;
      } else {
#ifdef DEBUG_RRC_BROADCAST
        msg ("[RRC_BCH] Error, Value>65535, Code to be written : %d\n", pvalue);
#endif
        status = P_MISCLERROR;
      }
    }
  } else {
    status = P_OUTOFBOUNDS;
  }
  return status;
}

//-----------------------------------------------------------------------------
int rrc_PERDec_ConsUnsigned (PERParms * pParms, unsigned int *pvalue, unsigned int lower, unsigned int upper){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  unsigned char  *pRead;
  unsigned short int *pshort;

  unsigned char value_read_1 = 0;
  short int value_read_2 = 0;

  pRead = ((unsigned char *) pParms->buffer) + pParms->buff_index;
  if (upper <= 255) {
    value_read_1 = *(pRead);
    if (value_read_1 >= lower && value_read_1 <= upper) {
      *pvalue = value_read_1;
      pParms->buff_index += 1;
      pParms->buff_size -= 1;
    } else {
      status = P_OUTOFBOUNDS;
    }
  } else {
    if (upper <= 65535) {
      pshort = (unsigned short int *) pRead;
      value_read_2 = *(pshort);
      if (value_read_2 >= lower && value_read_2 <= upper) {
        *pvalue = value_read_2;
        pParms->buff_index += 2;
        pParms->buff_size -= 2;
      } else {
        status = P_OUTOFBOUNDS;
      }
    } else {
#ifdef DEBUG_RRC_BROADCAST
      msg ("[RRC_BCH] Error, Value>65535, Code to be written : %d\n", *pRead);
#endif
      status = P_MISCLERROR;
    }
  }
  return status;
}

/**************************************************************/
/*  Bit String                                                */
/**************************************************************/
// working on octets in 1st step - 3rd parameter not used , FFS
//-----------------------------------------------------------------------------
int rrc_PEREnc_BitString (PERParms * pParms, unsigned int numbits, unsigned char *data){
//-----------------------------------------------------------------------------
  int             status = P_SUCCESS;
  unsigned int    length;
  int             i;
  char           *pwrite;
  char           *psource;
  if (numbits == 0) {           //data buffer empty. Get encoded values from pParms
    length = rrc_get_per_length (pParms->data);
    pwrite = ((char *) pParms->buffer) + pParms->buff_index;
    psource = (char *) pParms->data;

    for (i = 0; i < length; i++) {
      *pwrite = *psource;
      pwrite++;
      psource++;
    }
    pParms->buff_index += length;
    pParms->buff_size += length;
  } else {
#ifdef DEBUG_RRC_BROADCAST
    msg ("[RRC_BCH] Error, numbits>0, Code to be written \n");
#endif
    status = P_MISCLERROR;
    pParms->errInfo = P_MISCLERROR;
  }
  return status;
}

//-----------------------------------------------------------------------------
int rrc_PERDec_BitString (PERParms * pParms, unsigned int *numbits_p, char *buffer, unsigned int bufsiz){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  return status;
}

/**************************************************************/
/*  Octet string                                                */
/**************************************************************/
//-----------------------------------------------------------------------------
int rrc_PEREnc_OctetString (PERParms * pParms, unsigned int numocts, char *data){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  char *pwrite;

  pwrite = ((char *) pParms->buffer) + pParms->buff_index;
  memcpy (pwrite, data, numocts);

  pParms->buff_index += numocts;
  pParms->buff_size += numocts;

  return status;
}

//-----------------------------------------------------------------------------
int rrc_PERDec_OctetString (PERParms * pParms, unsigned int *numocts_p, char *data, unsigned int bufsize){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  //int i;
  char *pwrite;
  char *psource;
  unsigned int numocts;

  psource = ((char *) pParms->buffer) + pParms->buff_index;
  pwrite = data;

  numocts = *psource;
  *numocts_p = bufsize;

  memcpy (data, psource, bufsize);
  pParms->buff_index += numocts + 1;
  pParms->buff_size -= (numocts + 1);


  return status;
}

/**************************************************************/
/*  Var Octet String - same as string, but with length        */
/**************************************************************/
//-----------------------------------------------------------------------------
int rrc_PEREnc_VarOctetString (PERParms * pParms, unsigned int numocts, unsigned char *data){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  char *pwrite;

  pwrite = ((char *) pParms->buffer) + pParms->buff_index;

  *pwrite = numocts;
  pwrite += 1;

  memcpy (pwrite, data, numocts);
  pParms->buff_index += numocts + 1;
  pParms->buff_size += numocts + 1;

  return status;
}

//-----------------------------------------------------------------------------
int rrc_PERDec_VarOctetString (PERParms * pParms, unsigned int *numocts_p, unsigned char *data){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  char *psource;
  unsigned int numocts;
  u8 read_value;

  psource = ((char *) pParms->buffer) + pParms->buff_index;

  read_value = *psource;
  numocts = (unsigned int)read_value;
//  (u8) numocts = *((u8 *)psource);
  *numocts_p = numocts;
  psource += 1;

  memcpy (data, psource, numocts);
  pParms->buff_index += numocts + 1;
  pParms->buff_size -= (numocts + 1);

  return status;
}

/**************************************************************/
/*  Digit                                                     */
/**************************************************************/
//-----------------------------------------------------------------------------
int rrc_PEREnc_Digit (PERParms * pParms, Digit value){
//-----------------------------------------------------------------------------
  int  status = P_SUCCESS;
  // status = rrc_PEREnc_ConstrInteger (pParms, value, 0, 9);
  char *pwrite;
  if (value >= 0 && value <= 9) {
    pwrite = ((char *) pParms->buffer) + pParms->buff_index;
    *pwrite = (char) value;
    pParms->buff_index += 1;
    pParms->buff_size += 1;
  } else {
    status = P_INVDIGIT;
  }
  return (status);
}

//-----------------------------------------------------------------------------
int rrc_PERDec_Digit (PERParms * pParms, Digit * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  //status = rrc_PERDec_ConstrInteger (pParms, pvalue, 0, 9);
  char  *pRead;
  unsigned char value_read;

  pRead = ((char *) pParms->buffer) + pParms->buff_index;
  value_read = *(pRead);
//   if (value_read>=0 && value_read<=9){
  *pvalue = value_read;
  pParms->buff_index += 1;
  pParms->buff_size -= 1;
//   }else{
//   status = P_INVDIGIT;
//   }
  return (status);
}

/**************************************************************/
/*  Length                                                    */
/**************************************************************/
//-----------------------------------------------------------------------------
int rrc_PEREnc_Length (PERParms * pParms, unsigned int value){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  char *pwrite;
  if (value <= 255) {
    pwrite = ((char *) pParms->buffer) + pParms->buff_index;
    *pwrite = (char) value;
    pParms->buff_index += 1;
    pParms->buff_size += 1;
  } else {
#ifdef DEBUG_RRC_BROADCAST
    msg ("[RRC_BCH] Error, length>255, value : %d\n", value);
#endif
    status = P_OUTOFBOUNDS;
  }
  return status;
}

//-----------------------------------------------------------------------------
int rrc_PERDec_Length (PERParms * pParms, unsigned int *pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  char *pRead;

  pRead = ((char *) pParms->buffer) + pParms->buff_index;
  *pvalue = *pRead;
  pParms->buff_index += 1;
  pParms->buff_size -= 1;

  return status;
}
