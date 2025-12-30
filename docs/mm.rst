.. default-domain:: C

mm (Memory Management)
======================

Header: cando/mm.h

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

1. :c:struct:`cando_mm`

=========
Functions
=========

1. :c:func:`cando_mm_alloc`
#. :c:func:`cando_mm_sub_alloc`
#. :c:func:`cando_mm_free`
#. :c:func:`cando_mm_destroy`

API Documentation
~~~~~~~~~~~~~~~~~

| This interface was built to force caller to
| be more consciously concern about virtual
| heap memory management.

==================
cando_mm (private)
==================

.. c:struct:: cando_mm

	.. c:member::
		struct udo_log_error_struct err;
		size_t                      buff_sz;
		size_t                      data_sz;
		size_t                      ab_sz;
		size_t                      offset;

	:c:member:`err`
		| Stores information about the error that occured
		| for the given instance and may later be retrieved.

	:c:member:`buff_sz`
		| Full size of the ``struct`` :struct:`cando_mm` instance.
		| Not all bytes in the buffer are writable.

	:c:member:`data_sz`
		| Full size of the caller writable data.

	:c:member:`ab_sz`
		| The amount of available bytes the caller
		| can still write to.

	:c:member:`offset`
 		| Buffer offset used when allocating new blocks
		| in constant time.

=========================================================================================================================================

==============
cando_mm_alloc
==============

.. c:function:: struct cando_mm *cando_mm_alloc(struct cando_mm *mm, const size_t size);

| Returns pointer to an allocated heap memory.
| The goal of this is to allocate a large block
| of memory once. If re-allocation required pass
| the previous large block to clone all data.
|
| Addresses returned from function should not
| be used to write to. Writable addresses
| are return from a call to :c:func:`cando_mm_sub_alloc`.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - mm
		  - | If ``NULL`` the inital allocation will be performed.
		    | If not ``NULL`` must pass a pointer to a ``struct`` :c:struct:`cando_mm`.
		* - size
		  - | Size of data caller may allocate. If the
		    | size is greater than the larger block
		    | remapping of memory will occur.

	Returns:
		| **on success:** Pointer to a ``struct`` :c:struct:`cando_mm`
		| **on failure:** ``NULL``

=========================================================================================================================================

==================
cando_mm_sub_alloc
==================

.. c:function:: void *cando_mm_sub_alloc(struct cando_mm *mm, const size_t size);

| Returns pointer to an allocated heap memory
| segment. From an allocated large block of
| memory sub-allocate from that larger block.
|
| Addresses returned from function can be
| used for writing.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - mm
		  - | Must pass a pointer to a ``struct`` :c:struct:`cando_mm`.
		* - size
		  - | Size of buffer to sub-allocate.

	Returns:
		| **on success:** Pointer to writable memory
		| **on failure:** ``NULL``

=========================================================================================================================================

=============
cando_mm_free
=============

.. c:function:: void cando_mm_free(struct cando_mm *mm, void *data, const size_t size);

| Wipes the bytes at a given subregion of memory.
| Shifts the memory after the subregion up to a
| tracked buffer offset over to the subregion
| up to the new buffer offset.
|
| **NOTE:** This function should be used sparingly
| as the caller would have to keep track of the
| new pointer address for every object allocated.
| It's better to only allocate memory if you know
| the address it resides in won't change. Usages
| of bounded buffer for strings is encouraged.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - mm
		  - | Must pass a pointer to a ``struct`` :c:struct:`cando_mm`.
		* - data
		  - | Address to the data caller wants to zero out.
		* - size
		  - | Size of buffer to sub-allocate.

=========================================================================================================================================

================
cando_mm_destroy
================

.. c:function:: void cando_mm_destroy(struct cando_mm *mm);

| Free's the large block of allocated memory created after
| :c:func:`cando_mm_alloc` call.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - flops
		  - | Must pass a pointer to a ``struct`` :c:struct:`cando_mm`.
