#ifndef UDO_USOCK_TCP_H
#define UDO_USOCK_TCP_H

#include "macros.h"

#include <sys/socket.h>
#include <sys/un.h>

/*
 * Stores information about the udo_usock_tcp instance.
 * usock - Unix domain socket.
 */
struct udo_usock_tcp;


/*
 * @brief Structure passed to udo_usock_tcp_server_create(3)
 *        used to define how to create the server.
 *
 * @member unix_path   - Absolute path to unix domain socket.
 * @member connections - Amount of connections that may be queued
 *                       at a given moment.
 */
struct udo_usock_tcp_server_create_info
{
	const char *unix_path;
	int        connections;
};


/*
 * @brief Creates a socket that may be utilized for server socket operations.
 *
 * @param usock      - May be NULL or a pointer to a struct udo_usock_tcp.
 *                     If NULL memory will be allocated and return to
 *                     caller. If not NULL address passed will be used
 *                     to store the newly created struct udo_usock_tcp
 *                     instance.
 * @param usock_info - Implementation uses a pointer to a
 *                     struct udo_usock_tcp_server_create_info
 *                     no other implementation may be passed to
 *                     this parameter.
 *
 * @returns
 *	on success: Pointer to a struct udo_usock_tcp
 *	on failure: NULL
 */
UDO_API
struct udo_usock_tcp *
udo_usock_tcp_server_create (struct udo_usock_tcp *usock,
                             const void *usock_info);


/*
 * @brief Accepts client connections returns file descriptor
 *        to the connected client.
 *
 * @param usock - Must pass a pointer to a struct udo_usock_tcp.
 * @param addr  - May be NULL or a pointer to a struct sockaddr_un.
 *                If not NULL @addr is filled in via accept(2) call.
 *
 * @returns
 *	on success: File descriptor to accepted client
 *	on failure: -1
 */
UDO_API
int
udo_usock_tcp_server_accept (struct udo_usock_tcp *usock,
                             struct sockaddr_un *addr);


/*
 * @brief Structure passed to udo_usock_tcp_client_create(3)
 *        used to define how to create the client.
 *
 * @member unix_path - Absolute path to unix domain socket.
 */
struct udo_usock_tcp_client_create_info
{
	const char *unix_path;
};


/*
 * @brief Creates a socket that may be utilized for client socket operations.
 *
 * @param usock      - May be NULL or a pointer to a struct udo_usock_tcp.
 *                     If NULL memory will be allocated and return to
 *                     caller. If not NULL address passed will be used
 *                     to store the newly created struct udo_usock_tcp
 *                     instance.
 * @param usock_info - Implementation uses a pointer to a
 *                     struct udo_usock_tcp_client_create_info
 *                     no other implementation may be passed to
 *                     this parameter.
 *
 * @returns
 *	on success: Pointer to a struct udo_usock_tcp
 *	on failure: NULL
 */
UDO_API
struct udo_usock_tcp *
udo_usock_tcp_client_create (struct udo_usock_tcp *usock,
                             const void *usock_info);


/*
 * @brief Connects client socket to address provided via
 *        call to udo_usock_tcp_client_create.
 *
 * @param usock - Must pass a pointer to a struct udo_usock_tcp.
 *
 * @returns
 *	on success: 0
 *	on failure: -1
 */
UDO_API
int
udo_usock_tcp_client_connect (struct udo_usock_tcp *usock);


/*
 * @brief Send data to client socket address provided via
 *        call to udo_usock_tcp_client_create(3).
 *
 * @param usock      - Must pass a pointer to a struct udo_usock_tcp.
 * @param data       - Pointer to buffer to send through socket.
 * @param size       - Size of data to send through socket.
 * @param usock_info - Reserved for future usage. For now used
 *                     to set the flag argument of send(2).
 *
 * @returns
 *	on success: Amount of bytes sent
 *	on failure: # < 0
 */
UDO_API
ssize_t
udo_usock_tcp_client_send_data (struct udo_usock_tcp *usock,
                                const void *data,
                                const size_t size,
                                const void *usock_info);


/*
 * @brief Acquire socket file descriptor associated with
 *        struct udo_usock_tcp instance.
 *
 * @param usock - Must pass a pointer to a struct udo_usock_tcp.
 *
 * @returns
 *	on success: TCP unix domain socket file descriptor
 *	on failure: -1
 */
UDO_API
int
udo_usock_tcp_get_fd (struct udo_usock_tcp *usock);


/*
 * @brief Acquire textual path to unix domain socket file in string
 *        format associated with struct udo_usock_tcp instance.
 *
 * @param usock - Must pass a pointer to a struct udo_usock_tcp.
 *
 * @returns
 *	on success: Textual path to unix domain socket
 *	on failure: NULL
 */
UDO_API
const char *
udo_usock_tcp_get_unix_path (struct udo_usock_tcp *usock);


/*
 * @brief Frees any allocated memory and closes FD's (if open) created after
 *        udo_usock_tcp_server_create() or udo_usock_tcp_client_create() call.
 *
 * @param usock - Pointer to a valid struct udo_usock_tcp.
 */
UDO_API
void
udo_usock_tcp_destroy (struct udo_usock_tcp *usock);


/*
 * @brief Returns size of the internal structure. So,
 *        if caller decides to allocate memory outside
 *        of API interface they know the exact amount
 *        of bytes.
 *
 * @returns
 *	on success: sizeof(struct udo_usock_tcp)
 *	on failure: sizeof(struct udo_usock_tcp)
 */
UDO_API
int
udo_usock_tcp_get_sizeof (void);


/*
 * @brief Receive data from socket file descriptor.
 *
 * @param sock_fd    - Socket file descriptor to receive data from.
 * @param data       - Pointer to buffer to store data received from a socket.
 * @param size       - Size of data to receive from a socket.
 * @param usock_info - Reserved for future usage. For now used
 *                     to set the flag argument of recv(2).
 *
 * @returns
 *	on success: Amount of bytes received
 *	on failure: # < 0
 */
UDO_API
ssize_t
udo_usock_tcp_recv_data (const int sock_fd,
                         void *data,
                         const size_t size,
                         const void *usock_info);


/*
 * @brief Send data to socket file descriptor.
 *
 * @param sock_fd    - Socket file descriptor to send data to.
 * @param data       - Pointer to buffer to send through socket.
 * @param size       - Size of data to send through socket.
 * @param usock_info - Reserved for future usage. For now used
 *                     to set the flag argument of send(2).
 *
 * @returns
 *	on success: Amount of bytes sent
 *	on failure: # < 0
 */
UDO_API
ssize_t
udo_usock_tcp_send_data (const int sock_fd,
                         const void *data,
                         const size_t size,
                         const void *usock_info);

#endif /* UDO_USOCK_TCP_H */
