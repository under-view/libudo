#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>

/* Required by cmocka */
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "log.h"
#include "file-ops.h"

/*******************************************
 * Start of test_file_ops_create functions *
 *******************************************/

static void UDO_UNUSED
test_file_ops_create (void UDO_UNUSED **state)
{
	int ret = -1;

	struct stat fstats;

	struct udo_file_ops *flops = NULL;

	struct udo_file_ops_create_info file_info;

	memset(&fstats, 0, sizeof(fstats));
	memset(&file_info, 0, sizeof(file_info));

	file_info.fname = "/tmp/some-file.txt";
	file_info.create_dir = 1;
	flops = udo_file_ops_create(NULL, &file_info);
	assert_non_null(flops);

	udo_file_ops_destroy(flops);

	ret = stat(file_info.fname, &fstats);
	assert_int_equal(ret, 0);
	remove(file_info.fname);
}


static void UDO_UNUSED
test_file_ops_create_empty_file (void UDO_UNUSED **state)
{
	/*
	 * Remember this test tests the
	 * udo_file_ops_truncate function.
	 */
	int ret = -1;

	struct stat fstats;

	struct udo_file_ops *flops = NULL;

	struct udo_file_ops_create_info file_info;

	memset(&fstats, 0, sizeof(fstats));
	memset(&file_info, 0, sizeof(file_info));

	file_info.size = (1<<12);
	file_info.fname = "/tmp/some-file.txt";
	flops = udo_file_ops_create(NULL, &file_info);
	assert_non_null(flops);

	udo_file_ops_destroy(flops);

	ret = stat(file_info.fname, &fstats);
	assert_int_equal(ret, 0);
	assert_int_equal(fstats.st_size, file_info.size);
	remove(file_info.fname);
}

/*****************************************
 * End of test_file_ops_create functions *
 *****************************************/


/**********************************************
 * Start of test_file_ops_zero_copy functions *
 **********************************************/

static void UDO_UNUSED
test_file_ops_zero_copy (void UDO_UNUSED **state)
{
	int ret = -1;

	char buffer[32];

	const char *data = NULL;

	struct udo_file_ops *flops = NULL, *flops_two = NULL;

	struct udo_file_ops_create_info file_info;
	struct udo_file_ops_zero_copy_info zcopy_info;

	memset(&zcopy_info, 0, sizeof(zcopy_info));
	memset(&file_info, 0, sizeof(file_info));

	file_info.create_pipe = 0x01;
	file_info.fname = TESTER_FILE_ONE;
	flops = udo_file_ops_create(NULL, &file_info);
	assert_non_null(flops);

	file_info.create_pipe = 0x00;
	file_info.fname = "/tmp/test-file.txt";
	file_info.size = udo_file_ops_get_data_size(flops);
	flops_two = udo_file_ops_create(NULL, &file_info);
	assert_non_null(flops_two);

	zcopy_info.size = file_info.size;
	zcopy_info.in_fd = udo_file_ops_get_fd(flops);
	zcopy_info.in_off = &(off_t){0};
	zcopy_info.out_fd = udo_file_ops_get_fd(flops_two);
	zcopy_info.out_off = &(off_t){0};
	ret = udo_file_ops_zero_copy(flops, &zcopy_info);
	assert_int_equal(ret, 0x62);

	udo_file_ops_destroy(flops); flops = NULL;
	udo_file_ops_destroy(flops_two); flops_two = NULL;

	/* Re-open newly created file */
	file_info.fname = "/tmp/test-file.txt";
	file_info.size = 0;
	flops_two = udo_file_ops_create(NULL, &file_info);
	assert_non_null(flops_two);

	memset(buffer, 0, sizeof(buffer));
	data = udo_file_ops_get_line(flops_two, 1);
	memccpy(buffer, data, '\n', sizeof(buffer));
	assert_string_equal(buffer, "line one\n");

	memset(buffer, 0, sizeof(buffer));
	data = udo_file_ops_get_line(flops_two, 4);
	memccpy(buffer, data, '\n', sizeof(buffer));
	assert_string_equal(buffer, "line four : check me\n");

	udo_file_ops_destroy(flops_two);
	remove("/tmp/test-file.txt");
}

/********************************************
 * End of test_file_ops_zero_copy functions *
 ********************************************/


