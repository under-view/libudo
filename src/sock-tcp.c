#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "log.h"
#include "macros.h"

#include "sock-tcp.h"

/*
 * @brief Structure defining Cando Socket TCP interface implementation.
 *
 * @member err     - Stores information about the error that occured
 *                   for the given instance and may later be retrieved
 *                   by caller.
 * @member free    - If structure allocated with calloc(3) member will be
 *                   set to true so that, we know to call free(3) when
 *                   destroying the instance.
 * @member fd      - File descriptor to the open TCP socket.
 * @member ip_addr - Textual network IP address to connect(2) to or accept(2) with.
 * @member port    - Network port number to connect(2) to or accept(2) with.
 * @member addr    - Stores network byte information about the TCP socket context.
 *                   Is used for client connect(2) and server accept(2).
 */
struct cando_sock_tcp
{
	struct cando_log_error_struct err;
	bool                          free;
	int                           fd;
	char                          ip_addr[INET6_ADDRSTRLEN];
	int                           port;
	struct sockaddr_in6           addr;
};


/*****************************************
 * Start of global to C source functions *
 *****************************************/

static int
p_set_sock_opts (struct cando_sock_tcp *sock,
                 const int sock_fd,
                 const bool ipv6)
{
	int err = -1;

	const int enable = 1, disable = 0;

	err = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	if (err == -1) {
		cando_log_set_error(sock, errno, "setsockopt: %s", strerror(errno));
		close(sock_fd);
		return -1;
	}

	err = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));
	if (err == -1) {
		cando_log_set_error(sock, errno, "setsockopt: %s", strerror(errno));
		close(sock_fd);
		return -1;
	}

	err = setsockopt(sock_fd, IPPROTO_IPV6, IPV6_V6ONLY,
		(ipv6) ? &enable : &disable, sizeof(int));
	if (err == -1) {
		cando_log_set_error(sock, errno, "setsockopt: %s", strerror(errno));
		close(sock_fd);
		return -1;
	}

	return sock_fd;
}


static struct cando_sock_tcp *
p_create_sock (struct cando_sock_tcp *p_sock,
               const void *p_sock_info)
{
	int err = -1;

	struct cando_sock_tcp *sock = p_sock;

	const struct cando_sock_tcp_create_info
	{
		unsigned char ipv6 : 1;
		const char    *ip_addr;
		int           port;
	} *sock_info = p_sock_info;

	if (!sock) {
		sock = calloc(1, sizeof(struct cando_sock_tcp));
		if (!sock) {
			cando_log_error("calloc: %s\n", strerror(errno));
			return NULL;
		}

		sock->free = true;
	}

	sock->fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (sock->fd == -1) {
		cando_log_error("socket: %s\n", strerror(errno));
		cando_sock_tcp_destroy(sock);
		return NULL;
	}

	err = p_set_sock_opts(sock, sock->fd, sock_info->ipv6);
	if (err == -1) {
		cando_log_error("%s\n", cando_log_get_error(sock));
		cando_sock_tcp_destroy(sock);
		return NULL;
	}

	sock->port = sock_info->port;
	snprintf(sock->ip_addr, sizeof(sock->ip_addr), "%s%s",
	         (sock_info->ipv6) ? "" : "::ffff:", sock_info->ip_addr);

	sock->addr.sin6_family = AF_INET6;
	sock->addr.sin6_port = htons(sock_info->port);
	err = inet_pton(AF_INET6, sock->ip_addr, &(sock->addr.sin6_addr));
	if (err == 0) {
		cando_log_error("'%s' invalid\n", sock->ip_addr);
		cando_sock_tcp_destroy(sock);
		return NULL;
	} else if (err == -1) {
		cando_log_error("inet_pton: %s\n", strerror(errno));
		cando_sock_tcp_destroy(sock);
		return NULL;
	}

	return sock;
}

/***************************************
 * End of global to C source functions *
 ***************************************/


/********************************************
 * Start of cando_sock_tcp_server functions *
 ********************************************/

struct cando_sock_tcp *
cando_sock_tcp_server_create (struct cando_sock_tcp *p_sock,
                              const void *p_sock_info)
{
	int err = -1, flags = 0;

	struct cando_sock_tcp *sock = NULL;

	const struct cando_sock_tcp_server_create_info *sock_info = p_sock_info;

	sock = p_create_sock(p_sock, p_sock_info);
	if (!sock)
		return NULL;

	flags = sock_info->connections;
	err = setsockopt(sock->fd, SOL_TCP, TCP_KEEPCNT, &flags, sizeof(int));
	if (err == -1) {
		cando_sock_tcp_destroy(sock);
		cando_log_error("setsockopt: %s\n", strerror(errno));
		return NULL;
	}

	flags = 5; // 5 seconds
	err = setsockopt(sock->fd, SOL_TCP, TCP_KEEPIDLE, &flags, sizeof(int));
	if (err == -1) {
		cando_sock_tcp_destroy(sock);
		cando_log_error("setsockopt: %s\n", strerror(errno));
		return NULL;
	}

	err = setsockopt(sock->fd, SOL_TCP, TCP_KEEPINTVL, &flags, sizeof(int));
	if (err == -1) {
		cando_sock_tcp_destroy(sock);
		cando_log_error("setsockopt: %s\n", strerror(errno));
		return NULL;
	}

	err = bind(sock->fd, (struct sockaddr*) &(sock->addr),
			sizeof(struct sockaddr_in6));
	if (err == -1) {
		cando_sock_tcp_destroy(sock);
		cando_log_error("bind: %s\n", strerror(errno));
		return NULL;
	}

	err = listen(sock->fd, sock_info->connections);
	if (err == -1) {
		cando_sock_tcp_destroy(sock);
		cando_log_error("listen: %s\n", strerror(errno));
		return NULL;
	}

	return sock;
}


