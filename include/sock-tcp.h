#ifndef UDO_SOCK_TCP_H
#define UDO_SOCK_TCP_H

#include "macros.h"

#include <sys/socket.h>
#include <netinet/in.h>

/*
 * Stores information about the udo_sock_tcp instace.
 */
struct udo_sock_tcp;


/*
 * @brief Structure passed to udo_sock_tcp_server_create(3)
 *        used to define how to create the server.
 *
 * @member ipv6        - Boolean to determine if a socket is soley an ipv6
 *                       socket or requires an ipv4-mapped-ipv6-address.
 * @member ip_addr     - Textual network IP addresses to accept(2) with.
 * @member port        - Network port to accept(2) with.
 * @member connections - Amount of connections that may be queued
 *                       at a given moment.
 */
struct udo_sock_tcp_server_create_info
{
	unsigned char ipv6 : 1;
	const char    *ip_addr;
	int           port;
	int           connections;
};


/*
 * @brief Creates a socket that may be utilized for server socket operations.
 *
 * @param sock      - May be NULL or a pointer to a struct udo_sock_tcp.
 *                    If NULL memory will be allocated and return to
 *                    caller. If not NULL address passed will be used
 *                    to store the newly created struct udo_sock_tcp
 *                    instance.
 * @param sock_info - Implementation uses a pointer to a
 *                    struct udo_sock_tcp_server_create_info
 *                    no other implementation may be passed to
 *                    this parameter.
 *
 * @returns
 *	on success: Pointer to a struct udo_sock_tcp
 *	on failure: NULL
 */
UDO_API
struct udo_sock_tcp *
udo_sock_tcp_server_create (struct udo_sock_tcp *sock,
                            const void *sock_info);


/*
 * @brief Accepts client connections returns file descriptor
 *        to the connected client.
 *
 * @param sock - Must pass a pointer to a struct udo_sock_tcp.
 * @param addr - May be NULL or a pointer to a struct sockaddr_in6
 *               If not NULL @addr is filled in via accept(2) call.
 *
 * @returns
 *	on success: File descriptor to accepted client
 *	on failure: -1
 */
UDO_API
int
udo_sock_tcp_server_accept (struct udo_sock_tcp *sock,
                            struct sockaddr_in6 *addr);


/*
 * @brief Structure passed to udo_sock_tcp_client_create(3)
 *        used to define how to create the client.
 *
 * @member ipv6    - Boolean to determine if a socket is soley an ipv6
 *                   socket or requires an ipv4-mapped-ipv6-address.
 * @member ip_addr - Textual network IP addresses to connect(2)/send(2) to.
 * @member port    - Network port to connect(2)/send(2) to.
 */
struct udo_sock_tcp_client_create_info
{
	unsigned char ipv6 : 1;
	const char    *ip_addr;
	int           port;
};


/*
 * @brief Creates a socket that may be utilized for client socket operations.
 *
 * @param sock      - May be NULL or a pointer to a struct udo_sock_tcp.
 *                    If NULL memory will be allocated and return to
 *                    caller. If not NULL address passed will be used
 *                    to store the newly created struct udo_sock_tcp
 *                    instance.
 * @param sock_info - Implementation uses a pointer to a
 *                    struct udo_sock_tcp_client_create_info
 *                    no other implementation may be passed to
 *                    this parameter.
 *
 * @returns
 *	on success: Pointer to a struct udo_sock_tcp
 *	on failure: NULL
 */
UDO_API
struct udo_sock_tcp *
udo_sock_tcp_client_create (struct udo_sock_tcp *sock,
                            const void *sock_info);


/*
 * @brief Connects client socket to address provided via
 *        call to udo_sock_tcp_client_create.
 *
 * @param sock - Must pass a pointer to a struct udo_sock_tcp.
 *
 * @returns
 *	on success: 0
 *	on failure: -1
 */
UDO_API
int
udo_sock_tcp_client_connect (struct udo_sock_tcp *sock);


/*
 * @brief Send data to client socket address provided via
 *        call to udo_sock_tcp_client_create(3).
 *
 * @param sock      - Must pass a pointer to a struct udo_sock_tcp.
 * @param data      - Pointer to buffer to send through socket.
 * @param size      - Size of data to send through socket.
 * @param sock_info - Reserved for future usage. For now used
 *                    to set the flag argument of send(2).
 *
 * @returns
 *	on success: Amount of bytes sent
 *	on failure: # < 0
 */
UDO_API
ssize_t
udo_sock_tcp_client_send_data (struct udo_sock_tcp *sock,
                               const void *data,
                               const size_t size,
                               const void *sock_info);


/*
 * @brief Acquire socket file descriptor associated with
 *        struct udo_sock_tcp instance.
 *
 * @param sock - Must pass a pointer to a struct udo_sock_tcp.
 *
 * @returns
 *	on success: TCP socket file descriptor
 *	on failure: -1
 */
UDO_API
int
udo_sock_tcp_get_fd (struct udo_sock_tcp *sock);


/*
 * @brief Acquire textual network address in string format
 *        associated with struct udo_sock_tcp instance.
 *
 * @param sock - Must pass a pointer to a struct udo_sock_tcp.
 *
 * @returns
 *	on success: Textual network address
 *	on failure: NULL
 */
UDO_API
const char *
udo_sock_tcp_get_ip_addr (struct udo_sock_tcp *sock);


/*
 * @brief Acquire network port associated with
 *        struct udo_sock_tcp instance.
 *
 * @param sock - Must pass a pointer to a struct udo_sock_tcp.
 *
 * @returns
 *	on success: Network port connected to instance
 *	on failure: -1
 */
UDO_API
int
udo_sock_tcp_get_port (struct udo_sock_tcp *sock);


/*
 * @brief Frees any allocated memory and closes FD's (if open) created after
 *        udo_sock_tcp_server_create() or udo_sock_tcp_client_create() call.
 *
 * @param sock - Pointer to a valid struct udo_sock_tcp.
 */
UDO_API
void
udo_sock_tcp_destroy (struct udo_sock_tcp *sock);


/*
 * @brief Returns size of the internal structure. So,
 *        if caller decides to allocate memory outside
 *        of API interface they know the exact amount
 *        of bytes.
 *
 * @returns
 *	on success: sizeof(struct udo_sock_tcp)
 *	on failure: sizeof(struct udo_sock_tcp)
 */
UDO_API
int
udo_sock_tcp_get_sizeof (void);


/*
 * @brief Receive data from socket file descriptor.
 *
 * @param sock_fd   - Socket file descriptor to receive data from.
 * @param data      - Pointer to buffer to store data received from a socket.
 * @param size      - Size of data to receive from a socket.
 * @param sock_info - Reserved for future usage. For now used
 *                    to set the flag argument of recv(2).
 *
 * @returns
 *	on success: Amount of bytes received
 *	on failure: # < 0
 */
UDO_API
ssize_t
udo_sock_tcp_recv_data (const int sock_fd,
                        void *data,
                        const size_t size,
                        const void *sock_info);


/*
 * @brief Send data to socket file descriptor.
 *
 * @param sock_fd   - Socket file descriptor to send data to.
 * @param data      - Pointer to buffer to send through socket.
 * @param size      - Size of data to send through socket.
 * @param sock_info - Reserved for future usage. For now used
 *                    to set the flag argument of send(2).
 *
 * @returns
 *	on success: Amount of bytes sent
 *	on failure: # < 0
 */
UDO_API
ssize_t
udo_sock_tcp_send_data (const int sock_fd,
                        const void *data,
                        const size_t size,
                        const void *sock_info);

#endif /* UDO_SOCK_TCP_H */
