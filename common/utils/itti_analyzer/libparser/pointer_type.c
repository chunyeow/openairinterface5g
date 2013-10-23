#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "pointer_type.h"
#include "ui_interface.h"

int pointer_dissect_from_buffer(
    struct types_s *type, ui_set_signal_text_cb_t ui_set_signal_text_cb, gpointer user_data,
    buffer_t *buffer, uint32_t offset, uint32_t parent_offset, int indent)
{
    int          length = 0;
    char         cbuf[200];
    uint32_t     value;

    DISPLAY_PARSE_INFO("pointer", type->name, offset, parent_offset);

    memset (cbuf, 0, 200);

    value = buffer_get_uint32_t (buffer, parent_offset + offset);

    DISPLAY_TYPE("Ptr");
    if (type->child->name) {
        /*
         INDENTED(stdout, indent, fprintf(stdout, "<%s>0x%08x</%s>\n",
         type->child->name, value, type->child->name));
         */
         INDENTED_STRING(cbuf, indent, sprintf(cbuf, "(%s *) 0x%08x;\n", type->child->name, value));
    }
    else {
        /*
         INDENTED(stdout, indent, fprintf(stdout, "<Pointer>0x%08x</Pointer>\n",
         value));
         */
        INDENTED_STRING(cbuf, indent, sprintf(cbuf, "(void *) 0x%08x;\n", value));
    }

    length = strlen (cbuf);

    ui_set_signal_text_cb(user_data, cbuf, length);

    return 0;
}

int pointer_type_file_print(struct types_s *type, int indent, FILE *file) {
    if (type == NULL)
        return -1;
    INDENTED(file, indent, fprintf(file, "<Pointer>\n"));
    INDENTED(file, indent+4, fprintf(file, "Id .........: %d\n", type->id));
    INDENTED(file, indent+4, fprintf(file, "Type .......: %d\n", type->type_xml));
    INDENTED(file, indent+4, fprintf(file, "Size .......: %d\n", type->size));
    INDENTED(file, indent+4, fprintf(file, "Align ......: %d\n", type->align));
    if (type->child != NULL)
        type->child->type_file_print (type->child, indent + 4, file);
    if (type->file_ref != NULL)
        type->file_ref->type_file_print (type->file_ref, indent + 4, file);
    INDENTED(file, indent, fprintf(file, "</Pointer>\n"));

    return 0;
}

int pointer_type_hr_display(struct types_s *type, int indent) {
    if (type == NULL)
        return -1;
    INDENTED(stdout, indent, printf("<Pointer>\n"));
    INDENTED(stdout, indent+4, printf("Id .........: %d\n", type->id));
    INDENTED(stdout, indent+4, printf("Type .......: %d\n", type->type_xml));
    INDENTED(stdout, indent+4, printf("Size .......: %d\n", type->size));
    INDENTED(stdout, indent+4, printf("Align ......: %d\n", type->align));
    if (type->child != NULL)
        type->child->type_hr_display (type->child, indent + 4);
    if (type->file_ref != NULL)
        type->file_ref->type_hr_display (type->file_ref, indent + 4);
    INDENTED(stdout, indent, printf("</Pointer>\n"));

    return 0;
}
