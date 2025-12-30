#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/un.h>

#include "log.h"
#include "macros.h"

#include "usock-udp.h"

/*
 * Unix domain socket path length size defined in <sys/un.h>.
 */
#define UNIX_PATH_SIZE 108

/*
 * @brief Structure defining Cando Unix Socket UDP interface implementation.
 *
 * @member err   - Stores information about the error that occured
 *                 for the given instance and may later be retrieved
 *                 by caller.
 * @member free  - If structure allocated with calloc(3) member will be
 *                 set to true so that, we know to call free(3) when
 *                 destroying the instance.
 * @member fd    - File descriptor to the open UDP unix domain socket.
 * @member addr  - Stores byte information about the UDP unix domain
 *                 socket context. Is used for client and server bind(2).
 * @member saddr - Stores byte information about the UDP unix domain
 *                 socket context. Is used for client connect(2).
 */
struct cando_usock_udp
{
	struct udo_log_error_struct err;
	bool                        free;
	int                         fd;
	struct sockaddr_un          addr;
	struct sockaddr_un          saddr;
};


/*****************************************
 * Start of global to C source functions *
 *****************************************/

struct cando_usock_udp_create_info
{
	const char *unix_path;
	const char *cli_unix_path;
};


static int
p_create_sock_fd (struct cando_usock_udp *usock)
{
	const int enable = 1;

	int sock_fd = -1, err = -1;

	sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock_fd == -1) {
		udo_log_set_error(usock, errno, "socket: %s\n", strerror(errno));
		close(sock_fd);
		return -1;
	}

	err = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	if (err == -1) {
		udo_log_set_error(usock, errno, "setsockopt: %s", strerror(errno));
		close(sock_fd);
		return -1;
	}

	return sock_fd;
}


static struct cando_usock_udp *
p_create_usock (struct cando_usock_udp *p_usock,
                const void UDO_UNUSED *p_usock_info)
{
	struct cando_usock_udp *usock = p_usock;

	if (!usock) {
		usock = calloc(1, sizeof(struct cando_usock_udp));
		if (!usock) {
			udo_log_error("calloc: %s\n", strerror(errno));
			return NULL;
		}

		usock->free = true;
	}

	usock->fd = p_create_sock_fd(usock);
	if (usock->fd == -1) {
		udo_log_error("%s\n", udo_log_get_error(usock));
		cando_usock_udp_destroy(usock);
		return NULL;
	}

	return usock;
}

/***************************************
 * End of global to C source functions *
 ***************************************/


/*********************************************
 * Start of cando_usock_udp_server functions *
 *********************************************/

struct cando_usock_udp *
cando_usock_udp_server_create (struct cando_usock_udp *p_usock,
                               const void *p_usock_info)
{
	int err = -1;

	struct cando_usock_udp *usock = NULL;

	const struct cando_usock_udp_create_info *usock_info = p_usock_info;

	if (!usock_info || \
	    !(usock_info->unix_path))
	{
		udo_log_error("Incorrect data passed\n");
		return NULL;
	}

	usock = p_create_usock(p_usock, p_usock_info);
	if (!usock)
		return NULL;

	usock->addr.sun_family = AF_UNIX;
	strncpy(usock->addr.sun_path, usock_info->unix_path, UNIX_PATH_SIZE-1);

	err = bind(usock->fd, (const struct sockaddr*)&(usock->addr),
			sizeof(struct sockaddr_un));
	if (err == -1) {
		udo_log_error("bind: %s\n", strerror(errno));
		cando_usock_udp_destroy(usock);
		return NULL;
	}

	return usock;
}


ssize_t
cando_usock_udp_server_recv_data (struct cando_usock_udp *usock,
                                  void *data,
                                  const size_t size,
                                  struct sockaddr_un *addr,
                                  const void *usock_info)
{
	if (!usock)
		return -1;

	return cando_usock_udp_recv_data(usock->fd, data,
	                         size, addr, usock_info);
}

/*******************************************
 * End of cando_usock_udp_server functions *
 *******************************************/


/*********************************************
 * Start of cando_usock_udp_client functions *
 *********************************************/

struct cando_usock_udp *
cando_usock_udp_client_create (struct cando_usock_udp *p_usock,
                               const void *p_usock_info)
{
	int err = -1;

	struct cando_usock_udp *usock = NULL;

	const struct cando_usock_udp_create_info *usock_info = p_usock_info;

	if (!usock_info || \
	    !(usock_info->unix_path) || \
	    !(usock_info->cli_unix_path))
	{
		udo_log_error("Incorrect data passed\n");
		return NULL;
	}

	usock = p_create_usock(p_usock, usock_info);
	if (!usock)
		return NULL;

	usock->addr.sun_family = AF_UNIX;
	usock->saddr.sun_family = AF_UNIX;
	strncpy(usock->addr.sun_path, usock_info->cli_unix_path, UNIX_PATH_SIZE-1);
	strncpy(usock->saddr.sun_path, usock_info->unix_path, UNIX_PATH_SIZE-1);

	err = bind(usock->fd, (const struct sockaddr*)&(usock->addr),
			sizeof(struct sockaddr_un));
	if (err == -1) {
		udo_log_error("bind: %s\n", strerror(errno));
		cando_usock_udp_destroy(usock);
		return NULL;
	}

	err = connect(usock->fd, (struct sockaddr*)&(usock->saddr),
			sizeof(struct sockaddr_un));
	if (err == -1) {
		udo_log_set_error(usock, errno, "connect: %s", strerror(errno));
		cando_usock_udp_destroy(usock);
		return NULL;
	}

	return usock;
}


