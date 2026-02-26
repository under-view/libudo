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
	int err = -1;

	void *addr = NULL;
	char *page = NULL;

	assert_int_equal((1<<12), UDO_PAGE_SIZE);

	addr = mmap(NULL, 4 * UDO_PAGE_SIZE,
	            PROT_READ, MAP_ANONYMOUS,
		    -1, 0);
	assert_non_null(addr);

	page = UDO_PAGE_GET(((char*)addr+6555));
	assert_ptr_equal(page, ((char*)addr+UDO_PAGE_SIZE));

	err = UDO_PAGE_SET_WRITE(page, UDO_PAGE_SIZE);
	assert_int_equal(err, 0);

	*page = 'A';

	err = UDO_PAGE_SET_READ(page, UDO_PAGE_SIZE);
	assert_int_equal(err, 0);

	munmap(addr, 4 * UDO_PAGE_SIZE);
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
	unsigned int hash;
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
		cmocka_unit_test(test_macro_udo_strtou),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
