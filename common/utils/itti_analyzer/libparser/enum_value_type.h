#include "types.h"

#ifndef ENUM_VALUE_TYPE_H_
#define ENUM_VALUE_TYPE_H_

int enum_value_dissect_from_buffer(
    struct types_s *type, ui_set_signal_text_cb_t ui_set_signal_text_cb, gpointer user_data,
    buffer_t *buffer, uint32_t offset, uint32_t parent_offset, int indent);

int enum_value_file_print(struct types_s *type, int indent, FILE *file);

int enum_value_type_hr_display(struct types_s *type, int indent);

#endif /* ENUM_VALUE_TYPE_H_ */
