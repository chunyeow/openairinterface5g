#include <pthread.h>
#include <stdint.h>

#ifndef UDP_PRIMITIVES_CLIENT_H_
#define UDP_PRIMITIVES_CLIENT_H_

/** \brief UDP recv callback prototype. Will be called every time a payload is
 *  received on socket.
 *  \param port Port on which data had been received
 *  \param address Sender Ipv4 address
 *  \param buffer Pointer to data (should be freed by user)
 *  \param length Length of message received
 *  \param arg_p User provided argument (transparent for wrappers)
 *  @returns Execution result
 */
typedef int (*udp_recv_callback)(uint16_t  port,
                                 uint32_t  address,
                                 uint8_t  *buffer,
                                 uint32_t  length,
                                 void     *arg_p);

typedef struct {
    udp_recv_callback recv_callback;
    pthread_t         recv_thread;
    int               sd;
    void             *arg_p;
} udp_data_t;

/** \brief Create new datagram connection-less socket and create new thread
 *  for data in downstream.
 *  \param ip_addr Local IPv4 address to use
 *  \param port Local port to use
 *  \param udp_data_p
 *  \param recv_callback Pointer to data (should be freed by user)
 *  \param arg_p Optionnal argument to pass to recv_callback
 *  @returns < 0 on failure or socket descriptor on success
 */
int udp_create_connection(char *ip_addr, uint16_t port,
                          udp_data_t *udp_data_p,
                          udp_recv_callback recv_callback,
                          void *arg_p);

/** \brief Send buffer to remote peer.
 *  \param sd socket descriptor to use
 *  \param port remote port
 *  \param address remote address
 *  \param buffer Data buffer to send
 *  \param length Buffer length
 *  @returns < 0 on failure or socket descriptor on success
 */
int udp_send_to(int sd, uint16_t port, uint32_t address,
                const uint8_t *buffer, uint32_t length);

#endif /* UDP_PRIMITIVES_CLIENT_H_ */
