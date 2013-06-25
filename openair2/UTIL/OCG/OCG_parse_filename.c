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

/*! \file OCG_parse_filename.c
* \brief Parse the filename of the XML file
* \author Lusheng Wang
* \date 2011
* \version 0.1
* \company Eurecom
* \email: lusheng.wang@eurecom.fr
* \note
* \warning
*/

/*--- INCLUDES ---------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "OCG_vars.h"
#include "OCG_parse_filename.h"
#include "UTIL/LOG/log.h"

/*----------------------------------------------------------------------------*/

int parse_filename(char filename[FILENAME_LENGTH_MAX]) {
	char *delim = "._";
	//char *result;
	char tmp_filename[FILENAME_LENGTH_MAX];
	char *fd_tmp;
	char *un_tmp;
	char *ex_tmp;

        //delim = "._";
	strcpy(tmp_filename, filename);
	
	un_tmp = strtok(tmp_filename, delim);
	fd_tmp = strtok(NULL, delim);
	ex_tmp = strtok(NULL, delim);
	
	if ((ex_tmp == NULL) || ((strcmp(ex_tmp, "xml")) && (strcmp(ex_tmp, "XML")))) {
		LOG_E(OCG, "Please use .xml file for configuration with the format \"user_name.file_date.xml\"\nfile_date = \"year month day hour minute second\" without space, \ne.g. 20100201193045 represents in the year 2010, February 1st, 19:30:45\n");
		return MODULE_ERROR;
	} else {
		strcpy(file_date, fd_tmp);
		strcpy(user_name, un_tmp);
		LOG_I(OCG, "File name is parsed as user_name = %s, file_date = %s\n", user_name, file_date);
		return MODULE_OK;
	}
}
