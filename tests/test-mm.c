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
#include "mm.h"

/************************************
 * Start of test_mm_alloc functions *
 ************************************/

static void UDO_UNUSED
test_mm_alloc (void UDO_UNUSED **state)
{
	struct cando_mm *mm = NULL;

	cando_log_set_level(UDO_LOG_ALL);

	mm = cando_mm_alloc(NULL, UDO_PAGE_SIZE*2);
	assert_non_null(mm);

	/* Test remapping */
	mm = cando_mm_alloc(mm, UDO_PAGE_SIZE*5);
	assert_non_null(mm);

	cando_mm_destroy(mm);
}

/**********************************
 * End of test_mm_alloc functions *
 **********************************/


/****************************************
 * Start of test_mm_sub_alloc functions *
 ****************************************/

static void UDO_UNUSED
test_mm_sub_alloc (void UDO_UNUSED **state)
{
	struct cando_mm *mm = NULL;

	char *red = NULL, *blue = NULL;

	cando_log_set_level(UDO_LOG_ALL);

	mm = cando_mm_alloc(NULL, UDO_PAGE_SIZE*5);
	assert_non_null(mm);

	red = cando_mm_sub_alloc(mm, UDO_PAGE_SIZE);
	assert_non_null(red);

	memset(red, 'G', UDO_PAGE_SIZE);

	blue = cando_mm_sub_alloc(mm, UDO_PAGE_SIZE);
	assert_non_null(blue);

	memset(blue, 'G', UDO_PAGE_SIZE);
	assert_memory_equal(red, blue, UDO_PAGE_SIZE);

	memset(blue, 'R', UDO_PAGE_SIZE);
	assert_memory_not_equal(red, blue, UDO_PAGE_SIZE);

	/* Test over allocation */
	blue = cando_mm_sub_alloc(mm, UDO_PAGE_SIZE*10);
	assert_null(blue);

	cando_log_print(UDO_LOG_ERROR, "%s\n", cando_log_get_error(mm));

	cando_mm_destroy(mm);
}

/**************************************
 * End of test_mm_sub_alloc functions *
 **************************************/


/***********************************
 * Start of test_mm_free functions *
 ***********************************/

static void UDO_UNUSED
test_mm_free (void UDO_UNUSED **state)
{
	struct cando_mm *mm = NULL;

	char *red = NULL, *blue = NULL, *green = NULL;

	cando_log_set_level(UDO_LOG_ALL);

	mm = cando_mm_alloc(NULL, UDO_PAGE_SIZE*8);
	assert_non_null(mm);

	red = cando_mm_sub_alloc(mm, UDO_PAGE_SIZE);
	assert_non_null(red);

	blue = cando_mm_sub_alloc(mm, UDO_PAGE_SIZE);
	assert_non_null(blue);

	green = cando_mm_sub_alloc(mm, UDO_PAGE_SIZE);
	assert_non_null(green);

	memset(red, 'G', UDO_PAGE_SIZE);
	memset(blue, 'B', UDO_PAGE_SIZE);
	memset(green, 'G', UDO_PAGE_SIZE);

	cando_mm_free(mm, blue, UDO_PAGE_SIZE);
	assert_memory_equal(red, blue, UDO_PAGE_SIZE);

	cando_mm_destroy(mm);
}

/*********************************
 * End of test_mm_free functions *
 *********************************/

int
main (void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_mm_alloc),
		cmocka_unit_test(test_mm_sub_alloc),
		cmocka_unit_test(test_mm_free),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
