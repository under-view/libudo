/*
 * MIT License
 *
 * Copyright (c) 2023-2026 Underview
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
	struct udo_mm *mm = NULL;

	udo_log_set_level(UDO_LOG_ALL);

	mm = udo_mm_alloc(NULL, UDO_PAGE_SIZE*2);
	assert_non_null(mm);

	/* Test remapping */
	mm = udo_mm_alloc(mm, UDO_PAGE_SIZE*5);
	assert_non_null(mm);

	udo_mm_destroy(mm);
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
	struct udo_mm *mm = NULL;

	char *red = NULL, *blue = NULL;

	udo_log_set_level(UDO_LOG_ALL);

	mm = udo_mm_alloc(NULL, UDO_PAGE_SIZE*5);
	assert_non_null(mm);

	red = udo_mm_sub_alloc(mm, UDO_PAGE_SIZE);
	assert_non_null(red);

	memset(red, 'G', UDO_PAGE_SIZE);

	blue = udo_mm_sub_alloc(mm, UDO_PAGE_SIZE);
	assert_non_null(blue);

	memset(blue, 'G', UDO_PAGE_SIZE);
	assert_memory_equal(red, blue, UDO_PAGE_SIZE);

	memset(blue, 'R', UDO_PAGE_SIZE);
	assert_memory_not_equal(red, blue, UDO_PAGE_SIZE);

	/* Test over allocation */
	blue = udo_mm_sub_alloc(mm, UDO_PAGE_SIZE*10);
	assert_null(blue);

	udo_log_print(UDO_LOG_ERROR, "%s\n", udo_log_get_error(mm));

	udo_mm_destroy(mm);
}


static void UDO_UNUSED
test_mm_sub_alloc_get_size (void UDO_UNUSED **state)
{
	struct udo_mm *mm = NULL;
	char *red = NULL, *blue = NULL;
	size_t red_sz = (1<<8), blue_sz = (1<<5);

	mm = udo_mm_alloc(NULL, UDO_PAGE_SIZE*5);
	assert_non_null(mm);

	red = udo_mm_sub_alloc(mm, red_sz);
	assert_non_null(red);

	blue = udo_mm_sub_alloc(mm, blue_sz);
	assert_non_null(blue);

	assert_int_equal(red_sz, udo_mm_sub_alloc_get_size(red));
	assert_int_equal(blue_sz, udo_mm_sub_alloc_get_size(blue));

	udo_mm_destroy(mm);
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
	struct udo_mm *mm = NULL;

	char *red = NULL, *blue = NULL, *green = NULL;

	udo_log_set_level(UDO_LOG_ALL);

	mm = udo_mm_alloc(NULL, UDO_PAGE_SIZE*8);
	assert_non_null(mm);

	red = udo_mm_sub_alloc(mm, UDO_PAGE_SIZE);
	assert_non_null(red);

	blue = udo_mm_sub_alloc(mm, UDO_PAGE_SIZE);
	assert_non_null(blue);

	green = udo_mm_sub_alloc(mm, UDO_PAGE_SIZE);
	assert_non_null(green);

	memset(red, 'G', UDO_PAGE_SIZE);
	memset(blue, 'B', UDO_PAGE_SIZE);
	memset(green, 'G', UDO_PAGE_SIZE);

	udo_mm_free(mm, blue);
	assert_memory_equal(red, blue, UDO_PAGE_SIZE);

	udo_mm_destroy(mm);
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
		cmocka_unit_test(test_mm_sub_alloc_get_size),
		cmocka_unit_test(test_mm_free),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
