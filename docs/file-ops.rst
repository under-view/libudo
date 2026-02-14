.. default-domain:: C

file-ops (File Operations)
==========================

Header: udo/file-ops.h

Table of contents (click to go)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

======
Macros
======

=====
Enums
=====

======
Unions
======

=======
Structs
=======

1. :c:struct:`udo_file_ops`
#. :c:struct:`udo_file_ops_create_info`
#. :c:struct:`udo_file_ops_zero_copy_info`
#. :c:struct:`udo_file_ops_set_data_info`

=========
Functions
=========

1. :c:func:`udo_file_ops_create`
#. :c:func:`udo_file_ops_truncate`
#. :c:func:`udo_file_ops_zero_copy`
#. :c:func:`udo_file_ops_get_data`
#. :c:func:`udo_file_ops_get_line`
#. :c:func:`udo_file_ops_get_line_count`
#. :c:func:`udo_file_ops_get_fd`
#. :c:func:`udo_file_ops_get_alloc_size`
#. :c:func:`udo_file_ops_get_data_size`
#. :c:func:`udo_file_ops_get_filename`
#. :c:func:`udo_file_ops_get_dirname`
#. :c:func:`udo_file_ops_set_data`
#. :c:func:`udo_file_ops_destroy`
#. :c:func:`udo_file_ops_get_sizeof`

API Documentation
~~~~~~~~~~~~~~~~~

======================
udo_file_ops (private)
======================

| Structure defining UDO File Operations instance.

.. c:struct:: udo_file_ops

	.. c:member::
		struct udo_log_error_struct err;
		bool                        free;
		bool                        protect;
		int                         fd;
		int                         pipe_fds[2];
		char                        fname[FILE_NAME_LEN_MAX];
		size_t                      data_sz;
		void                        *data;

	:c:member:`err`
		| Stores information about the error that occured
		| for the given instance and may later be retrieved
		| by caller.

	:c:member:`free`
		| If structure allocated with `calloc(3)`_ member will be
		| set to true so that, we know to call `free(3)`_ when
		| destroying the instance.

	:c:member:`protect`
		| If true sets `mmap(2)`_ file pages to read only.

	:c:member:`fd`
		| File descriptor to open file.

	:c:member:`pipe_fds`
		| File descriptors associated with an open pipe.
		| **pipe_fds[0]** - Read end of the pipe
		| **pipe_fds[1]** - Write end of the pipe

	:c:member:`fname`
		| String representing the file name.

	:c:member:`dname`
		| String representing the directory ``fname`` resides in.

	:c:member:`dname`
		| String representing the directory ``fname`` resides in.

	:c:member:`alloc_sz`
		| Total size of the file that was mapped with `mmap(2)`_.

	:c:member:`data_sz`
		| Total size of data written to file. Used when destroying
		| the ``struct`` :c:struct:`udo_file_ops` context to truncate file to a
		| smaller size than :c:member:`alloc_sz`.

	:c:member:`data`
		| Pointer to `mmap(2)`_ file data.

========================
udo_file_ops_create_info
========================

.. c:struct:: udo_file_ops_create_info

	.. c:member::
		const char        *fname;
		unsigned long int size;
		off_t             offset;
		unsigned char     create_pipe : 1;
		unsigned char     create_dir : 1;
		unsigned char     protect : 1;

	:c:member:`fname`
		| Full path to file caller wants to `open(2)`_ | `creat(2)`_.
		| Size in characters is restricted to ``4096``.

	:c:member:`size`
		| Size in bytes caller newly created file will be.
		| If :c:member:`create_pipe` is true this member is ignored.

	:c:member:`offset`
		| Offset within the file to `mmap(2)`_.
		| If :c:member:`create_pipe` is true this member is ignored.

	:c:member:`create_pipe`
		| Boolean to enable/disable creation of a `pipe(2)`_.

	:c:member:`create_dir`
		| Boolean to enable/disable the creation of folders
    		| :c:member:`fname` resides in.

	:c:member:`protect`
		| Boolean to enable/disable setting of `mmap(2)`_ file
		| pages to read only or not.

===================
udo_file_ops_create
===================

.. c:function:: struct udo_file_ops *udo_file_ops_create(struct udo_file_ops *flops, const void *file_info);

| Creates or opens caller define file.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - flops
		  - | May be ``NULL`` or a pointer to a ``struct`` :c:struct:`udo_file_ops`.
		    | If ``NULL`` memory will be allocated and return to
		    | caller. If not ``NULL`` address passed will be used
		    | to store the newly created ``struct`` :c:struct:`udo_file_ops`
		    | instance.
		* - file_info
		  - | Pointer to a ``struct`` :c:struct:`udo_file_ops_create_info`.
		    | The use of pointer to a void is to limit amount
		    | of columns required to define a function.

	Returns:
		| **on success:** Pointer to a ``struct`` :c:struct:`udo_file_ops`
		| **on failure:** ``NULL``

=========================================================================================================================================

=====================
udo_file_ops_truncate
=====================

.. c:function:: int udo_file_ops_truncate(struct udo_file_ops *flops, const off_t size);

