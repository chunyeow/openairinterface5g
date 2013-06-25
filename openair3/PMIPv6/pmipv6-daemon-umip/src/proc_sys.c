/*
 * $Id: proc_sys.c 1.9 06/04/25 13:24:14+03:00 anttit@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Author: Ville Nuorvala <vnuorval@tcs.hut.fi>
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
 * 02111-1307 USA.
 */

#include <string.h>
#include <stdio.h>

#include <net/if.h>

#include "proc_sys.h"

int set_iface_proc_entry(const char *tmpl, const char *if_name, int val)
{
	FILE *fp;
	char path[64+IF_NAMESIZE];
	int ret = -1;

	sprintf(path, tmpl, if_name);
	fp = fopen(path, "w");
	if (!fp)
		return ret;
	ret = fprintf(fp, "%d", val);
	fclose(fp);
	return ret;
}

int get_iface_proc_entry(const char *tmpl, const char *if_name, int *val)
{
	FILE *fp;
	char path[64+IF_NAMESIZE];
	int ret = -1;

	sprintf(path, tmpl, if_name);
	fp = fopen(path, "r");
	if (!fp)
		return ret;
	ret = fscanf(fp, "%d", val);
	fclose(fp);
	return ret;
}
