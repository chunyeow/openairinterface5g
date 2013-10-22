#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <gtk/gtk.h>

#include "xml_parse.h"
#include "resolvers.h"
#include "locate_root.h"
#include "file.h"
#include "ui_main_screen.h"

#include "rc.h"

int debug_buffers = 1;
int debug_parser = 0;

static void
console_log_handler(const char *log_domain, GLogLevelFlags log_level,
                    const char *message, gpointer user_data)
{
    time_t curr;
    struct tm *today;
    const char *level;

    switch(log_level & G_LOG_LEVEL_MASK) {
        case G_LOG_LEVEL_ERROR:
            level = "Err ";
            break;
        case G_LOG_LEVEL_CRITICAL:
            level = "Crit";
            break;
        case G_LOG_LEVEL_WARNING:
            level = "Warn";
            break;
        case G_LOG_LEVEL_MESSAGE:
            level = "Msg ";
            break;
        case G_LOG_LEVEL_INFO:
            level = "Info";
            break;
        case G_LOG_LEVEL_DEBUG:
            level = "Dbg ";
            break;
        default:
            fprintf(stderr, "unknown log_level %u\n", log_level);
            level = NULL;
            g_assert_not_reached();
    }

    /* create a "timestamp" */
    time(&curr);
    today = localtime(&curr);

    fprintf(stderr, "%02u:%02u:%02u %8s %s %s\n",
            today->tm_hour, today->tm_min, today->tm_sec,
            log_domain != NULL ? log_domain : "",
            level, message);
}

int main(int argc, char *argv[])
{
    int ret = 0;

    GLogLevelFlags log_flags;

    log_flags = (GLogLevelFlags)
        (G_LOG_LEVEL_ERROR      |
        G_LOG_LEVEL_CRITICAL    |
        G_LOG_LEVEL_WARNING     |
        G_LOG_LEVEL_MESSAGE     |
        G_LOG_LEVEL_INFO        |
        G_LOG_LEVEL_DEBUG       |
        G_LOG_FLAG_FATAL        |
        G_LOG_FLAG_RECURSION);

    /* Initialize the widget set */
    gtk_init(&argc, &argv);

    g_log_set_handler(NULL, log_flags, console_log_handler, NULL);

    CHECK_FCT(ui_gtk_initialize(argc, argv));

    return ret;
}