/****************************************
 * Start of test_file_ops_get functions *
 ****************************************/

static void UDO_UNUSED
test_file_ops_get_data (void UDO_UNUSED **state)
{
	struct stat fstats;

	const void *data = NULL;

	struct udo_file_ops *flops = NULL;

	struct udo_file_ops_create_info file_info;

	memset(&fstats, 0, sizeof(fstats));
	memset(&file_info, 0, sizeof(file_info));

	file_info.fname = TESTER_FILE_ONE;
	flops = udo_file_ops_create(NULL, &file_info);
	assert_non_null(flops);

	data = udo_file_ops_get_data(flops, 0);
	assert_non_null(data);

	udo_file_ops_destroy(flops);
}


static void UDO_UNUSED
test_file_ops_get_line (void UDO_UNUSED **state)
{
	char buffer[32];

	struct stat fstats;

	const void *data = NULL;

	struct udo_file_ops *flops = NULL;

	struct udo_file_ops_create_info file_info;

	memset(&fstats, 0, sizeof(fstats));
	memset(&file_info, 0, sizeof(file_info));

	file_info.fname = TESTER_FILE_ONE;
	flops = udo_file_ops_create(NULL, &file_info);
	assert_non_null(flops);

	data = udo_file_ops_get_line(flops, 0);
	assert_null(data);

	memset(buffer, 0, sizeof(buffer));
	data = udo_file_ops_get_line(flops, 1);
	memccpy(buffer, data, '\n', sizeof(buffer));
	assert_string_equal(buffer, "line one\n");

	memset(buffer, 0, sizeof(buffer));
	data = udo_file_ops_get_line(flops, 2);
	memccpy(buffer, data, '\n', sizeof(buffer));
	assert_string_equal(buffer, "line two\n");

	memset(buffer, 0, sizeof(buffer));
	data = udo_file_ops_get_line(flops, 4);
	memccpy(buffer, data, '\n', sizeof(buffer));
	assert_string_equal(buffer, "line four : check me\n");

	udo_file_ops_destroy(flops);
}


static void UDO_UNUSED
test_file_ops_get_line_count (void UDO_UNUSED **state)
{
	unsigned long int line_count = 0;

	struct udo_file_ops *flops = NULL;

	struct udo_file_ops_create_info file_info;

	memset(&file_info, 0, sizeof(file_info));

	file_info.fname = TESTER_FILE_ONE;
	flops = udo_file_ops_create(NULL, &file_info);
	assert_non_null(flops);

	line_count = udo_file_ops_get_line_count(flops);
	assert_int_equal(line_count, 8);

	udo_file_ops_destroy(flops);
}


static void UDO_UNUSED
test_file_ops_get_fd (void UDO_UNUSED **state)
{
	int fd = -1;

	struct udo_file_ops *flops = NULL;

	struct udo_file_ops_create_info file_info;

	memset(&file_info, 0, sizeof(file_info));

	file_info.fname = TESTER_FILE_ONE;
	flops = udo_file_ops_create(NULL, &file_info);
	assert_non_null(flops);

	fd = udo_file_ops_get_fd(flops);
	assert_int_not_equal(fd, -1);

	fd = udo_file_ops_get_fd(NULL);
	assert_int_equal(fd, -1);

	udo_file_ops_destroy(flops);
}


static void UDO_UNUSED
test_file_ops_get_alloc_size (void UDO_UNUSED **state)
{
	size_t size = 0;

	struct udo_file_ops *flops = NULL;

	struct udo_file_ops_create_info file_info;

	memset(&file_info, 0, sizeof(file_info));

	file_info.size = 3 * (1<<12);
	file_info.fname = "/tmp/tester-file.txt";
	flops = udo_file_ops_create(NULL, &file_info);
	assert_non_null(flops);

	size = udo_file_ops_get_alloc_size(flops);
	assert_int_equal(size, file_info.size);

	size = udo_file_ops_get_alloc_size(NULL);
	assert_int_equal(size, -1);

	udo_file_ops_destroy(flops);
	remove(file_info.fname);
}


