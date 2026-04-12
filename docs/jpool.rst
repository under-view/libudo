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
#. :c:struct:`udo_jpool_thread`
#. :c:struct:`udo_jpool`
#. :c:struct:`udo_jpool_create_info`

=========
Functions
=========

1. :c:func:`udo_jpool_create`
#. :c:func:`udo_jpool_add_job`
#. :c:func:`udo_jpool_wait`
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
		udo_atomic_u32 *job_count;
		udo_atomic_u32 *front;
		udo_atomic_u32 *rear;
		void           *data;
		uint32_t       size;

	:c:member:`job_free`
		| Futex used to wake threads or put them
    		| to sleep if jobs are available.

	:c:member:`job_count`
		| Amount of jobs currently in the given
 		| threads pool. 

	:c:member:`front`
		| Byte offset to the front of the queue.

	:c:member:`rear`
		| Byte offset to the rear of the queue.

	:c:member:`data`
		| Starting address caller may store data in.

	:c:member:`size`
		| Byte size of queue associated with thread.

==========================
udo_jpool_thread (private)
==========================

| Structure defining information used by threads.

.. c:struct:: udo_jpool_thread

	.. c:member::
		pthread_t              tid;
		struct udo_jpool_queue queue;

	:c:member:`tid`
		| POSIX thread ID associated with thread.

	:c:member:`queue`
		| Structure keeping track of current jobs
		| a thread can execute.

===================
udo_jpool (private)
===================

| Structure defining the udo_jpool (Udo Job Pool) context.

.. c:struct:: udo_jpool

	.. c:member::
		struct udo_log_error_struct err;
		bool                        free;
		uint32_t                    queue_sz;
		void                        *queue_data;
		udo_atomic_u32              *cur_thread;
		uint32_t                    thread_count;
		struct udo_jpool_thread     threads[THREADS_MAX];

	:c:member:`err`
		| Stores information about the error that occured
		| for the given context and may later be retrieved
		| by caller.

	:c:member:`free`
		| If structure allocated with `calloc(3)`_ member will be
		| set to true so that, we know to call `free(3)`_ when
		| destroying the context.

	:c:member:`queue_sz`
		| Byte size of :c:member:`queue_data`.

	:c:member:`queue_data`
		| Shared memory buffer storing actual
		| addresses to jobs.

	:c:member:`cur_thread`
		| Current thread index whose queue will have
		| work placed in it.

	:c:member:`thread_count`
		| Amount of threads in the pool.

	:c:member:`threads`
		| Array of threads storing location of each
		| threads queue and unique ID.

=========================================================================================================================================

=====================
udo_jpool_create_info
=====================

| Structure passed to :c:func:`udo_jpool_create` used
| to define size of shared memory queue and
| the amount of threads to create.

.. c:struct:: udo_jpool_create_info

	.. c:member::
		size_t   size;
		uint32_t count;

	:c:member:`size`
		| Minimum size of each threads shared
		| memory segment used to store a threads
		| queue'd data.

	:c:member:`count`
		| Amount of threads able to read and
		| write to and from the shared memory
		| block.

.. c:function:: struct udo_jpool *udo_jpool_create(struct udo_jpool *jpool, const void *jpool_info);

| Creates pool a threads to execute task.

	.. list-table:: Job Queue (2 threads)
		:header-rows: 1

		* - Variable
		  - Offset In Bytes
		  - Size In Bytes
		  - Initial Value
		* - cur_thread (main process)
		  - 0
		  - 4
		  - 1
		* - Job Free (thread=1)
		  - 4
		  - 4
		  - 0
		* - Front Of Queue (thread=1)
		  - 8
		  - 4
		  - 0
		* - Rear Of Queue (thread=1)
		  - 12
		  - 4
		  - 0
		* - Job Free (thread=2)
		  - 16
		  - 4
		  - 0
		* - Front Of Queue (thread=2)
		  - 20
		  - 4
		  - 0
		* - Rear Of Queue (thread=2)
		  - 24
		  - 4
		  - 0
		* - :c:struct:`udo_jpool_job`
		  - 28
		  - Size of queue
		  - 0
		* - :c:struct:`udo_jpool_job`
		  - End of thread 1 queue
		  - Size of queue
		  - 0

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - jpool
		  - | May be ``NULL`` or a pointer to a ``struct`` :c:struct:`udo_jpool`.
		    | If ``NULL`` memory will be allocated and return to
		    | caller. If not ``NULL`` address passed will be used
		    | to store the newly created ``struct`` :c:struct:`udo_jpool`
		    | context.
		* - jpool_info
		  - | Implementation uses a pointer to a
		    | ``struct`` :c:struct:`udo_jpool_create_info`.

	Returns:
		| **on success:** Pointer to a ``struct`` :c:struct:`udo_jpool`
		| **on failure:** ``NULL``

=========================================================================================================================================

=================
udo_jpool_add_job
=================

.. c:function:: uint32_t udo_jpool_add_job(struct udo_jpool *jpool, void (*func)(void *arg), void *arg);

| Adds a job to a given thread's job queue
| to then later execute. If a given thread's
| queue is full function blocks until all
| jobs in that queue are completed before
| adding a new task.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - jpool
		  - | Pointer to a valid ``struct`` :c:struct:`udo_jpool`.
		* - func
		  - | Pointer to function that a separate
		    | thread will execute.
		* - arg
		  - | Pointer to a memory which will be
		    | passed as the argument to ``func``.

	Returns:
		| **on success:** 0
		| **on failure:** -1

=========================================================================================================================================

==============
udo_jpool_wait
==============

.. c:function:: void udo_jpool_wait(struct udo_jpool *jpool);

| Blocks until all jobs in every
| threads queue have been completed.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - jpool
		  - | Pointer to a valid ``struct`` :c:struct:`udo_jpool`.

=========================================================================================================================================

=================
udo_jpool_destroy
=================

.. c:function:: void udo_jpool_destroy(struct udo_jpool *jpool);

| Frees any allocated memory and closes FD's (if open) created after
| :c:func:`udo_jpool_create` call. Function waits for all jobs in every 
| threads queue to execute before destroying the pool.

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
