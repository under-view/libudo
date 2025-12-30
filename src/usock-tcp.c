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

#include "usock-tcp.h"

/*
 * Unix domain socket path length size defined in <sys/un.h>.
 */
#define UNIX_PATH_SIZE 108

/*
 * @brief Structure defining Cando Unix Socket TCP interface implementation.
 *
 * @member err       - Stores information about the error that occured
 *                     for the given instance and may later be retrieved
 *                     by caller.
 * @member free      - If structure allocated with calloc(3) member will be
 *                     set to true so that, we know to call free(3) when
 *                     destroying the instance.
 * @member fd        - File descriptor to the open TCP unix domain socket.
 * @member addr      - Stores byte information about the TCP unix domain socket
 *                     context. Is used for client connect(2) and server accept(2).
 */
struct cando_usock_tcp
{
	struct cando_log_error_struct err;
	bool                          free;
	int                           fd;
	struct sockaddr_un            addr;
};


/*****************************************
 * Start of global to C source functions *
 *****************************************/

static int
p_set_sock_opts (struct cando_usock_tcp *usock,
                 const int sock_fd)
{
	int err = -1;

	const int enable = 1;

	err = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	if (err == -1) {
		cando_log_set_error(usock, errno, "setsockopt: %s", strerror(errno));
		close(sock_fd);
		return -1;
	}

	return 0;
}


static struct cando_usock_tcp *
p_create_sock (struct cando_usock_tcp *p_sock,
               const void *p_usock_info)
{
	int err = -1;

	struct cando_usock_tcp *usock = p_sock;

	const struct cando_usock_tcp_create_info
	{
		const char *unix_path;
	} *usock_info = p_usock_info;

	if (!usock_info || \
	    !(usock_info->unix_path))
	{
		cando_log_error("Incorrect data passed\n");
		return NULL;
	}

	if (!usock) {
		usock = calloc(1, sizeof(struct cando_usock_tcp));
		if (!usock) {
			cando_log_error("calloc: %s\n", strerror(errno));
			return NULL;
		}

		usock->free = true;
	}

	usock->fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (usock->fd == -1) {
		cando_log_error("socket: %s\n", strerror(errno));
		cando_usock_tcp_destroy(usock);
		return NULL;
	}

	err = p_set_sock_opts(usock, usock->fd);
	if (err == -1) {
		cando_log_error("%s\n", cando_log_get_error(usock));
		cando_usock_tcp_destroy(usock);
		return NULL;
	}

	usock->addr.sun_family = AF_UNIX;
	snprintf(usock->addr.sun_path, UNIX_PATH_SIZE, "%s", usock_info->unix_path);

	return usock;
}

/***************************************
 * End of global to C source functions *
 ***************************************/


/*********************************************
 * Start of cando_usock_tcp_server functions *
 *********************************************/

struct cando_usock_tcp *
cando_usock_tcp_server_create (struct cando_usock_tcp *p_sock,
                               const void *p_usock_info)
{
	int err = -1;

	struct cando_usock_tcp *usock = NULL;

	const struct cando_usock_tcp_server_create_info *usock_info = p_usock_info;

	usock = p_create_sock(p_sock, p_usock_info);
	if (!usock)
		return NULL;

	err = bind(usock->fd, (struct sockaddr*) &(usock->addr),
			sizeof(struct sockaddr_un));
	if (err == -1) {
		cando_usock_tcp_destroy(usock);
		cando_log_error("bind: %s\n", strerror(errno));
		return NULL;
	}

	err = listen(usock->fd, usock_info->connections);
	if (err == -1) {
		cando_usock_tcp_destroy(usock);
		cando_log_error("listen: %s\n", strerror(errno));
		return NULL;
	}

	return usock;
}


int
cando_usock_tcp_server_accept (struct cando_usock_tcp *usock,
                               struct sockaddr_un *p_addr)
{
	struct sockaddr_un inaddr;
	struct sockaddr_un *addr = NULL;

	int client_sock = -1, err = -1, enabled = 1;

	socklen_t len = sizeof(struct sockaddr_un);

	if (!usock)
		return -1;

	addr = (p_addr) ? p_addr : &inaddr;
	client_sock = accept(usock->fd, (struct sockaddr*)addr, &len);
	if (client_sock == -1) {
		cando_log_set_error(usock, errno, "accept: %s", strerror(errno));
		return -1;
	}

