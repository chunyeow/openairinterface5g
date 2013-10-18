#include "types.h"

#ifndef UNION_TYPE_H_
#define UNION_TYPE_H_

int union_msg_dissect_from_buffer(struct types_s *type, buffer_t *buffer,
                                  uint32_t offset, uint32_t parent_offset, int indent);

int union_dissect_from_buffer(struct types_s *type, buffer_t *buffer,
                              uint32_t offset, uint32_t parent_offset, int indent);

int union_type_file_print(struct types_s *type, int indent, FILE *file);

int union_type_hr_display(struct types_s *type, int indent);

#endif /* UNION_TYPE_H_ */
