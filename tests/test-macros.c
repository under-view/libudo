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

#include <inttypes.h>
#include <sys/mman.h>

/*
 * Required by cmocka
 */
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "macros.h"

/*********************************************
 * Start of test_macro_udo_min_max functions *
 *********************************************/

static void UDO_UNUSED
test_macro_udo_min_max (void UDO_UNUSED **state)
{
	int max, min;

	max = UDO_MAX(100, 55);
	assert_int_equal(max, 100);

	max = UDO_MAX(55, 100);
	assert_int_equal(max, 100);

	min = UDO_MIN(100, 55);
	assert_int_equal(min, 55);

	min = UDO_MIN(55, 100);
	assert_int_equal(min, 55);
}

/*******************************************
 * End of test_macro_udo_min_max functions *
 *******************************************/


/************************************************
 * Start of test_macro_udo_byte_align functions *
 ************************************************/

static void UDO_UNUSED
test_macro_udo_byte_align (void UDO_UNUSED **state)
{
	int align = 7043;
	align = UDO_BYTE_ALIGN(align, UDO_PAGE_SIZE);
	assert_int_equal(align, 2*UDO_PAGE_SIZE);
}

/**********************************************
 * End of test_macro_udo_byte_align functions *
 **********************************************/


/******************************************
 * Start of test_macro_udo_page functions *
 ******************************************/

static void UDO_UNUSED
test_macro_udo_page (void UDO_UNUSED **state)
{
	size_t size = 0;
	char *addr = NULL, *page = NULL;

	size = 4 * UDO_PAGE_SIZE;
	addr = mmap(NULL, size, PROT_READ,
	            MAP_PRIVATE|MAP_ANONYMOUS,
	            -1, 0);
	assert_false((addr == (void*)-1));

	page = (char*)UDO_PAGE_GET(((char*)addr+6555));
	assert_ptr_equal(page, ((char*)addr+UDO_PAGE_SIZE));

	munmap(addr, size);
}

/****************************************
 * End of test_macro_udo_page functions *
 ****************************************/


/********************************************
 * Start of test_macro_udo_strtou functions *
 ********************************************/

static void UDO_UNUSED
test_macro_udo_strtou (void UDO_UNUSED **state)
{
	uint32_t hash;

	hash = UDO_STRTOU(NULL);
	assert_int_equal(hash, 0);

	hash = UDO_STRTOU("my-string");
	assert_int_equal(hash, 938);

	hash = UDO_STRTOU("my-string-two");
	assert_int_equal(hash, 1329);

	hash = UDO_STRTOU("tester");
	assert_int_equal(hash, 663);
}

/******************************************
 * End of test_macro_udo_strtou functions *
 ******************************************/

int
main (void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_macro_udo_min_max),
		cmocka_unit_test(test_macro_udo_byte_align),
		cmocka_unit_test(test_macro_udo_page),
		cmocka_unit_test(test_macro_udo_strtou),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
