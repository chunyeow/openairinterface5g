#include <stdio.h>
#include <string.h>

#include <glib.h>

#include "rc.h"

#include "types.h"
#include "enum_type.h"
#include "locate_root.h"
#include "xml_parse.h"

types_t *messages_id_enum = NULL;
types_t *origin_task_id_type = NULL;
types_t *destination_task_id_type = NULL;

int locate_root(const char *root_name, types_t *head, types_t **root_elm) {
    types_t *next_type;

    /* The root element is for example : MessageDef.
     * This element is the entry for other sub-types.
     */
    if (!root_name || (strlen (root_name) == 0)) {
        g_warning("FATAL: no root element name provided");
        return -1;
    }
    if (!head) {
        g_warning("Empty list detected");
        return -1;
    }
    if (!root_elm) {
        g_warning("NULL root reference");
        return -1;
    }

    for (next_type = head; next_type; next_type = next_type->next) {
        if (next_type->name == NULL)
            continue;
        if (strcmp (root_name, next_type->name) == 0) {
            /* Matching reference */
            break;
        }
    }
    *root_elm = next_type;
    return (next_type == NULL) ? -2 : 0;
}

int locate_type(const char *type_name, types_t *head, types_t **type) {
    types_t *next_type;

    /* The root element is for example : MessageDef.
     * This element is the entry for other sub-types.
     */
    if (!type_name) {
        g_warning("FATAL: no element name provided");
        return RC_BAD_PARAM;
    }
    if (!head) {
        g_warning("Empty list detected");
        return RC_BAD_PARAM;
    }

    for (next_type = head; next_type; next_type = next_type->next) {
        if (next_type->name == NULL)
            continue;
        if (strcmp (type_name, next_type->name) == 0) {
            /* Matching reference */
            break;
        }
    }
    if (type)
        *type = next_type;
    return (next_type == NULL) ? RC_FAIL : RC_OK;
}

int get_message_id(types_t *head, buffer_t *buffer, uint32_t *message_id) {
    uint32_t value;

    g_assert(message_id != NULL);
    g_assert(buffer != NULL);

    /* MessageId is an offset from start of buffer */
    value = buffer_get_uint32_t(buffer, messages_id_enum->offset);

    *message_id = value;
    return RC_OK;
}

char *get_origin_task_id(buffer_t *buffer) {
    char     *origin_task_id = "UNKNOWN";
    uint32_t  origin_task_id_value;

    /* Fetch task id value */
    if (buffer_fetch_bits(buffer, origin_task_id_type->offset,
        origin_task_id_type->child->size, &origin_task_id_value) == RC_OK) {
        origin_task_id = enum_type_get_name_from_value(origin_task_id_type->child,
                                                       origin_task_id_value);
    }

    return origin_task_id;
}

char *get_destination_task_id(buffer_t *buffer) {
    char     *destination_task_id = "UNKNOWN";
    uint32_t  destination_task_id_value;

    /* Fetch task id value */
    if (buffer_fetch_bits(buffer, destination_task_id_type->offset,
        destination_task_id_type->child->size, &destination_task_id_value) == RC_OK) {
        destination_task_id = enum_type_get_name_from_value(destination_task_id_type->child,
                                                            destination_task_id_value);
    }

    return destination_task_id;
}

char *message_id_to_string(uint32_t message_id)
{
    return enum_type_get_name_from_value(messages_id_enum, message_id);
}
