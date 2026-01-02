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
#include "macros.h"
#include "tpool.h"

/****************************************
 * Start of test_tpool_create functions *
 ****************************************/

static void UDO_UNUSED
test_tpool_create (void UDO_UNUSED **state)
{
	struct udo_tpool *tpool = NULL;

	struct udo_tpool_create_info tpool_info;
	memset(&tpool_info, 0, sizeof(tpool_info));

	tpool = udo_tpool_create(NULL, NULL);
	assert_null(tpool);

	tpool_info.count = 2;
	tpool_info.size  = UDO_PAGE_SIZE;
	tpool = udo_tpool_create(NULL, &tpool_info);
	assert_non_null(tpool);

	udo_tpool_destroy(tpool);
}

/**************************************
 * End of test_tpool_create functions *
 **************************************/


/********************************************
 * Start of test_tpool_get_sizeof functions *
 ********************************************/

static void UDO_UNUSED
test_tpool_get_sizeof (void UDO_UNUSED **state)
{
	int size = 0;
	size = udo_tpool_get_sizeof();
	assert_int_not_equal(size, 0);
}

/******************************************
 * End of test_tpool_get_sizeof functions *
 ******************************************/

int
main (void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_tpool_create),
		cmocka_unit_test(test_tpool_get_sizeof),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
