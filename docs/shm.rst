.. default-domain:: C

shm (Shared Memory)
===================

Header: cando/shm.h

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

1. :c:struct:`cando_shm`
#. :c:struct:`cando_shm_create_info`
#. :c:struct:`cando_shm_data_info`

=========
Functions
=========

1. :c:func:`cando_shm_create`
#. :c:func:`cando_shm_data_read`
#. :c:func:`cando_shm_data_write`
#. :c:func:`cando_shm_get_fd`
#. :c:func:`cando_shm_get_data`
#. :c:func:`cando_shm_get_data_size`
#. :c:func:`cando_shm_destroy`
#. :c:func:`cando_shm_sizeof`

API Documentation
~~~~~~~~~~~~~~~~~

========================
cando_shm_proc (private)
========================

| Structure defining the cando_shm_proc
| (Cando Shared Memory Process) instance.

.. c:struct:: cando_shm_proc

	.. c:member::
		udo_atomic_u32  *rd_fux;
		udo_atomic_u32  *wr_fux;
		udo_atomic_addr data;
		size_t          data_sz;

	:c:member:`rd_fux`
		| Pointer to a given process read futex
		| stored in front segment of shared memory.

	:c:member:`wr_fux`
		| Pointer to a given process write futex
		| stored in front segment of shared memory.

	:c:member:`data`
		| Unsigned long long int storing the integer
		| representation of a pointer to a location
		| within shared memory. This pointer is a
		| given processes shared memory segment
		| staring address.

	:c:member:`data_sz`
		| Stores the size of a given processes
		| shared memory segment.

===================
cando_shm (private)
===================

| Structure defining the cando_shm instance.

.. c:struct:: cando_shm

	.. c:member::
		struct cando_log_error_struct err;
		bool                          free;
		int                           fd;
		char                          shm_file[SHM_FILE_NAME_MAX];
		void                          *data;
		size_t                        data_sz;
		struct cando_shm_proc         procs[SHM_PROC_MAX];

	:c:member:`err`
		| Stores information about the error that occured
		| for the given instance and may later be retrieved
		| by caller.

	:c:member:`free`
		| If structure allocated with `calloc(3)`_ member will be
		| set to true so that, we know to call `free(3)`_ when
		| destroying the instance.

	:c:member:`fd`
		| Open file descriptor to POSIX shared memory.

	:c:member:`shm_file`
		| Name of the POSIX shared memory file starting with ``'/'``.

	:c:member:`data`
		| Pointer to `mmap(2)`_ map'd shared memory data.

	:c:member:`data_sz`
		| Total size of the shared memory region mapped with `mmap(2)`_.

	:c:member:`procs`
		| An array storing the shared memory locations
		| of each processes futexes and data.

=========================================================================================================================================

=====================
cando_shm_create_info
=====================

| Structure passed to :c:func:`cando_shm_create` used
| to define shared memory file name, shm size,
| and process count.

.. c:struct:: cando_shm_create_info

	.. c:member::
		const char *shm_file;
		size_t     shm_size;

	:c:member:`shm_file`
		| Shared memory file name. Must start
		| with the character ``'/'``.

	:c:member:`shm_size`
		| Size of shared memory.

	:c:member:`proc_count`
		| Amount of processes able to read and
		| write to and from the shared memory
		| block.

.. c:function:: struct cando_shm *cando_shm_create(struct cando_shm *shm, const void *shm_info);

| Creates POSIX shared memory and futexes.
| Each process gets:
| 1. Read futex (initialized to locked)
| 2. Write futex (initialized to unlocked)
| 3. Segment within shared memory to store data

	.. list-table:: Shared Memory Block (3 Processes)
		:header-rows: 1

		* - Data Stored
		  - Offset In Bytes
		  - Byte Size
		  - Initial Value
		* - Process Count
		  - 0
		  - 4
		  - 0
		* - P1 Read Futex
		  - 4
		  - 4
		  - 1
		* - P1 Write Futex
		  - 8
		  - 4
		  - 0
		* - P2 Read Futex
		  - 12
		  - 4
		  - 1
		* - P2 Write Futex
		  - 16
		  - 4
		  - 0
		* - P3 Read Futex
		  - 20
		  - 4
		  - 1
		* - P3 Write Futex
		  - 24
		  - 4
		  - 0
		* - P1 Data Segment
		  - 28
		  - Varies based upon ``shm_size``
		  - 0
		* - P2 Data Segment
		  - Varies based upon ``shm_size``
		  - Varies based upon ``shm_size``
		  - 0
		* - P3 Data Segment
		  - Varies based upon ``shm_size``
		  - Varies based upon ``shm_size``
		  - 0

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - shm
		  - | May be ``NULL`` or a pointer to a ``struct`` :c:struct:`cando_shm`.
		    | If ``NULL`` memory will be allocated and return to
		    | caller. If not ``NULL`` address passed will be used
		    | to store the newly created ``struct`` :c:struct:`cando_shm`
		    | instance.
		* - shm_info
		  - | Implementation uses a pointer to a
		    | ``struct`` :c:struct:`cando_shm_create_info`.
		    | no other implementation may be passed to
		    | this parameter.

	Returns:
		| **on success:** Pointer to a ``struct`` :c:struct:`cando_shm`
		| **on failure:** ``NULL``

