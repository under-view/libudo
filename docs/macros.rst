.. default-domain:: C

macros (C Macros)
=================

Header: udo/macros.h

Table of contents (click to go)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

======
Macros
======

1. :c:macro:`UDO_API`
#. :c:macro:`UDO_UNUSED`
#. :c:macro:`UDO_INLINE`
#. :c:macro:`UDO_STATIC_INLINE`
#. :c:macro:`UDO_DIR_NAME_MAX`
#. :c:macro:`UDO_FILE_NAME_MAX`
#. :c:macro:`UDO_FILE_PATH_MAX`
#. :c:macro:`UDO_MAX`
#. :c:macro:`UDO_MIN`
#. :c:macro:`UDO_BYTE_ALIGN`
#. :c:macro:`UDO_PAGE_SIZE`
#. :c:macro:`UDO_PAGE_GET`
#. :c:macro:`UDO_STRTOU`
#. :c:macro:`UDO_ATOMIC_DEF`

=====
Enums
=====

======
Unions
======

=======
Structs
=======

=========
Functions
=========

API Documentation
~~~~~~~~~~~~~~~~~

=======
UDO_API
=======

.. c:macro:: UDO_API

| Used to prevent C++ name mangling when
| using library in a C++ software package.

	.. code-block::

		/* Prevent c++ name mangling */
		#ifdef __cplusplus
		#define UDO_API extern "C"
		#else
		#define UDO_API
		#endif

=========================================================================================================================================

==========
UDO_UNUSED
==========

.. c:macro:: UDO_UNUSED

| Informs the compiler that you expect a variable
| to be unused and instructs compiler to not issue
| a warning on the variable.

	.. code-block::

		#define UDO_UNUSED __attribute__((unused))

=========================================================================================================================================

==========
UDO_INLINE
==========

.. c:macro:: UDO_INLINE

| **"always_inline"** instructs GCC to

1. | Ignore flag ``-fno-inline``
2. | Don't produce external definition of a
   | function with external linkage.
3. | Ignore inlining limits. Use `alloca(3)`_ to inline.

	.. code-block::

		#define UDO_INLINE inline __attribute__((always_inline))

=================
UDO_STATIC_INLINE
=================

.. c:macro:: UDO_STATIC_INLINE

| **"always_inline"** instructs GCC to

1. | Ignore flag ``-fno-inline``
2. | Don't produce external definition of a
   | function with external linkage.
3. | Ignore inlining limits. Use `alloca(3)`_ to inline.

| Any function declared with static keyword will
| be visible only to the source file it's declared in.

	.. code-block::

		#define UDO_STATIC_INLINE static inline __attribute__((always_inline))

=========================================================================================================================================

================
UDO_DIR_NAME_MAX
================

.. c:macro:: UDO_DIR_NAME_MAX

| Amount of character ext file system
| support for directory paths.

	.. code-block::

		#define UDO_DIR_NAME_MAX (1<<12)

=================
UDO_FILE_NAME_MAX
=================

.. c:macro:: UDO_FILE_NAME_MAX

| Amount of character ext file system
| support for a given file name.

	.. code-block::

		#define UDO_FILE_NAME_MAX (1<<8)

=================
UDO_FILE_PATH_MAX
=================

.. c:macro:: UDO_FILE_PATH_MAX

| Amount of character ext filesystem
| supports when specifying a path and
| file.

	.. code-block::

		#define UDO_FILE_PATH_MAX (UDO_DIR_NAME_MAX + UDO_FILE_NAME_MAX)

=========================================================================================================================================

=======
UDO_MAX
=======

.. c:macro:: UDO_MAX

| Returns the maximum value between ``a`` and ``b``.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - a
		  - | Must pass an integer value.
		* - b
		  - | Must pass an integer value.

	.. code-block::

		#define UDO_MAX(a,b) \
			({ typeof (a) _a = (a); \
			   typeof (b) _b = (b); \
			   _a > _b ? _a : _b; })

=======
UDO_MIN
=======

.. c:macro:: UDO_MIN

| Returns the minimum value between ``a`` and ``b``.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - a
		  - | Must pass an integer value.
		* - b
		  - | Must pass an integer value.

	.. code-block::

		#define UDO_MIN(a,b) \
			({ typeof (a) _a = (a); \
			   typeof (b) _b = (b); \
			   _a < _b ? _a : _b; })

	Returns:
		| Minimum value between ``2`` numbers

=========================================================================================================================================

==============
UDO_BYTE_ALIGN
==============

.. c:macro:: UDO_BYTE_ALIGN

| Returns the upper bound byte alignment value.
| Alignment value must be a power of ``2`` number.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - bytes
		  - | Integer value to align.
		* - power_two_align
		  - | Power of ``2`` number to align ``bytes`` to.

	.. code-block::

		#define UDO_BYTE_ALIGN(bytes, power_two_align) \
			((bytes+(power_two_align-1))&~(power_two_align-1))

	Returns
		| Upper bound memory alignment

=========================================================================================================================================

=============
UDO_PAGE_SIZE
=============

.. c:macro:: UDO_PAGE_SIZE

| Defines typical page size.

	.. code-block::

		#define UDO_PAGE_SIZE (1<<12)

============
UDO_PAGE_GET
============

.. c:macro:: UDO_PAGE_GET

| Retrieves the starting address of
| the page ``ptr`` resides in.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - ptr
		  - | Address caller wants to find page of.

	.. code-block::

		#define UDO_PAGE_GET(ptr) \
			((void*)((uintptr_t)ptr & ~(UDO_PAGE_SIZE-1)))

	Returns:
		| Starting address of the page

=========================================================================================================================================

==========
UDO_STRTOU
==========

.. c:macro:: UDO_STRTOU

| Converts string to an unsigned integer.
| Simple one way hash function.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - str
		  - | Pointer to string to hash.

	.. code-block::

		#define UDO_STRTOU(str) \
			__extension__ \
			({ \
				unsigned int hash=0; \
				const char *s = str; \
				while (*s) hash += *s++; \
				hash; \
			})

	Returns:
		| ``unsigned`` integer representing string

=========================================================================================================================================

==============
UDO_ATOMIC_DEF
==============

.. c:macro:: UDO_ATOMIC_DEF

| Create atomic variable type of a
| caller defined data type.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - name
		  - | Name of atomic type.
		* - type
    		  - | Data type of the atomic type.

	.. code-block::

		#define UDO_ATOMIC_DEF(name, type) \
			typedef _Atomic __typeof__(type) name;

		/* Atomic Types */
		UDO_ATOMIC_DEF(udo_atomic_int, int)
		UDO_ATOMIC_DEF(udo_atomic_bool, unsigned char)
		UDO_ATOMIC_DEF(udo_atomic_u32, unsigned int)
		UDO_ATOMIC_DEF(udo_atomic_addr, unsigned char *)

	Returns:
		| The atomic type

=========================================================================================================================================

.. _alloca(3): https://man7.org/linux/man-pages/man3/alloca.3.html