static void UDO_UNUSED
test_file_ops_get_data_size (void UDO_UNUSED **state)
{
	size_t size = 0;

	struct udo_file_ops *flops = NULL;

	struct udo_file_ops_create_info file_info;

	memset(&file_info, 0, sizeof(file_info));

	file_info.fname = TESTER_FILE_ONE;
	flops = udo_file_ops_create(NULL, &file_info);
	assert_non_null(flops);

	size = udo_file_ops_get_data_size(flops);
	assert_int_not_equal(size, -1);

	size = udo_file_ops_get_data_size(NULL);
	assert_int_equal(size, -1);

	udo_file_ops_destroy(flops);
}


static void UDO_UNUSED
test_file_ops_get_filename (void UDO_UNUSED **state)
{
	const char *fname = NULL;

	struct udo_file_ops *flops = NULL;

	struct udo_file_ops_create_info file_info;

	memset(&file_info, 0, sizeof(file_info));

	file_info.fname = TESTER_FILE_ONE;
	flops = udo_file_ops_create(NULL, &file_info);
	assert_non_null(flops);

	fname = udo_file_ops_get_filename(flops);
	assert_string_equal(fname, "tester-file-one.txt");

	fname = udo_file_ops_get_filename(NULL);
	assert_null(fname);

	udo_file_ops_destroy(flops);
}


static void UDO_UNUSED
test_file_ops_get_dirname (void UDO_UNUSED **state)
{
	const char *dname = NULL;

	char dir_path[UDO_PAGE_SIZE];

	struct udo_file_ops *flops = NULL;

	struct udo_file_ops_create_info file_info;

	memset(&file_info, 0, sizeof(file_info));

	file_info.fname = TESTER_FILE_ONE;
	flops = udo_file_ops_create(NULL, &file_info);
	assert_non_null(flops);

	strncpy(dir_path, TESTER_FILE_ONE, UDO_PAGE_SIZE);
	dirname(dir_path);

	dname = udo_file_ops_get_dirname(flops);
	assert_string_equal(dname, dir_path);

	dname = udo_file_ops_get_dirname(NULL);
	assert_null(dname);

	udo_file_ops_destroy(flops);
}


static void UDO_UNUSED
test_file_ops_get_full_path (void UDO_UNUSED **state)
{
	const char *full_path = NULL;

	struct udo_file_ops *flops = NULL;

	char full_path_buff[UDO_PAGE_SIZE+256];

	struct udo_file_ops_create_info file_info;
	memset(&file_info, 0, sizeof(file_info));

	file_info.fname = TESTER_FILE_ONE;
	flops = udo_file_ops_create(NULL, &file_info);
	assert_non_null(flops);

	strncpy(full_path_buff, TESTER_FILE_ONE, UDO_PAGE_SIZE);

	full_path = udo_file_ops_get_full_path(flops);
	assert_string_equal(full_path, full_path_buff);

	full_path = udo_file_ops_get_full_path(NULL);
	assert_null(full_path);

	udo_file_ops_destroy(flops);
}

/**************************************
 * End of test_file_ops_get functions *
 **************************************/


/****************************************
 * Start of test_file_ops_set functions *
 ****************************************/

static void UDO_UNUSED
test_file_ops_set_data (void UDO_UNUSED **state)
{
	int ret = -1;

	const void *data = NULL;

	struct udo_file_ops *flops = NULL;

	struct udo_file_ops_create_info file_info;
	struct udo_file_ops_set_data_info sd_info;

	memset(&sd_info, 0, sizeof(sd_info));
	memset(&file_info, 0, sizeof(file_info));

	file_info.fname = "/tmp/testing-one.txt";
	file_info.size = 1 << 12;
	flops = udo_file_ops_create(NULL, &file_info);
	assert_non_null(flops);

	sd_info.offset = 0;
	sd_info.data = "Adding data on line one.\n";
	sd_info.size = strnlen(sd_info.data, file_info.size);
	ret = udo_file_ops_set_data(flops, &sd_info);
	assert_int_equal(ret, 0);

	data = udo_file_ops_get_data(flops, 0);
	assert_string_equal(data, sd_info.data);

	udo_file_ops_destroy(flops);
	remove(file_info.fname);
}

/**************************************
 * End of test_file_ops_set functions *
 **************************************/


/****************************************
 * End of test_file_ops_reset functions *
 ****************************************/

