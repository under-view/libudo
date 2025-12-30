#include <string.h>
#include <unistd.h>

/*
 * Required by cmocka
 */
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "log.h"
#include "shm.h"

/**************************************
 * Start of test_shm_create functions *
 **************************************/

static void UDO_UNUSED
test_shm_create (void UDO_UNUSED **state)
{
	struct cando_shm *shm = NULL;

	struct cando_shm_create_info shm_info;
	memset(&shm_info, 0, sizeof(shm_info));

	/* Test shm_size zero */
	shm_info.proc_count = 2;
	shm_info.shm_file   = "/kms-shm-testing";
	shm = cando_shm_create(NULL, &shm_info);
	assert_null(shm);

	/* Test shm no leading '/' */
	shm_info.shm_file = "kms-shm-testing";
	shm_info.shm_size = UDO_PAGE_SIZE;
	shm = cando_shm_create(NULL, &shm_info);
	assert_null(shm);

	/* Test shm name to long */
	shm_info.shm_file = "/kms-shm-testing-XXXXXXXXXXXXXXX";
	shm = cando_shm_create(NULL, &shm_info);
	assert_null(shm);

	/* Test process count invalid */
	shm_info.shm_file   = "/kms-shm-testing";
	shm_info.shm_size   = UDO_PAGE_SIZE;
	shm_info.proc_count = (1<<5);
	shm = cando_shm_create(NULL, &shm_info);
	assert_null(shm);

	/* Test shm create successfullly */
	shm_info.proc_count = 2;
	shm = cando_shm_create(NULL, &shm_info);
	assert_non_null(shm);

	cando_shm_destroy(shm);
}

/************************************
 * End of test_shm_create functions *
 ************************************/


/************************************
 * Start of test_shm_data functions *
 ************************************/

static void UDO_UNUSED
test_shm_data (void **state UDO_UNUSED)
{
	pid_t pid;

	struct some_data {
		char buf[128];
		int value;
	};

	pid = fork();
	if (pid == 0) {
		int err = -1;

		struct cando_shm *shm = NULL;

		struct some_data sdata;
		struct cando_shm_create_info shm_info;
		struct cando_shm_data_info shm_data_info;

		shm_info.proc_count = 2;
		shm_info.shm_file   = "/kms-shm-testing";
		shm_info.shm_size   = UDO_PAGE_SIZE;
		shm = cando_shm_create(NULL, &shm_info);
		assert_non_null(shm);

		sdata.value = 821;
		memset(sdata.buf, 'T', sizeof(sdata.buf));

		shm_data_info.proc_index = 0;
		shm_data_info.size = sizeof(sdata);
		shm_data_info.data = (void*) &sdata;
		err = cando_shm_data_write(shm, &shm_data_info);
		assert_int_equal(err, 0);

		cando_shm_destroy(shm);
	} else {
		int err = -1;

		char buf[128];

		struct cando_shm *shm = NULL;

		struct some_data sdata;
		struct cando_shm_create_info shm_info;
		struct cando_shm_data_info shm_data_info;

		memset(buf, 'T', sizeof(buf));

		shm_info.proc_count = 2;
		shm_info.shm_file   = "/kms-shm-testing";
		shm_info.shm_size   = UDO_PAGE_SIZE;
		shm = cando_shm_create(NULL, &shm_info);
		assert_non_null(shm);

		shm_data_info.proc_index = 0;
		shm_data_info.size = sizeof(sdata);
		shm_data_info.data = (void*) &sdata;
		err = cando_shm_data_read(shm, &shm_data_info);
		assert_int_equal(err, 0);
		assert_int_equal(sdata.value, 821);
		assert_memory_equal(sdata.buf, buf, sizeof(buf));

		cando_shm_destroy(shm);
	}
}

/**********************************
 * End of test_shm_data functions *
 **********************************/


/**************************************
 * Start of test_shm_get_fd functions *
 **************************************/

