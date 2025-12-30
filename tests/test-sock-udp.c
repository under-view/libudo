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
#include "sock-udp.h"

/**************************************************
 * Start of test_sock_udp_server_create functions *
 **************************************************/

static void UDO_UNUSED
test_sock_udp_server_create (void UDO_UNUSED **state)
{
	struct cando_sock_udp *server = NULL;

	struct cando_sock_udp_server_create_info server_info;

	cando_log_set_level(UDO_LOG_ALL);

	server_info.ipv6 = 0;
	server_info.port = 7777;
	server_info.ip_addr = "127.0.0.1";
	server = cando_sock_udp_server_create(NULL, &server_info);
	assert_non_null(server);

	cando_sock_udp_destroy(server);
}

/************************************************
 * End of test_sock_udp_server_create functions *
 ************************************************/


/**************************************************
 * Start of test_sock_udp_client_create functions *
 **************************************************/

static void UDO_UNUSED
test_sock_udp_client_create (void UDO_UNUSED **state)
{
	struct cando_sock_udp *client = NULL;

	struct cando_sock_udp_client_create_info client_info;

	client_info.ipv6 = 0;
	client_info.port = 7777;
	client_info.ip_addr = "127.0.0.1";
	client = cando_sock_udp_client_create(NULL, &client_info);
	assert_non_null(client);

	cando_sock_udp_destroy(client);
}

/************************************************
 * End of test_sock_udp_client_create functions *
 ************************************************/


/***************************************************
 * Start of test_sock_udp_accept_connect functions *
 ***************************************************/

static void
p_test_sock_udp_accept_connect_client (void)
{
	int err = -1;

	const int accept = 0x44;

	struct cando_sock_udp *client = NULL;

	struct cando_sock_udp_client_create_info client_info;

	client_info.ipv6 = 0;
	client_info.port = 7777;
	client_info.ip_addr = "127.0.0.1";
	client = cando_sock_udp_client_create(NULL, &client_info);
	assert_non_null(client);

	err = cando_sock_udp_client_connect(client);
	assert_int_equal(err, 0);

	/* Connect client to server */
	err = cando_sock_udp_client_send_data(client, &accept, sizeof(int), NULL);
	assert_int_equal(err, sizeof(int));

	cando_sock_udp_destroy(client);

	exit(0);
}


static void UDO_UNUSED
test_sock_udp_accept_connect (void UDO_UNUSED **state)
{
	pid_t pid;

	struct sockaddr_in6 addr;

	int client_sock = -1, err = -1, data;

	struct cando_sock_udp *server = NULL;

	struct cando_sock_udp_server_create_info server_info;

	cando_log_set_level(UDO_LOG_ALL);

	server_info.ipv6 = 0;
	server_info.port = 7777;
	server_info.ip_addr = "127.0.0.1";
	server = cando_sock_udp_server_create(NULL, &server_info);
	assert_non_null(server);

	pid = fork();
	if (pid == 0) {
		p_test_sock_udp_accept_connect_client();
	}

	err = cando_sock_udp_server_recv_data(server, &data,
					sizeof(int), &addr, NULL);
	assert_int_equal(err, sizeof(int));
	assert_int_equal(data, 0x44);

	client_sock = cando_sock_udp_server_accept(server, &addr, server_info.ipv6);
	assert_int_not_equal(client_sock, -1);

	waitpid(pid, NULL, -1);

	close(client_sock);
	cando_sock_udp_destroy(server);
}

/*************************************************
 * End of test_sock_udp_accept_connect functions *
 *************************************************/


/**********************************************
 * Start of test_sock_udp_send_recv functions *
 **********************************************/

static void
p_test_sock_udp_send_recv_client (void)
{
	int err = -1;

	char buffer[512];

	ssize_t size = 0;

	const int accept = 0x44;

	struct cando_sock_udp *client = NULL;

	struct cando_sock_udp_client_create_info client_info;

	client_info.ipv6 = 1;
	client_info.port = 7777;
	client_info.ip_addr = "::1";
	client = cando_sock_udp_client_create(NULL, &client_info);
	assert_non_null(client);

	err = cando_sock_udp_client_connect(client);
	assert_int_equal(err, 0);

	/* Connect client to server */
	err = cando_sock_udp_client_send_data(client, &accept, sizeof(int), NULL);
	assert_int_equal(err, sizeof(int));

	usleep(1000);

	memset(buffer, 'T', sizeof(buffer));
	size = cando_sock_udp_client_send_data(client, buffer, sizeof(buffer), 0);
	assert_int_equal(size, sizeof(buffer));

	cando_sock_udp_destroy(client);

	exit(0);
}


