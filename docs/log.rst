.. default-domain:: C

log
===

Header: cando/log.h

Table of contents (click to go)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

======
Macros
======

1. :c:macro:`cando_log`
#. :c:macro:`cando_log_success`
#. :c:macro:`cando_log_info`
#. :c:macro:`cando_log_warning`
#. :c:macro:`cando_log_error`
#. :c:macro:`cando_log_print`
#. :c:macro:`cando_log_set_error`

=====
Enums
=====

1. :c:enum:`cando_log_level_type`
#. :c:enum:`cando_log_error_type`

======
Unions
======

=======
Structs
=======

1. :c:struct:`cando_log_error_struct`

=========
Functions
=========

1. :c:func:`cando_log_set_level`
#. :c:func:`cando_log_set_write_fd`
#. :c:func:`cando_log_remove_colors`
#. :c:func:`cando_log_reset_colors`
#. :c:func:`cando_log_get_error`
#. :c:func:`cando_log_get_error_code`
#. :c:func:`cando_log_set_error_struct`
#. :c:func:`cando_log_time`
#. :c:func:`cando_log_notime`

API Documentation
~~~~~~~~~~~~~~~~~

====================
cando_log_level_type
====================

.. c:enum:: cando_log_level_type

	#. Log level options used by
		* :c:func:`cando_log_set_level`
		* :c:macro:`cando_log`
		* :c:macro:`cando_log_error`
		* :c:macro:`cando_log_print`

	.. c:enumerator::
		CANDO_LOG_NONE
		CANDO_LOG_SUCCESS
		CANDO_LOG_DANGER
		CANDO_LOG_INFO
		CANDO_LOG_WARNING
		CANDO_LOG_RESET
		CANDO_LOG_ALL

	:c:enumerator:`CANDO_LOG_NONE`
		| Value set to ``0x00000000``
		| Term color

	:c:enumerator:`CANDO_LOG_SUCCESS`
		| Value set to ``0x00000001``
		| Green

	:c:enumerator:`CANDO_LOG_DANGER`
		| Value set to ``0x00000002``
		| Red

	:c:enumerator:`CANDO_LOG_INFO`
		| Value set to ``0x00000004``
		| Light purple

	:c:enumerator:`CANDO_LOG_WARNING`
		| Value set to ``0x00000008``
		| Yellow

	:c:enumerator:`CANDO_LOG_RESET`
		| Value set to ``0x00000010``
		| Term color

	:c:enumerator:`CANDO_LOG_ALL`
		| Value set to ``0xFFFFFFFF``
		| Term color

=========================================================================================================================================

===================
cando_log_set_level
===================

.. c:function:: void cando_log_set_level(enum cando_log_level_type level);

| Sets which type of messages that are allowed to be printed to an open file.
|
| Default is set to ``CANDO_LOG_NONE``.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - level
		  - | 32-bit integer representing the type of log to print to
		    | an open file. Each log type has a different color

=========================================================================================================================================

======================
cando_log_set_write_fd
======================

.. c:function:: void cando_log_set_write_fd(const int fd);

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

=======================
cando_log_remove_colors
=======================

.. c:function:: void cando_log_remove_colors(void);

| Sets the internal global ansi color
| storage array to remove the ansi colors
| and replace with "[LOG_TYPE] ".

=========================================================================================================================================

======================
cando_log_reset_colors
======================

.. c:function:: void cando_log_reset_colors(void);

| Sets the internal global ansi color
| storage array to it's original values.

=========================================================================================================================================

====================
cando_log_error_type
====================

.. c:enum:: cando_log_error_type

| Enum with macros defining and error type
| Add on userspace error codes should be well out
| of range of any known common error code.

	#. Log error types used by
		* :c:func:`cando_log_get_error`
		* :c:func:`cando_log_get_error_code`

	.. c:enumerator::
		CANDO_LOG_ERR_UNCOMMON
		CANDO_LOG_ERR_INCORRECT_DATA

	:c:enumerator:`CANDO_LOG_ERR_UNCOMMON`
		| Value set to ``0x1000``
		| Errors that can't be given a common
		| error string are given this error code.
		| Caller would then need to set buffer themselves.

	:c:enumerator:`CANDO_LOG_ERR_INCORRECT_DATA`
		| Value set to ``0x1001``
		| Code for incorrect data passed in function arguments

======================
cando_log_error_struct
======================

.. c:struct:: cando_log_error_struct

| Structure used to store and acquire
| error string and code for multiple
| struct context's.

	.. c:member::
		unsigned int code;
		char         buffer[CANDO_PAGE_SIZE];

	:c:member:`code`
		| Error code or errno

	:c:member:`buffer`
		| Buffer to store error string

===================
cando_log_get_error
===================

.. c:function:: const char *cando_log_get_error(const void *context);

| Returns a string with the error defined given
| caller provided a context with first members
| of the context being a ``struct`` :c:struct:`cando_log_error_struct`.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - context
		  - | Pointer to an arbitrary context.
		    | Start of context must be a ``struct`` :c:struct:`cando_log_error_struct`.

	Returns:
		| **on success:** Passed context error string
		| **on failure:** ``NULL``

