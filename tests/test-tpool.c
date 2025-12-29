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

int
main (void)
{
	const struct CMUnitTest tests[] = {
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
