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

#include <stdio.h>

/*
 * Required by cmocka
 */
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "version.h"

/***************************************
 * Start of test_version_get functions *
 ***************************************/

static void UDO_UNUSED
test_version_get (void UDO_UNUSED **state)
{
	char buf[64];

	snprintf(buf, \
		sizeof(buf), \
		"%u.%u.%u", \
		UDO_VERSION_MAJOR, \
		UDO_VERSION_MINOR, \
		UDO_VERSION_PATCH);

	assert_string_equal(buf, udo_version_get());
}


static void UDO_UNUSED
test_version_get_major (void UDO_UNUSED **state)
{
	assert_uint_equal(UDO_VERSION_MAJOR, udo_version_get_major());
}


static void UDO_UNUSED
test_version_get_minor (void UDO_UNUSED **state)
{
	assert_uint_equal(UDO_VERSION_MINOR, udo_version_get_minor());
}


static void UDO_UNUSED
test_version_get_patch (void UDO_UNUSED **state)
{
	assert_uint_equal(UDO_VERSION_PATCH, udo_version_get_patch());
}

/*************************************
 * End of test_version_get functions *
 *************************************/

int
main (void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_version_get),
		cmocka_unit_test(test_version_get_major),
		cmocka_unit_test(test_version_get_minor),
		cmocka_unit_test(test_version_get_patch),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
