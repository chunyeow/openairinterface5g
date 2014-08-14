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
#include <assert.h>
#include <string.h>

#define G_LOG_DOMAIN ("PARSER")

#include "enum_value_type.h"
#include "ui_interface.h"

uint32_t last_enum_value;

int enum_value_dissect_from_buffer(
    types_t *type, ui_set_signal_text_cb_t ui_set_signal_text_cb, gpointer user_data,
    buffer_t *buffer, uint32_t offset, uint32_t parent_offset, int indent, gboolean new_line)
{
    types_t *type_parent = NULL;
    uint32_t value = 0;

    DISPLAY_PARSE_INFO("enum_value", type->name, offset, parent_offset);

    value = buffer_get_uint32_t(buffer, parent_offset + offset);
    if (type->name) {
        int length = 0;
        char cbuf[50 + strlen(type->name)];

        /* Search for enum variable name and check if it is used in association with a union type */
        {
            if (type->parent != NULL) {
                /* Ignore ENUMERATION and TYPEDEF parents */
                for (type_parent = type->parent; (type_parent != NULL) && ((type_parent->type == TYPE_ENUMERATION) || (type_parent->type == TYPE_TYPEDEF));
                        type_parent = type_parent->parent) {
                }
            }
            if ((type_parent != NULL) && (type_parent->name != NULL) && (strcmp(type_parent->name, "present") == 0))
            {
                /* ASN1 union, keep the "present" member */
                last_enum_value = value;
            }
            else
            {
                last_enum_value = 0;
            }
        }
        length = sprintf(cbuf, "(0x%08x) %s;\n", value, type->name);
        ui_set_signal_text_cb(user_data, cbuf, length);
    }

    return 0;
}

int enum_value_file_print(types_t *type, int indent, FILE *file)
{
    if (type == NULL)
        return -1;
    INDENTED(file, indent,   fprintf(file, "<Value>\n"));
    INDENTED(file, indent+4, fprintf(file, "Name .......: %s\n", type->name));
    INDENTED(file, indent+4, fprintf(file, "Value ......: %d\n", type->init_value));
    if (type->file_ref != NULL)
        type->file_ref->type_file_print(type->file_ref, indent+4, file);
    INDENTED(file, indent, fprintf(file, "</Value>\n"));

    return 0;
}

int enum_value_type_hr_display(types_t *type, int indent)
{
    if (type == NULL)
        return -1;
    INDENTED(stdout, indent,  printf("<Value>\n"));
    INDENTED(stdout, indent+4, printf("Name .......: %s\n", type->name));
    INDENTED(stdout, indent+4, printf("Value ......: %d\n", type->init_value));
    if (type->file_ref != NULL)
        type->file_ref->type_hr_display(type->file_ref, indent+4);
    INDENTED(stdout, indent, printf("</Value>\n"));

    return 0;
}
