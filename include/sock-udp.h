#ifndef UDO_SOCK_UDP_H
#define UDO_SOCK_UDP_H

#include "macros.h"

#include <netinet/in.h>

/*
 * Stores information about the cando_sock_udp instace.
 */
struct cando_sock_udp;


/*
 * @brief Structure passed to cando_sock_udp_server_create(3)
 *        used to define how to create the server.
 *
 * @member ipv6    - Boolean to determine if a socket is soley an ipv6
 *                   socket or requires an ipv4-mapped-ipv6-address.
 * @member ip_addr - Textual network IP address to recvfrom(2)/sendto(2) data with.
 * @member port    - Network port to recvfrom(2)/sendto(2) data with.
 */
struct cando_sock_udp_server_create_info
{
	unsigned char ipv6 : 1;
	const char    *ip_addr;
	int           port;
};


/*
 * @brief Creates a socket that may be utilized for server socket operations.
 *
 * @param sock      - May be NULL or a pointer to a struct cando_sock_udp.
 *                    If NULL memory will be allocated and return to
 *                    caller. If not NULL address passed will be used
 *                    to store the newly created struct cando_sock_udp
 *                    instance.
 * @param sock_info - Implementation uses a pointer to a
 *                    struct cando_sock_udp_server_create_info
 *                    no other implementation may be passed to
 *                    this parameter.
 *
 * @return
 *	on success: Pointer to a struct cando_sock_udp
 *	on failure: NULL
 */
UDO_API
struct cando_sock_udp *
cando_sock_udp_server_create (struct cando_sock_udp *sock,
                              const void *sock_info);


/*
 * @brief Creates file descriptor that can filter for
 *        @addr data comming to server file descriptor.
 *        Useful to utilize in an epoll(2) event loop
 *        if caller wants to implement tcp like event
 *        handling with UDP sockets.
 *
 * @param sock - Must pass a pointer to a struct cando_sock_udp.
 * @param addr - Must pass a pointer to a populated struct sockaddr_in6.
 * @param ipv6 - Boolean determines if created socket is soley an ipv6
 *               socket or requires an ipv4-mapped-ipv6-address.
 *
 * @return
 *	on success: File descriptor to accepted client
 *	on failure: -1
 */
UDO_API
int
cando_sock_udp_server_accept (struct cando_sock_udp *sock,
                              const struct sockaddr_in6 *addr,
                              const unsigned char ipv6);


/*
 * @brief Receive data from server socket file descriptor.
 *
 * @param sock      - Pointer to a struct cando_sock_udp instance.
 * @param data      - Pointer to a buffer to store data received from a socket.
 * @param size      - Size of data to receive from a socket.
 * @param addr      - Pointer to struct sockaddr_in6 which stores the
 *                    address information of the socket that data
 *                    was received from.
 * @param sock_info - Reserved for future usage. For now used
 *                    to set the flag argument of recvfrom(2).
 *
 * @return
 *	on success: Amount of bytes received
 *	on failure: # < 0
 */
UDO_API
ssize_t
cando_sock_udp_server_recv_data (struct cando_sock_udp *sock,
                                 void *data,
                                 const size_t size,
                                 struct sockaddr_in6 *addr,
                                 const void *sock_info);


/*
 * @brief Structure passed to cando_sock_udp_client_create(3)
 *        used to define how to create the client.
 *
 * @member ipv6    - Boolean to determine if a socket is soley an ipv6
 *                   socket or requires an ipv4-mapped-ipv6-address.
 * @member ip_addr - Textual network IP address to recvfrom(2)/sendto(2) data with.
 * @member port    - Network port to recvfrom(2)/sendto(2) data with.
 */
struct cando_sock_udp_client_create_info
{
	unsigned char ipv6 : 1;
	const char    *ip_addr;
	int           port;
};


/*
 * @brief Creates a socket that may be utilized for client socket operations.
 *
 * @param sock      - May be NULL or a pointer to a struct cando_sock_udp.
 *                    If NULL memory will be allocated and return to
 *                    caller. If not NULL address passed will be used
 *                    to store the newly created struct cando_sock_udp
 *                    instance.
 * @param sock_info - Implementation uses a pointer to a
 *                    struct cando_sock_udp_client_create_info
 *                    no other implementation may be passed to
 *                    this parameter.
 *
 * @return
 *	on success: Pointer to a struct cando_sock_udp
 *	on failure: NULL
 */
UDO_API
struct cando_sock_udp *
cando_sock_udp_client_create (struct cando_sock_udp *sock,
                              const void *sock_info);


