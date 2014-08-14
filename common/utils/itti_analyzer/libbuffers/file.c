/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
   included in this distribution in the file called "COPYING". If not, 
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define G_LOG_DOMAIN ("BUFFERS")

#include <glib.h>

#include "rc.h"
#include "buffers.h"
#include "file.h"

#define READ_BUFFER_SIZE 1024

int file_read_dump(buffer_t **buffer, const char *filename)
{
    int fd = -1;
    buffer_t *new_buf = NULL;
    uint8_t   data[READ_BUFFER_SIZE];
    ssize_t   current_read;

    if (!filename)
        return RC_BAD_PARAM;

    if ((fd = open(filename, O_RDONLY)) == -1) {
        g_warning("Cannot open %s for reading, returned %d:%s\n",
                  filename, errno, strerror(errno));
        return RC_FAIL;
    }

    CHECK_FCT(buffer_new_from_data(&new_buf, NULL, 0, 0));

    do {
        current_read = read(fd, data, READ_BUFFER_SIZE);
        if (current_read == -1)
        {
            g_warning("Failed to read data from file, returned %d:%s\n",
                      errno, strerror(errno));
            return RC_FAIL;
        }
        CHECK_FCT(buffer_append_data(new_buf, data, current_read));
    } while(current_read == READ_BUFFER_SIZE);

    *buffer = new_buf;

    buffer_dump(new_buf, stdout);

    close(fd);

    return RC_OK;
}