	err = setsockopt(client_sock, SOL_SOCKET, SO_KEEPALIVE, &enabled, sizeof(int));
	if (err == -1) {
		cando_log_set_error(usock, errno, "accept: %s", strerror(errno));
		close(client_sock);
		return -1;
	}

	cando_log_info("[+] Connected client fd '%d' at '%s'\n",
	               client_sock, usock->addr.sun_path);

	return client_sock;
}

/*******************************************
 * End of cando_usock_tcp_server functions *
 *******************************************/


/*********************************************
 * Start of cando_usock_tcp_client functions *
 *********************************************/

struct cando_usock_tcp *
cando_usock_tcp_client_create (struct cando_usock_tcp *p_sock,
                               const void *usock_info)
{
	struct cando_usock_tcp *usock = NULL;

	usock = p_create_sock(p_sock, usock_info);
	if (!usock)
		return NULL;

	return usock;
}


int
cando_usock_tcp_client_connect (struct cando_usock_tcp *usock)
{
	int err = -1;

	if (!usock)
		return -1;

	if (usock->fd <= 0) {
		cando_log_set_error(usock, UDO_LOG_ERR_INCORRECT_DATA, "");
		return -1;
	}

	err = connect(usock->fd, (struct sockaddr*)&(usock->addr),
			sizeof(struct sockaddr_un));
	if (err == -1) {
		cando_log_set_error(usock, errno, "connect: %s", strerror(errno));
		return -1;
	}

	cando_log_success("[+] Connected to <unix_path> '%s'\n", usock->addr.sun_path);

	return 0;
}


ssize_t
cando_usock_tcp_client_send_data (struct cando_usock_tcp *usock,
                                  const void *data,
                                  const size_t size,
                                  const void *usock_info)
{
	if (!usock)
		return -1;

	return cando_usock_tcp_send_data(usock->fd, data, size, usock_info);
}

/*******************************************
 * End of cando_usock_tcp_client functions *
 *******************************************/


/******************************************
 * Start of cando_usock_tcp_get functions *
 ******************************************/

int
cando_usock_tcp_get_fd (struct cando_usock_tcp *usock)
{
	if (!usock)
		return -1;

	return usock->fd;
}


const char *
cando_usock_tcp_get_unix_path (struct cando_usock_tcp *usock)
{
	if (!usock || \
	    !(*usock->addr.sun_path))
	{
		return NULL;
	}

	return usock->addr.sun_path;
}

/****************************************
 * End of cando_usock_tcp_get functions *
 ****************************************/


/**********************************************
 * Start of cando_usock_tcp_destroy functions *
 **********************************************/

void
cando_usock_tcp_destroy (struct cando_usock_tcp *usock)
{
	if (!usock)
		return;

	close(usock->fd);
	remove(usock->addr.sun_path);

	if (usock->free) {
		free(usock);
	} else {
		memset(usock, 0, sizeof(struct cando_usock_tcp));
		usock->fd = -1;
	}
}

/********************************************
 * End of cando_usock_tcp_destroy functions *
 ********************************************/


/*******************************************************
 * Start of non struct cando_usock_tcp param functions *
 *******************************************************/

int
cando_usock_tcp_get_sizeof (void)
{
	return sizeof(struct cando_usock_tcp);
}


ssize_t
cando_usock_tcp_recv_data (const int sock_fd,
                           void *data,
                           const size_t size,
                           const void *usock_info)
{
	ssize_t ret = 0;

	const int flags = (usock_info) ? *((const int*)usock_info) : 0;

	if (sock_fd < 0 || \
	    !data || \
	    !size)
	{
		return -1;
	}

	ret = recv(sock_fd, data, size, flags);
	if (errno == EINTR || errno == EAGAIN) {
		return -errno;
	} else if (ret == -1) {
		cando_log_error("recv: %s", strerror(errno));
		return -1;
	}

	return ret;
}


ssize_t
cando_usock_tcp_send_data (const int sock_fd,
                           const void *data,
                           const size_t size,
                           const void *usock_info)
{
	ssize_t ret = 0;

	const int flags = (usock_info) ? *((const int*)usock_info) : 0;

	if (sock_fd < 0 || \
	    !data || \
	    !size)
	{
		return -1;
	}

	ret = send(sock_fd, data, size, flags);
	if (errno == EINTR || errno == EAGAIN) {
		return -errno;
	} else if (ret == -1) {
		cando_log_error("send: %s", strerror(errno));
		return -1;
	}

	return ret;
}

/*****************************************************
 * End of non struct cando_usock_tcp param functions *
 *****************************************************/
