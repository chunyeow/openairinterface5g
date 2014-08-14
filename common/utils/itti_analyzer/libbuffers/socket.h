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

#include <arpa/inet.h>
#include <netinet/in.h>

#ifndef SOCKET_H_
#define SOCKET_H_

typedef struct {
    pthread_t thread;
    int       sd;
    char     *ip_address;
    uint16_t  port;

    /* The pipe used between main thread (running GTK) and the socket thread */
    int       pipe_fd;

    /* Time used to avoid refreshing UI every time a new signal is incoming */
    gint64    last_data_notification;
    uint8_t   nb_signals_since_last_update;

    /* The last signals received which are not yet been updated in GUI */
    GList    *signal_list;
} socket_data_t;

gboolean socket_abort_connection;

int socket_connect_to_remote_host(const char *remote_ip, const uint16_t port,
                                  int pipe_fd);

int socket_disconnect_from_remote_host(void);

#endif /* SOCKET_H_ */
