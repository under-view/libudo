.. default-domain:: C

log
===

Header: cando/log.h

Table of contents (click to go)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

======
Macros
======

1. :c:macro:`udo_log`
#. :c:macro:`udo_log_success`
#. :c:macro:`udo_log_info`
#. :c:macro:`udo_log_warning`
#. :c:macro:`udo_log_error`
#. :c:macro:`udo_log_print`
#. :c:macro:`udo_log_set_error`

=====
Enums
=====

1. :c:enum:`udo_log_level_type`
#. :c:enum:`udo_log_error_type`

======
Unions
======

=======
Structs
=======

1. :c:struct:`udo_log_error_struct`

=========
Functions
=========

1. :c:func:`udo_log_set_level`
#. :c:func:`udo_log_set_write_fd`
#. :c:func:`udo_log_remove_colors`
#. :c:func:`udo_log_reset_colors`
#. :c:func:`udo_log_get_error`
#. :c:func:`udo_log_get_error_code`
#. :c:func:`udo_log_set_error_struct`
#. :c:func:`udo_log_time`
#. :c:func:`udo_log_notime`

API Documentation
~~~~~~~~~~~~~~~~~

==================
udo_log_level_type
==================

.. c:enum:: udo_log_level_type

	#. Log level options used by
		* :c:func:`udo_log_set_level`
		* :c:macro:`udo_log`
		* :c:macro:`udo_log_error`
		* :c:macro:`udo_log_print`

	.. c:enumerator::
		UDO_LOG_NONE
		UDO_LOG_SUCCESS
		UDO_LOG_DANGER
		UDO_LOG_INFO
		UDO_LOG_WARNING
		UDO_LOG_RESET
		UDO_LOG_ALL

	:c:enumerator:`UDO_LOG_NONE`
		| Value set to ``0x00000000``
		| Term color

	:c:enumerator:`UDO_LOG_SUCCESS`
		| Value set to ``0x00000001``
		| Green

	:c:enumerator:`UDO_LOG_DANGER`
		| Value set to ``0x00000002``
		| Red

	:c:enumerator:`UDO_LOG_INFO`
		| Value set to ``0x00000004``
		| Light purple

	:c:enumerator:`UDO_LOG_WARNING`
		| Value set to ``0x00000008``
		| Yellow

	:c:enumerator:`UDO_LOG_RESET`
		| Value set to ``0x00000010``
		| Term color

	:c:enumerator:`UDO_LOG_ALL`
		| Value set to ``0xFFFFFFFF``
		| Term color

=========================================================================================================================================

=================
udo_log_set_level
=================

.. c:function:: void udo_log_set_level(enum udo_log_level_type level);

| Sets which type of messages that are allowed to be printed to an open file.
|
| Default is set to ``UDO_LOG_NONE``.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - level
		  - | 32-bit integer representing the type of log to print to
		    | an open file. Each log type has a different color

=========================================================================================================================================

====================
udo_log_set_write_fd
====================

.. c:function:: void udo_log_set_write_fd(const int fd);

| Sets the internal global write file descriptor
| to caller define file descriptor.
|
| Default is set to ``STDOUT_FILENO``.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - fd
		  - | File descriptor to an open file.

=========================================================================================================================================

=====================
udo_log_remove_colors
=====================

.. c:function:: void udo_log_remove_colors(void);

| Sets the internal global ansi color
| storage array to remove the ansi colors
| and replace with "[LOG_TYPE] ".

=========================================================================================================================================

====================
udo_log_reset_colors
====================

.. c:function:: void udo_log_reset_colors(void);

| Sets the internal global ansi color
| storage array to it's original values.

=========================================================================================================================================

==================
udo_log_error_type
==================

.. c:enum:: udo_log_error_type

| Enum with macros defining and error type
| Add on userspace error codes should be well out
| of range of any known common error code.

	#. Log error types used by
		* :c:func:`udo_log_get_error`
		* :c:func:`udo_log_get_error_code`

	.. c:enumerator::
		UDO_LOG_ERR_UNCOMMON
		UDO_LOG_ERR_INCORRECT_DATA

	:c:enumerator:`UDO_LOG_ERR_UNCOMMON`
		| Value set to ``0x1000``
		| Errors that can't be given a common
		| error string are given this error code.
		| Caller would then need to set buffer themselves.

	:c:enumerator:`UDO_LOG_ERR_INCORRECT_DATA`
		| Value set to ``0x1001``
		| Code for incorrect data passed in function arguments

====================
udo_log_error_struct
====================

.. c:struct:: udo_log_error_struct

| Structure used to store and acquire
| error string and code for multiple
| struct context's.

	.. c:member::
		unsigned int code;
		char         buffer[UDO_PAGE_SIZE];

	:c:member:`code`
		| Error code or errno

	:c:member:`buffer`
		| Buffer to store error string

=================
udo_log_get_error
=================

.. c:function:: const char *udo_log_get_error(const void *context);

| Returns a string with the error defined given
| caller provided a context with first members
| of the context being a ``struct`` :c:struct:`udo_log_error_struct`.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - context
		  - | Pointer to an arbitrary context.
		    | Start of context must be a ``struct`` :c:struct:`udo_log_error_struct`.

	Returns:
		| **on success:** Passed context error string
		| **on failure:** ``NULL``

======================
udo_log_get_error_code
======================

.. c:function:: unsigned int udo_log_get_error_code(const void *context);

