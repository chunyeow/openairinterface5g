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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define G_LOG_DOMAIN ("PARSER")

#include "fundamental_type.h"
#include "ui_interface.h"

uint64_t fundamental_read_from_buffer(types_t *type, buffer_t *buffer, uint32_t offset, uint32_t parent_offset) {
    uint64_t value;

    switch (type->size) {
        case 8:
            value = buffer_get_uint8_t (buffer, offset + parent_offset);
            break;

        case 16:
            value = buffer_get_uint16_t (buffer, offset + parent_offset);
            break;

        case 32:
            value = buffer_get_uint32_t (buffer, offset + parent_offset);
            break;

        case 64:
            value = buffer_get_uint64_t (buffer, offset + parent_offset);
            break;

        default:
            /* ??? */
            value = 0;
            break;
    }
    return value;
}

int fundamental_dissect_from_buffer(
    types_t *type, ui_set_signal_text_cb_t ui_set_signal_text_cb, gpointer user_data,
    buffer_t *buffer, uint32_t offset, uint32_t parent_offset, int indent, gboolean new_line)
{
    int         length = 0;
    char        cbuf[200];
    int         type_unsigned;
    uint8_t     value8;
    uint16_t    value16;
    uint32_t    value32;
    uint64_t    value64;

    DISPLAY_PARSE_INFO("fundamental", type->name, offset, parent_offset);

    memset (cbuf, 0, 200);

    type_unsigned = strstr (type->name, "unsigned") == NULL ? 0 : 1;

    value64 = fundamental_read_from_buffer(type, buffer, offset, parent_offset);

    indent = new_line ? indent : 0;
    if (indent > 0)
    {
        DISPLAY_TYPE("Fun");
    }
    switch (type->size) {
        case 8:
            value8 = (uint8_t) value64;
            INDENTED_STRING(cbuf, indent,
                            length = sprintf(cbuf, type_unsigned ? "(0x%02" PRIx8 ") %3" PRIu8 ";\n" : "(0x%02" PRIx8 ") %4" PRId8 ";\n", value8, value8));
            break;

        case 16:
            value16 = (uint16_t) value64;
            INDENTED_STRING(cbuf, indent,
                            length = sprintf(cbuf, type_unsigned ? "(0x%04" PRIx16 ") %5" PRIu16 ";\n" : "(0x%04" PRIx16 ") %6" PRId16 ";\n", value16, value16));
            break;

        case 32:
            value32 = (uint32_t) value64;
            INDENTED_STRING(cbuf, indent,
                            length = sprintf(cbuf, type_unsigned ? "(0x%08" PRIx32 ") %9" PRIu32 ";\n" : "(0x%08" PRIx32 ") %10" PRId32 ";\n", value32, value32));
            break;

        case 64:
            INDENTED_STRING(cbuf, indent,
                            length = sprintf(cbuf, type_unsigned ? "(0x%016" PRIx64 ") %20" PRIu64 ";\n" : "(0x%016" PRIx64 ") %20" PRId64 ";\n", value64, value64));
            break;

        default:
            /* ??? */
            break;
    }
    ui_set_signal_text_cb(user_data, cbuf, length);

    return 0;
}

int fundamental_type_file_print(types_t *type, int indent, FILE *file) {
    if (type == NULL)
        return -1;
    INDENTED(file, indent, fprintf(file, "<Fundamental>\n"));
    INDENTED(file, indent+4, fprintf(file, "Name .......: %s\n", type->name));
    INDENTED(file, indent+4, fprintf(file, "Id .........: %d\n", type->id));
    INDENTED(file, indent+4, fprintf(file, "Size .......: %d\n", type->size));
    INDENTED(file, indent+4, fprintf(file, "Align ......: %d\n", type->align));
    if (type->file_ref != NULL)
        type->file_ref->type_file_print (type->file_ref, indent + 4, file);
    INDENTED(file, indent, fprintf(file, "</Fundamental>\n"));
    return 0;
}

int fundamental_type_hr_display(types_t *type, int indent) {
    if (type == NULL)
        return -1;
    INDENTED(stdout, indent, printf("<Fundamental>\n"));
    INDENTED(stdout, indent+4, printf("Name .......: %s\n", type->name));
    INDENTED(stdout, indent+4, printf("Id .........: %d\n", type->id));
    INDENTED(stdout, indent+4, printf("Size .......: %d\n", type->size));
    INDENTED(stdout, indent+4, printf("Align ......: %d\n", type->align));
    if (type->file_ref != NULL)
        type->file_ref->type_hr_display (type->file_ref, indent + 4);
    INDENTED(stdout, indent, printf("</Fundamental>\n"));
    return 0;
}