=========================================================================================================================================

===================
cando_shm_data_info
===================

| Structure defining what operations to perform
| and data to retrieve during calls to
| :c:func:`cando_shm_data_read` and :c:func:`cando_shm_data_write`.

.. c:struct:: cando_shm_data_info

	.. c:member::
		void         *data;
		size_t       size;
		unsigned int proc_index;

	:c:member:`data`
		| Pointer to a buffer that will either be used
		| to store shm data or write to shm data.

	:c:member:`size`
		| Size in bytes to read from or write to shared memory.

	:c:member:`proc_index`
		| Index of process to write data to or read data from.

===================
cando_shm_data_read
===================

.. c:function:: int cando_shm_data_read(struct cando_shm *shm, const void *shm_info);

| Reads data stored in shared memory at
| caller defined offset and writes into
| a caller defined buffer.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - shm
		  - | Pointer to a valid ``struct`` :c:struct:`cando_shm`.
		* - shm_info
		  - | Must pass a pointer to a ``struct`` :c:struct:`cando_shm_data_info`.

	Returns:
		| **on success:** 0
		| **on failure:** -1

====================
cando_shm_data_write
====================

.. c:function:: int cando_shm_data_write(struct cando_shm *shm, const void *shm_info);

| Write data stored in caller defined buffer
| into shared memory at a caller defined
| shared memory offset.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - shm
		  - | Pointer to a valid ``struct`` :c:struct:`cando_shm`.
		* - shm_info
		  - | Must pass a pointer to a ``struct`` :c:struct:`cando_shm_data_info`.

	Returns:
		| **on success:** 0
		| **on failure:** -1

=========================================================================================================================================

================
cando_shm_get_fd
================

.. c:function:: int cando_shm_get_fd(struct cando_shm *shm);

| Returns file descriptor to the POSIX shared memory
| created after call to :c:func:`cando_shm_create`.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - shm
		  - | Pointer to a valid ``struct`` :c:struct:`cando_shm`.

	Returns:
		| **on success:** File descriptor to POSIX shared memory
		| **on failure:** -1

=========================================================================================================================================

==================
cando_shm_get_data
==================

.. c:function:: void *cando_shm_get_data(struct cando_shm *shm, const unsigned int proc_index);

| Returns starting address of a processes segment
| in the `mmap(2)`_ map'd POSIX shared memory buffer
| created after call to :c:func:`cando_shm_create`.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - shm
		  - | Pointer to a valid ``struct`` :c:struct:`cando_shm`.
		* - proc_index
		  - | Process index to acquire it's shared
		    | memory segment starting address.

	Returns:
		| **on success:** Pointer to processes SHM segment
		| **on failure:** NULL

=========================================================================================================================================

=======================
cando_shm_get_data_size
=======================

.. c:function:: size_t cando_shm_get_data_size(struct cando_shm *shm, const unsigned int proc_index);

| Returns size of a given process POSIX shared
| memory segment size created after call to
| :c:func:`cando_shm_create`.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - shm
		  - | Pointer to a valid ``struct`` :c:struct:`cando_shm`.
		* - proc_index
		  - | Process index to acquire it's shared
		    | memory segment starting address.

	Returns:
		| **on success:** Size of processes SHM segment
		| **on failure:** Maximum size or -1

=========================================================================================================================================

=================
cando_shm_destroy
=================

.. c:function:: void cando_shm_destroy(struct cando_shm *shm);

| Frees any allocated memory and closes FD's (if open) created after
| :c:func:`cando_shm_create` call.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - shm
		  - | Pointer to a valid ``struct`` :c:struct:`cando_shm`.

=========================================================================================================================================

====================
cando_shm_get_sizeof
====================

.. c:function:: int cando_shm_get_sizeof(void);

| Returns size of the internal structure. So,
| if caller decides to allocate memory outside
| of API interface they know the exact amount
| of bytes.

	Returns:
		| **on success:** sizeof(``struct`` :c:struct:`cando_shm`)
		| **on failure:** sizeof(``struct`` :c:struct:`cando_shm`)

=========================================================================================================================================

.. _mmap(2): https://www.man7.org/linux/man-pages/man2/mmap.2.html
.. _calloc(3): https://www.man7.org/linux/man-pages/man3/malloc.3.html
.. _free(3): https://www.man7.org/linux/man-pages/man3/free.3.html