| Returns unsigned integer with the error code
| given caller provided a context with first members
| of the context being a ``struct`` :c:struct:`udo_log_error_struct`.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - context
		  - | Pointer to an arbitrary context.
		    | Start of context must be a ``struct`` :c:struct:`udo_log_error_struct`.

	Returns:
		| **on success:** Passed context error code or errno
		| **on failure:** ``UINT32_MAX``

========================
udo_log_set_error_struct
========================

.. c:function:: void udo_log_set_error_struct(void *context, const unsigned int code, const char *fmt, ...);

| Sets ``struct`` :c:struct:`udo_log_error_struct` members value.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - context
		  - | Pointer to an arbitrary context.
		    | Start of context must be a ``struct`` :c:struct:`udo_log_error_struct`.
		* - code
		  - | Error code to set for a ``context``
		    | May be ``errno`` or ``enum`` :c:enum:`udo_log_error_type`.
		* - fmt
		  - | Format of the log passed to va_args
		* - ...
		  - | Variable list arguments

=========================================================================================================================================

============
udo_log_time
============

.. c:function:: void udo_log_time(enum udo_log_level_type type, const char *fmt, ...);

| Provides applications/library way to write to an open file
| with a time stamp and ansi color codes to colorize
| different message.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - type
		  - | The type of color to use with log 
		* - fmt
		  - | Format of the log passed to va_args
		* - ...
		  - | Variable list arguments

==============
udo_log_notime
==============

.. c:function:: void udo_log_notime(enum udo_log_level_type type, const char *fmt, ...);

| Provides applications/library way to write to an open file
| without time stamp with ansi color codes to colorize
| different message.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - type
		  - | The type of color to use with log 
		* - fmt
		  - | Format of the log passed to va_args
		* - ...
		  - | Variable list arguments

=========================================================================================================================================

=======
udo_log
=======

.. c:macro:: udo_log(log_type, fmt, ...)

| Log format
|
| timestamp [file:line] message
|
| Default prints to ``stdout`` using ansi color codes to color text.
|
| Caller may change the open file in which logs are printed to via
| a call to :c:func:`udo_log_set_write_fd`

	.. code-block::

		#define udo_log(log_type, fmt, ...) \
			udo_log_time(log_type, "[%s:%d] " fmt, __FILE_NAME__, __LINE__, ##__VA_ARGS__)

===============
udo_log_success
===============

.. c:macro:: udo_log_success(fmt, ...)

| Log format
|
| timestamp [file:line] message
|
| Prints to ``stdout`` with ansi color codes the color **GREEN**.
|
| Caller may change the open file in which logs are printed to via
| a call to :c:func:`udo_log_set_write_fd`

	.. code-block::

		#define udo_log_success(fmt, ...) \
			udo_log_time(UDO_LOG_SUCCESS, "[%s:%d] " fmt, __FILE_NAME__, __LINE__, ##__VA_ARGS__)

============
udo_log_info
============

.. c:macro:: udo_log_info(fmt, ...)

| Log format
|
| timestamp [file:line] message
|
| Prints to ``stdout`` with ansi color codes the color **BLUE**.
|
| Caller may change the open file in which logs are printed to via
| a call to :c:func:`udo_log_set_write_fd`

	.. code-block::

		#define udo_log_info(fmt, ...) \
			udo_log_time(UDO_LOG_INFO, "[%s:%d] " fmt, __FILE_NAME__, __LINE__, ##__VA_ARGS__)

===============
udo_log_warning
===============

.. c:macro:: udo_log_warning(fmt, ...)

| Log format
|
| timestamp [file:line] message
|
| Prints to ``stdout`` with ansi color codes the color **YELLOW**.
|
| Caller may change the open file in which logs are printed to via
| a call to :c:func:`udo_log_set_write_fd`

	.. code-block::

		#define udo_log_warning(fmt, ...) \
			udo_log_time(UDO_LOG_WARNING, "[%s:%d] " fmt, __FILE_NAME__, __LINE__, ##__VA_ARGS__)

=============
udo_log_error
=============

.. c:macro:: udo_log_error(fmt, ...)

| Log format
|
| timestamp [file:line] message
|
| Prints to ``stderr`` with ansi color codes the color **RED**.
|
| Caller may change the open file in which logs are printed to via
| a call to :c:func:`udo_log_set_write_fd`

	.. code-block::

		#define udo_log_error(fmt, ...) \
			udo_log_time(UDO_LOG_ERROR, "[%s:%d] " fmt, __FILE_NAME__, __LINE__, ##__VA_ARGS__)

=============
udo_log_print
=============

.. c:macro:: udo_log_print(log_type, fmt, ...)

| Log format
|
| NONE
|
| Default prints to ``stdout`` using ansi color codes to color text.
|
| Caller may change the open file in which logs are printed to via
| a call to :c:func:`udo_log_set_write_fd`

	.. code-block::

		#define udo_log_print(log_type, fmt, ...) \
			udo_log_notime(log_type, fmt, ##__VA_ARGS__)

=================
udo_log_set_error
=================

.. c:macro:: udo_log_set_error(ptr, code, fmt, ...)

| Log format
|
| [file:line] message
|
| Sets ``struct`` :c:struct:`udo_log_error_struct` to later be called by
| :c:func:`udo_log_get_error` and :c:func:`udo_log_get_error_code`.

	.. code-block::

		#define udo_log_set_error(ptr, code, fmt, ...) \
			udo_log_set_error_struct(ptr, code, "[%s:%d] " fmt, __FILE_NAME__, __LINE__, ##__VA_ARGS__)
