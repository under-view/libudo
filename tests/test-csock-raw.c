#include <stdlib.h>
#include <stdio.h>
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
#include "csock-raw.h"

/********************************************
 * Start of test_csock_raw_create functions *
 ********************************************/

static void UDO_UNUSED
test_csock_raw_create (void UDO_UNUSED **state)
{
	struct udo_csock_raw *csock = NULL;

	struct udo_csock_raw_create_info csock_info;

	csock_info.iface = TEST_VCAN_IFACE;
	csock = udo_csock_raw_create(NULL, &csock_info);
	assert_non_null(csock);

	udo_csock_raw_destroy(csock);
}

/******************************************
 * End of test_csock_raw_create functions *
 ******************************************/


/***********************************************
 * Start of test_csock_raw_send_recv functions *
 ***********************************************/

static void
p_test_csock_raw_send_recv_client (void)
{
	int err = -1;

	struct can_frame frame;

	struct udo_csock_raw *csock = NULL;

	struct udo_csock_raw_create_info csock_info;

	csock_info.iface = TEST_VCAN_IFACE;
	csock = udo_csock_raw_create(NULL, &csock_info);
	assert_non_null(csock);

	usleep(2000);

	frame.len = 5;
	frame.can_id = 0x555;
	snprintf((char*)frame.data, sizeof(frame.data), "Hello");
	err = udo_csock_raw_send_data(csock, &frame, NULL);
	assert_int_equal(err, sizeof(struct can_frame));

	udo_csock_raw_destroy(csock);

	exit(0);
}


static void UDO_UNUSED
test_csock_raw_send_recv (void UDO_UNUSED **state)
{
	pid_t pid;

	int err = -1;

	struct can_frame frame;

	struct udo_csock_raw *csock = NULL;

	struct udo_csock_raw_create_info csock_info;

	udo_log_set_level(UDO_LOG_ALL);

	pid = fork();
	if (pid == 0) {
		p_test_csock_raw_send_recv_client();
	}

	csock_info.iface = TEST_VCAN_IFACE;
	csock = udo_csock_raw_create(NULL, &csock_info);
	assert_non_null(csock);

	err = udo_csock_raw_recv_data(csock, &frame, NULL);
	assert_int_equal(err, sizeof(struct can_frame));

	assert_int_equal(frame.can_id, 0x555);
	assert_string_equal((char*)frame.data, "Hello");

	waitpid(pid, NULL, 0);

	udo_csock_raw_destroy(csock);
}

/*********************************************
 * End of test_csock_raw_send_recv functions *
 *********************************************/


/********************************************
 * Start of test_csock_raw_get_fd functions *
 ********************************************/

static void UDO_UNUSED
test_csock_raw_get_fd (void UDO_UNUSED **state)
{
	int sock_fd = -1;

	struct udo_csock_raw *csock = NULL;

	struct udo_csock_raw_create_info csock_info;

	csock_info.iface = TEST_VCAN_IFACE;
	csock = udo_csock_raw_create(NULL, &csock_info);
	assert_non_null(csock);

	sock_fd = udo_csock_raw_get_fd(NULL);
	assert_int_equal(sock_fd, -1);

	sock_fd = udo_csock_raw_get_fd(csock);
	assert_int_not_equal(sock_fd, -1);

	udo_csock_raw_destroy(csock);
}

/******************************************
 * End of test_csock_raw_get_fd functions *
 ******************************************/


/***********************************************
 * Start of test_csock_raw_get_iface functions *
 ***********************************************/

static void UDO_UNUSED
test_csock_raw_get_iface (void UDO_UNUSED **state)
{
	const char *iface = NULL;

	struct udo_csock_raw *csock = NULL;

	struct udo_csock_raw_create_info csock_info;

	csock_info.iface = TEST_VCAN_IFACE;
	csock = udo_csock_raw_create(NULL, &csock_info);
	assert_non_null(csock);

	iface = udo_csock_raw_get_iface(NULL);
	assert_null(iface);

	iface = udo_csock_raw_get_iface(csock);
	assert_string_equal(iface, csock_info.iface);

	udo_csock_raw_destroy(csock);
}

/*********************************************
 * End of test_csock_raw_get_iface functions *
 *********************************************/


/************************************************
 * Start of test_csock_raw_get_sizeof functions *
 ************************************************/

static void UDO_UNUSED
test_csock_raw_get_sizeof (void UDO_UNUSED **state)
{
	int size = 0;
	size = udo_csock_raw_get_sizeof();
	assert_int_not_equal(size, 0);
}

/**********************************************
 * End of test_csock_raw_get_sizeof functions *
 **********************************************/

int
main (void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_csock_raw_create),
		cmocka_unit_test(test_csock_raw_send_recv),
		cmocka_unit_test(test_csock_raw_get_fd),
		cmocka_unit_test(test_csock_raw_get_iface),
		cmocka_unit_test(test_csock_raw_get_sizeof),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
