#include <string.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "types.h"
#include "xml_parse.h"
#include "union_type.h"

#include "ui_interface.h"
#include "ui_main_screen.h"
#include "ui_notif_dlg.h"
#include "ui_filters.h"

#include "../libresolver/locate_root.h"
#include "../libresolver/resolvers.h"

extern int debug_parser;

#if (ENABLE_DISPLAY_PARSE_INFO != 0)
# define INDENT_START 30
#else
# define INDENT_START 0
#endif

#define PARSER_DEBUG(fmt, args...)       \
do {                                     \
    if (debug_parser)                    \
        g_debug("WARNING: "fmt, ##args); \
} while(0)

#define PARSER_ERROR(fmt, args...)      \
do {                                    \
    g_error("FATAL: "fmt, ##args);      \
} while(0)

types_t *root = NULL;

static int xml_parse_doc(xmlDocPtr doc);

static int parse_attribute_name(xmlNode *node, types_t *type, int mandatory) {
    xmlAttrPtr node_attribute;
    if ((node_attribute = xmlHasProp (node, (xmlChar *) "name")) == NULL) {
        if (mandatory) {
            PARSER_ERROR("cannot retrieve name attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            return -1;
        }
        else {
            PARSER_DEBUG("cannot retrieve name attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            return 0;
        }
    }
    type->name = strdup ((char *) node_attribute->children->content);
    return 0;
}

static int parse_attribute_id(xmlNode *node, types_t *type) {
    xmlAttrPtr node_attribute;
    if ((node_attribute = xmlHasProp (node, (xmlChar *) "id")) == NULL) {
        PARSER_ERROR("cannot retrieve id attribute in node %s, %s:%ld",
                     (char *)node->name, node->doc->URL, XML_GET_LINE(node));
        return -1;
    }
    type->id = atoi ((char *) &node_attribute->children->content[1]);
    return 0;
}

static int parse_attribute_size(xmlNode *node, types_t *type, int mandatory) {
    xmlAttrPtr node_attribute;
    if ((node_attribute = xmlHasProp (node, (xmlChar *) "size")) == NULL) {
        if (mandatory) {
            PARSER_ERROR("cannot retrieve size attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->size = -1;
            return -1;
        }
        else {
            PARSER_DEBUG("cannot retrieve size attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->size = -1;
            return 0;
        }
    }
    type->size = atoi ((char *) node_attribute->children->content);
    return 0;
}

static int parse_attribute_align(xmlNode *node, types_t *type, int mandatory) {
    xmlAttrPtr node_attribute;
    if ((node_attribute = xmlHasProp (node, (xmlChar *) "align")) == NULL) {
        if (mandatory) {
            PARSER_ERROR("cannot retrieve align attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->align = -1;
            return -1;
        }
        else {
            PARSER_DEBUG("cannot retrieve align attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->align = -1;
            return 0;
        }
    }
    type->align = atoi ((char *) node_attribute->children->content);
    return 0;
}

static int parse_attribute_line(xmlNode *node, types_t *type) {
    xmlAttrPtr node_attribute;
    if ((node_attribute = xmlHasProp (node, (xmlChar *) "line")) == NULL) {
        type->line = -1;
        return 0;
    }
    type->line = atoi ((char *) node_attribute->children->content);
    return 0;
}

static int parse_attribute_file(xmlNode *node, types_t *type) {
    xmlAttrPtr node_attribute;
    if ((node_attribute = xmlHasProp (node, (xmlChar *) "file")) == NULL) {
        type->file = NULL;
        return 0;
    }
    type->file = strdup ((char *) node_attribute->children->content);
    return 0;
}

static int parse_attribute_context(xmlNode *node, types_t *type, int mandatory) {
    xmlAttrPtr node_attribute;
    if ((node_attribute = xmlHasProp (node, (xmlChar *) "context")) == NULL) {
        if (mandatory) {
            PARSER_ERROR("cannot retrieve context attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->context = -1;
            return -1;
        }
        else {
            type->context = -1;
            return 0;
        }
    }
    type->context = atoi ((char *) &node_attribute->children->content[1]);
    return 0;
}

static int parse_attribute_artificial(xmlNode *node, types_t *type) {
    xmlAttrPtr node_attribute;
    if ((node_attribute = xmlHasProp (node, (xmlChar *) "artificial")) == NULL) {
        PARSER_DEBUG("cannot retrieve artificial attribute in node %s, %s:%ld",
                     (char *)node->name, node->doc->URL, XML_GET_LINE(node));
        type->artificial = -1;
        return 0;
    }
    type->artificial = atoi ((char *) &node_attribute->children->content[1]);
    return 0;
}

static int parse_attribute_init(xmlNode *node, types_t *type) {
    xmlAttrPtr node_attribute;
    if ((node_attribute = xmlHasProp (node, (xmlChar *) "init")) == NULL) {
        PARSER_ERROR("cannot retrieve init attribute in node %s, %s:%ld",
                     (char *)node->name, node->doc->URL, XML_GET_LINE(node));
        type->init_value = -1;
        return 0;
    }
    type->init_value = atoi ((char *) node_attribute->children->content);
    return 0;
}

static int parse_attribute_members(xmlNode *node, types_t *type, int mandatory) {
    xmlAttrPtr node_attribute;
    if ((node_attribute = xmlHasProp (node, (xmlChar *) "members")) == NULL) {
        if (mandatory) {
            PARSER_ERROR("cannot retrieve members attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->members = 0;
            return -1;
        }
        else {
            PARSER_DEBUG("cannot retrieve members attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->members = 0;
            return 0;
        }
    }
    type->members = strdup ((char *) node_attribute->children->content);
    return 0;
}

static int parse_attribute_mangled(xmlNode *node, types_t *type, int mandatory) {
    xmlAttrPtr node_attribute;
    if ((node_attribute = xmlHasProp (node, (xmlChar *) "mangled")) == NULL) {
        if (mandatory) {
            PARSER_ERROR("cannot retrieve mangled attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->mangled = 0;
            return -1;
        }
        else {
            PARSER_DEBUG("cannot retrieve mangled attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->mangled = 0;
            return 0;
        }
    }
    type->mangled = strdup ((char *) node_attribute->children->content);
    return 0;
}

static int parse_attribute_demangled(xmlNode *node, types_t *type, int mandatory) {
    xmlAttrPtr node_attribute;
    if ((node_attribute = xmlHasProp (node, (xmlChar *) "mangled")) == NULL) {
        if (mandatory) {
            PARSER_ERROR("cannot retrieve demangled attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->demangled = 0;
            return -1;
        }
        else {
            PARSER_DEBUG("cannot retrieve demangled attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->demangled = 0;
            return 0;
        }
    }
    type->demangled = strdup ((char *) node_attribute->children->content);
    return 0;
}

static int parse_attribute_offset(xmlNode *node, types_t *type, int mandatory) {
    xmlAttrPtr node_attribute;
    if ((node_attribute = xmlHasProp (node, (xmlChar *) "offset")) == NULL) {
        if (mandatory) {
            PARSER_ERROR("cannot retrieve offset attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->offset = -1;
            return -1;
        }
        else {
            PARSER_DEBUG("cannot retrieve offset attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->offset = -1;
            return 0;
        }
    }
    type->offset = atoi ((char *) node_attribute->children->content);
    return 0;
}

static int parse_attribute_bits(xmlNode *node, types_t *type, int mandatory) {
    xmlAttrPtr node_attribute;
    if ((node_attribute = xmlHasProp (node, (xmlChar *) "bits")) == NULL) {
        if (mandatory) {
            PARSER_ERROR("cannot retrieve bits attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->bits = -1;
            return -1;
        }
        else {
            PARSER_DEBUG("cannot retrieve bits attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->bits = -1;
            return 0;
        }
    }
    type->bits = atoi ((char *) node_attribute->children->content);
    return 0;
}

static int parse_attribute_type(xmlNode *node, types_t *type, int mandatory) {
    xmlAttrPtr node_attribute;
    if ((node_attribute = xmlHasProp (node, (xmlChar *) "type")) == NULL) {
        if (mandatory) {
            PARSER_ERROR("cannot retrieve type attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->type_xml = 0;
            return -1;
        }
        else {
            PARSER_DEBUG("cannot retrieve type attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->type_xml = 0;
            return 0;
        }
    }
    type->type_xml = atoi ((char *) &node_attribute->children->content[1]);
    return 0;
}

static int parse_attribute_min(xmlNode *node, types_t *type, int mandatory) {
    xmlAttrPtr node_attribute;
    if ((node_attribute = xmlHasProp (node, (xmlChar *) "min")) == NULL) {
        if (mandatory) {
            PARSER_ERROR("cannot retrieve min attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->min = 0;
            return -1;
        }
        else {
            PARSER_DEBUG("cannot retrieve min attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->min = 0;
            return 0;
        }
    }
    type->min = atoi ((char *) node_attribute->children->content);
    return 0;
}

static int parse_attribute_max(xmlNode *node, types_t *type, int mandatory) {
    xmlAttrPtr node_attribute;
    if ((node_attribute = xmlHasProp (node, (xmlChar *) "max")) == NULL) {
        if (mandatory) {
            PARSER_ERROR("cannot retrieve max attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->max = 0;
            return -1;
        }
        else {
            PARSER_DEBUG("cannot retrieve max attribute in node %s, %s:%ld",
                         (char *)node->name, node->doc->URL, XML_GET_LINE(node));
            type->max = 0;
            return 0;
        }
    }
    type->max = atoi ((char *) &node_attribute->children->content[1]);
    return 0;
}

static int parse_enum_values(xmlNode *node, types_t *parent) {
    types_t *new;

    if (node == NULL || parent == NULL)
        return -1;

    new = type_new (TYPE_ENUMERATION_VALUE);

    CHECK_FCT(parse_attribute_name(node, new, 1));
    CHECK_FCT(parse_attribute_init(node, new));

    CHECK_FCT(types_insert_tail(&parent->child, new));

    return 0;
}

static int parse_enumeration(xmlNode *node, types_t **head) {
    types_t *new;
    xmlNode *enum_value_node;

    if (node == NULL)
        return -1;

    new = type_new (TYPE_ENUMERATION);

    CHECK_FCT(parse_attribute_name(node, new, 1));
    CHECK_FCT(parse_attribute_id(node, new));
    CHECK_FCT(parse_attribute_size(node, new, 1));
    CHECK_FCT(parse_attribute_align(node, new, 1));
    CHECK_FCT(parse_attribute_context(node, new, 0));
    CHECK_FCT(parse_attribute_line(node, new));
    CHECK_FCT(parse_attribute_file(node, new));
    CHECK_FCT(parse_attribute_artificial(node, new));

    CHECK_FCT(types_insert_tail(head, new));

    /* Parse enum values */
    for (enum_value_node = node->children; enum_value_node; enum_value_node = enum_value_node->next) {
        if (strcmp ((char *) enum_value_node->name, "EnumValue") == 0)
            CHECK_FCT(parse_enum_values(enum_value_node, new));
    }

    return 0;
}

static int parse_union(xmlNode *node, types_t **head) {
    types_t *new;

    if (node == NULL)
        return -1;

    new = type_new (TYPE_UNION);

    CHECK_FCT(parse_attribute_name(node, new, 0));
    CHECK_FCT(parse_attribute_id(node, new));
    CHECK_FCT(parse_attribute_size(node, new, 1));
    CHECK_FCT(parse_attribute_align(node, new, 1));
    CHECK_FCT(parse_attribute_context(node, new, 0));
    CHECK_FCT(parse_attribute_members(node, new, 0));
    CHECK_FCT(parse_attribute_line(node, new));
    CHECK_FCT(parse_attribute_file(node, new));
    CHECK_FCT(parse_attribute_artificial(node, new));
    CHECK_FCT(parse_attribute_mangled(node, new, 0));
    CHECK_FCT(parse_attribute_demangled(node, new, 0));

    if (new->name)
        if (strcmp (new->name, "msg_s") == 0)
            new->type_dissect_from_buffer = union_msg_dissect_from_buffer;

    CHECK_FCT(types_insert_tail(head, new));

    return 0;
}

static int parse_fundamental(xmlNode *node, types_t **head) {
    types_t *new;

    if (node == NULL)
        return -1;

    new = type_new (TYPE_FUNDAMENTAL);

    CHECK_FCT(parse_attribute_name(node, new, 1));
    CHECK_FCT(parse_attribute_id(node, new));
    CHECK_FCT(parse_attribute_size(node, new, 0));
    CHECK_FCT(parse_attribute_align(node, new, 0));

    CHECK_FCT(types_insert_tail(head, new));

    return 0;
}

static int parse_struct(xmlNode *node, types_t **head) {
    types_t *new;

    if (node == NULL)
        return -1;

    new = type_new (TYPE_STRUCT);

    CHECK_FCT(parse_attribute_name(node, new, 0));
    CHECK_FCT(parse_attribute_id(node, new));
    CHECK_FCT(parse_attribute_size(node, new, 0));
    CHECK_FCT(parse_attribute_align(node, new, 0));
    CHECK_FCT(parse_attribute_file(node, new));
    CHECK_FCT(parse_attribute_line(node, new));
    CHECK_FCT(parse_attribute_context(node, new, 0));
    CHECK_FCT(parse_attribute_artificial(node, new));
    CHECK_FCT(parse_attribute_members(node, new, 0));
    CHECK_FCT(parse_attribute_mangled(node, new, 0));
    CHECK_FCT(parse_attribute_demangled(node, new, 0));

    CHECK_FCT(types_insert_tail(head, new));

    return 0;
}

static int parse_typedef(xmlNode *node, types_t **head) {
    types_t *new;

    if (node == NULL)
        return -1;

    new = type_new (TYPE_TYPEDEF);

    CHECK_FCT(parse_attribute_name(node, new, 1));
    CHECK_FCT(parse_attribute_id(node, new));
    CHECK_FCT(parse_attribute_file(node, new));
    CHECK_FCT(parse_attribute_line(node, new));
    CHECK_FCT(parse_attribute_type(node, new, 1));
    CHECK_FCT(parse_attribute_size(node, new, 0));
    CHECK_FCT(parse_attribute_align(node, new, 0));
    CHECK_FCT(parse_attribute_context(node, new, 0));
    CHECK_FCT(parse_attribute_artificial(node, new));
    CHECK_FCT(parse_attribute_members(node, new, 0));
    CHECK_FCT(parse_attribute_mangled(node, new, 0));
    CHECK_FCT(parse_attribute_demangled(node, new, 0));

    CHECK_FCT(types_insert_tail(head, new));

    return 0;
}

static int parse_field(xmlNode *node, types_t **head) {
    types_t *new;

    if (node == NULL)
        return -1;

    new = type_new (TYPE_FIELD);

    CHECK_FCT(parse_attribute_name(node, new, 1));
    CHECK_FCT(parse_attribute_id(node, new));
    CHECK_FCT(parse_attribute_type(node, new, 1));
    CHECK_FCT(parse_attribute_size(node, new, 0));
    CHECK_FCT(parse_attribute_bits(node, new, 0));
    CHECK_FCT(parse_attribute_offset(node, new, 0));
    CHECK_FCT(parse_attribute_align(node, new, 0));
    CHECK_FCT(parse_attribute_context(node, new, 0));
    CHECK_FCT(parse_attribute_artificial(node, new));
    CHECK_FCT(parse_attribute_members(node, new, 0));
    CHECK_FCT(parse_attribute_mangled(node, new, 0));
    CHECK_FCT(parse_attribute_demangled(node, new, 0));

    CHECK_FCT(types_insert_tail(head, new));

    return 0;
}

static int parse_file(xmlNode *node, types_t **head) {
    types_t *new;

    if (node == NULL)
        return -1;

    new = type_new (TYPE_FILE);

    CHECK_FCT(parse_attribute_name(node, new, 1));
    CHECK_FCT(parse_attribute_id(node, new));

    CHECK_FCT(types_insert_tail(head, new));

    return 0;
}

static int parse_reference_type(xmlNode *node, types_t **head) {
    types_t *new;

    if (node == NULL)
        return -1;

    new = type_new (TYPE_REFERENCE);

    CHECK_FCT(parse_attribute_id(node, new));
    CHECK_FCT(parse_attribute_type(node, new, 1));
    CHECK_FCT(parse_attribute_size(node, new, 1));
    CHECK_FCT(parse_attribute_align(node, new, 1));
    CHECK_FCT(parse_attribute_context(node, new, 0));
    CHECK_FCT(parse_attribute_offset(node, new, 0));
    CHECK_FCT(parse_attribute_file(node, new));
    CHECK_FCT(parse_attribute_line(node, new));

    CHECK_FCT(types_insert_tail(head, new));

    return 0;
}

static int parse_array_type(xmlNode *node, types_t **head) {
    types_t *new;

    if (node == NULL)
        return -1;

    new = type_new (TYPE_ARRAY);

    CHECK_FCT(parse_attribute_id(node, new));
    CHECK_FCT(parse_attribute_type(node, new, 1));
    CHECK_FCT(parse_attribute_size(node, new, 0));
    CHECK_FCT(parse_attribute_align(node, new, 1));
    CHECK_FCT(parse_attribute_min(node, new, 1));
    CHECK_FCT(parse_attribute_max(node, new, 1));

    CHECK_FCT(types_insert_tail(head, new));

    return 0;
}

static int parse_pointer_type(xmlNode *node, types_t **head) {
    types_t *new;

    if (node == NULL)
        return -1;

    new = type_new (TYPE_POINTER);

    CHECK_FCT(parse_attribute_id(node, new));
    CHECK_FCT(parse_attribute_type(node, new, 1));
    CHECK_FCT(parse_attribute_size(node, new, 1));
    CHECK_FCT(parse_attribute_align(node, new, 1));

    CHECK_FCT(types_insert_tail(head, new));

    return 0;
}

static int parse_function_type(xmlNode *node, types_t **head) {
    types_t *new;

    if (node == NULL)
        return -1;

    new = type_new (TYPE_FUNCTION);

    CHECK_FCT(parse_attribute_id(node, new));

    CHECK_FCT(types_insert_tail(head, new));

    return 0;
}

/**
 * print_element_names:
 * @a_node: the initial xml node to consider.
 *
 * Prints the names of the all the xml elements
 * that are siblings or children of a given xml node.
 */
static int parse_elements(xmlNode * a_node, types_t **head) {
    xmlNode *cur_node = NULL;
    xmlNode *child_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        for (child_node = cur_node->children; child_node; child_node = child_node->next) {
            if (child_node->type == XML_ELEMENT_NODE) {
                if (strcmp ((char *) child_node->name, "Enumeration") == 0) {
                    CHECK_FCT_DO(parse_enumeration(child_node, head), return RC_FAIL);
                }
                else
                    if (strcmp ((char *) child_node->name, "FundamentalType") == 0) {
                        CHECK_FCT_DO(parse_fundamental(child_node, head), return RC_FAIL);
                    }
                    else
                        if (strcmp ((char *) child_node->name, "Struct") == 0) {
                            CHECK_FCT_DO(parse_struct(child_node, head), return RC_FAIL);
                        }
                        else
                            if (strcmp ((char *) child_node->name, "Union") == 0) {
                                CHECK_FCT_DO(parse_union(child_node, head), return RC_FAIL);
                            }
                            else
                                if (strcmp ((char *) child_node->name, "Typedef") == 0) {
                                    CHECK_FCT_DO(parse_typedef(child_node, head), return RC_FAIL);
                                }
                                else
                                    if (strcmp ((char *) child_node->name, "File") == 0) {
                                        CHECK_FCT_DO(parse_file(child_node, head), return RC_FAIL);
                                    }
                                    else
                                        if (strcmp ((char *) child_node->name, "Field") == 0) {
                                            CHECK_FCT_DO(parse_field(child_node, head), return RC_FAIL);
                                        }
                                        else
                                            if (strcmp ((char *) child_node->name, "ReferenceType") == 0) {
                                                CHECK_FCT_DO(parse_reference_type(child_node, head), return RC_FAIL);
                                            }
                                            else
                                                if (strcmp ((char *) child_node->name, "ArrayType") == 0) {
                                                    CHECK_FCT_DO(parse_array_type(child_node, head), return RC_FAIL);
                                                }
                                                else
                                                    if (strcmp ((char *) child_node->name, "PointerType") == 0) {
                                                        CHECK_FCT_DO(parse_pointer_type(child_node, head), return RC_FAIL);
                                                    }
                                                    else
                                                        if (strcmp ((char *) child_node->name, "FunctionType") == 0) {
                                                            CHECK_FCT_DO(parse_function_type(child_node, head), return RC_FAIL);
                                                        }
            }
        }
    }

    return RC_OK;
}

int xml_parse_buffer(const char *xml_buffer, const int size) {
    xmlDocPtr doc; /* the resulting document tree */

    if (xml_buffer == NULL) {
        return -1;
    }

    g_message("Parsing XML definition from buffer ...");

    doc = xmlReadMemory(xml_buffer, size, NULL, NULL, 0);

    if (doc == NULL) {
        g_warning("Failed to parse XML buffer: %s", xml_buffer);
        ui_notification_dialog(GTK_MESSAGE_ERROR, "parse messages format definition", "Fail to parse XML buffer");
        return RC_FAIL;
    }

    return xml_parse_doc(doc);
}

int xml_parse_file(const char *filename) {
    xmlDocPtr doc; /* the resulting document tree */

    if (filename == NULL) {
        return -1;
    }

    doc = xmlReadFile (filename, NULL, 0);

    if (doc == NULL) {
        ui_notification_dialog(GTK_MESSAGE_ERROR, "parse messages format definition", "Failed to parse file \"%s\"", filename);
        return RC_FAIL;
    }

    return xml_parse_doc(doc);
}

static int update_filters() {
    types_t *types;

    ui_init_filters(FALSE, TRUE);

    types = messages_id_enum;
    if (types != NULL)
    {
        types = types->child;

        while (types != NULL)
        {
            if (strcmp (types->name, "MESSAGES_ID_MAX") != 0)
            {
                ui_filters_add (FILTER_MESSAGES, types->init_value, types->name, ENTRY_ENABLED_UNDEFINED);
            }
            types = types->next;
        }
    }

    types = origin_task_id_type;
    if (types != NULL)
    {
        types = types->child->child;

        while (types != NULL) {
            if ((strcmp (types->name, "TASK_FIRST") != 0) && (strcmp (types->name, "TASK_MAX") != 0))
            {
            	ui_filters_add(FILTER_ORIGIN_TASKS, types->init_value, types->name, ENTRY_ENABLED_UNDEFINED);
            }
            types = types->next;
        }
    }

    types = destination_task_id_type;
    if (types != NULL)
    {
        types = types->child->child;

        while (types != NULL) {
            if ((strcmp (types->name, "TASK_FIRST") != 0) && (strcmp (types->name, "TASK_MAX") != 0))
            {
            	ui_filters_add(FILTER_DESTINATION_TASKS, types->init_value, types->name, ENTRY_ENABLED_UNDEFINED);
            }
            types = types->next;
        }
    }

    return RC_OK;
}

static int xml_parse_doc(xmlDocPtr doc) {
    xmlNode *root_element = NULL;
    types_t *head = NULL;
    int ret = 0;
    FILE *dissect_file = NULL;

    if (ui_main_data.dissect_file_name != NULL) {
        dissect_file = fopen (ui_main_data.dissect_file_name, "w");
    }

    /* Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    ret = parse_elements(root_element, &head);

    /* Free the document */
    xmlFreeDoc(doc);

    if (ret == RC_OK) {
        resolve_typedefs (&head);
        resolve_pointer_type (&head);
        resolve_field (&head);
        resolve_array (&head);
        resolve_reference (&head);
        resolve_struct (&head);
        resolve_file (&head);
        resolve_union (&head);
        resolve_function (&head);

        /* Locate the root element which corresponds to the MessageDef struct */
        CHECK_FCT(locate_root("MessageDef", head, &root));

        /* Locate the LTE time fields */
        if (locate_type("lte_time", head, &lte_time_type) == RC_OK)
        {
            CHECK_FCT(locate_type("frame", lte_time_type->child->child, &lte_time_frame_type));
            CHECK_FCT(locate_type("slot", lte_time_type->child->child, &lte_time_slot_type));
        }

        /* Locate the message id field */
        CHECK_FCT(locate_type("MessagesIds", head, &messages_id_enum));

        /* Locate the header part of a message */
        CHECK_FCT(locate_type("ittiMsgHeader", head, &message_header_type));
        /* Locate the origin task id field */
        CHECK_FCT(locate_type("originTaskId", message_header_type, &origin_task_id_type));
        /* Locate the destination task id field */
        CHECK_FCT(locate_type("destinationTaskId", message_header_type, &destination_task_id_type));
        /* Locate the instance field */
        CHECK_FCT(locate_type("instance", message_header_type, &instance_type));
        /* Locate the message size field */
        CHECK_FCT(locate_type("ittiMsgSize", message_header_type, &message_size_type));

        // root->type_hr_display(root, 0);

        update_filters();
        if (dissect_file != NULL) {
            g_debug("generating dissected types file \"%s\" ...", ui_main_data.dissect_file_name);
            root->type_file_print (root, 0, dissect_file);
        }
    }

    if (dissect_file != NULL) {
        fclose (dissect_file);
    }

    g_message("Parsed XML definition");

    return ret;
}

int dissect_signal_header(buffer_t *buffer, ui_set_signal_text_cb_t ui_set_signal_text_cb,
                          gpointer cb_user_data)
{
    if (message_header_type == NULL) {
        g_error("No messages format definition provided");
        return RC_FAIL;
    }

    if (buffer == NULL) {
        g_error("Failed buffer is NULL");
        return RC_FAIL;
    }

    message_header_type->type_dissect_from_buffer(
        message_header_type, ui_set_signal_text_cb, cb_user_data,
        buffer, 0, 0, INDENT_START);

    return RC_OK;
}

int dissect_signal(buffer_t *buffer, ui_set_signal_text_cb_t ui_set_signal_text_cb,
                   gpointer cb_user_data)
{
    if (root == NULL) {
        g_error("No messages format definition provided");
        return RC_FAIL;
    }

    if (buffer == NULL) {
        g_error("Failed buffer is NULL");
        return RC_FAIL;
    }

    root->type_dissect_from_buffer(root, ui_set_signal_text_cb, cb_user_data,
                                   buffer, 0, 0, INDENT_START);

    return RC_OK;
}
