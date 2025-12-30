#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/*
 * Required by cmocka
 */
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "log.h"

/*******************************
 * Start of test_log functions *
 *******************************/

static void UDO_UNUSED
test_log (void UDO_UNUSED **state)
{
	cando_log_success("SUCCESS\n");
	cando_log_error("DANGER\n");
	cando_log_info("INFO\n");
	cando_log_warning("WARNING\n");

	cando_log_set_level(UDO_LOG_ERROR|UDO_LOG_INFO);
	cando_log_success("SUCCESS: After log level set\n");
	cando_log_error("DANGER: After log level set\n");
	cando_log_info("INFO: After log level set\n");
	cando_log_warning("WARNING: After log level set\n");
}

/*****************************
 * End of test_log functions *
 *****************************/


/*********************************************
 * Start of test_log_remove_colors functions *
 *********************************************/

static void UDO_UNUSED
test_log_remove_reset_colors (void UDO_UNUSED **state)
{
	cando_log_set_level(UDO_LOG_ALL);

	cando_log_remove_colors();

	cando_log_success("SUCCESS\n");
	cando_log_error("DANGER\n");
	cando_log_info("INFO\n");
	cando_log_warning("WARNING\n");

	cando_log_reset_colors();

	cando_log_success("SUCCESS: After reset\n");
	cando_log_error("DANGER: After reset\n");
	cando_log_info("INFO: After reset\n");
	cando_log_warning("WARNING: After reset\n");
}

/*******************************************
 * End of test_log_remove_colors functions *
 *******************************************/


/*************************************
 * Start of test_log_print functions *
 *************************************/

static void UDO_UNUSED
test_log_print (void UDO_UNUSED **state)
{
	cando_log_print(UDO_LOG_SUCCESS, "SUCCESS\n");
	cando_log_print(UDO_LOG_ERROR, "DANGER\n");
	cando_log_print(UDO_LOG_INFO, "INFO\n");
	cando_log_print(UDO_LOG_WARNING, "WARNING\n");

	cando_log_set_level(UDO_LOG_SUCCESS|UDO_LOG_WARNING);
	cando_log_print(UDO_LOG_SUCCESS, "SUCCESS: After log level set\n");
	cando_log_print(UDO_LOG_ERROR, "DANGER: After log level set\n");
	cando_log_print(UDO_LOG_INFO, "INFO: After log level set\n");
	cando_log_print(UDO_LOG_WARNING, "WARNING: After log level set\n");
}

/***********************************
 * End of test_log_print functions *
 ***********************************/


/********************************************
 * Start of test_log_set_write_fd functions *
 ********************************************/

static void UDO_UNUSED
test_log_set_write_fd (void UDO_UNUSED **state)
{
	int fd = -1;

	const char *test_file = "/tmp/test-file.txt";
	fd = open(test_file, O_CREAT|O_RDWR, 0644);
	assert_int_not_equal(fd, -1);

	cando_log_set_write_fd(fd);

	cando_log_print(UDO_LOG_SUCCESS, "SUCCESS\n");
	cando_log_print(UDO_LOG_ERROR, "DANGER\n");
	cando_log_print(UDO_LOG_INFO, "INFO\n");
	cando_log_print(UDO_LOG_WARNING, "WARNING\n");

	cando_log_set_level(UDO_LOG_SUCCESS|UDO_LOG_WARNING);
	cando_log_print(UDO_LOG_SUCCESS, "SUCCESS: After log level set\n");
	cando_log_print(UDO_LOG_ERROR, "DANGER: After log level set\n");
	cando_log_print(UDO_LOG_INFO, "INFO: After log level set\n");
	cando_log_print(UDO_LOG_WARNING, "WARNING: After log level set\n");

	close(fd);
	remove(test_file);
}

/******************************************
 * End of test_log_set_write_fd functions *
 ******************************************/


/*************************************
 * Start of test_log_error functions *
 *************************************/

static void UDO_UNUSED
test_log_error (void UDO_UNUSED **state)
{
	const char *error = NULL;
	unsigned int err_code = 0;

	struct some_context
	{
		struct cando_log_error_struct err;
		int someData;
	} context;

	/* Test NULL passed */
	err_code = cando_log_get_error_code(NULL);
	error = cando_log_get_error(NULL);
	assert_int_equal(err_code, UINT32_MAX);
	assert_null(error);

	/* Test common error passed */
	cando_log_set_error(&context, UDO_LOG_ERR_INCORRECT_DATA, "");
	err_code = cando_log_get_error_code(&context);
	error = cando_log_get_error(&context);
	assert_int_equal(err_code, UDO_LOG_ERR_INCORRECT_DATA);
	assert_string_equal(error, "[test-log.c:152] Incorrect data passed");

	/* Test context passed */
	memset(&context, 0, sizeof(context));

	context.err.code = UDO_LOG_ERR_UNCOMMON;
	strncpy(context.err.buffer, "BUFFFER 2 Copy", sizeof(context.err.buffer)-1);
	err_code = cando_log_get_error_code(&context);
	error = cando_log_get_error(&context);
	assert_int_equal(err_code, UDO_LOG_ERR_UNCOMMON);
	assert_string_equal(error, "BUFFFER 2 Copy");
}

/***********************************
 * End of test_log_error functions *
 ***********************************/


int
main (void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_log),
		cmocka_unit_test(test_log_remove_reset_colors),
		cmocka_unit_test(test_log_print),
		cmocka_unit_test(test_log_set_write_fd),
		cmocka_unit_test(test_log_error),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
