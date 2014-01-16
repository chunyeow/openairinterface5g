
#include <stdint.h>
#include <unistd.h>

#include "config.h"
#include "test_utils.h"
#include "test_fd.h"

void
doit (void)
{
    s6a_fd_init();

    sleep(1);

    s6a_fd_stop();

    success ("freediameter start/stop ok\n");
}
