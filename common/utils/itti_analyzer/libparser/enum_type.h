#include "types.h"

#ifndef ENUM_TYPE_H_
#define ENUM_TYPE_H_

int enum_type_dissect_from_buffer(
    struct types_s *type, buffer_t *buffer, uint32_t offset,
    uint32_t parent_offset, int indent);

int enum_type_file_print(struct types_s *type, int indent, FILE *file);

int enum_type_hr_display(struct types_s *type, int indent);

#endif /* ENUM_TYPE_H_ */
