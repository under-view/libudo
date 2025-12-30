#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * Required by cmocka
 */
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "log.h"
#include "vsock-tcp.h"

/***************************************************
 * Start of test_vsock_tcp_server_create functions *
 ***************************************************/

static void UDO_UNUSED
test_vsock_tcp_server_create (void UDO_UNUSED **state)
{
	struct cando_vsock_tcp *server = NULL;

	struct cando_vsock_tcp_server_create_info server_info;

	server_info.vcid = 1;
	server_info.port = 7777;
	server_info.connections = 1;
	server = cando_vsock_tcp_server_create(NULL, &server_info);
	assert_non_null(server);

	cando_vsock_tcp_destroy(server);
}

/*************************************************
 * End of test_vsock_tcp_server_create functions *
 *************************************************/


/***************************************************
 * Start of test_vsock_tcp_client_create functions *
 ***************************************************/

static void UDO_UNUSED
test_vsock_tcp_client_create (void UDO_UNUSED **state)
{
	struct cando_vsock_tcp *client = NULL;

	struct cando_vsock_tcp_client_create_info client_info;

	client_info.vcid = 1;
	client_info.port = 7777;
	client = cando_vsock_tcp_client_create(NULL, &client_info);
	assert_non_null(client);

	cando_vsock_tcp_destroy(client);
}

/*************************************************
 * End of test_vsock_tcp_client_create functions *
 *************************************************/


/****************************************************
 * Start of test_vsock_tcp_accept_connect functions *
 ****************************************************/

static void
p_test_vsock_tcp_accept_connect_client (void)
{
	int err = -1;

	struct cando_vsock_tcp *client = NULL;

	struct cando_vsock_tcp_client_create_info client_info;

	client_info.vcid = 1;
	client_info.port = 7777;
	client = cando_vsock_tcp_client_create(NULL, &client_info);
	assert_non_null(client);

	err = cando_vsock_tcp_client_connect(client);
	assert_int_equal(err, 0);

	cando_vsock_tcp_destroy(client);

	exit(0);
}


static void UDO_UNUSED
test_vsock_tcp_accept_connect (void UDO_UNUSED **state)
{
	pid_t pid;

	int client_sock = -1;

	struct cando_vsock_tcp *server = NULL;

	struct cando_vsock_tcp_server_create_info server_info;

	cando_log_set_level(UDO_LOG_ALL);

	server_info.vcid = 1;
	server_info.port = 7777;
	server_info.connections = 1;
	server = cando_vsock_tcp_server_create(NULL, &server_info);
	assert_non_null(server);

	pid = fork();
	if (pid == 0) {
		p_test_vsock_tcp_accept_connect_client();
	}

	client_sock = cando_vsock_tcp_server_accept(server, NULL);
	assert_int_not_equal(client_sock, -1);

	waitpid(pid, NULL, -1);

	close(client_sock);
	cando_vsock_tcp_destroy(server);

	usleep(2000);
}

/**************************************************
 * End of test_vsock_tcp_accept_connect functions *
 **************************************************/


/***********************************************
 * Start of test_vsock_tcp_send_recv functions *
 ***********************************************/

static void
p_test_vsock_tcp_send_recv_client (void)
{
	int err = -1;

	char buffer[512];

	ssize_t size = 0;

	struct cando_vsock_tcp *client = NULL;

	struct cando_vsock_tcp_client_create_info client_info;

	client_info.vcid = 1;
	client_info.port = 7777;
	client = cando_vsock_tcp_client_create(NULL, &client_info);
	assert_non_null(client);

	err = cando_vsock_tcp_client_connect(client);
	assert_int_equal(err, 0);

	memset(buffer, 'T', sizeof(buffer));
	size = cando_vsock_tcp_client_send_data(client, buffer, sizeof(buffer), 0);
	assert_int_equal(size, sizeof(buffer));

	cando_vsock_tcp_destroy(client);

	exit(0);
}


static void UDO_UNUSED
test_vsock_tcp_send_recv (void UDO_UNUSED **state)
{
	pid_t pid;

	int client_sock = -1;

	char buffer[512], buffer_two[512];

	struct cando_vsock_tcp *server = NULL;

	struct cando_vsock_tcp_server_create_info server_info;

	cando_log_set_level(UDO_LOG_ALL);

	server_info.vcid = 1;
	server_info.port = 7777;
	server_info.connections = 1;
	server = cando_vsock_tcp_server_create(NULL, &server_info);
	assert_non_null(server);

	pid = fork();
	if (pid == 0) {
		p_test_vsock_tcp_send_recv_client();
	}

	client_sock = cando_vsock_tcp_server_accept(server, NULL);
	assert_int_not_equal(client_sock, -1);

	memset(buffer, 'T', sizeof(buffer));
	cando_vsock_tcp_recv_data(client_sock, buffer_two, sizeof(buffer_two), 0);
	assert_memory_equal(buffer, buffer_two, sizeof(buffer));

	waitpid(pid, NULL, -1);

	close(client_sock);
	cando_vsock_tcp_destroy(server);

	usleep(2000);
}