========================
cando_log_get_error_code
========================

.. c:function:: unsigned int cando_log_get_error_code(const void *context);

| Returns unsigned integer with the error code
| given caller provided a context with first members
| of the context being a ``struct`` :c:struct:`cando_log_error_struct`.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - context
		  - | Pointer to an arbitrary context.
		    | Start of context must be a ``struct`` :c:struct:`cando_log_error_struct`.

	Returns:
		| **on success:** Passed context error code or errno
		| **on failure:** ``UINT32_MAX``

==========================
cando_log_set_error_struct
==========================

.. c:function:: void cando_log_set_error_struct(void *context, const unsigned int code, const char *fmt, ...);

| Sets struct cando_log_error_struct members value.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - context
		  - | Pointer to an arbitrary context.
		    | Start of context must be a ``struct`` :c:struct:`cando_log_error_struct`.
		* - code
		  - | Error code to set for a ``context``
		    | May be ``errno`` or ``enum`` :c:enum:`cando_log_error_type`.
		* - fmt
		  - | Format of the log passed to va_args
		* - ...
		  - | Variable list arguments

=========================================================================================================================================

==============
cando_log_time
==============

.. c:function:: void cando_log_time(enum cando_log_level_type type, const char *fmt, ...);

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

================
cando_log_notime
================

.. c:function:: void cando_log_notime(enum cando_log_level_type type, const char *fmt, ...);

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

=========
cando_log
=========

.. c:macro:: cando_log(logType, fmt, ...)

| Log format
|
| timestamp [file:line] message
|
| Default prints to ``stdout`` using ansi color codes to color text.
|
| Caller may change the open file in which logs are printed to via
| a call to :c:func:`cando_log_set_write_fd`

	.. code-block::

		#define cando_log(logType, fmt, ...) \
			cando_log_time(logType, "[%s:%d] " fmt, __FILE_NAME__, __LINE__, ##__VA_ARGS__)

=================
cando_log_success
=================

.. c:macro:: cando_log_success(fmt, ...)

| Log format
|
| timestamp [file:line] message
|
| Prints to ``stdout`` with ansi color codes the color **GREEN**.
|
| Caller may change the open file in which logs are printed to via
| a call to :c:func:`cando_log_set_write_fd`

	.. code-block::

		#define cando_log_success(fmt, ...) \
			cando_log_time(CANDO_LOG_SUCCESS, "[%s:%d] " fmt, __FILE_NAME__, __LINE__, ##__VA_ARGS__)

==============
cando_log_info
==============

.. c:macro:: cando_log_info(fmt, ...)

| Log format
|
| timestamp [file:line] message
|
| Prints to ``stdout`` with ansi color codes the color **BLUE**.
|
| Caller may change the open file in which logs are printed to via
| a call to :c:func:`cando_log_set_write_fd`

	.. code-block::

		#define cando_log_info(fmt, ...) \
			cando_log_time(CANDO_LOG_INFO, "[%s:%d] " fmt, __FILE_NAME__, __LINE__, ##__VA_ARGS__)

=================
cando_log_warning
=================

.. c:macro:: cando_log_warning(fmt, ...)

| Log format
|
| timestamp [file:line] message
|
| Prints to ``stdout`` with ansi color codes the color **YELLOW**.
|
| Caller may change the open file in which logs are printed to via
| a call to :c:func:`cando_log_set_write_fd`

	.. code-block::

		#define cando_log_warning(fmt, ...) \
			cando_log_time(CANDO_LOG_WARNING, "[%s:%d] " fmt, __FILE_NAME__, __LINE__, ##__VA_ARGS__)

===============
cando_log_error
===============

.. c:macro:: cando_log_error(fmt, ...)

| Log format
|
| timestamp [file:line] message
|
| Prints to ``stderr`` with ansi color codes the color **RED**.
|
| Caller may change the open file in which logs are printed to via
| a call to :c:func:`cando_log_set_write_fd`

	.. code-block::

		#define cando_log_error(fmt, ...) \
			cando_log_time(CANDO_LOG_ERROR, "[%s:%d] " fmt, __FILE_NAME__, __LINE__, ##__VA_ARGS__)

===============
cando_log_print
===============

.. c:macro:: cando_log_print(logType, fmt, ...)

| Log format
|
| NONE
|
| Default prints to ``stdout`` using ansi color codes to color text.
|
| Caller may change the open file in which logs are printed to via
| a call to :c:func:`cando_log_set_write_fd`

	.. code-block::

		#define cando_log_print(logType, fmt, ...) \
			cando_log_notime(logType, fmt, ##__VA_ARGS__)

===================
cando_log_set_error
===================

.. c:macro:: cando_log_set_error(ptr, code, fmt, ...)

| Log format
|
| [file:line] message
|
| Sets ``struct`` :c:struct:`cando_log_error_struct` to later be called by
| :c:func:`cando_log_get_error` and :c:func:`cando_log_get_error_code`.

	.. code-block::

		#define cando_log_set_error(ptr, code, fmt, ...) \
			cando_log_set_error_struct(ptr, code, "[%s:%d] " fmt, __FILE_NAME__, __LINE__, ##__VA_ARGS__)
