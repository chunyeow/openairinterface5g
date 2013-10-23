#include "types.h"

#ifndef STRUCT_TYPE_H_
#define STRUCT_TYPE_H_

int struct_dissect_from_buffer(
    struct types_s *type, ui_set_signal_text_cb_t ui_set_signal_text_cb, gpointer user_data,
    buffer_t *buffer, uint32_t offset, uint32_t parent_offset, int indent);

int struct_type_hr_display(struct types_s *type, int indent);

int struct_type_file_print(struct types_s *type, int indent, FILE *file);

#endif /* STRUCT_TYPE_H_ */