/*********************************************
 * End of test_vsock_tcp_send_recv functions *
 *********************************************/


/********************************************
 * Start of test_vsock_tcp_get_fd functions *
 ********************************************/

static void UDO_UNUSED
test_vsock_tcp_get_fd (void UDO_UNUSED **state)
{
	int sock_fd = -1;

	struct cando_vsock_tcp *client = NULL;

	struct cando_vsock_tcp_client_create_info client_info;

	client_info.vcid = 1;
	client_info.port = 7777;
	client = cando_vsock_tcp_client_create(NULL, &client_info);
	assert_non_null(client);

	sock_fd = cando_vsock_tcp_get_fd(NULL);
	assert_int_equal(sock_fd, -1);

	sock_fd = cando_vsock_tcp_get_fd(client);
	assert_int_not_equal(sock_fd, -1);

	cando_vsock_tcp_destroy(client);
}

/******************************************
 * End of test_vsock_tcp_get_fd functions *
 ******************************************/


/**********************************************
 * Start of test_vsock_tcp_get_vcid functions *
 **********************************************/

static void UDO_UNUSED
test_vsock_tcp_get_vcid (void UDO_UNUSED **state)
{
	unsigned int vcid = -1;

	struct cando_vsock_tcp *server = NULL;

	struct cando_vsock_tcp_server_create_info server_info;

	server_info.vcid = 1;
	server_info.port = 7777;
	server_info.connections = 1;
	server = cando_vsock_tcp_server_create(NULL, &server_info);
	assert_non_null(server);

	vcid = cando_vsock_tcp_get_vcid(NULL);
	assert_int_equal(vcid, UINT32_MAX);

	vcid = cando_vsock_tcp_get_vcid(server);
	assert_int_equal(vcid, server_info.vcid);

	cando_vsock_tcp_destroy(server);
}

/********************************************
 * End of test_vsock_tcp_get_vcid functions *
 ********************************************/


/**********************************************
 * Start of test_vsock_tcp_get_port functions *
 **********************************************/

static void UDO_UNUSED
test_vsock_tcp_get_port (void UDO_UNUSED **state)
{
	int port = -1;

	struct cando_vsock_tcp *client = NULL;

	struct cando_vsock_tcp_client_create_info client_info;

	client_info.vcid = 1;
	client_info.port = 7777;
	client = cando_vsock_tcp_client_create(NULL, &client_info);
	assert_non_null(client);

	port = cando_vsock_tcp_get_port(NULL);
	assert_int_equal(port, -1);

	port = cando_vsock_tcp_get_port(client);
	assert_int_equal(port, client_info.port);

	cando_vsock_tcp_destroy(client);
}

/********************************************
 * End of test_vsock_tcp_get_port functions *
 ********************************************/


/************************************************
 * Start of test_vsock_tcp_get_sizeof functions *
 ************************************************/

static void UDO_UNUSED
test_vsock_tcp_get_sizeof (void UDO_UNUSED **state)
{
	int size = 0;
	size = cando_vsock_tcp_get_sizeof();
	assert_int_not_equal(size, 0);
}

/**********************************************
 * End of test_vsock_tcp_get_sizeof functions *
 **********************************************/


/****************************************************
 * Start of test_vsock_tcp_get_local_vcid functions *
 ****************************************************/

static void UDO_UNUSED
test_vsock_tcp_get_local_vcid (void UDO_UNUSED **state)
{
	unsigned int vcid = -1;
	cando_log_set_level(UDO_LOG_ALL);
	vcid = cando_vsock_tcp_get_local_vcid();
	assert_int_equal(vcid, 2);
}

/**************************************************
 * End of test_vsock_tcp_get_local_vcid functions *
 **************************************************/

int
main (void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_vsock_tcp_server_create),
		cmocka_unit_test(test_vsock_tcp_client_create),
		cmocka_unit_test(test_vsock_tcp_accept_connect),
		cmocka_unit_test(test_vsock_tcp_send_recv),
		cmocka_unit_test(test_vsock_tcp_get_fd),
		cmocka_unit_test(test_vsock_tcp_get_vcid),
		cmocka_unit_test(test_vsock_tcp_get_port),
		cmocka_unit_test(test_vsock_tcp_get_sizeof),
		cmocka_unit_test(test_vsock_tcp_get_local_vcid),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
