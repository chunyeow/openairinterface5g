#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "rc.h"

#include "struct_type.h"
#include "buffers.h"
#include "ui_interface.h"

int struct_dissect_from_buffer(struct types_s *type, buffer_t *buffer, uint32_t offset, uint32_t parent_offset,
                               int indent) {
    int i;
    int length = 0;
    char cbuf[200];
    char *cpy = NULL;

    DISPLAY_PARSE_INFO("structure", type->name, offset, parent_offset);

    memset (cbuf, 0, 200);

    if (type->name) {
        DISPLAY_TYPE("Str");
        INDENTED_STRING(cbuf, indent, sprintf (cbuf, "%s =%s\n", type->name, "" DISPLAY_BRACE(" {")));
    }

    length = strlen (cbuf);
    cpy = malloc (length * sizeof(char));
    memcpy (cpy, cbuf, length);
    ui_interface.ui_signal_set_text (cpy, length);

    if (cpy)
        free (cpy);

    for (i = 0; i < type->nb_members; i++) {
        if (type->members_child[i] != NULL)
            type->members_child[i]->type_dissect_from_buffer (type->members_child[i], buffer, offset, parent_offset,
                                                              type->name == NULL ? indent : indent + 4);
    }

    DISPLAY_BRACE(
            if (type->name) {
                DISPLAY_TYPE("Str");
                INDENTED_STRING(cbuf, indent, sprintf(cbuf, "};\n"));
            }
            length = strlen (cbuf);
            cpy = malloc (length * sizeof(char));
            memcpy (cpy, cbuf, length);
            ui_interface.ui_signal_set_text (cpy, length);
            if (cpy)
                free (cpy);)

    return 0;
}

int struct_type_file_print(struct types_s *type, int indent, FILE *file) {
    int i;
    if (type == NULL)
        return -1;
    if (file == NULL)
        return -1;
    INDENTED(file, indent, fprintf(file, "<Struct>\n"));
    INDENTED(file, indent+4, fprintf(file, "Name .......: %s\n", type->name));
    INDENTED(file, indent+4, fprintf(file, "Id .........: %d\n", type->id));
    INDENTED(file, indent+4, fprintf(file, "Size .......: %d\n", type->size));
    INDENTED(file, indent+4, fprintf(file, "Align ......: %d\n", type->align));
    INDENTED(file, indent+4, fprintf(file, "Artificial .: %d\n", type->artificial));
    INDENTED(file, indent+4, fprintf(file, "File .......: %s\n", type->file));
    INDENTED(file, indent+4, fprintf(file, "Line .......: %d\n", type->line));
    INDENTED(file, indent+4, fprintf(file, "Members ....: %s\n", type->members));
    INDENTED(file, indent+4, fprintf(file, "Mangled ....: %s\n", type->mangled));
    INDENTED(file, indent+4, fprintf(file, "Demangled ..: %s\n", type->demangled));
    if (type->file_ref != NULL)
        type->file_ref->type_file_print (type->file_ref, indent + 4, file);
    for (i = 0; i < type->nb_members; i++) {
        if (type->members_child[i] != NULL)
            type->members_child[i]->type_file_print (type->members_child[i], indent + 4, file);
    }
    INDENTED(file, indent, fprintf(file, "</Struct>\n"));

    return 0;
}

int struct_type_hr_display(struct types_s *type, int indent) {
    int i;
    if (type == NULL)
        return -1;
    INDENTED(stdout, indent, printf("<Struct>\n"));
    INDENTED(stdout, indent+4, printf("Name .......: %s\n", type->name));
    INDENTED(stdout, indent+4, printf("Id .........: %d\n", type->id));
    INDENTED(stdout, indent+4, printf("Size .......: %d\n", type->size));
    INDENTED(stdout, indent+4, printf("Align ......: %d\n", type->align));
    INDENTED(stdout, indent+4, printf("Artificial .: %d\n", type->artificial));
    INDENTED(stdout, indent+4, printf("File .......: %s\n", type->file));
    INDENTED(stdout, indent+4, printf("Line .......: %d\n", type->line));
    INDENTED(stdout, indent+4, printf("Members ....: %s\n", type->members));
    INDENTED(stdout, indent+4, printf("Mangled ....: %s\n", type->mangled));
    INDENTED(stdout, indent+4, printf("Demangled ..: %s\n", type->demangled));
    if (type->file_ref != NULL)
        type->file_ref->type_hr_display (type->file_ref, indent + 4);
    for (i = 0; i < type->nb_members; i++) {
        if (type->members_child[i] != NULL)
            type->members_child[i]->type_hr_display (type->members_child[i], indent + 4);
    }
    INDENTED(stdout, indent, printf("</Struct>\n"));

    return 0;
}
