.. default-domain:: C

version (UDO Version)
=====================

Header: udo/version.h

Table of contents (click to go)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

======
Macros
======

1. :c:macro:`udo_version`
#. :c:macro:`udo_version_get_major`
#. :c:macro:`udo_version_get_minor`
#. :c:macro:`udo_version_get_patch`

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

===========
udo_version
===========

.. c:macro:: udo_version

| Returns version of library in C string format.

	.. code-block::

		#define udo_version() \
			UDO_STRINGIFY(UDO_VERSION_MAJOR) "." \
			UDO_STRINGIFY(UDO_VERSION_MINOR) "." \
			UDO_STRINGIFY(UDO_VERSION_PATCH)

	Returns:
		| Library version

=====================
udo_version_get_major
=====================

.. c:macro:: udo_version_get_major

| Returns library version major in integer format.

	.. code-block::

		#define udo_version_get_major() UDO_VERSION_MAJOR

	Returns:
		| Library version major

=====================
udo_version_get_minor
=====================

.. c:macro:: udo_version_get_minor

| Returns library version minor in integer format.

	.. code-block::

		#define udo_version_get_minor() UDO_VERSION_MINOR

	Returns:
		| Library version minor

=====================
udo_version_get_patch
=====================

.. c:macro:: udo_version_get_patch

| Returns library version patch in integer format.

	.. code-block::

		#define udo_version_get_patch() UDO_VERSION_MINOR

	Returns:
		| Library version patch
