/*
 * $Id: debug.c 1.23 06/05/07 21:52:42+03:00 anttit@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Author: Antti Tuominen <anttit@tcs.hut.fi>
 *
 * Copyright 2003-2005 Go-Core Project
 * Copyright 2003-2006 Helsinki University of Technology
 *
 * MIPL Mobile IPv6 for Linux is free software; you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; version 2 of
 * the License.
 *
 * MIPL Mobile IPv6 for Linux is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MIPL Mobile IPv6 for Linux; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "debug.h"

static FILE *sdbg;

static const char *dbg_strdate(char *str)
{
	struct timespec ts;
	time_t t;
	int ret;

	memset(&ts, 0, sizeof(ts));

	ret = clock_gettime(CLOCK_REALTIME, &ts);
	if (ret != 0)
		sprintf(str, "(clock_gettime error=%d)", errno);
	else {
		t = (time_t)ts.tv_sec; /* XXX: fix it! */
		if (t == 0) {
			strcpy(str, "(undefined)");
		} else {
			char buf[1024];
			sprintf(str, "%.19s",
				(ctime_r(&t, buf) ? buf : "(ctime_r error)"));
		}
	}

	return str;
}

void dbgprint(const char *fname, const char *fmt, ...)
{
        char s[1024];
        char stime[1024];
        va_list args;
 
        va_start(args, fmt);
        vsprintf(s, fmt, args);
        va_end(args);

	memset(stime, '\0', sizeof(stime));
	fprintf(sdbg, "%s ", dbg_strdate(stime));

	if (fname)
		fprintf(sdbg, "%s: ", fname);
	fprintf(sdbg, "%s", s);
	fflush(sdbg);
}

void debug_print_buffer(const void *data, int len, const char *fname, 
			const char *fmt, ...)
{ 
	int i; 
	char s[1024];
        va_list args;
 
        va_start(args, fmt);
        vsprintf(s, fmt, args);
        fprintf(sdbg, "%s: %s", fname, s);
        va_end(args);
	for (i = 0; i < len; i++) { 
		if (i % 16 == 0) fprintf(sdbg, "\n%04x: ", i);
		fprintf(sdbg, "%02x ", ((unsigned char *)data)[i]);
	} 
	fprintf(sdbg, "\n\n");
	fflush(sdbg);
}

void debug_print_func(void *arg, void (*func)(void *arg, void *stream))
{
	func(arg, sdbg);
	fflush(sdbg);
}

int debug_open(const char *path)
{
	FILE *fp;

	if (!path)
		return -EINVAL;
	if (sdbg && sdbg != stderr)
		return -EALREADY;

	fp = fopen(path, "a");
	if (!fp)
		return -errno;
	sdbg = fp;

	return 0;
}

void debug_close(void)
{
	if (sdbg && sdbg != stderr)
		fclose(sdbg);
	debug_init();
}

void debug_init(void)
{
	sdbg = stderr;
}

