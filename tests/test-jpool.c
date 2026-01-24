#include <string.h>
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
	udo_log_print(UDO_LOG_INFO, "var = %d\n", var);
}


static void UDO_UNUSED
test_jpool_add_job (void UDO_UNUSED **state)
{
	int arg = 5;

	uint32_t ret, i;

	struct udo_jpool *jpool = NULL;

	struct udo_jpool_create_info jpool_info;
	memset(&jpool_info, 0, sizeof(jpool_info));

	jpool = udo_jpool_create(NULL, NULL);
	assert_null(jpool);

	jpool_info.count = 2;
	jpool_info.size  = UDO_PAGE_SIZE;
	jpool = udo_jpool_create(NULL, &jpool_info);
	assert_non_null(jpool);

	/* Check that queue full */
	for (i = 24; i < UDO_PAGE_SIZE;) {
		ret = udo_jpool_add_job(jpool, run_func, &arg); i += 16;
		assert_int_equal(ret, (ret>=UDO_PAGE_SIZE) ? UINT32_MAX : i);
	}

	udo_jpool_destroy(jpool);
}

/***************************************
 * End of test_jpool_add_job functions *
 ***************************************/


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
		cmocka_unit_test(test_jpool_get_sizeof),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
