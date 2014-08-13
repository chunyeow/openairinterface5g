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
#include "rtai_fifos.h"






#    define FIFO_PRINTF_MAX_STRING_SIZE   500
#    define FIFO_PRINTF_NO              63
#    define FIFO_PRINTF_SIZE            65536

static unsigned char       fifo_print_buffer[FIFO_PRINTF_MAX_STRING_SIZE];

void
fifo_printf_init (void)
{
//-----------------------------------------------------------------------------
  printk ("[OPENAIR1] TRACE INIT\n");
  rtf_create (FIFO_PRINTF_NO, FIFO_PRINTF_SIZE);
}

//-----------------------------------------------------------------------------
void
fifo_printf_clean_up (void)
{
//-----------------------------------------------------------------------------
  rtf_destroy (FIFO_PRINTF_NO);
}

//-----------------------------------------------------------------------------
int
fifo_printf (const char *fmt, ...)
{
//-----------------------------------------------------------------------------
  int             i;
  va_list         args;

  va_start (args, fmt);

  i = vsprintf (fifo_print_buffer, fmt, args);
  va_end (args);

  /* perhaps we should discard old data instead */
  if (i > FIFO_PRINTF_MAX_STRING_SIZE) {
    rt_printk ("[OPENAIR] FIFO_PRINTF WROTE OUTSIDE ITS MEMORY BOUNDARY : ERRORS WILL OCCUR\n");
  }
  if (i <= 0) {
    return 0;
  }
  rtf_put (FIFO_PRINTF_NO, fifo_print_buffer, i);
  
  return i;

}
