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

/*! \file OCG_save_XML.c
* \brief Save the XML configuration file in the created directory for current emulation
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
#include "OCG_save_XML.h"
#include "UTIL/LOG/log.h"
/*----------------------------------------------------------------------------*/
	
//int save_XML(int copy_or_move, char src_file[FILENAME_LENGTH_MAX + DIR_LENGTH_MAX], char dst_dir[DIR_LENGTH_MAX], char filename[FILENAME_LENGTH_MAX]) {
int save_XML(int copy_or_move, char *src_file, char *output_dir, char *filename) {

	FILE *fs, *ft;
	char ch;
	char dst_file[FILENAME_LENGTH_MAX + DIR_LENGTH_MAX + 32] = "";
	char XML_saving_dir[FILENAME_LENGTH_MAX + DIR_LENGTH_MAX + 32] = "";
	
	strcpy(dst_file, output_dir);
	//strcat(dst_file, "SCENARIO/XML/");
	strcpy(XML_saving_dir, dst_file);
	strcat(dst_file, filename);
	fs = fopen(src_file, "r");
	ft = fopen(dst_file, "w");
	if ((ft !=NULL)&&(fs!=NULL)){
	  while(1) {
	    ch = getc(fs);
	    if(ch == EOF) {
	      break;
	    } else {
	      putc(ch, ft);
	    }
	  }

	  fclose(fs);
	  fclose(ft);
	}
	if (copy_or_move == 2) remove(src_file);
	
	LOG_I(OCG, "The file is saved in directory \"%s\"\n", XML_saving_dir);
	return MODULE_OK;
}
