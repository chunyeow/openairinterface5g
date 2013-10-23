#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "fundamental_type.h"
#include "ui_interface.h"

uint32_t fundamental_read_from_buffer(struct types_s *type, buffer_t *buffer, uint32_t offset, uint32_t parent_offset) {
    uint32_t value;

    switch (type->size) {
        case 8: {
            value = buffer_get_uint8_t (buffer, offset + parent_offset);
        }
            break;
        case 16: {
            value = buffer_get_uint16_t (buffer, offset + parent_offset);
        }
            break;
        case 32: {
            value = buffer_get_uint32_t (buffer, offset + parent_offset);
        }
            break;
        default:
            /* ??? */
            value = 0;
            break;
    }
    return value;
}

int fundamental_dissect_from_buffer(
    struct types_s *type, ui_set_signal_text_cb_t ui_set_signal_text_cb, gpointer user_data,
    buffer_t *buffer, uint32_t offset, uint32_t parent_offset, int indent)
{
    int         length = 0;
    char        cbuf[200];
    int         type_unsigned;
    uint32_t    value;

    DISPLAY_PARSE_INFO("fundamental", type->name, offset, parent_offset);

    memset (cbuf, 0, 200);

    type_unsigned = strstr (type->name, "unsigned") == NULL ? 0 : 1;

    value = fundamental_read_from_buffer(type, buffer, offset, parent_offset);

    if (indent > 0)
    {
        DISPLAY_TYPE("Fun");
    }
    switch (type->size) {
        case 8: {
            INDENTED_STRING(cbuf, indent,
                            sprintf(cbuf, type_unsigned ? "(0x%02x)  %3u  '%c';\n" : "(0x%02x) %4d  '%c';\n", value, value, isprint(value) ? value : '.'));
        }
            break;
        case 16: {
            INDENTED_STRING(cbuf, indent,
                            sprintf(cbuf, type_unsigned ? "(0x%04x)  %5u;\n" : "(0x%04x)  %6d;\n", value, value));
        }
            break;
        case 32: {
            INDENTED_STRING(cbuf, indent,
                            sprintf(cbuf, type_unsigned ? "(0x%08x)  %9u;\n" : "(0x%08x)  %10d;\n", value, value));
        }
            break;
        default:
            /* ??? */
            break;
    }

    length = strlen (cbuf);

    ui_set_signal_text_cb(user_data, cbuf, length);

    return 0;
}

int fundamental_type_file_print(struct types_s *type, int indent, FILE *file) {
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

int fundamental_type_hr_display(struct types_s *type, int indent) {
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
