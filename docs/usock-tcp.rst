.. default-domain:: C

usock-tcp (Unix Socket TCP)
===========================

Header: cando/usock-tcp.h

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

1. :c:struct:`cando_usock_tcp`
#. :c:struct:`cando_usock_tcp_server_create_info`
#. :c:struct:`cando_usock_tcp_client_create_info`

=========
Functions
=========

1. :c:func:`cando_usock_tcp_server_create`
#. :c:func:`cando_usock_tcp_server_accept`
#. :c:func:`cando_usock_tcp_client_create`
#. :c:func:`cando_usock_tcp_client_connect`
#. :c:func:`cando_usock_tcp_client_send_data`
#. :c:func:`cando_usock_tcp_get_fd`
#. :c:func:`cando_usock_tcp_get_unix_path`
#. :c:func:`cando_usock_tcp_destroy`
#. :c:func:`cando_usock_tcp_get_sizeof`
#. :c:func:`cando_usock_tcp_recv_data`
#. :c:func:`cando_usock_tcp_send_data`

API Documentation
~~~~~~~~~~~~~~~~~

=========================
cando_usock_tcp (private)
=========================

| Structure defining Cando Unix Domain Socket TCP interface implementation.

.. c:struct:: cando_usock_tcp

	.. c:member::
		struct udo_log_error_struct err;
		bool                        free;
		int                         fd;
		struct sockaddr_un          addr;

	:c:member:`err`
		| Stores information about the error that occured
		| for the given instance and may later be retrieved
		| by caller.

	:c:member:`free`
		| If structure allocated with `calloc(3)`_ member will be
		| set to true so that, we know to call `free(3)`_ when
		| destroying the instance.

	:c:member:`fd`
		| File descriptor to the open TCP unix domain socket.

	:c:member:`addr`
		| Stores byte information about the TCP unix domain socket
		| context. Is used for client `connect(2)`_ and server `accept(2)`_.

=========================================================================================================================================

==================================
cando_usock_tcp_server_create_info
==================================

| Structure passed to :c:func:`cando_usock_tcp_server_create`
| used to define how to create the server.

.. c:struct:: cando_usock_tcp_server_create_info

	.. c:member::
		const char *unix_path;
		int        connections;

	:c:member:`unix_path`
		| Absolute path to unix domain socket.

	:c:member:`connections`
		| Amount of connections that may be queued
		| at a given moment.

=============================
cando_usock_tcp_server_create
=============================

.. c:function:: struct cando_usock_tcp *cando_usock_tcp_server_create(struct cando_usock_tcp *usock, const void *usock_info);

| Creates a socket that may be utilized for server socket operations.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - usock
		  - | May be ``NULL`` or a pointer to a ``struct`` :c:struct:`cando_usock_tcp`.
		    | If ``NULL`` memory will be allocated and return to
		    | caller. If not ``NULL`` address passed will be used
		    | to store the newly created ``struct`` :c:struct:`cando_usock_tcp`
		    | instance.
		* - usock_info
		  - | Implementation uses a pointer to a
		    | ``struct`` :c:struct:`cando_usock_tcp_server_create_info`.
		    | no other implementation may be passed to
		    | this parameter.

	Returns:
		| **on success:** Pointer to a ``struct`` :c:struct:`cando_usock_tcp`
		| **on failure:** ``NULL``

=========================================================================================================================================

=============================
cando_usock_tcp_server_accept
=============================

.. c:function:: int cando_usock_tcp_server_accept(struct cando_usock_tcp *usock, struct sockaddr_un *addr);

| Accepts client connections returns file descriptor
| to the connected client.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - usock
		  - | Must pass a pointer to a ``struct`` :c:struct:`cando_usock_tcp`.
		* - addr
		  - | May be ``NULL`` or a pointer to a ``struct`` `sockaddr_un`_
		    | If not NULL ``addr`` is filled in via `accept(2)`_ call.

	Returns:
		| **on success:** File descriptor to accepted client
		| **on failure:** ``NULL``

=========================================================================================================================================

==================================
cando_usock_tcp_client_create_info
==================================

| Structure passed to :c:func:`cando_usock_tcp_client_create`
| used to define how to create the client.

.. c:struct:: cando_usock_tcp_client_create_info

	.. c:member::
		const char    *unix_path;

	:c:member:`unix_path`
		| Absolute path to unix domain socket.

=============================
cando_usock_tcp_client_create
=============================

.. c:function:: struct cando_usock_tcp *cando_usock_tcp_client_create(struct cando_usock_tcp *usock, const void *usock_info);

| Creates a socket that may be utilized for client socket operations.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - usock
		  - | May be ``NULL`` or a pointer to a ``struct`` :c:struct:`cando_usock_tcp`.
		    | If ``NULL`` memory will be allocated and return to
		    | caller. If not ``NULL`` address passed will be used
		    | to store the newly created ``struct`` :c:struct:`cando_usock_tcp`
		    | instance.
		* - usock_info
		  - | Implementation uses a pointer to a
		    | ``struct`` :c:struct:`cando_usock_tcp_client_create_info`.
		    | no other implementation may be passed to
		    | this parameter.

	Returns:
		| **on success:** Pointer to a ``struct`` :c:struct:`cando_usock_tcp`
		| **on failure:** ``NULL``

