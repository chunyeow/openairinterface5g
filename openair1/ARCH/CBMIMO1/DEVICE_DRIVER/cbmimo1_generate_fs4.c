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
#ifndef USER_MODE
#define __NO_VERSION__

//#include "rt_compat.h"

#endif

#include "cbmimo1_device.h"
#include "defs.h"
#include "extern.h"
#include "cbmimo1_pci.h"
#include "SCHED/defs.h"
#include "SCHED/extern.h"
#include "PHY/defs.h"
#include "PHY/extern.h"

#include "from_grlib_softconfig.h"
#include "from_grlib_softregs.h"

#ifdef IFFT_FPGA
void openair_generate_fs4(unsigned char IQ_imb) {

  int i,j;

  for (i=0;i<120;i++) {
    for (j=0; j<300; j++) {
      ((mod_sym_t*)TX_DMA_BUFFER[0][0])[300*i+j] = 0; 
      ((mod_sym_t*)TX_DMA_BUFFER[0][1])[300*i+j] = 0; 
    }
    ((mod_sym_t*)TX_DMA_BUFFER[0][0])[300*i+127] = 148; // corresponds to 2^14 in mod_table
    ((mod_sym_t*)TX_DMA_BUFFER[0][1])[300*i+127] = 148; // corresponds to 2^14 in mod_table
  }

}
#else
void openair_generate_fs4(unsigned char IQ_imb) {

  int i,j;

#ifndef BIT8_TXMUX
  unsigned int pQ = 0x7fff-IQ_imb,mQ=0x8001+IQ_imb;
#else
  unsigned char pQ = 0x7f-IQ_imb,mQ=0x81+IQ_imb;
#endif

  for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX;i+=4) {
#ifdef BIT8_TXMUX
    ((unsigned int *)TX_DMA_BUFFER[0][0])[i]   = (0x007f<<16) | (0x007f);
    ((unsigned int *)TX_DMA_BUFFER[0][0])[i+1] = (pQ<<24) | (pQ<<8);
    ((unsigned int *)TX_DMA_BUFFER[0][0])[i+2] = (0x0081<<16) | (0x0081);
    ((unsigned int *)TX_DMA_BUFFER[0][0])[i+3] = (mQ<<24) | (mQ<<8);
#else
    ((unsigned int *)TX_DMA_BUFFER[0][0])[i]   = 0x7fff;
    ((unsigned int *)TX_DMA_BUFFER[0][0])[i+1] = (pQ<<16);
    ((unsigned int *)TX_DMA_BUFFER[0][0])[i+2] = 0x8001;
    ((unsigned int *)TX_DMA_BUFFER[0][0])[i+3] = mQ<<16;
#endif
  }

}
#endif
