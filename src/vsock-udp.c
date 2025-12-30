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

#include "vsock-udp.h"

/*
 * @brief Structure defining Cando VM Socket UDP interface implementation.
 *
 * @member err  - Stores information about the error that occured
 *                for the given instance and may later be retrieved
 *                by caller.
 * @member free - If structure allocated with calloc(3) member will be
 *                set to true so that, we know to call free(3) when
 *                destroying the instance.
 * @member fd   - File descriptor to the open VM socket.
 * @member vcid - VM Context Identifier.
 * @member port - Network port number to recvfrom(2)/sendto(2) with.
 * @member addr - Stores byte information about the VM socket context.
 *                Is used for client connect(2) and server bind(2)/connect(2).
 */
struct cando_vsock_udp
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

struct cando_vsock_udp_create_info
{
	unsigned int vcid;
	int          port;
};


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
p_create_sock_fd (struct cando_vsock_udp *vsock)
{
	const int enable = 1;

	int sock_fd = -1, err = -1;

	sock_fd = socket(AF_VSOCK, SOCK_DGRAM, 0);
	if (sock_fd == -1) {
		udo_log_set_error(vsock, errno, "socket: %s\n", strerror(errno));
		close(sock_fd);
		return -1;
	}

	err = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	if (err == -1) {
		udo_log_set_error(vsock, errno, "setsockopt: %s", strerror(errno));
		close(sock_fd);
		return -1;
	}

	err = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));
	if (err == -1) {
		udo_log_set_error(vsock, errno, "setsockopt: %s", strerror(errno));
		close(sock_fd);
		return -1;
	}

	return sock_fd;
}


