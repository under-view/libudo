#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "log.h"
#include "macros.h"

#include "sock-udp.h"

/*
 * @brief Structure defining Cando Socket UDP interface implementation.
 *
 * @member err     - Stores information about the error that occured
 *                   for the given instance and may later be retrieved
 *                   by caller.
 * @member free    - If structure allocated with calloc(3) member will be
 *                   set to true so that, we know to call free(3) when
 *                   destroying the instance.
 * @member fd      - File descriptor to the open UDP socket.
 * @member ip_addr - Textual network IP address to sendto(2)/recvfrom(2).
 * @member port    - Network port number to sendto(2)/recvfrom(2).
 * @member addr    - Stores IPV6 network byte information about the socket context.
 *                   Is used for client connect(2) and server bind(2)/connect(2).
 */
struct cando_sock_udp
{
	struct udo_log_error_struct err;
	bool                        free;
	int                         fd;
	char                        ip_addr[INET6_ADDRSTRLEN];
	int                         port;
	struct sockaddr_in6         addr;
};


/*****************************************
 * Start of global to C source functions *
 *****************************************/

struct cando_sock_udp_create_info
{
	unsigned char ipv6 : 1;
	const char    *ip_addr;
	int           port;
};


static int
p_create_sock_fd (struct cando_sock_udp *sock, const bool ipv6)
{
	int sock_fd = -1, err = -1;

	const int enable = 1, disable = 0;

	sock_fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	if (sock_fd == -1) {
		udo_log_set_error(sock, errno, "socket: %s\n", strerror(errno));
		close(sock_fd);
		return -1;
	}

	err = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	if (err == -1) {
		udo_log_set_error(sock, errno, "setsockopt: %s", strerror(errno));
		close(sock_fd);
		return -1;
	}

	err = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));
	if (err == -1) {
		udo_log_set_error(sock, errno, "setsockopt: %s", strerror(errno));
		close(sock_fd);
		return -1;
	}

	err = setsockopt(sock_fd, IPPROTO_IPV6, IPV6_V6ONLY,
		(ipv6) ? &enable : &disable, sizeof(int));
	if (err == -1) {
		udo_log_set_error(sock, errno, "setsockopt: %s", strerror(errno));
		close(sock_fd);
		return -1;
	}

	return sock_fd;
}


static struct cando_sock_udp *
p_create_sock (struct cando_sock_udp *p_sock,
               const void *p_sock_info)
{
	int err = -1;

	struct cando_sock_udp *sock = p_sock;

	const struct cando_sock_udp_create_info *sock_info = p_sock_info;

	if (!sock_info || \
	    !(sock_info->ip_addr))
	{
		udo_log_error("Incorrect data passed\n");
		return NULL;
	}

	if (!sock) {
		sock = calloc(1, sizeof(struct cando_sock_udp));
		if (!sock) {
			udo_log_error("calloc: %s\n", strerror(errno));
			return NULL;
		}

		sock->free = true;
	}

	sock->fd = p_create_sock_fd(sock, sock_info->ipv6);
	if (sock->fd == -1) {
		udo_log_error("%s\n", udo_log_get_error(sock));
		cando_sock_udp_destroy(sock);
		return NULL;
	}

	sock->port = sock_info->port;
	snprintf(sock->ip_addr, sizeof(sock->ip_addr), "%s%s",
	         (sock_info->ipv6) ? "" : "::ffff:", sock_info->ip_addr);

	sock->addr.sin6_family = AF_INET6;
	sock->addr.sin6_port = htons(sock_info->port);
	err = inet_pton(AF_INET6, sock->ip_addr, &(sock->addr.sin6_addr));
	if (err == 0) {
		udo_log_error("'%s' invalid\n", sock->ip_addr);
		cando_sock_udp_destroy(sock);
		return NULL;
	} else if (err == -1) {
		udo_log_error("inet_pton: %s\n", strerror(errno));
		cando_sock_udp_destroy(sock);
		return NULL;
	}

	return sock;
}

/***************************************
 * End of global to C source functions *
 ***************************************/


/********************************************
 * Start of cando_sock_udp_server functions *
 ********************************************/

struct cando_sock_udp *
cando_sock_udp_server_create (struct cando_sock_udp *p_sock,
                              const void *p_sock_info)
{
	int err = -1;

	struct cando_sock_udp *sock = NULL;

	sock = p_create_sock(p_sock, p_sock_info);
	if (!sock)
		return NULL;

	err = bind(sock->fd, (const struct sockaddr*)&(sock->addr),
			sizeof(struct sockaddr_in6));
	if (err == -1) {
		udo_log_error("bind: %s\n", strerror(errno));
		cando_sock_udp_destroy(sock);
		return NULL;
	}

	return sock;
}


int
cando_sock_udp_server_accept (struct cando_sock_udp *sock,
                              const struct sockaddr_in6 *addr,
                              const unsigned char ipv6)
{
	const char *ip_addr = NULL;

	int err = -1, sock_fd = -1;

	char buff[INET6_ADDRSTRLEN];

	socklen_t len = sizeof(struct sockaddr_in6);

	if (!sock)
		return -1;

	if (!addr) {
		udo_log_set_error(sock, UDO_LOG_ERR_INCORRECT_DATA, "");
		return -1;
	}

	sock_fd = p_create_sock_fd(sock, ipv6);
	if (sock_fd == -1)
		return -1;

	/*
	 * Will temporary take over receiving from all,
	 * but released after call to connect(2).
	 */
	err = bind(sock_fd, (const struct sockaddr*)&(sock->addr), len);
	if (err == -1) {
		udo_log_set_error(sock, errno, "bind: %s", strerror(errno));
		close(sock_fd);
		return -1;
	}

	err = connect(sock_fd, (const struct sockaddr*)addr, len);
	if (err == -1) {
		udo_log_set_error(sock, errno, "connect: %s", strerror(errno));
		close(sock_fd);
		return -1;
	}

	ip_addr = inet_ntop(AF_INET6, addr, buff, len);
	udo_log_info("[+] Connected client fd '%d' at '%s:%u'\n",
	             sock_fd, ip_addr, ntohs(addr->sin6_port));

	return sock_fd;
}


