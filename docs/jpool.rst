.. default-domain:: C

jpool (Job Pool)
================

Header: udo/jpool.h

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

1. :c:struct:`udo_jpool_job`
#. :c:struct:`udo_jpool_queue`
#. :c:struct:`udo_jpool`
#. :c:struct:`udo_jpool_create_info`

=========
Functions
=========

1. :c:func:`udo_jpool_create`
#. :c:func:`udo_jpool_add_job`
#. :c:func:`udo_jpool_destroy`
#. :c:func:`udo_jpool_sizeof`

API Documentation
~~~~~~~~~~~~~~~~~

=======================
udo_jpool_job (private)
=======================

| Structure defining information about the job to execute.
| Is used in :c:func:`udo_jpool_add_job` to add job a to the job
| queue.

.. c:struct:: udo_jpool_job

	.. c:member::
		void (*func)(void *arg);
		void *arg;

	:c:member:`func`
		| Function pointer to a function for thread to execute.

	:c:member:`arg`
		| Argument to pass to function.

=========================
udo_jpool_queue (private)
=========================

| Structure defining information about the job queue.

.. c:struct:: udo_jpool_queue

	.. c:member::
		udo_atomic_u32 *job_free;
		udo_atomic_u32 *front;
		udo_atomic_u32 *rear;

	:c:member:`job_free`
		| Futex used to wake threads or put them
    		| to sleep if jobs are available.

	:c:member:`front`
		| Byte offset to the front of the queue.

	:c:member:`rear`
		| Byte offset to the rear of the queue.

===================
udo_jpool (private)
===================

| Structure defining the udo_jpool (Udo Job Pool) instance.

.. c:struct:: udo_jpool

	.. c:member::
		struct udo_log_error_struct err;
		bool                        free;
		unsigned int                thread_count;
		pthread_t                   thread_ids[THREADS_MAX];
		size_t                      queue_sz;
		struct udo_jpool_queue      queue;
		void                        *queue_data;

	:c:member:`err`
		| Stores information about the error that occured
		| for the given instance and may later be retrieved
		| by caller.

	:c:member:`free`
		| If structure allocated with `calloc(3)`_ member will be
		| set to true so that, we know to call `free(3)`_ when
		| destroying the instance.

	:c:member:`thread_count`
		| Amount of threads in the pool.

	:c:member:`thread_ids`
		| Array of POSIX thread ID's.

	:c:member:`queue_sz`
		| Byte size of :c:member:`queue`.

	:c:member:`queue`
		| Structure keeping track of current jobs
		| a thread can run.

	:c:member:`queue_data`
		| Shared memory buffer storing actual
		| addresses to jobs.

=========================================================================================================================================

=====================
udo_jpool_create_info
=====================

| Structure passed to :c:func:`udo_jpool_create` used
| to define size of shared memory queue and
| the amount of threads to create.

.. c:struct:: udo_jpool_create_info

	.. c:member::
		size_t       size;
		unsigned int count;

	:c:member:`size`
		| Size of shared memory used to store
		| the queue shared between all threads.

	:c:member:`count`
		| Amount of threads able to read and
		| write to and from the shared memory
		| block.

.. c:function:: struct udo_jpool *udo_jpool_create(struct udo_jpool *jpool, const void *jpool_info);

| Creates pool a threads to execute task.

	.. list-table:: Job Queue
		:header-rows: 1

		* - Variable
		  - Offset In Bytes
		  - Size In Bytes
		  - Initial Value
		* - Job Free
		  - 0
		  - 4
		  - 0
		* - Front Of Queue
		  - 4
		  - 4
		  - 24
		* - Rear Of Queue
		  - 8
		  - 4
		  - 24
		* - :c:struct:`udo_jpool_queue`
		  - 12 to size of queue
		  - Size of queue - 12
		  - | ``NULL`` or pointer to function
		    | and function argument.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - jpool
		  - | May be ``NULL`` or a pointer to a ``struct`` :c:struct:`udo_jpool`.
		    | If ``NULL`` memory will be allocated and return to
		    | caller. If not ``NULL`` address passed will be used
		    | to store the newly created ``struct`` :c:struct:`udo_jpool`
		    | instance.
		* - jpool_info
		  - | Implementation uses a pointer to a
		    | ``struct`` :c:struct:`udo_jpool_create_info`.
		    | no other implementation may be passed to
		    | this parameter.

	Returns:
		| **on success:** Pointer to a ``struct`` :c:struct:`udo_jpool`
		| **on failure:** ``NULL``

=========================================================================================================================================

=================
udo_jpool_add_job
=================

.. c:function:: uint32_t udo_jpool_add_job(struct udo_jpool *jpool, void (*func)(void *arg), void *arg);

| Adds a job to the job queue for threads
| to then later execute.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - jpool
		  - | Pointer to a valid ``struct`` :c:struct:`udo_jpool`.
		* - func
		  - | Pointer to function that a seperate
		    | thread will execute.
		* - arg
		  - | Pointer to a memory which will be
		    | passed as the argument to ``func``.

	Returns:
		| **on success:** Queue buffer byte offset
		| **on failure:** -1 or ``UINT32_MAX``

=========================================================================================================================================

=================
udo_jpool_destroy
=================

.. c:function:: void udo_jpool_destroy(struct udo_jpool *jpool);

| Frees any allocated memory and closes FD's (if open) created after
| :c:func:`udo_jpool_create` call.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - jpool
		  - | Pointer to a valid ``struct`` :c:struct:`udo_jpool`.

=========================================================================================================================================

====================
udo_jpool_get_sizeof
====================

.. c:function:: int udo_jpool_get_sizeof(void);

| Returns size of the internal structure. So,
| if caller decides to allocate memory outside
| of API interface they know the exact amount
| of bytes.

	Returns:
		| **on success:** sizeof(``struct`` :c:struct:`udo_jpool`)
		| **on failure:** sizeof(``struct`` :c:struct:`udo_jpool`)

=========================================================================================================================================

.. _calloc(3): https://www.man7.org/linux/man-pages/man3/malloc.3.html
.. _free(3): https://www.man7.org/linux/man-pages/man3/free.3.html