| Adjust file to a size of precisely length bytes.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - flops
		  - | Pointer to a valid ``struct`` :c:struct:`udo_file_ops`.
		* - size
		  - | Size in bytes file will be `truncate(2)`_'d to.

	Returns:
		| **on success:** 0
		| **on failure:** # < 0

=========================================================================================================================================

===========================
udo_file_ops_zero_copy_info
===========================

.. c:struct:: udo_file_ops_zero_copy_info

	.. c:member::
		size_t size;
		int    in_fd;
		off_t  *in_off;
		int    out_fd;
		off_t  *out_off;

	:c:member:`size`
		| Total size of the data to copy.

	:c:member:`in_fd`
		| Input file descriptor to copy data from.

	:c:member:`in_off`
		| Byte offset in the :c:member:`in_fd` open file to copy from.
		| **NOTE:** `splice(2)`_ may updates the variable.

	:c:member:`out_fd`
		| Output file descriptor to copy data to.

	:c:member:`out_off`
		| Byte offset in the :c:member:`out_fd` open file to copy X amount
		| of data from the given offset.
		| **NOTE:** `splice(2)`_ may updates the variable

======================
udo_file_ops_zero_copy
======================

.. c:function:: ssize_t udo_file_ops_zero_copy(struct udo_file_ops *flops, const void *file_info);

| Sets the data in a file at a given offset up to a given size
| without copying the buffer into userspace.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - flops
		  - | Pointer to a valid ``struct`` :c:struct:`udo_file_ops`.
		* - file_info
		  - | Pointer to a ``struct`` :c:struct:`udo_file_ops_zero_copy_info`.
		    | The use of pointer to a void is to limit amount
		    | of columns required to define a function.

	Returns:
		| **on success:** Amount of bytes `splice(2)`_ to/from a `pipe(2)`_
		| **on failure:** -1

=========================================================================================================================================

=====================
udo_file_ops_get_data
=====================

.. c:function:: const void *udo_file_ops_get_data(struct udo_file_ops *flops, const size_t offset);

| Returns file data stored at a given offset.
| Caller would have to copy into a secondary
| buffer to exclude new line character like bellow.

	.. code-block:: c

		size_t len;
		char buffer[32];
		const void *data = NULL;

		memset(buffer, 0, sizeof(buffer));
		data = udo_file_ops_get_data(flops, 54);
		memccpy(buffer, data, '\n', sizeof(buffer));
		len = strnlen(buffer, sizeof(buffer));
		buffer[len-1] = '\0';
		fprintf(stdout, "%s", buffer);

		// OR
		data = udo_file_ops_get_data(flops, 54);
		fprintf(stdout, "%.*s\n", 32, data);

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - flops
		  - | Pointer to a valid ``struct`` :c:struct:`udo_file_ops`.
		* - offset
		  - | Byte offset within the file.

	Returns:
		| **on success:** Pointer to file data at a given offset
		| **on failure:** ``NULL``

=========================================================================================================================================

=====================
udo_file_ops_get_line
=====================

.. c:function:: const char *udo_file_ops_get_line(struct udo_file_ops *flops, const size_t line);

| Returns file data stored at a given line.
| Caller would have to copy into a secondary
| buffer to exclude new line character like bellow.

	.. code-block:: c

		size_t len;
		char buffer[32];
		const char *line = NULL;

		memset(buffer, 0, sizeof(buffer));
		line = udo_file_ops_get_line(flops, 4);
		memccpy(buffer, line, '\n', sizeof(buffer));
		len = strnlen(buffer, sizeof(buffer));
		buffer[len-1] = '\0';
		fprintf(stdout, "%s", buffer);

		// OR
		line = udo_file_ops_get_line(flops, 4);
		fprintf(stdout, "%.*s\n", 32, line);

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - flops
		  - | Pointer to a valid ``struct`` :c:struct:`udo_file_ops`.
		* - line
		  - | Line in file to get data from.

	Returns:
		| **on success:** Pointer to file data at a given line
		| **on failure:** ``NULL``

=========================================================================================================================================

===========================
udo_file_ops_get_line_count
===========================

.. c:function:: size_t udo_file_ops_get_line_count(struct udo_file_ops *flops);

| Returns the amount of lines a file contains.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - flops
		  - | Pointer to a valid ``struct`` :c:struct:`udo_file_ops`.

	Returns:
		| **on success:** Line count
		| **on failure:** size_t max size or -1

=========================================================================================================================================

===================
udo_file_ops_get_fd
===================

.. c:function:: int udo_file_ops_get_fd(struct udo_file_ops *flops);

| Returns file descriptor to open file.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - flops
		  - | Pointer to a valid ``struct`` :c:struct:`udo_file_ops`.

	Returns:
		| **on success:** File descriptor to open file
		| **on failure:** -1

=========================================================================================================================================

===========================
udo_file_ops_get_alloc_size
===========================

.. c:function:: size_t udo_file_ops_get_alloc_size(struct udo_file_ops *flops);