ssize_t
cando_sock_udp_server_recv_data (struct cando_sock_udp *sock,
                                 void *data,
                                 const size_t size,
                                 struct sockaddr_in6 *addr,
                                 const void *sock_info)
{
	if (!sock)
		return -1;

	return cando_sock_udp_recv_data(sock->fd, data,
	                         size, addr, sock_info);
}

/******************************************
 * End of cando_sock_udp_server functions *
 ******************************************/


/********************************************
 * Start of cando_sock_udp_client functions *
 ********************************************/

struct cando_sock_udp *
cando_sock_udp_client_create (struct cando_sock_udp *p_sock,
                              const void *sock_info)
{
	struct cando_sock_udp *sock = NULL;

	sock = p_create_sock(p_sock, sock_info);
	if (!sock)
		return NULL;

	return sock;
}


int
cando_sock_udp_client_connect (struct cando_sock_udp *sock)
{
	int err = -1;

	if (!sock)
		return -1;

	if (sock->fd <= 0) {
		udo_log_set_error(sock, UDO_LOG_ERR_INCORRECT_DATA, "");
		return -1;
	}

	err = connect(sock->fd, (struct sockaddr*)&(sock->addr),
			sizeof(struct sockaddr_in6));
	if (err == -1) {
		udo_log_set_error(sock, errno, "connect: %s", strerror(errno));
		return -1;
	}

	udo_log_success("[+] Filtering to <ip_addr:port> '%s:%d'\n",
	                sock->ip_addr, sock->port);

	return 0;
}


ssize_t
cando_sock_udp_client_send_data (struct cando_sock_udp *sock,
                                 const void *data,
                                 const size_t size,
                                 const void *sock_info)
{
	if (!sock)
		return -1;

	return cando_sock_udp_send_data(sock->fd, data, size,
	                            &(sock->addr), sock_info);
}

/******************************************
 * End of cando_sock_udp_client functions *
 ******************************************/


/*****************************************
 * Start of cando_sock_udp_get functions *
 *****************************************/

int
cando_sock_udp_get_fd (struct cando_sock_udp *sock)
{
	if (!sock)
		return -1;

	return sock->fd;
}


const char *
cando_sock_udp_get_ip_addr (struct cando_sock_udp *sock)
{
	if (!sock || \
	    !(*sock->ip_addr))
	{
		return NULL;
	}

	return sock->ip_addr;
}


int
cando_sock_udp_get_port (struct cando_sock_udp *sock)
{
	if (!sock)
		return -1;

	return sock->port;
}

/***************************************
 * End of cando_sock_udp_get functions *
 ***************************************/


/*********************************************
 * Start of cando_sock_udp_destroy functions *
 *********************************************/

void
cando_sock_udp_destroy (struct cando_sock_udp *sock)
{
	if (!sock)
		return;

	close(sock->fd);

	if (sock->free) {
		free(sock);
	} else {
		memset(sock, 0, sizeof(struct cando_sock_udp));
		sock->fd = -1;
	}
}

/*******************************************
 * End of cando_sock_udp_destroy functions *
 *******************************************/


/******************************************************
 * Start of non struct cando_sock_udp param functions *
 ******************************************************/

int
cando_sock_udp_get_sizeof (void)
{
	return sizeof(struct cando_sock_udp);
}


/* Used to verify data was received */
#define VERIFIER 0xFA4C62B5

ssize_t
cando_sock_udp_recv_data (const int sock_fd,
                          void *data,
                          const size_t size,
                          struct sockaddr_in6 *addr,
                          const void *sock_info)
{
	int err;

	ssize_t ret = 0;

	uint32_t received_data = VERIFIER;

	socklen_t len = sizeof(struct sockaddr_in6);

	const int flags = (sock_info) ? *((const int*)sock_info) : 0;

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
		udo_log_error("recvfrom: %s", strerror(errno));
		return -1;
	}

	err = sendto(sock_fd, &received_data, sizeof(received_data),
	             flags, (struct sockaddr*) addr, len);
	if (errno == EINTR || errno == EAGAIN) {
		return -errno;
	} else if (err == -1) {
		udo_log_error("sendto: %s", strerror(errno));
		return -1;
	}

	return ret;
}


ssize_t
cando_sock_udp_send_data (const int sock_fd,
                          const void *data,
                          const size_t size,
                          const struct sockaddr_in6 *addr,
                          const void *sock_info)
{
	int err = -1;

	ssize_t ret = 0;

	uint32_t received_data = 0;

	socklen_t len = sizeof(struct sockaddr_in6);

	const int flags = (sock_info) ? *((const int*)sock_info) : 0;

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
		udo_log_error("sendto: %s", strerror(errno));
		return -1;
	}

	err = recvfrom(sock_fd, &received_data, sizeof(received_data),
	               flags, (struct sockaddr*) addr, &len);
	if (errno == EINTR || errno == EAGAIN) {
		return -errno;
	} else if (err == -1) {
		udo_log_error("recvfrom: %s", strerror(errno));
		return -1;
	}

	if (received_data != VERIFIER) {
		udo_log_error("Data not received\n");
		return -1;
	}

	return ret;
}

/****************************************************
 * End of non struct cando_sock_udp param functions *
 ****************************************************/
