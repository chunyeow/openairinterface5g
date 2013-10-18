#ifndef LOCATE_ROOT_H_
#define LOCATE_ROOT_H_

int locate_root(const char *root_name, types_t *head, types_t **root);

int locate_type(const char *type_name, types_t *head, types_t **type);

int get_message_id(types_t *head, buffer_t *buffer, uint32_t *message_id);

#endif /* LOCATE_ROOT_H_ */
