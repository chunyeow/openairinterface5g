#include <arpa/inet.h>
#include <netinet/in.h>

#ifndef SOCKET_H_
#define SOCKET_H_

typedef struct {
    pthread_t thread;
    uint16_t  port;
    char     *remote_ip;
    int       sd;
    struct sockaddr_in si_me;
} socket_data_t;

int socket_connect_to_remote_host(const char *remote_ip, const uint16_t port);

int socket_disconnect_from_remote_host(void);

#endif /* SOCKET_H_ */
