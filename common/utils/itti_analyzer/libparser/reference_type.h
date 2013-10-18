#include "types.h"

#ifndef REFERENCE_TYPE_H_
#define REFERENCE_TYPE_H_

int reference_dissect_from_buffer(struct types_s *type, buffer_t *buffer,
                                  uint32_t offset, uint32_t parent_offset, int indent);

int reference_type_file_print(struct types_s *type, int indent, FILE *file);

int reference_type_hr_display(struct types_s *type, int indent);

#endif /* REFERENCE_TYPE_H_ */