static void UDO_UNUSED
test_sock_udp_send_recv (void UDO_UNUSED **state)
{
	pid_t pid;

	struct sockaddr_in6 addr;

	char buffer[512], buffer_two[512];

	int client_sock = -1, err = -1, data;

	struct cando_sock_udp *server = NULL;

	struct cando_sock_udp_server_create_info server_info;

	cando_log_set_level(UDO_LOG_ALL);

	server_info.ipv6 = 1;
	server_info.port = 7777;
	server_info.ip_addr = "::1";
	server = cando_sock_udp_server_create(NULL, &server_info);
	assert_non_null(server);

	pid = fork();
	if (pid == 0) {
		p_test_sock_udp_send_recv_client();
	}

	err = cando_sock_udp_server_recv_data(server, &data,
					sizeof(int), &addr, NULL);
	assert_int_equal(err, sizeof(int));
	assert_int_equal(data, 0x44);

	client_sock = cando_sock_udp_server_accept(server, &addr, server_info.ipv6);
	assert_int_not_equal(client_sock, -1);

	memset(buffer, 'T', sizeof(buffer));
	err = cando_sock_udp_recv_data(client_sock, buffer_two, sizeof(buffer_two), NULL, 0);
	assert_int_equal(err, sizeof(buffer_two));
	assert_memory_equal(buffer, buffer_two, sizeof(buffer));

	waitpid(pid, NULL, -1);

	close(client_sock);
	cando_sock_udp_destroy(server);
}

/********************************************
 * End of test_sock_udp_send_recv functions *
 ********************************************/


/*******************************************
 * Start of test_sock_udp_get_fd functions *
 *******************************************/

static void UDO_UNUSED
test_sock_udp_get_fd (void UDO_UNUSED **state)
{
	int sock_fd = -1;

	struct cando_sock_udp *client = NULL;

	struct cando_sock_udp_client_create_info client_info;

	client_info.ipv6 = 0;
	client_info.port = 7777;
	client_info.ip_addr = "127.0.0.1";
	client = cando_sock_udp_client_create(NULL, &client_info);
	assert_non_null(client);

	sock_fd = cando_sock_udp_get_fd(NULL);
	assert_int_equal(sock_fd, -1);

	sock_fd = cando_sock_udp_get_fd(client);
	assert_int_not_equal(sock_fd, -1);

	cando_sock_udp_destroy(client);
}

/*****************************************
 * End of test_sock_udp_get_fd functions *
 *****************************************/


/************************************************
 * Start of test_sock_udp_get_ip_addr functions *
 ************************************************/

static void UDO_UNUSED
test_sock_udp_get_ip_addr (void UDO_UNUSED **state)
{
	const char *ip_addr = NULL;

	struct cando_sock_udp *client = NULL;

	struct cando_sock_udp_client_create_info client_info;

	client_info.ip_addr = "::1";
	client_info.port = 7777;
	client_info.ipv6 = 1;
	client = cando_sock_udp_client_create(NULL, &client_info);
	assert_non_null(client);

	ip_addr = cando_sock_udp_get_ip_addr(NULL);
	assert_null(ip_addr);

	ip_addr = cando_sock_udp_get_ip_addr(client);
	assert_string_equal(ip_addr, client_info.ip_addr);

	cando_sock_udp_destroy(client);
}

/**********************************************
 * End of test_sock_udp_get_ip_addr functions *
 **********************************************/


/*********************************************
 * Start of test_sock_udp_get_port functions *
 *********************************************/

static void UDO_UNUSED
test_sock_udp_get_port (void UDO_UNUSED **state)
{
	int port = -1;

	struct cando_sock_udp *client = NULL;

	struct cando_sock_udp_client_create_info client_info;

	client_info.ipv6 = 0;
	client_info.port = 7777;
	client_info.ip_addr = "127.0.0.1";
	client = cando_sock_udp_client_create(NULL, &client_info);
	assert_non_null(client);

	port = cando_sock_udp_get_port(NULL);
	assert_int_equal(port, -1);

	port = cando_sock_udp_get_port(client);
	assert_int_equal(port, client_info.port);

	cando_sock_udp_destroy(client);
}

/*******************************************
 * End of test_sock_udp_get_port functions *
 *******************************************/


/***********************************************
 * Start of test_sock_udp_get_sizeof functions *
 ***********************************************/

static void UDO_UNUSED
test_sock_udp_get_sizeof (void UDO_UNUSED **state)
{
	int size = 0;
	size = cando_sock_udp_get_sizeof();
	assert_int_not_equal(size, 0);
}

/*********************************************
 * End of test_sock_udp_get_sizeof functions *
 *********************************************/

int
main (void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_sock_udp_server_create),
		cmocka_unit_test(test_sock_udp_client_create),
		cmocka_unit_test(test_sock_udp_accept_connect),
		cmocka_unit_test(test_sock_udp_send_recv),
		cmocka_unit_test(test_sock_udp_get_fd),
		cmocka_unit_test(test_sock_udp_get_ip_addr),
		cmocka_unit_test(test_sock_udp_get_port),
		cmocka_unit_test(test_sock_udp_get_sizeof),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
