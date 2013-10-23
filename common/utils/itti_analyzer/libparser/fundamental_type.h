#include "types.h"

#ifndef FUNDAMENTAL_TYPE_H_
#define FUNDAMENTAL_TYPE_H_

uint32_t fundamental_read_from_buffer(struct types_s *type, buffer_t *buffer, uint32_t offset, uint32_t parent_offset);

int fundamental_dissect_from_buffer(
    struct types_s *type, ui_set_signal_text_cb_t ui_set_signal_text_cb, gpointer user_data,
    buffer_t *buffer, uint32_t offset, uint32_t parent_offset, int indent);

int fundamental_type_file_print(struct types_s *type, int indent, FILE *file);

int fundamental_type_hr_display(struct types_s *type, int indent);

#endif /* FUNDAMENTAL_TYPE_H_ */
