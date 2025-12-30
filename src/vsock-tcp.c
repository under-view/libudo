#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/vm_sockets.h>

#include "log.h"
#include "macros.h"

#include "vsock-tcp.h"

/*
 * @brief Structure defining Cando VM Socket TCP interface implementation.
 *
 * @member err  - Stores information about the error that occured
 *                for the given instance and may later be retrieved
 *                by caller.
 * @member free - If structure allocated with calloc(3) member will be
 *                set to true so that, we know to call free(3) when
 *                destroying the instance.
 * @member fd   - File descriptor to the open VM socket.
 * @member vcid - VM Context Identifier.
 * @member port - Network port number to connect(2) to or accept(2) from.
 * @member addr - Stores byte information about the VM socket context.
 *                Is used for client connect(2) and server accept(2).
 */
struct cando_vsock_tcp
{
	struct udo_log_error_struct err;
	bool                        free;
	int                         fd;
	unsigned int                vcid;
	int                         port;
	struct sockaddr_vm          addr;
};


/*****************************************
 * Start of global to C source functions *
 *****************************************/

static unsigned int
p_vsock_get_local_vcid (void)
{
	unsigned int vcid = 0;

	int fd = -1, err = -1;

	fd = open("/dev/vsock", O_RDONLY);
	if (fd == -1) {
		udo_log_error("open('/dev/vsock'): %s\n", strerror(errno));
		return UINT32_MAX;
	}

	err = ioctl(fd, IOCTL_VM_SOCKETS_GET_LOCAL_CID, &vcid);
	if (err == -1 || vcid == UINT32_MAX) {
		close(fd);
		udo_log_error("ioctl: %s\n", strerror(errno));
		return UINT32_MAX;
	}

	close(fd);

	return vcid;
}


static int
p_set_sock_opts (struct cando_vsock_tcp *sock,
                 const int sock_fd)
{
	int err = -1;

	const int enable = 1;

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

	return sock_fd;
}


static struct cando_vsock_tcp *
p_create_vsock (struct cando_vsock_tcp *p_vsock,
                const void *p_vsock_info,
                const bool server)
{
	int err = -1;

	struct cando_vsock_tcp *vsock = p_vsock;

	const struct cando_vsock_tcp_create_info {  
		unsigned int vcid;
		int          port;
	} *vsock_info = p_vsock_info;

	if (!vsock) {
		vsock = calloc(1, sizeof(struct cando_vsock_tcp));
		if (!vsock) {
			udo_log_error("calloc: %s\n", strerror(errno));
			return NULL;
		}

		vsock->free = true;
	}

	vsock->fd = socket(AF_VSOCK, SOCK_STREAM, 0);
	if (vsock->fd == -1) {
		cando_vsock_tcp_destroy(vsock);
		return NULL;
	}

	err = p_set_sock_opts(vsock, vsock->fd);
	if (err == -1) {
		udo_log_error("%s\n", udo_log_get_error(vsock));
		cando_vsock_tcp_destroy(vsock);
		return NULL;
	}

	vsock->port = vsock_info->port;
	vsock->vcid = (server && vsock_info->vcid != 1) ? \
		p_vsock_get_local_vcid() : vsock_info->vcid;

	vsock->addr.svm_family = AF_VSOCK;
	vsock->addr.svm_reserved1 = 0;
	vsock->addr.svm_port = vsock_info->port;
	vsock->addr.svm_cid = vsock->vcid;

	return vsock;
}

/***************************************
 * End of global to C source functions *
 ***************************************/


/*********************************************
 * Start of cando_vsock_tcp_server functions *
 *********************************************/

struct cando_vsock_tcp *
cando_vsock_tcp_server_create (struct cando_vsock_tcp *p_vsock,
                               const void *p_vsock_info)
{
	int err = -1;

	struct cando_vsock_tcp *vsock = NULL;

	const struct cando_vsock_tcp_server_create_info *vsock_info = p_vsock_info;

	vsock = p_create_vsock(p_vsock, p_vsock_info, 1);
	if (!vsock)
		return NULL;

	err = bind(vsock->fd, (struct sockaddr*) &(vsock->addr),
			sizeof(struct sockaddr_vm));
	if (err == -1) {
		udo_log_error("bind: %s\n", strerror(errno));
		cando_vsock_tcp_destroy(vsock);
		return NULL;
	}

	err = listen(vsock->fd, vsock_info->connections);
	if (err == -1) {
		udo_log_error("listen: %s\n", strerror(errno));
		cando_vsock_tcp_destroy(vsock);
		return NULL;
	}

	return vsock;
}


int
cando_vsock_tcp_server_accept (struct cando_vsock_tcp *vsock,
                               struct sockaddr_vm *p_addr)
{
	int client_sock = -1;

