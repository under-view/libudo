#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

/*
 * Required by cmocka
 */
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "log.h"
#include "macros.h"
#include "jpool.h"

/****************************************
 * Start of test_jpool_create functions *
 ****************************************/

static void UDO_UNUSED
test_jpool_create (void UDO_UNUSED **state)
{
	struct udo_jpool *jpool = NULL;

	struct udo_jpool_create_info jpool_info;
	memset(&jpool_info, 0, sizeof(jpool_info));

	jpool = udo_jpool_create(NULL, NULL);
	assert_null(jpool);

	jpool_info.count = 2;
	jpool_info.size  = 0;
	jpool = udo_jpool_create(NULL, &jpool_info);
	assert_null(jpool);

	jpool_info.count = (1<<10);
	jpool_info.size  = 0;
	jpool = udo_jpool_create(NULL, &jpool_info);
	assert_null(jpool);

	jpool_info.count = 4;
	jpool_info.size  = UDO_PAGE_SIZE;
	jpool = udo_jpool_create(NULL, &jpool_info);
	assert_non_null(jpool);

	udo_jpool_destroy(jpool);
}

/**************************************
 * End of test_jpool_create functions *
 **************************************/


/*****************************************
 * Start of test_jpool_add_job functions *
 *****************************************/

static void
run_func (void *arg)
{
	int var = *((int*)arg);
	fprintf(stdout, "var = %d\n", var);
}


static void UDO_UNUSED
test_jpool_add_job (void UDO_UNUSED **state)
{
	int ret;
	size_t i;
	struct udo_jpool *jpool;

	struct udo_jpool_create_info jpool_info;
	memset(&jpool_info, 0, sizeof(jpool_info));

	jpool = udo_jpool_create(NULL, NULL);
	assert_null(jpool);

	jpool_info.count = 4;
	jpool_info.size  = (1<<6);
	jpool = udo_jpool_create(NULL, &jpool_info);
	assert_non_null(jpool);

	int arg[2048];
	for (i = 0; i < sizeof(arg) / sizeof(arg[0]) ; i++) {
		arg[i] = i;
		ret = udo_jpool_add_job(jpool, run_func, &(arg[i]));
		assert_int_equal(ret, 0);
	}

	udo_jpool_destroy(jpool);
}

/***************************************
 * End of test_jpool_add_job functions *
 ***************************************/


/**************************************
 * Start of test_jpool_wait functions *
 **************************************/

static void UDO_UNUSED
run_func_wait (void *arg)
{
	sleep(3);
	int var = *((int*)arg);
	fprintf(stdout, "var = %d\n", var);
}


static void UDO_UNUSED
test_jpool_wait (void UDO_UNUSED **state)
{
	int ret;
	struct udo_jpool *jpool;

	struct udo_jpool_create_info jpool_info;
	memset(&jpool_info, 0, sizeof(jpool_info));

	jpool_info.count = 1;
	jpool_info.size  = (1<<4);
	jpool = udo_jpool_create(NULL, &jpool_info);
	assert_non_null(jpool);

	ret = udo_jpool_add_job(jpool, run_func_wait, &(int){33});
	assert_int_equal(ret, 0);

	udo_jpool_wait(jpool);

	udo_jpool_destroy(jpool);
}

/************************************
 * End of test_jpool_wait functions *
 ************************************/


/********************************************
 * Start of test_jpool_get_sizeof functions *
 ********************************************/

static void UDO_UNUSED
test_jpool_get_sizeof (void UDO_UNUSED **state)
{
	int size = 0;
	size = udo_jpool_get_sizeof();
	assert_int_not_equal(size, 0);
}

/******************************************
 * End of test_jpool_get_sizeof functions *
 ******************************************/

int
main (void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_jpool_create),
		cmocka_unit_test(test_jpool_add_job),
		cmocka_unit_test(test_jpool_wait),
		cmocka_unit_test(test_jpool_get_sizeof),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
