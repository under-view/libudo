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

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

/* Required by cmocka */
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "log.h"
#include "futex.h"

/****************************************
 * Start of test_futex_create functions *
 ****************************************/

static void UDO_UNUSED
test_futex_create (void UDO_UNUSED **state)
{
	udo_atomic_u32 *fux;

	struct udo_futex_create_info futex_info;

	futex_info.count = 0;
	futex_info.size = UDO_PAGE_SIZE;
	fux = udo_futex_create(&futex_info);
	assert_null(fux);

	futex_info.count = UDO_PAGE_SIZE + 1;
	fux = udo_futex_create(&futex_info);
	assert_null(fux);

	futex_info.count = 3;
	fux = udo_futex_create(&futex_info);
	assert_non_null(fux);

	udo_futex_destroy(fux, futex_info.size);
}

/**************************************
 * End of test_futex_create functions *
 **************************************/


/*********************************************
 * Start of test_futex_lock_unlock functions *
 *********************************************/

static void UDO_UNUSED
test_futex_lock_unlock (void UDO_UNUSED **state)
{
	pid_t pid;

	udo_atomic_u32 *fux;

	struct udo_futex_create_info futex_info;

	futex_info.count = 1;
	futex_info.size = UDO_PAGE_SIZE;
	fux = udo_futex_create(&futex_info);
	assert_non_null(fux);

	pid = fork();
	if (pid == 0) {
		udo_futex_unlock(fux);

		exit(0);
	}

	udo_futex_lock(fux);

	wait(NULL);

	udo_futex_destroy(fux, futex_info.size);
}

/*******************************************
 * End of test_futex_lock_unlock functions *
 *******************************************/


/***************************************************
 * Start of test_futex_lock_unlock_force functions *
 ***************************************************/

static void UDO_UNUSED
test_futex_lock_unlock_force (void UDO_UNUSED **state)
{
	pid_t pid;

	udo_atomic_u32 *fux;

	struct udo_futex_create_info futex_info;

	futex_info.count = 1;
	futex_info.size = UDO_PAGE_SIZE;
	fux = udo_futex_create(&futex_info);
	assert_non_null(fux);

	pid = fork();
	if (pid == 0) {
		udo_futex_unlock_force(fux);

		exit(0);
	}

	udo_futex_lock(fux);
	assert_int_equal(errno, EINTR);

	wait(NULL);

	udo_futex_destroy(fux, futex_info.size);
}

/*************************************************
 * End of test_futex_lock_unlock_force functions *
 *************************************************/


/*******************************************
 * Start of test_futex_wait_wake functions *
 *******************************************/

static void UDO_UNUSED
test_futex_wait_wake (void UDO_UNUSED **state)
{
	pid_t pid;

	udo_atomic_u32 *fux;

	struct udo_futex_create_info futex_info;

	futex_info.count = 1;
	futex_info.size = UDO_PAGE_SIZE;
	fux = udo_futex_create(&futex_info);
	assert_non_null(fux);

	pid = fork();
	if (pid == 0) {
		udo_futex_wake(fux, 64);

		exit(0);
	}

	udo_futex_wait(fux, 64);

	wait(NULL);

	udo_futex_destroy(fux, futex_info.size);
}

/*****************************************
 * End of test_futex_wait_wake functions *
 *****************************************/


/************************************************
 * Start of test_futex_wait_wake_cond functions *
 ************************************************/

static void UDO_UNUSED
test_futex_wait_wake_cond (void UDO_UNUSED **state)
{
	pid_t pid;

	udo_atomic_u32 *fux;

	struct udo_futex_create_info futex_info;

	futex_info.count = 1;
	futex_info.size = UDO_PAGE_SIZE;
	fux = udo_futex_create(&futex_info);
	assert_non_null(fux);

	pid = fork();
	if (pid == 0) {
		__atomic_store_n(fux, 10,__ATOMIC_RELEASE);
		udo_futex_wake_cond(fux);

		exit(0);
	}

	udo_futex_wait_cond(fux, \
	__atomic_load_n(fux, __ATOMIC_ACQUIRE) == 10);

	wait(NULL);

	udo_futex_destroy(fux, futex_info.size);
}

/**********************************************
 * End of test_futex_wait_wake_cond functions *
 **********************************************/

int
main (void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_futex_create),
		cmocka_unit_test(test_futex_lock_unlock),
		cmocka_unit_test(test_futex_lock_unlock_force),
		cmocka_unit_test(test_futex_wait_wake),
		cmocka_unit_test(test_futex_wait_wake_cond),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
