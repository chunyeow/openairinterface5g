#include "types.h"

#ifndef ARRAY_TYPE_H_
#define ARRAY_TYPE_H_

int array_dissect_from_buffer(struct types_s *type, buffer_t *buffer,
                              uint32_t offset, uint32_t parent_offset, int indent);

int array_type_hr_display(struct types_s *type, int indent);

int array_type_file_print(struct types_s *type, int indent, FILE *file);

#endif /* ARRAY_TYPE_H_ */