static struct cando_vsock_udp *
p_create_vsock (struct cando_vsock_udp *p_vsock,
                const void *p_vsock_info,
                const bool server)
{
	struct cando_vsock_udp *vsock = p_vsock;

	const struct cando_vsock_udp_create_info *vsock_info = p_vsock_info;

	if (!vsock_info) {
		udo_log_error("Incorrect data passed\n");
		return NULL;
	}

	if (!vsock) {
		vsock = calloc(1, sizeof(struct cando_vsock_udp));
		if (!vsock) {
			udo_log_error("calloc: %s\n", strerror(errno));
			return NULL;
		}

		vsock->free = true;
	}

	vsock->fd = p_create_sock_fd(vsock);
	if (vsock->fd == -1) {
		udo_log_error("%s\n", udo_log_get_error(vsock));
		cando_vsock_udp_destroy(vsock);
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
 * Start of cando_vsock_udp_server functions *
 *********************************************/

struct cando_vsock_udp *
cando_vsock_udp_server_create (struct cando_vsock_udp *p_vsock,
                               const void *p_vsock_info)
{
	int err = -1;

	struct cando_vsock_udp *vsock = NULL;

	vsock = p_create_vsock(p_vsock, p_vsock_info, 1);
	if (!vsock)
		return NULL;

	err = bind(vsock->fd, (struct sockaddr*) &(vsock->addr),
			sizeof(struct sockaddr_vm));
	if (err == -1) {
		cando_vsock_udp_destroy(vsock);
		udo_log_error("bind: %s\n", strerror(errno));
		return NULL;
	}

	return vsock;
}


int
cando_vsock_udp_server_accept (struct cando_vsock_udp *vsock,
                               const struct sockaddr_vm *addr)
{
	int err = -1, client_sock = -1;

	socklen_t len = sizeof(struct sockaddr_vm);

	if (!vsock)
		return -1;

	if (!addr) {
		udo_log_set_error(vsock, UDO_LOG_ERR_INCORRECT_DATA, "");
		return -1;
	}

	client_sock = p_create_sock_fd(vsock);
	if (client_sock == -1)
		return -1;

	/*
	 * Will temporary take over receiving from all,
	 * but released after call to connect(2).
	 */
	err = bind(client_sock, (const struct sockaddr*)&(vsock->addr), len);
	if (err == -1) {
		udo_log_set_error(vsock, errno, "bind: %s", strerror(errno));
		close(client_sock);
		return -1;
	}

	err = connect(client_sock, (const struct sockaddr*)addr, len);
	if (err == -1) {
		udo_log_set_error(vsock, errno, "connect: %s", strerror(errno));
		close(client_sock);
		return -1;
	}

	udo_log_info("[+] Connected client fd '%d' at '%lu:%u'\n",
	             client_sock, addr->svm_cid, ntohs(addr->svm_port));

	return client_sock;
}


ssize_t
cando_vsock_udp_server_recv_data (struct cando_vsock_udp *vsock,
                                  void *data,
                                  const size_t size,
                                  struct sockaddr_vm *addr,
                                  const void *vsock_info)
{
	if (!vsock)
		return -1;

	return cando_vsock_udp_recv_data(vsock->fd, data,
	                          size, addr, vsock_info);
}

/*******************************************
 * End of cando_vsock_udp_server functions *
 *******************************************/


/*********************************************
 * Start of cando_vsock_udp_client functions *
 *********************************************/

struct cando_vsock_udp *
cando_vsock_udp_client_create (struct cando_vsock_udp *p_vsock,
                               const void *vsock_info)
{
	struct cando_vsock_udp *vsock = NULL;

	vsock = p_create_vsock(p_vsock, vsock_info, 0);
	if (!vsock)
		return NULL;

	return vsock;
}


int
cando_vsock_udp_client_connect (struct cando_vsock_udp *vsock)
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

	udo_log_success("[+] Filtering to <VM cid:port> '%lu:%d'\n",
	                vsock->vcid, vsock->port);

	return 0;
}


ssize_t
cando_vsock_udp_client_send_data (struct cando_vsock_udp *vsock,
                                  const void *data,
                                  const size_t size,
                                  const void *vsock_info)
{
	if (!vsock)
		return -1;

	return cando_vsock_udp_send_data(vsock->fd, data, size,
	                            &(vsock->addr), vsock_info);
}

/*******************************************
 * End of cando_vsock_udp_client functions *
 *******************************************/


/******************************************
 * Start of cando_vsock_udp_get functions *
 ******************************************/

int
cando_vsock_udp_get_fd (struct cando_vsock_udp *vsock)
{
	if (!vsock)
		return -1;

	return vsock->fd;
}


unsigned int
cando_vsock_udp_get_vcid (struct cando_vsock_udp *vsock)
{
	if (!vsock)
		return UINT32_MAX;

	return vsock->vcid;
}


int
cando_vsock_udp_get_port (struct cando_vsock_udp *vsock)
{
	if (!vsock)
		return -1;

	return vsock->port;
}

/****************************************
 * End of cando_vsock_udp_get functions *
 ****************************************/


/**********************************************
 * Start of cando_vsock_udp_destroy functions *
 **********************************************/

void
cando_vsock_udp_destroy (struct cando_vsock_udp *vsock)
{
	if (!vsock)
		return;

	close(vsock->fd);

	if (vsock->free) {
		free(vsock);
	} else {
		memset(vsock, 0, sizeof(struct cando_vsock_udp));
		vsock->fd = -1;
	}
}

/********************************************
 * End of cando_vsock_udp_destroy functions *
 ********************************************/


/***************************************************
 * Start of non struct cando_vsock param functions *
 ***************************************************/

int
cando_vsock_udp_get_sizeof (void)
{
	return sizeof(struct cando_vsock_udp);
}


unsigned int
cando_vsock_udp_get_local_vcid (void)
{
	return p_vsock_get_local_vcid();
}


/* Used to verify data was received */
#define VERIFIER 0xFA4C62B5

ssize_t
cando_vsock_udp_recv_data (const int sock_fd,
                           void *data,
                           const size_t size,
                           struct sockaddr_vm *addr,
                           const void *vsock_info)
{
	int err = -1;

	ssize_t ret = 0;

	uint32_t received_data = VERIFIER;

	socklen_t len = sizeof(struct sockaddr_vm);

	const int flags = (vsock_info) ? *((const int*)vsock_info) : 0;

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
cando_vsock_udp_send_data (const int sock_fd,
                           const void *data,
                           const size_t size,
                           const struct sockaddr_vm *addr,
                           const void *vsock_info)
{
	int err = -1;

	ssize_t ret = 0;

	uint32_t received_data = 0;

	socklen_t len = sizeof(struct sockaddr_vm);

	const int flags = (vsock_info) ? *((const int*)vsock_info) : 0;

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
		udo_log_error("recvfrom: %s", strerror(errno));
		return -1;
	}

	if (received_data != VERIFIER) {
		udo_log_error("Data not received\n");
		return -1;
	}

	return ret;
}

/*************************************************
 * End of non struct cando_vsock param functions *
 *************************************************/
