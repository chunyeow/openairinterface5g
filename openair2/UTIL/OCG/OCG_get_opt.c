/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2011 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/

/*! \file OCG_get_opt.c
* \brief Get Options of the OCG command
* \author Lusheng Wang
* \date 2011
* \version 0.1
* \company Eurecom
* \email: lusheng.wang@eurecom.fr
* \note
* \warning
*/

/*--- INCLUDES ---------------------------------------------------------------*/
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "OCG.h"
#include "OCG_get_opt.h"
//#include "log.h"
/*----------------------------------------------------------------------------*/

char filename[FILENAME_LENGTH_MAX];

int get_opt(int argc, char *argv[]) {
	char opts;

	while((opts = getopt(argc, argv, "f:h")) != -1) {

		switch (opts) {
			case 'f' :
				strcpy(filename, optarg);
				LOG_D(OCG, "User specified configuration file is \"%s\"\n", filename);
				return MODULE_OK;
			case 'h' : 
				LOG_I(OCG, "OCG command :	OCG -f \"filename.xml\"\n");
				return GET_HELP;
			default : 
				LOG_E(OCG, "OCG command :	OCG -f \"filename.xml\"\n");
				return GET_HELP;
		}
	}

	return NO_FILE;
}
