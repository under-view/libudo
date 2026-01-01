.. default-domain:: C

usock-udp (Unix Socket UDP)
============================

Header: cando/usock-udp.h

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

1. :c:struct:`udo_usock_udp`
#. :c:struct:`udo_usock_udp_server_create_info`
#. :c:struct:`udo_usock_udp_client_create_info`

=========
Functions
=========

1. :c:func:`udo_usock_udp_server_create`
#. :c:func:`udo_usock_udp_server_recv_data`
#. :c:func:`udo_usock_udp_client_create`
#. :c:func:`udo_usock_udp_client_send_data`
#. :c:func:`udo_usock_udp_get_fd`
#. :c:func:`udo_usock_udp_get_unix_path`
#. :c:func:`udo_usock_udp_destroy`
#. :c:func:`udo_usock_udp_get_sizeof`
#. :c:func:`udo_usock_udp_recv_data`
#. :c:func:`udo_usock_udp_send_data`

API Documentation
~~~~~~~~~~~~~~~~~

=======================
udo_usock_udp (private)
=======================

| Structure defining UDO Unix Socket UDP instance.

.. c:struct:: udo_usock_udp

	.. c:member::
		struct udo_log_error_struct err;
		bool                        free;
		int                         fd;
		struct sockaddr_un          addr;
		struct sockaddr_un          saddr;

	:c:member:`err`
		| Stores information about the error that occured
		| for the given instance and may later be retrieved
		| by caller.

	:c:member:`free`
		| If structure allocated with `calloc(3)`_ member will be
		| set to true so that, we know to call `free(3)`_ when
		| destroying the instance.

	:c:member:`fd`
		| File descriptor to the open UDP socket

	:c:member:`addr`
		| Stores byte information about the UDP unix domain
		| socket context. Is used for client and server `bind(2)`_.

	:c:member:`saddr`
		| Stores byte information about the UDP unix domain
		| socket context. Is used for client `connect(2)`_.

=========================================================================================================================================

================================
udo_usock_udp_server_create_info
================================

| Structure passed to :c:func:`udo_usock_udp_server_create`
| used to define how to create the server.

.. c:struct:: udo_usock_udp_server_create_info

	.. c:member::
		const char *unix_path;

	:c:member:`unix_path`
		| Absolute path to unix domain socket
		| to `recvfrom(2)`_ data with.

===========================
udo_usock_udp_server_create
===========================

.. c:function:: struct udo_usock_udp *udo_usock_udp_server_create(struct udo_usock_udp *usock, const void *usock_info);

| Creates a socket that may be utilized for server socket operations.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - usock
		  - | May be ``NULL`` or a pointer to a ``struct`` :c:struct:`udo_usock_udp`.
		    | If ``NULL`` memory will be allocated and return to
		    | caller. If not ``NULL`` address passed will be used
		    | to store the newly created ``struct`` :c:struct:`udo_usock_udp`
		    | instance.
		* - usock_info
		  - | Implementation uses a pointer to a
		    | ``struct`` :c:struct:`udo_usock_udp_server_create_info`.
		    | no other implementation may be passed to
		    | this parameter.

	Returns:
		| **on success:** Pointer to a ``struct`` :c:struct:`udo_usock_udp`
		| **on failure:** ``NULL``

=========================================================================================================================================

==============================
udo_usock_udp_server_recv_data
==============================

.. c:function:: ssize_t udo_usock_udp_server_recv_data(struct udo_usock_udp *usock, void *data, const size_t size, struct sockaddr_un *addr, const void *usock_info);

| Receive data from server socket file descriptor.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - usock
		  - | Pointer to a ``struct`` :c:struct:`udo_usock_udp` instance.
		* - data
		  - | Pointer to buffer to store data received from a socket.
		* - size
		  - | Size of data to receive from a socket.
		* - addr
		  - | Pointer to ``struct`` `sockaddr_un`_ which stores the
		    | address information of the socket that data
		    | was received from.
		* - usock_info
		  - | Reserved for future usage. For now used
		    | to set the flag argument of `recvfrom(2)`_.

	Returns:
		| **on success:** Amount of bytes received
		| **on failure:** # < 0

=========================================================================================================================================

================================
udo_usock_udp_client_create_info
================================

| Structure passed to :c:func:`udo_usock_udp_client_create`
| used to define how to create the client.

.. c:struct:: udo_usock_udp_client_create_info

	.. c:member::
		const char *srv_unix_path;
		const char *cli_unix_path;

	:c:member:`srv_unix_path`
		| Absolute path to unix domain socket to write to.

	:c:member:`cli_unix_path`

	| Absolute path to unix domain socket to read with.
	| Largely so the :c:member:`srv_unix_path` knows the path to
	| the client when leveraging the `recvfrom(2)`_ call.

===========================
udo_usock_udp_client_create
===========================

.. c:function:: struct udo_usock_udp *udo_usock_udp_client_create(struct udo_usock_udp *usock, const void *usock_info);