/*
 * @brief Fliters client socket to allow sending data
 *        without passing a struct sockaddr_in6 to sendto(2).
 *        Address is populated with a call to cando_sock_udp_client_create.
 *
 * @param sock - Must pass a pointer to a struct cando_sock_udp.
 *
 * @return
 *	on success: 0
 *	on failure: -1
 */
UDO_API
int
cando_sock_udp_client_connect (struct cando_sock_udp *sock);


/*
 * @brief Send data to client socket address provided via
 *        call to cando_sock_udp_client_create(3).
 *
 * @param sock      - Must pass a pointer to a struct cando_sock_udp.
 * @param data      - Pointer to buffer to send through socket.
 * @param size      - Size of data to send through socket.
 * @param sock_info - Reserved for future usage. For now used
 *                    to set the flag argument of sendto(2).
 *
 * @return
 *	on success: Amount of bytes sent
 *	on failure: # < 0
 */
UDO_API
ssize_t
cando_sock_udp_client_send_data (struct cando_sock_udp *sock,
                                 const void *data,
                                 const size_t size,
                                 const void *sock_info);


/*
 * @brief Acquire socket file descriptor associated with
 *        struct cando_sock_udp instance.
 *
 * @param sock - Must pass a pointer to a struct cando_sock_udp.
 *
 * @return
 *	on success: Socket file descriptor
 *	on failure: -1
 */
UDO_API
int
cando_sock_udp_get_fd (struct cando_sock_udp *sock);


/*
 * @brief Acquire textual network ip address associated
 *        with struct cando_sock_udp instance.
 *
 * @param sock - Must pass a pointer to a struct cando_sock_udp.
 *
 * @return
 *	on success: Textual network ip address
 *	on failure: NULL
 */
UDO_API
const char *
cando_sock_udp_get_ip_addr (struct cando_sock_udp *sock);


/*
 * @brief Acquire network port associated with
 *        struct cando_sock_udp instance.
 *
 * @param sock - Must pass a pointer to a struct cando_sock_udp.
 *
 * @return
 *	on success: Network port connected to instance
 *	on failure: -1
 */
UDO_API
int
cando_sock_udp_get_port (struct cando_sock_udp *sock);


/*
 * @brief Frees any allocated memory and closes FD's (if open) created after
 *        cando_sock_udp_server_create() or cando_sock_udp_client_create() call.
 *
 * @param sock - Pointer to a valid struct cando_sock_udp.
 */
UDO_API
void
cando_sock_udp_destroy (struct cando_sock_udp *sock);


/*
 * @brief Returns size of the internal structure. So,
 *        if caller decides to allocate memory outside
 *        of API interface they know the exact amount
 *        of bytes.
 *
 * @return
 *	on success: sizeof(struct cando_sock_udp)
 *	on failure: sizeof(struct cando_sock_udp)
 */
UDO_API
int
cando_sock_udp_get_sizeof (void);


/*
 * @brief Receive data from socket file descriptor.
 *
 * @param sock_fd   - Socket file descriptor to receive data from.
 * @param data      - Pointer to buffer to store data received from a socket.
 * @param size      - Size of data to receive from a socket.
 * @param addr      - Pointer to struct sockaddr_in6 which stores the
 *                    address information of the socket that data
 *                    was received from.
 * @param sock_info - Reserved for future usage. For now used
 *                    to set the flag argument of recvfrom(2).
 *
 * @return
 *	on success: Amount of bytes received
 *	on failure: # < 0
 */
UDO_API
ssize_t
cando_sock_udp_recv_data (const int sock_fd,
                          void *data,
                          const size_t size,
                          struct sockaddr_in6 *addr,
                          const void *sock_info);


/*
 * @brief Send data to socket file descriptor.
 *
 * @param sock_fd   - Socket file descriptor to send data to.
 * @param data      - Pointer to buffer to send through socket.
 * @param size      - Size of data to send through socket.
 * @param addr      - Pointer to struct sockaddr_in6 which stores the
 *                    address information of a socket that data
 *                    will be sent to.
 * @param sock_info - Reserved for future usage. For now used
 *                    to set the flag argument of sendto(2).
 *
 * @return
 *	on success: Amount of bytes sent
 *	on failure: # < 0
 */
UDO_API
ssize_t
cando_sock_udp_send_data (const int sock_fd,
                          const void *data,
                          const size_t size,
                          const struct sockaddr_in6 *addr,
                          const void *sock_info);

#endif /* UDO_SOCK_UDP_H */
