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

int main(int argc, char *argv[])
{
    int ret = 0;

    CHECK_FCT(ui_gtk_initialize(argc, argv));

    return ret;
}
