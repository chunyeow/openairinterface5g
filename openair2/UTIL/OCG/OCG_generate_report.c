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

/*! \file OCG_generate_report.c
* \brief Generate a brief report for debug of OCG
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
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include "OCG_vars.h"
#include "OCG_generate_report.h"
#include "UTIL/LOG/log.h"

/*----------------------------------------------------------------------------*/

	
int generate_report(char dst_dir[DIR_LENGTH_MAX], char filename[FILENAME_LENGTH_MAX]) {
	// for the xml writer, refer to http://xmlsoft.org/html/libxml-xmlwriter.html 
  
  char dst_file[FILENAME_LENGTH_MAX + DIR_LENGTH_MAX] = "";
  strcat(dst_file, dst_dir);
  strcat(dst_file, filename);
	
  xmlTextWriterPtr writer;
  
  writer = xmlNewTextWriterFilename(dst_file, 0);
	
  // set the output format of the XML file 
  xmlTextWriterSetIndent(writer, 1);
  xmlTextWriterSetIndentString(writer,(unsigned char*) "	");
  
  xmlTextWriterStartDocument(writer, NULL, NULL, NULL);
  
  /* Write an element named "X_ORDER_ID" as child of HEADER. */
  xmlTextWriterWriteFormatElement(writer,(unsigned char*) "COMMENT           ", "	in this output file, %d means NOT_PROCESSED; %d means NO_FILE; %d means ERROR; %d means OK	", MODULE_NOT_PROCESSED, NO_FILE, MODULE_ERROR, MODULE_OK);
  xmlTextWriterWriteFormatElement(writer,(unsigned char*) "OCG_GET_OPT       ", "	%d	", get_opt_OK);
  xmlTextWriterWriteFormatElement(writer,(unsigned char*) "OCG_DETECT_FILE   ", "	%d	", detect_file_OK);
  xmlTextWriterWriteFormatElement(writer,(unsigned char*) "OCG_PARSE_FILENAME", "	%d	", parse_filename_OK);
  xmlTextWriterWriteFormatElement(writer,(unsigned char*) "OCG_CREATE_DIR    ", "	%d	", create_dir_OK);
  xmlTextWriterWriteFormatElement(writer,(unsigned char*) "OCG_PARSE_XML     ", "	%d	", parse_XML_OK);
  xmlTextWriterWriteFormatElement(writer,(unsigned char*) "OCG_SAVE_XML      ", "	%d	", save_XML_OK);
  //	xmlTextWriterWriteFormatElement(writer, "OCG_CALL_EMU      ", "	%d	", call_emu_OK);
  
  xmlTextWriterEndDocument(writer);
  
  xmlFreeTextWriter(writer);
  
  LOG_I(OCG, "A report of OCG is generated in directory \"%s\"\n\n", dst_dir);
  return MODULE_OK;
}