int
cando_sock_tcp_server_accept (struct cando_sock_tcp *sock,
                              struct sockaddr_in6 *p_addr)
{
	const char *ip_addr = NULL;

	char buff[INET6_ADDRSTRLEN];

	struct sockaddr_in6 inaddr;
	struct sockaddr_in6 *addr = NULL;

	int client_sock = -1, err = -1, enabled = 1;

	socklen_t len = sizeof(struct sockaddr_in6);

	if (!sock)
		return -1;

	addr = (p_addr) ? p_addr : &inaddr;
	client_sock = accept(sock->fd, (struct sockaddr*)addr, &len);
	if (client_sock == -1) {
		cando_log_set_error(sock, errno, "accept: %s", strerror(errno));
		return -1;
	}

	err = setsockopt(client_sock, SOL_SOCKET, SO_KEEPALIVE, &enabled, sizeof(int));
	if (err == -1) {
		cando_log_set_error(sock, errno, "accept: %s", strerror(errno));
		close(client_sock);
		return -1;
	}

	ip_addr = inet_ntop(AF_INET6, addr, buff, len);
	cando_log_info("[+] Connected client fd '%d' at '%s:%u'\n",
	               client_sock, ip_addr, ntohs(addr->sin6_port));

	return client_sock;
}

/******************************************
 * End of cando_sock_tcp_server functions *
 ******************************************/


/********************************************
 * Start of cando_sock_tcp_client functions *
 ********************************************/

struct cando_sock_tcp *
cando_sock_tcp_client_create (struct cando_sock_tcp *p_sock,
                              const void *sock_info)
{
	struct cando_sock_tcp *sock = NULL;

	sock = p_create_sock(p_sock, sock_info);
	if (!sock)
		return NULL;

	return sock;
}


int
cando_sock_tcp_client_connect (struct cando_sock_tcp *sock)
{
	int err = -1;

	if (!sock)
		return -1;

	if (sock->fd <= 0) {
		cando_log_set_error(sock, UDO_LOG_ERR_INCORRECT_DATA, "");
		return -1;
	}

	err = connect(sock->fd, (struct sockaddr*)&(sock->addr),
			sizeof(struct sockaddr_in6));
	if (err == -1) {
		cando_log_set_error(sock, errno, "connect: %s", strerror(errno));
		return -1;
	}

	cando_log_success("[+] Connected to <ip_addr:port> '%s:%d'\n",
	                  sock->ip_addr, sock->port);

	return 0;
}


ssize_t
cando_sock_tcp_client_send_data (struct cando_sock_tcp *sock,
                                 const void *data,
                                 const size_t size,
                                 const void *sock_info)
{
	if (!sock)
		return -1;

	return cando_sock_tcp_send_data(sock->fd, data, size, sock_info);
}

/******************************************
 * End of cando_sock_tcp_client functions *
 ******************************************/


/*****************************************
 * Start of cando_sock_tcp_get functions *
 *****************************************/

int
cando_sock_tcp_get_fd (struct cando_sock_tcp *sock)
{
	if (!sock)
		return -1;

	return sock->fd;
}


const char *
cando_sock_tcp_get_ip_addr (struct cando_sock_tcp *sock)
{
	if (!sock || \
	    !(*sock->ip_addr))
	{
		return NULL;
	}

	return sock->ip_addr;
}


int
cando_sock_tcp_get_port (struct cando_sock_tcp *sock)
{
	if (!sock)
		return -1;

	return sock->port;
}

/***************************************
 * End of cando_sock_tcp_get functions *
 ***************************************/


/*********************************************
 * Start of cando_sock_tcp_destroy functions *
 *********************************************/

void
cando_sock_tcp_destroy (struct cando_sock_tcp *sock)
{
	if (!sock)
		return;

	close(sock->fd);

	if (sock->free) {
		free(sock);
	} else {
		memset(sock, 0, sizeof(struct cando_sock_tcp));
		sock->fd = -1;
	}
}

/*******************************************
 * End of cando_sock_tcp_destroy functions *
 *******************************************/


/******************************************************
 * Start of non struct cando_sock_tcp param functions *
 ******************************************************/

int
cando_sock_tcp_get_sizeof (void)
{
	return sizeof(struct cando_sock_tcp);
}


ssize_t
cando_sock_tcp_recv_data (const int sock_fd,
                          void *data,
                          const size_t size,
                          const void *sock_info)
{
	ssize_t ret = 0;

	const int flags = (sock_info) ? *((const int*)sock_info) : 0;

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
cando_sock_tcp_send_data (const int sock_fd,
                          const void *data,
                          const size_t size,
                          const void *sock_info)
{
	ssize_t ret = 0;

	const int flags = (sock_info) ? *((const int*)sock_info) : 0;

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

/****************************************************
 * End of non struct cando_sock_tcp param functions *
 ****************************************************/