=========================================================================================================================================

==============================
cando_usock_tcp_client_connect
==============================

.. c:function:: int cando_usock_tcp_client_connect(struct cando_usock_tcp *usock);

| Connects client socket to address provided via
| call to :c:func:`cando_usock_tcp_client_create`.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - usock
		  - | Must pass a pointer to a ``struct`` :c:struct:`cando_usock_tcp`.

	Returns:
		| **on success:** 0
		| **on failure:** -1

=========================================================================================================================================

================================
cando_usock_tcp_client_send_data
================================

.. c:function:: ssize_t cando_usock_tcp_client_send_data(struct cando_usock_tcp *usock, const void *data, const size_t size, const void *usock_info);

| Send data to client socket address provided via
| call to :c:func:`cando_usock_tcp_client_create`.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - usock
		  - | Must pass a pointer to a ``struct`` :c:struct:`cando_usock_tcp`.
		* - data
		  - | Pointer to buffer to send through socket.
		* - size
		  - | Size of data to send through socket.
		* - usock_info
		  - | Reserved for future usage. For now used
		    | to set the flag argument of `send(2)`_.

	Returns:
		| **on success:** Amount of bytes sent
		| **on failure:** # < 0

=========================================================================================================================================

======================
cando_usock_tcp_get_fd
======================

.. c:function:: int cando_usock_tcp_get_fd(struct cando_usock_tcp *usock);

| Acquire socket file descriptor associated with
| ``struct`` :c:struct:`cando_usock_tcp` instance.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - usock
		  - | Must pass a pointer to a ``struct`` :c:struct:`cando_usock_tcp`.

	Returns:
		| **on success:** TCP unix domain socket file descriptor
		| **on failure:** -1

=========================================================================================================================================

=============================
cando_usock_tcp_get_unix_path
=============================

.. c:function:: unsigned int cando_usock_tcp_get_unix_path(struct cando_usock_tcp *usock);

| Acquire textual path to unix domain socket file in string
| format associated with ``struct`` :c:struct:`cando_usock_tcp` instance.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - usock
		  - | Must pass a pointer to a ``struct`` :c:struct:`cando_usock_tcp`.

	Returns:
		| **on success:** Textual path to unix domain socket
		| **on failure:** ``NULL``

=========================================================================================================================================

=======================
cando_usock_tcp_destroy
=======================

.. c:function:: void cando_usock_tcp_destroy(struct cando_usock_tcp *usock);

| Frees any allocated memory and closes FD's (if open) created after
| :c:func:`cando_usock_tcp_server_create` or :c:func:`cando_usock_tcp_client_create` call.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - usock
		  - | Pointer to a valid ``struct`` :c:struct:`cando_usock_tcp`.

=========================================================================================================================================

==========================
cando_usock_tcp_get_sizeof
==========================

.. c:function:: int cando_usock_tcp_get_sizeof(void);

| Returns size of the internal structure. So,
| if caller decides to allocate memory outside
| of API interface they know the exact amount
| of bytes.

	Returns:
		| **on success:** sizeof(``struct`` :c:struct:`cando_usock_tcp`)
		| **on failure:** sizeof(``struct`` :c:struct:`cando_usock_tcp`)

=========================================================================================================================================

=========================
cando_usock_tcp_recv_data
=========================

.. c:function:: ssize_t cando_usock_tcp_recv_data(const int sock_fd, void *data, const size_t size, const void *usock_info);

| Receive data from socket file descriptor.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock_fd
		  - Socket file descriptor to receive data from.
		* - data
		  - | Pointer to buffer to store data received from a socket.
		* - size
		  - | Size of data to receive from a socket.
		* - usock_info
		  - | Reserved for future usage. For now used
		    | to set the flag argument of `recv(2)`_.

	Returns:
		| **on success:** Amount of bytes received
		| **on failure:** # < 0

=========================================================================================================================================

=========================
cando_usock_tcp_send_data
=========================

.. c:function:: ssize_t cando_usock_tcp_send_data(const int sock_fd, const void *data, const size_t size, const void *usock_info);

| Send data to socket file descriptor.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock_fd
		  - | Socket file descriptor to send data to.
		* - data
		  - | Pointer to buffer to send through socket.
		* - size
		  - | Size of data to send through socket.
		* - usock_info
		  - | Reserved for future usage. For now used
		    | to set the flag argument of `send(2)`_.

	Returns:
		| **on success:** Amount of bytes sent
		| **on failure:** # < 0

=========================================================================================================================================

.. _calloc(3): https://www.man7.org/linux/man-pages/man3/malloc.3.html
.. _free(3): https://www.man7.org/linux/man-pages/man3/free.3.html
.. _connect(2): https://www.man7.org/linux/man-pages/man2/connect.2.html
.. _send(2): https://www.man7.org/linux/man-pages/man2/send.2.html
.. _recv(2): https://www.man7.org/linux/man-pages/man2/recv.2.html
.. _accept(2): https://www.man7.org/linux/man-pages/man2/accept.2.html
.. _sockaddr_un: https://www.man7.org/linux/man-pages/man3/sockaddr.3type.html