static void UDO_UNUSED
test_file_ops_reset_full_path (void UDO_UNUSED **state)
{
	const char *dname = NULL;
	const char *full_path = NULL;

	struct udo_file_ops *flops = NULL;

	char full_path_buff[UDO_PAGE_SIZE+256];

	struct udo_file_ops_create_info file_info;
	memset(&file_info, 0, sizeof(file_info));

	file_info.fname = TESTER_FILE_ONE;
	flops = udo_file_ops_create(NULL, &file_info);
	assert_non_null(flops);

	strncpy(full_path_buff, TESTER_FILE_ONE, UDO_PAGE_SIZE);

	full_path = udo_file_ops_get_full_path(flops);
	assert_string_equal(full_path, full_path_buff);

	udo_file_ops_reset_full_path(flops);

	dname = udo_file_ops_get_dirname(flops);
	assert_string_not_equal(dname, full_path_buff);

	udo_file_ops_destroy(flops);
}

/****************************************
 * End of test_file_ops_reset functions *
 ****************************************/


/***********************************************
 * Start of test_file_ops_get_sizeof functions *
 ***********************************************/

static void UDO_UNUSED
test_file_ops_get_sizeof (void UDO_UNUSED **state)
{
	int size = 0;
	size = udo_file_ops_get_sizeof();
	assert_int_not_equal(size, 0);
}

/*********************************************
 * End of test_file_ops_get_sizeof functions *
 *********************************************/


/*************************************************
 * Start of test_file_ops_set_fd_flags functions *
 *************************************************/

static void UDO_UNUSED
test_file_ops_set_fd_flags (void UDO_UNUSED **state)
{
	int err = 0, fd = -1;

	struct udo_file_ops *flops = NULL;

	struct udo_file_ops_create_info file_info;
	memset(&file_info, 0, sizeof(file_info));

	file_info.fname = "/tmp/testing-one.txt";
	file_info.size = 1 << 9;
	flops = udo_file_ops_create(NULL, &file_info);
	assert_non_null(flops);

	fd = udo_file_ops_get_fd(flops);
	err = udo_file_ops_set_fd_flags(fd, O_NONBLOCK);
	assert_int_equal(err, 0);

	udo_file_ops_destroy(flops);
	remove(file_info.fname);
}

/***********************************************
 * End of test_file_ops_set_fd_flags functions *
 ***********************************************/


/***********************************************
 * Start of test_file_ops_remove_dir functions *
 ***********************************************/

static void UDO_UNUSED
test_file_ops_remove_dir (void UDO_UNUSED **state)
{
	int ret = -1;

	struct stat fstats;

	struct udo_file_ops *flops = NULL;

	struct udo_file_ops_create_info file_info;

	memset(&fstats, 0, sizeof(fstats));
	memset(&file_info, 0, sizeof(file_info));

	file_info.fname = "/tmp/data/dir3/cool/some-file.txt";
	file_info.create_dir = 1;
	flops = udo_file_ops_create(NULL, &file_info);
	assert_non_null(flops);
	udo_file_ops_destroy(flops);

	ret = udo_file_ops_remove_dir("/tmp/data");
	assert_int_equal(ret, 0);
}

/*********************************************
 * End of test_file_ops_remove_dir functions *
 *********************************************/

int
main (void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_file_ops_create),
		cmocka_unit_test(test_file_ops_create_empty_file),
		cmocka_unit_test(test_file_ops_zero_copy),
		cmocka_unit_test(test_file_ops_get_data),
		cmocka_unit_test(test_file_ops_get_line),
		cmocka_unit_test(test_file_ops_get_line_count),
		cmocka_unit_test(test_file_ops_get_fd),
		cmocka_unit_test(test_file_ops_get_alloc_size),
		cmocka_unit_test(test_file_ops_get_data_size),
		cmocka_unit_test(test_file_ops_get_filename),
		cmocka_unit_test(test_file_ops_get_dirname),
		cmocka_unit_test(test_file_ops_get_full_path),
		cmocka_unit_test(test_file_ops_set_data),
		cmocka_unit_test(test_file_ops_reset_full_path),
		cmocka_unit_test(test_file_ops_get_sizeof),
		cmocka_unit_test(test_file_ops_set_fd_flags),
		cmocka_unit_test(test_file_ops_remove_dir),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
