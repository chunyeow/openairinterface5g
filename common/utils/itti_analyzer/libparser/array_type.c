#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "array_type.h"
#include "fundamental_type.h"
#include "ui_interface.h"

int array_dissect_from_buffer(
    struct types_s *type, ui_set_signal_text_cb_t ui_set_signal_text_cb, gpointer user_data,
    buffer_t *buffer, uint32_t offset, uint32_t parent_offset, int indent)
{
    struct types_s *type_child;

    DISPLAY_PARSE_INFO("array", type->name, offset, parent_offset);

    /* Ignore TYPEDEF children */
    for (type_child = type->child; type_child != NULL && type_child->type == TYPE_TYPEDEF;
            type_child = type_child->child) {
    }

    if (type->name) {
        INDENTED(stdout, indent, fprintf(stdout, "<%s>\n", type->name));
    }
    if (type->child != NULL) {
        int items = type->size / type_child->size;
        int i;
        int zero_counter = 0;

        /* Factorizes trailing 0 */
        if ((items > 1) && (type_child->type == TYPE_FUNDAMENTAL))
        {
            for (i = items - 1; i >= 0; i--)
            {
                if (fundamental_read_from_buffer(type_child, buffer, parent_offset, offset + i * type_child->size) == 0)
                {
                    zero_counter ++;
                }
                else
                {
                    break;
                }
            }
            /* Do not factorize if there is only one item */
            if (zero_counter <= 1)
            {
                zero_counter = 0;
            }
        }
        for (i = 0; i < (items - zero_counter); i++)
            type->child->type_dissect_from_buffer (
                type->child, ui_set_signal_text_cb, user_data, buffer, parent_offset,
                offset + i * type_child->size, type->name == NULL ? indent : indent + DISPLAY_TAB_SIZE);
        if (zero_counter > 0)
        {
            int  length = 0;
            char cbuf[50];

            INDENTED_STRING(cbuf, type->name == NULL ? indent : indent + DISPLAY_TAB_SIZE,);

            length = sprintf(cbuf, "[%d .. %d]  ", i, items -1);

            ui_set_signal_text_cb(user_data, cbuf, length);
            type->child->type_dissect_from_buffer (
                type->child, ui_set_signal_text_cb, user_data,
                buffer, parent_offset, offset + i * type_child->size, 0);
        }
    }
    if (type->name) {
        INDENTED(stdout, indent, fprintf(stdout, "</%s>\n", type->name));
    }

    return 0;
}

int array_type_file_print(struct types_s *type, int indent, FILE *file) {
    if (type == NULL)
        return -1;
    INDENTED(file, indent, fprintf(file, "<Array>\n"));
    INDENTED(file, indent+4, fprintf(file, "Id .........: %d\n", type->id));
    INDENTED(file, indent+4, fprintf(file, "Min ........: %d\n", type->min));
    INDENTED(file, indent+4, fprintf(file, "Max ........: %d\n", type->max));
    INDENTED(file, indent+4, fprintf(file, "Type .......: %d\n", type->type_xml));
    INDENTED(file, indent+4, fprintf(file, "Size .......: %d\n", type->size));
    INDENTED(file, indent+4, fprintf(file, "Align ......: %d\n", type->align));
    if (type->file_ref != NULL)
        type->file_ref->type_file_print (type->file_ref, indent + 4, file);
    if (type->child != NULL)
        type->child->type_file_print (type->child, indent + 4, file);
    INDENTED(file, indent, fprintf(file, "</Array>\n"));

    return 0;
}

int array_type_hr_display(struct types_s *type, int indent) {
    if (type == NULL)
        return -1;
    INDENTED(stdout, indent, printf("<Array>\n"));
    INDENTED(stdout, indent+4, printf("Id .........: %d\n", type->id));
    INDENTED(stdout, indent+4, printf("Min ........: %d\n", type->min));
    INDENTED(stdout, indent+4, printf("Max ........: %d\n", type->max));
    INDENTED(stdout, indent+4, printf("Type .......: %d\n", type->type_xml));
    INDENTED(stdout, indent+4, printf("Size .......: %d\n", type->size));
    INDENTED(stdout, indent+4, printf("Align ......: %d\n", type->align));
    if (type->file_ref != NULL)
        type->file_ref->type_hr_display (type->file_ref, indent + 4);
    if (type->child != NULL)
        type->child->type_hr_display (type->child, indent + 4);
    INDENTED(stdout, indent, printf("</Array>\n"));

    return 0;
}
