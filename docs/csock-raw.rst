.. default-domain:: C

csock-raw (Can Socket Raw)
==========================

Header: udo/csock-raw.h

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

1. :c:struct:`udo_csock_raw`
#. :c:struct:`udo_csock_raw_create_info`

=========
Functions
=========

1. :c:func:`udo_csock_raw_create`
#. :c:func:`udo_csock_raw_send_data`
#. :c:func:`udo_csock_raw_recv_data`
#. :c:func:`udo_csock_raw_get_fd`
#. :c:func:`udo_csock_raw_get_iface`
#. :c:func:`udo_csock_raw_destroy`
#. :c:func:`udo_csock_raw_get_sizeof`

API Documentation
~~~~~~~~~~~~~~~~~

==============
Kernel Modules
==============

**On Host Machine**

.. code-block:: sh

	sudo modprobe -a can can_raw vcan
	sudo ip link add dev vcan0 type vcan
	sudo ip link set vcan0 up

=======================
udo_csock_raw (private)
=======================

| Structure defining UDO CAN Socket Raw instance.

.. c:struct:: udo_csock_raw

	.. c:member::
		struct udo_log_error_struct err;
		bool                        free;
		int                         fd;
		char                        iface[IFNAMSIZ];

	:c:member:`err`
		| Stores information about the error that occured
		| for the given instance and may later be retrieved
		| by caller.

	:c:member:`free`
		| If structure allocated with `calloc(3)`_ member will be
		| set to true so that, we know to call `free(3)`_ when
		| destroying the instance.

	:c:member:`fd`
		| File descriptor to the open CAN socket

	:c:member:`iface`
		| Textual CAN interface name in string format to `bind(2)`_ to.

=========================================================================================================================================

=========================
udo_csock_raw_create_info
=========================

| Structure passed to :c:func:`udo_csock_raw_create`
| used to define how to create the CAN socket.

.. c:struct:: udo_csock_raw_create_info

	.. c:member::
		const char *iface;

	:c:member:`iface`
		| Must pass textual CAN interface
    		| name in string format.

====================
udo_csock_raw_create
====================

.. c:function:: struct udo_csock_raw *udo_csock_raw_create(struct udo_csock_raw *csock, const void *csock_info);

| Creates a socket that may be utilized
| for sending & receiving CAN frames. On
| a caller specified CAN interface.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - csock
		  - | May be ``NULL`` or a pointer to a ``struct`` :c:struct:`udo_csock_raw`.
		    | If ``NULL`` memory will be allocated and return to
		    | caller. If not ``NULL`` address passed will be used
		    | to store the newly created ``struct`` :c:struct:`udo_csock_raw`
		    | instance.
		* - csock_info
		  - | Implementation uses a pointer to a
		    | ``struct`` :c:struct:`udo_csock_raw_create_info`.
		    | no other implementation may be passed to
		    | this parameter.

	Returns:
		| **on success:** Pointer to a ``struct`` :c:struct:`udo_csock_raw`
		| **on failure:** ``NULL``

=========================================================================================================================================

=======================
udo_csock_raw_send_data
=======================

.. c:function:: ssize_t udo_csock_raw_send_data(struct udo_csock_raw *csock, const struct can_frame *frame, const void *csock_info);

| Send CAN frame through socket bounded to CAN interface
| specified by a call to :c:func:`udo_csock_raw_create`.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - csock
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_csock_raw`.
		* - frame
		  - | Pointer to frame buffer to send through socket.
		* - csock_info
		  - | Reserved for future usage. For now used
		    | to set the flag argument of `send(2)`_.

	Returns:
		| **on success:** Size of CAN frame
		| **on failure:** # < 0

=========================================================================================================================================

=======================
udo_csock_raw_recv_data
=======================

.. c:function:: ssize_t udo_csock_raw_recv_data(struct udo_csock_raw *csock, struct can_frame *frame, const void *csock_info);

| Receive CAN frame from socket bounded to CAN interface
| specified by a call to :c:func:`udo_csock_raw_create`.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - csock
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_csock_raw`.
		* - frame
		  - | Pointer to buffer to store data received from a socket.
		* - csock_info
		  - | Reserved for future usage. For now used
		    | to set the flag argument of `send(2)`_.

	Returns:
		| **on success:** Size of CAN frame
		| **on failure:** # < 0

=========================================================================================================================================

====================
udo_csock_raw_get_fd
====================

.. c:function:: int udo_csock_raw_get_fd(struct udo_csock_raw *csock);

| Acquire socket file descriptor associated with
| ``struct`` :c:struct:`udo_csock_raw` instance.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - csock
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_csock_raw`.

	Returns:
		| **on success:** RAW socket file descriptor
		| **on failure:** -1

=========================================================================================================================================

=======================
udo_csock_raw_get_iface
=======================

.. c:function:: unsigned int udo_csock_raw_get_iface(struct udo_csock_raw *csock);

| Acquire textual CAN interface name in string format
| associated with ``struct`` :c:struct:`udo_csock_raw` instance.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - csock
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_csock_raw`.

	Returns:
		| **on success:** Textual CAN interface name in string format
		| **on failure:** ``NULL``

=========================================================================================================================================

=====================
udo_csock_raw_destroy
=====================

.. c:function:: void udo_csock_raw_destroy(struct udo_csock_raw *csock);

| Frees any allocated memory and closes FD's (if open)
| created after :c:func:`udo_csock_raw_create` call.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - csock
		  - | Pointer to a valid ``struct`` :c:struct:`udo_csock_raw`.

=========================================================================================================================================

========================
udo_csock_raw_get_sizeof
========================

.. c:function:: int udo_csock_raw_get_sizeof(void);

| Returns size of the internal structure. So,
| if caller decides to allocate memory outside
| of API interface they know the exact amount
| of bytes.

	Returns:
		| **on success:** sizeof(``struct`` :c:struct:`udo_csock_raw`)
		| **on failure:** sizeof(``struct`` :c:struct:`udo_csock_raw`)

=========================================================================================================================================

.. _calloc(3): https://www.man7.org/linux/man-pages/man3/malloc.3.html
.. _free(3): https://www.man7.org/linux/man-pages/man3/free.3.html
.. _send(2): https://www.man7.org/linux/man-pages/man2/send.2.html
.. _recv(2): https://www.man7.org/linux/man-pages/man2/recv.2.html
.. _bind(2): https://www.man7.org/linux/man-pages/man2/bind.2.html
