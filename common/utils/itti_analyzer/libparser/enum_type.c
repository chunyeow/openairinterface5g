#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "enum_type.h"
#include "ui_interface.h"

char *enum_type_get_name_from_value(struct types_s *type, uint32_t value)
{
    char    *enum_name = "UNKNOWN";
    types_t *enum_value;

    /* Loop on eache enumeration values */
    for (enum_value = type->child; enum_value; enum_value = enum_value->next) {
        if (value == enum_value->init_value) {
            enum_name = enum_value->name;
            break;
        }
    }
    return enum_name;
}

int enum_type_dissect_from_buffer(
    struct types_s *type, buffer_t *buffer, uint32_t offset,
    uint32_t parent_offset, int indent)
{
    uint32_t value = 0;
    types_t *values;

    DISPLAY_PARSE_INFO("enum", type->name, offset, parent_offset);

    value = buffer_get_uint32_t(buffer, parent_offset + offset);

//     if (type->name) {
//         INDENTED(stdout, indent,   fprintf(stdout, "<%s>\n", type->name));
//     }
    for (values = type->child; values; values = values->next) {
        if (value == values->init_value) {
            values->type_dissect_from_buffer(
                values, buffer, offset, parent_offset,
                type->name == NULL ? indent: indent+4);
            break;
        }
    }
    if (values == NULL) {
//         INDENTED(stdout, indent+4, fprintf(stdout, "<UNKNOWN/>\n"));
        int length = 0;
        char cbuf[50];
        char *cpy = NULL;

        length = sprintf(cbuf, "(0x%08x) UNKNOWN;\n", value);
        cpy = malloc(sizeof(char) * length);
        memcpy(cpy, cbuf, length);
        ui_interface.ui_signal_set_text(cpy, length);
        if (cpy)
            free(cpy);
    }
//     if (type->name) {
//         INDENTED(stdout, indent,   fprintf(stdout, "</%s>\n", type->name));
//     }

    return 0;
}

int enum_type_file_print(struct types_s *type, int indent, FILE *file)
{
    types_t *values;

    if (type == NULL)
        return -1;
    INDENTED(file, indent,   fprintf(file, "<Enumeration>\n"));
    INDENTED(file, indent+4, fprintf(file, "Name .......: %s\n", type->name));
    INDENTED(file, indent+4, fprintf(file, "Size .......: %d\n", type->size));
    INDENTED(file, indent+4, fprintf(file, "Align ......: %d\n", type->align));
    INDENTED(file, indent+4, fprintf(file, "Artificial .: %d\n", type->artificial));
    INDENTED(file, indent+4, fprintf(file, "File .......: %s\n", type->file));
    INDENTED(file, indent+4, fprintf(file, "Line .......: %d\n", type->line));
    if (type->file_ref != NULL)
        type->file_ref->type_file_print(type->file_ref, indent+4, file);
    /* Call enum values display */
    for (values = type->child; values; values = values->next) {
        values->type_file_print(values, indent + 4, file);
    }
    INDENTED(file, indent, fprintf(file, "</Enumeration>\n"));

    return 0;
}

int enum_type_hr_display(struct types_s *type, int indent)
{
    types_t *values;

    if (type == NULL)
        return -1;
    INDENTED(stdout, indent,   printf("<Enumeration>\n"));
    INDENTED(stdout, indent+4, printf("Name .......: %s\n", type->name));
    INDENTED(stdout, indent+4, printf("Size .......: %d\n", type->size));
    INDENTED(stdout, indent+4, printf("Align ......: %d\n", type->align));
    INDENTED(stdout, indent+4, printf("Artificial .: %d\n", type->artificial));
    INDENTED(stdout, indent+4, printf("File .......: %s\n", type->file));
    INDENTED(stdout, indent+4, printf("Line .......: %d\n", type->line));
    if (type->file_ref != NULL)
        type->file_ref->type_hr_display(type->file_ref, indent+4);
    /* Call enum values display */
    for (values = type->child; values; values = values->next) {
        values->type_hr_display(values, indent + 4);
    }
    INDENTED(stdout, indent, printf("</Enumeration>\n"));

    return 0;
}