static void UDO_UNUSED
test_shm_get_fd (void **state UDO_UNUSED)
{
	int fd = -1;

	struct cando_shm *shm = NULL;

	struct cando_shm_create_info shm_info;
	memset(&shm_info, 0, sizeof(shm_info));

	shm_info.proc_count = 2;
	shm_info.shm_file   = "/kms-shm-testing";
	shm_info.shm_size   = UDO_PAGE_SIZE;
	shm = cando_shm_create(NULL, &shm_info);
	assert_non_null(shm);

	fd = cando_shm_get_fd(NULL);
	assert_int_equal(fd, -1);

	fd = cando_shm_get_fd(shm);
	assert_int_not_equal(fd, -1);

	cando_shm_destroy(shm);
}

/************************************
 * End of test_shm_get_fd functions *
 ************************************/


/****************************************
 * Start of test_shm_get_data functions *
 ****************************************/

static void UDO_UNUSED
test_shm_get_data (void **state UDO_UNUSED)
{
	char buffer[512];

	void *shm_data = NULL;

	struct cando_shm *shm = NULL;

	struct cando_shm_create_info shm_info;
	memset(&shm_info, 0, sizeof(shm_info));

	memset(buffer, 'Q', sizeof(buffer));

	shm_info.proc_count = 2;
	shm_info.shm_file   = "/kms-shm-testing";
	shm_info.shm_size   = UDO_PAGE_SIZE;
	shm = cando_shm_create(NULL, &shm_info);
	assert_non_null(shm);

	shm_data = cando_shm_get_data(NULL, 0);
	assert_null(shm_data);

	shm_data = cando_shm_get_data(shm, (1<<6));
	assert_null(shm_data);

	shm_data = cando_shm_get_data(shm, 0);
	assert_non_null(shm_data);

	memset(shm_data, 'Q', sizeof(buffer));
	assert_memory_equal(shm_data, buffer, sizeof(buffer));

	cando_shm_destroy(shm);
}

/**************************************
 * End of test_shm_get_data functions *
 **************************************/


/*********************************************
 * Start of test_shm_get_data_size functions *
 *********************************************/

static void UDO_UNUSED
test_shm_get_data_size (void **state UDO_UNUSED)
{
	struct cando_shm *shm = NULL;

	size_t data_sz = 0, seg_sz = 0;

	struct cando_shm_create_info shm_info;
	memset(&shm_info, 0, sizeof(shm_info));

	shm_info.proc_count = 2;
	shm_info.shm_file   = "/kms-shm-testing";
	shm_info.shm_size   = UDO_PAGE_SIZE;
	shm = cando_shm_create(NULL, &shm_info);
	assert_non_null(shm);

	data_sz = cando_shm_get_data_size(NULL, 0);
	assert_int_equal(data_sz, -1);

	data_sz = sizeof(udo_atomic_u32) + (2 * sizeof(udo_atomic_u32) * shm_info.proc_count);
	seg_sz = (shm_info.shm_size - data_sz) / shm_info.proc_count;

	data_sz = cando_shm_get_data_size(shm, 0);
	assert_int_equal(data_sz, seg_sz);

	cando_shm_destroy(shm);
}

/*******************************************
 * End of test_shm_get_data_size functions *
 *******************************************/


/******************************************
 * Start of test_shm_get_sizeof functions *
 ******************************************/

static void UDO_UNUSED
test_shm_get_sizeof (void UDO_UNUSED **state)
{
	int size = 0;
	size = cando_shm_get_sizeof();
	assert_int_not_equal(size, 0);
}

/****************************************
 * End of test_shm_get_sizeof functions *
 ****************************************/

int
main (void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_shm_create),
		cmocka_unit_test(test_shm_data),
		cmocka_unit_test(test_shm_get_fd),
		cmocka_unit_test(test_shm_get_data),
		cmocka_unit_test(test_shm_get_data_size),
		cmocka_unit_test(test_shm_get_sizeof),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
