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

	fux = udo_futex_create(0);
	assert_null(fux);

	fux = udo_futex_create(1);
	assert_non_null(fux);

	udo_futex_destroy(fux);
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

	fux = udo_futex_create(1);
	assert_non_null(fux);

	pid = fork();
	if (pid == 0) {
		udo_futex_unlock(fux);

		exit(0);
	}

	udo_futex_lock(fux);

	wait(NULL);

	udo_futex_destroy(fux);
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

	fux = udo_futex_create(1);
	assert_non_null(fux);

	pid = fork();
	if (pid == 0) {
		udo_futex_unlock_force(fux);

		exit(0);
	}

	udo_futex_lock(fux);
	assert_int_equal(errno, EINTR);

	wait(NULL);

	udo_futex_destroy(fux);
}

/*************************************************
 * End of test_futex_lock_unlock_force functions *
 *************************************************/

int
main (void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_futex_create),
		cmocka_unit_test(test_futex_lock_unlock),
		cmocka_unit_test(test_futex_lock_unlock_force),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
