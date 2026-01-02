.. default-domain:: C

futex (Fast Userspace Mutex)
============================

Header: udo/futex.h

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

1. :c:struct:`udo_futex_create_info`

=========
Functions
=========

1. :c:func:`udo_futex_create`
#. :c:func:`udo_futex_lock`
#. :c:func:`udo_futex_unlock`
#. :c:func:`udo_futex_unlock_force`
#. :c:func:`udo_futex_destroy`

API Documentation
~~~~~~~~~~~~~~~~~

=====================
udo_futex_create_info
=====================

| Structure passed to :c:func:`udo_futex_create` used
| to define size of shared memory and amount of
| futexes contained at the start of shared memory.

.. c:struct:: udo_futex_create_info

	.. c:member::
		size_t       size;
		unsigned int count;

	:c:member:`size`
		| Size of shared memory block.

	:c:member:`count`
		| Amount of futexes stored in a single
		| shared memory block.

================
udo_futex_create
================

.. c:function:: udo_atomic_u32 *udo_futex_create(const void *futex_info);

| Allocates shared memory space that may be used
| to store a futex. This function usage should
| be limited to processes/threads that were created
| via `fork()`_ or `pthread_create()`_. For processes
| created without `fork()`_ (i.e seperate application)
| see `shm.c`_ implementation. By default all futexes
| are initialize in the locked state.

	.. list-table:: Futex Memory Block (3 futexes)
		:header-rows: 1

		* - Data Stored
		  - Offset In Bytes
		  - Byte Size
		  - Initial Value
		* - P1 Futex
		  - 0
		  - 4
		  - 1 (locked)
		* - P2 Futex
		  - 4
		  - 4
		  - 1 (locked)
		* - P3 Futex
		  - 8
		  - 4
		  - 1 (locked)

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - futex_info
		  - | Implementation uses a pointer to a
		    | ``struct`` :c:struct:`udo_futex_create_info`
		    | no other implementation may be passed
		    | to this parameter.

	Returns:
		| **on success:** Pointer to a ``udo_atomic_u32``
		| **on failure:** ``NULL``

=========================================================================================================================================

==============
udo_futex_lock
==============

.. c:function:: void udo_futex_lock(udo_atomic_u32 *fux);

| Atomically updates futex value to the locked state.
| If value can't be changed inform kernel that a
| process needs to be put to sleep. Sets errno to
| `EINTR`_ if a call to :c:func:`udo_futex_unlock_force`
| is made.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - fux
		  - | Pointer to 32-bit integer storing futex.

=========================================================================================================================================

================
udo_futex_unlock
================

.. c:function:: void udo_futex_unlock(udo_atomic_u32 *fux);

| Atomically update futex value to the unlocked state.
| Then inform kernel to wake up all processes/threads
| watching the futex.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - fux
		  - | Pointer to 32-bit integer storing futex.

======================
udo_futex_unlock_force
======================

.. c:function:: void udo_futex_unlock_force(udo_atomic_u32 *fux);

| Atomically update futex value to the force unlocked state.
| Then inform kernel to wake up all processes/threads
| watching the futex. When force unlocking if a
| process/thread is waiting on the lock. Process/thread
| will exit setting errno to `EINTR`_. Recommended to use
| function in a signal handler.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - fux
		  - | Pointer to 32-bit integer storing futex.

=========================================================================================================================================

=================
udo_futex_destroy
=================

.. c:function:: void udo_futex_destroy(udo_atomic_u32 *fux);

| Frees any allocated memory and closes FD's (if open)
| created after :c:func:`udo_futex_create` call.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - fux
		  - | Pointer to 32-bit integer storing futex.

=========================================================================================================================================

.. _EINTR: https://man7.org/linux/man-pages/man3/errno.3.html
.. _fork(): https://man7.org/linux/man-pages/man2/fork.2.html
.. _pthread_create(): https://man7.org/linux/man-pages/man3/pthread_create.3.html
.. _shm.c: https://github.com/under-view/libudo/blob/master/src/shm.c
