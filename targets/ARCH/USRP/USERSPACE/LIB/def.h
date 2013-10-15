#include <stdarg.h>

#define SAMPLES_PER_FRM 76800
#define SAMPLES_PER_SLOT 3840
#define HW_offset 32
#define N_slot_offset 4
#define T_start 3840*20*100

void format_printf(int flag,const char * fmt, ...)
{
	if(flag)
	{
		va_list args;
		va_start(args,fmt);
		vprintf(fmt,args);
		va_end(args);
	}
}
