#include <stdio.h>
#include <string.h>

#include <glib.h>

#include "rc.h"

#include "types.h"
#include "locate_root.h"

int locate_root(const char *root_name, types_t *head, types_t **root) {
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
    if (!root) {
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
    *root = next_type;
    return (next_type == NULL) ? -2 : 0;
}

int locate_type(const char *type_name, types_t *head, types_t **type) {
    types_t *next_type;

    /* The root element is for example : MessageDef.
     * This element is the entry for other sub-types.
     */
    if (!type_name) {
        g_warning("FATAL: no root element name provided");
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
    types_t *type_message_id;

    if (!head || !message_id || !buffer)
        return RC_BAD_PARAM;

    CHECK_FCT(locate_type("messageId", head, &type_message_id));

    /* MessageId is an offset from start of buffer */
    value = buffer_get_uint32_t (buffer, type_message_id->offset);

    *message_id = value;
    return RC_OK;
}