| Returns size of the `mmap(2)`_'d buffer associated
| with the open file.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - flops
		  - | Pointer to a valid ``struct`` :c:struct:`udo_file_ops`.

	Returns:
		| **on success:** Size of the `mmap(2)`_ buffer
		| **on failure:** size_t max size or -1

=========================================================================================================================================

==========================
udo_file_ops_get_data_size
==========================

.. c:function:: size_t udo_file_ops_get_data_size(struct udo_file_ops *flops);

| Returns size of the data within the open file.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - flops
		  - | Pointer to a valid ``struct`` :c:struct:`udo_file_ops`.

	Returns:
		| **on success:** Size of data in file
		| **on failure:** size_t max size or -1

=========================================================================================================================================

=========================
udo_file_ops_get_filename
=========================

.. c:function:: const char *udo_file_ops_get_filename(struct udo_file_ops *flops);

| Return file name of open file associated with
| the ``struct`` :c:struct:`udo_file_ops` context.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - flops
		  - | Pointer to a valid ``struct`` :c:struct:`udo_file_ops`.

	Returns:
		| **on success:** File name of open file
		| **on failure:** ``NULL``

=========================================================================================================================================

========================
udo_file_ops_get_dirname
========================

.. c:function:: const char *udo_file_ops_get_dirname(struct udo_file_ops *flops);

| Return directory path of open file associated
| with the ``struct`` :c:struct:`udo_file_ops` context.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - flops
		  - | Pointer to a valid ``struct`` :c:struct:`udo_file_ops`.

	Returns:
		| **on success:** Directory path a file resides in
		| **on failure:** ``NULL``

=========================================================================================================================================

==========================
udo_file_ops_set_data_info
==========================

.. c:struct:: udo_file_ops_set_data_info

	.. c:member::
		size_t     offset;
		size_t     size;
		const void *data;

	:c:member:`offset`
		| Byte offset within the file.

	:c:member:`size`
		| Size in bytes to copy into file at :c:member:`offset`.

	:c:member:`data`
		| Data to copy at the given file offset.

=====================
udo_file_ops_set_data
=====================

.. c:function:: int udo_file_ops_set_data(struct udo_file_ops *flops, const void *file_info);

| Sets data in a file at a given offset up to a given size.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - flops
		  - | Pointer to a valid ``struct`` :c:struct:`udo_file_ops`.
		* - file_info
		  - | Pointer to a ``struct`` :c:struct:`udo_file_ops_set_data_info`.
		    | The use of pointer to a void is to limit amount
		    | of columns required to define a function.

	Returns:
		| **on success:** 0
		| **on failure:** -1

=========================================================================================================================================

====================
udo_file_ops_destroy
====================

.. c:function:: void udo_file_ops_destroy(struct udo_file_ops *flops);

| Frees any allocated memory and closes FD's (if open) created after
| :c:func:`udo_file_ops_create` call.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - flops
		  - | Pointer to a valid ``struct`` :c:struct:`udo_file_ops`.

=========================================================================================================================================

=======================
udo_file_ops_get_sizeof
=======================

.. c:function:: int udo_file_ops_get_sizeof(void);

| Returns size of the internal structure. So,
| if caller decides to allocate memory outside
| of API interface they know the exact amount
| of bytes.

	Returns:
		| **on success:** sizeof(struct udo_file_ops)
		| **on failure:** sizeof(struct udo_file_ops)

=========================================================================================================================================

=========================
udo_file_ops_set_fd_flags
=========================

.. c:function:: int udo_file_ops_set_fd_flags(const int fd, const int flags);

| Updates the file descriptor to new status flags.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - fd
		  - | File descriptor who's flags to update.
		* - flags
		  - | Set the file status flags to the value
		    | specified by parameter. For more information
		    | on status flags see `fcntl(2)`_ and `open(2)`_.

	Returns:
		| **on success:** 0
		| **on failure:** -1

=========================================================================================================================================

=======================
udo_file_ops_remove_dir
=======================

.. c:function:: int udo_file_ops_remove_dir(const char *dir);

| Recursively delete files and directories
| contained inside caller defined directory.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - dir
		  - | Directory to delete. Size in characters
		    | is restricted to ``4096``.
		* - flags
		  - | Set the file status flags to the value
		    | specified by parameter. For more information
		    | on status flags see `fcntl(2)`_ and `open(2)`_.

	Returns:
		| **on success:** 0
		| **on failure:** -1

=========================================================================================================================================

.. _calloc(3): https://www.man7.org/linux/man-pages/man3/malloc.3.html
.. _free(3): https://www.man7.org/linux/man-pages/man3/free.3.html
.. _mmap(2):  https://man7.org/linux/man-pages/man2/mmap.2.html
.. _fcntl(2):  https://man7.org/linux/man-pages/man2/fcntl.2.html
.. _open(2):  https://man7.org/linux/man-pages/man2/open.2.html
.. _creat(2):  https://man7.org/linux/man-pages/man2/creat.2.html
.. _pipe(2):  https://man7.org/linux/man-pages/man2/pipe.2.html
.. _truncate(2):  https://man7.org/linux/man-pages/man2/pipe.2.html
.. _splice(2):  https://man7.org/linux/man-pages/man2/splice.2.html
