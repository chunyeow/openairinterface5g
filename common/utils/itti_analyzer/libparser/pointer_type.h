#include "types.h"

#ifndef POINTER_TYPE_H_
#define POINTER_TYPE_H_

int pointer_dissect_from_buffer(
    struct types_s *type, ui_set_signal_text_cb_t ui_set_signal_text_cb, gpointer user_data,
    buffer_t *buffer, uint32_t offset, uint32_t parent_offset, int indent);

int pointer_type_file_print(struct types_s *type, int indent, FILE *file);

int pointer_type_hr_display(struct types_s *type, int indent);

#endif /* POINTER_TYPE_H_ */
