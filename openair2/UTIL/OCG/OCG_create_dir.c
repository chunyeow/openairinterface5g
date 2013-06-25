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

/*! \file OCG_create_dir.c
* \brief Create directory for current emulation
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
#include <sys/stat.h>
#include "OCG_vars.h"
#include "OCG_create_dir.h"
#include "UTIL/LOG/log.h"
/*----------------------------------------------------------------------------*/

int create_dir(char output_dir[DIR_LENGTH_MAX], char user_name[FILENAME_LENGTH_MAX / 2], char file_date[FILENAME_LENGTH_MAX / 2]) {

	char directory[FILENAME_LENGTH_MAX + DIR_LENGTH_MAX] = "";
	mode_t process_mask = umask(0);

	strcpy(directory, output_dir);

	struct stat st;
	if(stat(directory, &st) != 0) { // if output_dir does not exist, we create it here
		mkdir(directory, S_IRWXU | S_IRWXG | S_IRWXO);
		LOG_I(OCG, "output_dir %s is created", directory);
	}

	strcat(directory, user_name);

	mkdir(directory, S_IRWXU | S_IRWXG | S_IRWXO);

	strcat(directory, "/");
	strcat(directory, file_date);

	mkdir(directory, S_IRWXU | S_IRWXG |S_IRWXO);

	//char directory_extension[FILENAME_LENGTH_MAX + DIR_LENGTH_MAX + 64] = "";
	/*strcpy(directory_extension, directory); // to create some more folders
	strcat(directory_extension, "/LOGS");
	mkdir(directory_extension, S_IRWXU | S_IRWXG |S_IRWXO);

	strcpy(directory_extension, directory);
	strcat(directory_extension, "/PACKET_TRACE");
	mkdir(directory_extension, S_IRWXU | S_IRWXG |S_IRWXO);

	strcpy(directory_extension, directory);
	strcat(directory_extension, "/SCENARIO");
	mkdir(directory_extension, S_IRWXU | S_IRWXG |S_IRWXO);

	strcpy(directory_extension, directory);
	strcat(directory_extension, "/SCENARIO/XML");
	mkdir(directory_extension, S_IRWXU | S_IRWXG |S_IRWXO);

	strcpy(directory_extension, directory);
	strcat(directory_extension, "/SCENARIO/STATE");
	mkdir(directory_extension, S_IRWXU | S_IRWXG |S_IRWXO);
*/
	umask(process_mask);

	LOG_I(OCG, "Directory for current emulation is created\n");
	return MODULE_OK;
	
}