| Creates a socket that may be utilized for client socket operations.
| Fliters client socket to allow sending data without passing a
| ``struct`` `sockaddr_un`_ to `sendto(2)`_.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - usock
		  - | May be ``NULL`` or a pointer to a ``struct`` :c:struct:`udo_usock_udp`.
		    | If ``NULL`` memory will be allocated and return to
		    | caller. If not ``NULL`` address passed will be used
		    | to store the newly created ``struct`` :c:struct:`udo_usock_udp`
		    | instance.
		* - usock_info
		  - | Implementation uses a pointer to a
		    | ``struct`` :c:struct:`udo_usock_udp_client_create_info`.
		    | no other implementation may be passed to
		    | this parameter.

	Returns:
		| **on success:** Pointer to a ``struct`` :c:struct:`udo_usock_udp`
		| **on failure:** ``NULL``

=========================================================================================================================================

==============================
udo_usock_udp_client_send_data
==============================

.. c:function:: ssize_t udo_usock_udp_client_send_data(struct udo_usock_udp *usock, const void *data, const size_t size, const void *usock_info);

| Send data to client socket address provided via
| call to :c:func:`udo_usock_udp_client_create`.

	.. list-table::
		:header-rows: 1

		* - Param
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_usock_udp`.
		* - data
		  - | Pointer to buffer to send through socket.
		* - size
		  - | Size of data to send through socket.
		* - usock_info
		  - | Reserved for future usage. For now used
		    | to set the flag argument of `sendto(2)`_.

	Returns:
		| **on success:** Amount of bytes sent
		| **on failure:** # < 0

=========================================================================================================================================

====================
udo_usock_udp_get_fd
====================

.. c:function:: int udo_usock_udp_get_fd(struct udo_usock_udp *usock);

| Acquire socket file descriptor associated with
| ``struct`` :c:struct:`udo_usock_udp` instance.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - usock
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_usock_udp`.

	Returns:
		| **on success:** UDP socket file descriptor
		| **on failure:** -1

=========================================================================================================================================

===========================
udo_usock_udp_get_unix_path
===========================

.. c:function:: const char * udo_usock_udp_get_unix_path(struct udo_usock_udp *usock);

| Acquire textual path to unix domain socket file in string
| format associated with ``struct`` :c:struct:`udo_usock_udp` instance.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - usock
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_usock_udp`.

	Returns:
		| **on success:** Textual path to unix domain socket
		| **on failure:** ``NULL``

=========================================================================================================================================

=====================
udo_usock_udp_destroy
=====================

.. c:function:: void udo_usock_udp_destroy(struct udo_usock_udp *usock);

| Frees any allocated memory and closes FD's (if open) created after
| :c:func:`udo_usock_udp_server_create` or :c:func:`udo_usock_udp_client_create` call.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - usock
		  - | Pointer to a valid ``struct`` :c:struct:`udo_usock_udp`.

=========================================================================================================================================

========================
udo_usock_udp_get_sizeof
========================

.. c:function:: int udo_usock_udp_get_sizeof(void);

| Returns size of the internal structure. So,
| if caller decides to allocate memory outside
| of API interface they know the exact amount
| of bytes.

	Returns:
		| **on success:** sizeof(``struct`` :c:struct:`udo_usock_udp`)
		| **on failure:** sizeof(``struct`` :c:struct:`udo_usock_udp`)

=========================================================================================================================================

=======================
udo_usock_udp_recv_data
=======================

.. c:function:: ssize_t udo_usock_udp_recv_data(const int usock_fd, void *data, const size_t size, struct sockaddr_un *addr, const void *usock_info);

| Receive data from socket file descriptor.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - usock_fd
		  - Socket file descriptor to receive data from.
		* - data
		  - | Pointer to buffer to store data received from a socket.
		* - size
		  - | Size of data to receive from a socket.
		* - addr
		  - | Pointer to ``struct`` `sockaddr_un`_ which stores the
		    | address information of the socket that data
		    | was received from.
		* - usock_info
		  - | Reserved for future usage. For now used
		    | to set the flag argument of `recvfrom(2)`_.

	Returns:
		| **on success:** Amount of bytes received
		| **on failure:** # < 0

=========================================================================================================================================

=======================
udo_usock_udp_send_data
=======================

.. c:function:: ssize_t udo_usock_udp_send_data(const int usock_fd, const void *data, const size_t size, const struct sockaddr_un *addr, const void *usock_info);

| Send data to socket file descriptor.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - usock_fd
		  - | Socket file descriptor to send data to.
		* - data
		  - | Pointer to buffer to send through socket.
		* - size
		  - | Size of data to send through socket.
		* - addr
		  - | Pointer to ``struct`` `sockaddr_un`_ which stores the
		    | address information of a socket that data
		    | will be sent to.
		* - usock_info
		  - | Reserved for future usage. For now used
		    | to set the flag argument of `sendto(2)`_.

	Returns:
		| **on success:** Amount of bytes sent
		| **on failure:** # < 0

=========================================================================================================================================

.. _calloc(3): https://www.man7.org/linux/man-pages/man3/malloc.3.html
.. _free(3): https://www.man7.org/linux/man-pages/man3/free.3.html
.. _connect(2): https://www.man7.org/linux/man-pages/man2/connect.2.html
.. _bind(2): https://www.man7.org/linux/man-pages/man2/bind.2.html
.. _sendto(2): https://www.man7.org/linux/man-pages/man2/sendto.2.html
.. _recvfrom(2): https://www.man7.org/linux/man-pages/man2/recvfrom.2.html
.. _sockaddr_un: https://www.man7.org/linux/man-pages/man3/sockaddr.3type.html
