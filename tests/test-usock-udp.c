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
#include "usock-udp.h"

#define TESTING_UNIX_SOCK "/tmp/testing.sock"
#define TESTING_CLIENT_UNIX_SOCK "/tmp/cli-testing.sock"

/***************************************************
 * Start of test_usock_udp_server_create functions *
 ***************************************************/

static void UDO_UNUSED
test_usock_udp_server_create (void UDO_UNUSED **state)
{
	struct udo_usock_udp *server = NULL;

	struct udo_usock_udp_server_create_info server_info;

	udo_log_set_level(UDO_LOG_ALL);

	server_info.unix_path = TESTING_UNIX_SOCK;
	server = udo_usock_udp_server_create(NULL, &server_info);
	assert_non_null(server);

	udo_usock_udp_destroy(server);
}

/*************************************************
 * End of test_usock_udp_server_create functions *
 *************************************************/


/***************************************************
 * Start of test_usock_udp_client_create functions *
 ***************************************************/

static void UDO_UNUSED
test_usock_udp_client_create (void UDO_UNUSED **state)
{
	struct udo_usock_udp *client = NULL;

	struct udo_usock_udp_client_create_info client_info;

	client_info.srv_unix_path = TESTING_UNIX_SOCK;
	client_info.cli_unix_path = TESTING_CLIENT_UNIX_SOCK;
	client = udo_usock_udp_client_create(NULL, &client_info);
	assert_non_null(client);

	udo_usock_udp_destroy(client);
}

/*************************************************
 * End of test_usock_udp_client_create functions *
 *************************************************/


/***********************************************
 * Start of test_usock_udp_send_recv functions *
 ***********************************************/

static void
p_test_usock_udp_send_recv_client (void)
{
	int err = -1;

	char buffer[512];

	ssize_t size = 0;

	const int accept = 0x44;

	struct udo_usock_udp *client = NULL;

	struct udo_usock_udp_client_create_info client_info;

	client_info.srv_unix_path = TESTING_UNIX_SOCK;
	client_info.cli_unix_path = TESTING_CLIENT_UNIX_SOCK;
	client = udo_usock_udp_client_create(NULL, &client_info);
	assert_non_null(client);

	/* Connect client to server */
	err = udo_usock_udp_client_send_data(client, &accept, sizeof(int), NULL);
	assert_int_equal(err, sizeof(int));

	memset(buffer, 'T', sizeof(buffer));
	size = udo_usock_udp_client_send_data(client, buffer, sizeof(buffer), 0);
	assert_int_equal(size, sizeof(buffer));

	udo_usock_udp_destroy(client);

	exit(0);
}


static void UDO_UNUSED
test_usock_udp_send_recv (void UDO_UNUSED **state)
{
	pid_t pid;

	struct sockaddr_un addr;

	char buffer[512], buffer_two[512];

	int client_sock = -1, err = -1, data;

	struct udo_usock_udp *server = NULL;

	struct udo_usock_udp_server_create_info server_info;

	udo_log_set_level(UDO_LOG_ALL);

	server_info.unix_path = TESTING_UNIX_SOCK;
	server = udo_usock_udp_server_create(NULL, &server_info);
	assert_non_null(server);

	pid = fork();
	if (pid == 0) {
		p_test_usock_udp_send_recv_client();
	}

	err = udo_usock_udp_server_recv_data(server, &data,
					sizeof(int), &addr, NULL);
	assert_int_equal(err, sizeof(int));
	assert_int_equal(data, 0x44);

	memset(buffer, 'T', sizeof(buffer));
	err = udo_usock_udp_server_recv_data(server, buffer_two,
		sizeof(buffer_two), &addr, NULL);
	assert_int_equal(err, sizeof(buffer_two));
	assert_memory_equal(buffer, buffer_two, sizeof(buffer));

	waitpid(pid, NULL, -1);

	close(client_sock);
	udo_usock_udp_destroy(server);
}

/*********************************************
 * End of test_usock_udp_send_recv functions *
 *********************************************/


/********************************************
 * Start of test_usock_udp_get_fd functions *
 ********************************************/

static void UDO_UNUSED
test_usock_udp_get_fd (void UDO_UNUSED **state)
{
	int sock_fd = -1;

	struct udo_usock_udp *client = NULL;

	struct udo_usock_udp_client_create_info client_info;

	client_info.srv_unix_path = TESTING_UNIX_SOCK;
	client_info.cli_unix_path = TESTING_CLIENT_UNIX_SOCK;
	client = udo_usock_udp_client_create(NULL, &client_info);
	assert_non_null(client);

	sock_fd = udo_usock_udp_get_fd(NULL);
	assert_int_equal(sock_fd, -1);

	sock_fd = udo_usock_udp_get_fd(client);
	assert_int_not_equal(sock_fd, -1);

	udo_usock_udp_destroy(client);
}

/******************************************
 * End of test_usock_udp_get_fd functions *
 ******************************************/


/***************************************************
 * Start of test_usock_udp_get_unix_path functions *
 ***************************************************/

static void UDO_UNUSED
test_usock_udp_get_unix_path (void UDO_UNUSED **state)
{
	const char *unix_path = NULL;

	struct udo_usock_udp *client = NULL;

	struct udo_usock_udp_client_create_info client_info;

	client_info.srv_unix_path = TESTING_UNIX_SOCK;
	client_info.cli_unix_path = TESTING_CLIENT_UNIX_SOCK;
	client = udo_usock_udp_client_create(NULL, &client_info);
	assert_non_null(client);

	unix_path = udo_usock_udp_get_unix_path(NULL);
	assert_null(unix_path);

	unix_path = udo_usock_udp_get_unix_path(client);
	assert_string_equal(unix_path, client_info.cli_unix_path);

	udo_usock_udp_destroy(client);
}

/*************************************************
 * End of test_usock_udp_get_unix_path functions *
 *************************************************/


/************************************************
 * Start of test_usock_udp_get_sizeof functions *
 ************************************************/

static void UDO_UNUSED
test_usock_udp_get_sizeof (void UDO_UNUSED **state)
{
	int size = 0;
	size = udo_usock_udp_get_sizeof();
	assert_int_not_equal(size, 0);
}

/**********************************************
 * End of test_usock_udp_get_sizeof functions *
 **********************************************/

int
main (void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_usock_udp_server_create),
		cmocka_unit_test(test_usock_udp_client_create),
		cmocka_unit_test(test_usock_udp_send_recv),
		cmocka_unit_test(test_usock_udp_get_fd),
		cmocka_unit_test(test_usock_udp_get_unix_path),
		cmocka_unit_test(test_usock_udp_get_sizeof),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
