#include <stdio.h>
#include <string.h>

#include "types.h"

int resolve_typedefs(types_t **head)
{
    types_t *next_type;

    if (!head) {
        printf("Empty list detected\n");
        return -1;
    }

    for (next_type = head; next_type; next_type = next_type->next)
    {
        /* Only resolve typedef */
        if (next_type->type != TYPE_TYPEDEF)
            continue;

        printf("Trying to resolve typedef %s\n", next_type->name);
    }

    return 0;
}