	struct sockaddr_vm inaddr;
	struct sockaddr_vm *addr = NULL;

	socklen_t len = sizeof(struct sockaddr_vm);

	if (!vsock)
		return -1;

	addr = (p_addr) ? p_addr : &inaddr;
	client_sock = accept(vsock->fd, (struct sockaddr*)addr, &len);
	if (client_sock == -1) {
		udo_log_set_error(vsock, errno, "accept: %s", strerror(errno));
		return -1;
	}

	udo_log_info("[+] Connected client fd '%d' at '%lu:%u'\n",
	             client_sock, addr->svm_cid, ntohs(addr->svm_port));

	return client_sock;
}

/*******************************************
 * End of cando_vsock_tcp_server functions *
 *******************************************/


/*********************************************
 * Start of cando_vsock_tcp_client functions *
 *********************************************/

struct cando_vsock_tcp *
cando_vsock_tcp_client_create (struct cando_vsock_tcp *p_vsock,
                               const void *vsock_info)
{
	struct cando_vsock_tcp *vsock = NULL;

	vsock = p_create_vsock(p_vsock, vsock_info, 0);
	if (!vsock)
		return NULL;

	return vsock;
}


int
cando_vsock_tcp_client_connect (struct cando_vsock_tcp *vsock)
{
	int err = -1;

	if (!vsock)
		return -1;

	if (vsock->fd <= 0) {
		udo_log_set_error(vsock, UDO_LOG_ERR_INCORRECT_DATA, "");
		return -1;
	}

	err = connect(vsock->fd, (struct sockaddr*)&(vsock->addr),
			sizeof(struct sockaddr_vm));
	if (err == -1) {
		udo_log_set_error(vsock, errno, "connect: %s", strerror(errno));
		return -1;
	}

	udo_log_success("[+] Connected to <VM cid:port> '%lu:%d'\n",
	                vsock->vcid, vsock->port);

	return 0;
}


ssize_t
cando_vsock_tcp_client_send_data (struct cando_vsock_tcp *vsock,
                                  const void *data,
                                  const size_t size,
                                  const void *vsock_info)
{
	if (!vsock)
		return -1;

	return cando_vsock_tcp_send_data(vsock->fd, data, size, vsock_info);
}

/*******************************************
 * End of cando_vsock_tcp_client functions *
 *******************************************/


/******************************************
 * Start of cando_vsock_tcp_get functions *
 ******************************************/

int
cando_vsock_tcp_get_fd (struct cando_vsock_tcp *vsock)
{
	if (!vsock)
		return -1;

	return vsock->fd;
}


unsigned int
cando_vsock_tcp_get_vcid (struct cando_vsock_tcp *vsock)
{
	if (!vsock)
		return UINT32_MAX;

	return vsock->vcid;
}


int
cando_vsock_tcp_get_port (struct cando_vsock_tcp *vsock)
{
	if (!vsock)
		return -1;

	return vsock->port;
}

/****************************************
 * End of cando_vsock_tcp_get functions *
 ****************************************/


/**********************************************
 * Start of cando_vsock_tcp_destroy functions *
 **********************************************/

void
cando_vsock_tcp_destroy (struct cando_vsock_tcp *vsock)
{
	if (!vsock)
		return;

	close(vsock->fd);

	if (vsock->free) {
		free(vsock);
	} else {
		memset(vsock, 0, sizeof(struct cando_vsock_tcp));
		vsock->fd = -1;
	}
}

/********************************************
 * End of cando_vsock_tcp_destroy functions *
 ********************************************/


/***************************************************
 * Start of non struct cando_vsock param functions *
 ***************************************************/

int
cando_vsock_tcp_get_sizeof (void)
{
	return sizeof(struct cando_vsock_tcp);
}


unsigned int
cando_vsock_tcp_get_local_vcid (void)
{
	return p_vsock_get_local_vcid();
}


ssize_t
cando_vsock_tcp_recv_data (const int sock_fd,
                           void *data,
                           const size_t size,
                           const void *vsock_info)
{
	ssize_t ret = 0;

	const int flags = (vsock_info) ? *((const int*)vsock_info) : 0;

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
		udo_log_error("recv: %s", strerror(errno));
		return -1;
	}

	return ret;
}


ssize_t
cando_vsock_tcp_send_data (const int sock_fd,
                           const void *data,
                           const size_t size,
                           const void *vsock_info)
{
	ssize_t ret = 0;

	const int flags = (vsock_info) ? *((const int*)vsock_info) : 0;

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
		udo_log_error("send: %s", strerror(errno));
		return -1;
	}

	return ret;
}

/*************************************************
 * End of non struct cando_vsock param functions *
 *************************************************/
