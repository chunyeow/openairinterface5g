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