ssize_t
cando_usock_udp_client_send_data (struct cando_usock_udp *usock,
                                  const void *data,
                                  const size_t size,
                                  const void *usock_info)
{
	if (!usock)
		return -1;

	return cando_usock_udp_send_data(usock->fd, data, size,
	                            &(usock->saddr), usock_info);
}

/*******************************************
 * End of cando_usock_udp_client functions *
 *******************************************/


/******************************************
 * Start of cando_usock_udp_get functions *
 ******************************************/

int
cando_usock_udp_get_fd (struct cando_usock_udp *usock)
{
	if (!usock)
		return -1;

	return usock->fd;
}


const char *
cando_usock_udp_get_unix_path (struct cando_usock_udp *usock)
{
	if (!usock || \
	    !(*usock->addr.sun_path))
	{
		return NULL;
	}

	return usock->addr.sun_path;
}

/****************************************
 * End of cando_usock_udp_get functions *
 ****************************************/


/**********************************************
 * Start of cando_usock_udp_destroy functions *
 **********************************************/

void
cando_usock_udp_destroy (struct cando_usock_udp *usock)
{
	if (!usock)
		return;

	close(usock->fd);
	remove(usock->addr.sun_path);

	if (usock->free) {
		free(usock);
	} else {
		memset(usock, 0, sizeof(struct cando_usock_udp));
		usock->fd = -1;
	}
}

/********************************************
 * End of cando_usock_udp_destroy functions *
 ********************************************/


/*******************************************************
 * Start of non struct cando_usock_udp param functions *
 *******************************************************/

int
cando_usock_udp_get_sizeof (void)
{
	return sizeof(struct cando_usock_udp);
}


/* Used to verify data was received */
#define VERIFIER 0xFA4C62B5

ssize_t
cando_usock_udp_recv_data (const int sock_fd,
                          void *data,
                          const size_t size,
                          struct sockaddr_un *addr,
                          const void *usock_info)
{
	int err;

	ssize_t ret = 0;

	uint32_t received_data = VERIFIER;

	socklen_t len = sizeof(struct sockaddr_un);

	const int flags = (usock_info) ? *((const int*)usock_info) : 0;

	if (sock_fd < 0 || \
	    !data || \
	    !size)
	{
		return -1;
	}

	ret = recvfrom(sock_fd, data, size, flags,
	               (struct sockaddr*) addr, &len);
	if (errno == EINTR || errno == EAGAIN) {
		return -errno;
	} else if (ret == -1) {
		udo_log_error("recvfrom: %s\n", strerror(errno));
		return -1;
	}

	err = sendto(sock_fd, &received_data, sizeof(received_data),
	             flags, (struct sockaddr*) addr, len);
	if (errno == EINTR || errno == EAGAIN) {
		return -errno;
	} else if (err == -1) {
		udo_log_error("sendto: %s\n", strerror(errno));
		return -1;
	}

	return ret;
}


ssize_t
cando_usock_udp_send_data (const int sock_fd,
                           const void *data,
                           const size_t size,
                           const struct sockaddr_un *addr,
                           const void *usock_info)
{
	int err = -1;

	ssize_t ret = 0;

	uint32_t received_data = 0;

	socklen_t len = sizeof(struct sockaddr_un);

	const int flags = (usock_info) ? *((const int*)usock_info) : 0;

	if (sock_fd < 0 || \
	    !data || \
	    !size)
	{
		return -1;
	}

	ret = sendto(sock_fd, data, size, flags,
	             (const struct sockaddr*) addr, len);
	if (errno == EINTR || errno == EAGAIN) {
		return -errno;
	} else if (ret == -1) {
		udo_log_error("sendto: %s\n", strerror(errno));
		return -1;
	}

	err = recvfrom(sock_fd, &received_data, sizeof(received_data),
	               flags, (struct sockaddr*) addr, &len);
	if (errno == EINTR || errno == EAGAIN) {
		return -errno;
	} else if (err == -1) {
		udo_log_error("recvfrom: %s\n", strerror(errno));
		return -1;
	}

	if (received_data != VERIFIER) {
		udo_log_error("Data not received\n");
		return -1;
	}

	return ret;
}

/*****************************************************
 * End of non struct cando_usock_udp param functions *
 *****************************************************/
