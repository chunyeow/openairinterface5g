#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "enum_value_type.h"
#include "ui_interface.h"

int enum_value_dissect_from_buffer(
    struct types_s *type, buffer_t *buffer, uint32_t offset,
    uint32_t parent_offset, int indent)
{
    uint32_t value = 0;

    DISPLAY_PARSE_INFO("enum_value", type->name, offset, parent_offset);

    value = buffer_get_uint32_t(buffer, parent_offset + offset);
    if (type->name) {
        int length = 0;
        char cbuf[50 + strlen(type->name)];
        char *cpy = NULL;

        sprintf(cbuf, "(0x%08x)  %s;\n", value, type->name);
        length = strlen(cbuf);
        cpy = malloc(sizeof(char) * length);
        memcpy(cpy, cbuf, length);
        ui_interface.ui_signal_set_text(cpy, length);
        if (cpy)
            free(cpy);
    }

    return 0;
}

int enum_value_file_print(struct types_s *type, int indent, FILE *file)
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

int enum_value_type_hr_display(struct types_s *type, int indent)
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
