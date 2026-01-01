.. default-domain:: C

sock-tcp (Socket TCP)
=====================

Header: udo/sock-tcp.h

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

1. :c:struct:`udo_sock_tcp`
#. :c:struct:`udo_sock_tcp_server_create_info`
#. :c:struct:`udo_sock_tcp_client_create_info`

=========
Functions
=========

1. :c:func:`udo_sock_tcp_server_create`
#. :c:func:`udo_sock_tcp_server_accept`
#. :c:func:`udo_sock_tcp_client_create`
#. :c:func:`udo_sock_tcp_client_connect`
#. :c:func:`udo_sock_tcp_client_send_data`
#. :c:func:`udo_sock_tcp_get_fd`
#. :c:func:`udo_sock_tcp_get_ip_addr`
#. :c:func:`udo_sock_tcp_get_port`
#. :c:func:`udo_sock_tcp_destroy`
#. :c:func:`udo_sock_tcp_get_sizeof`
#. :c:func:`udo_sock_tcp_recv_data`
#. :c:func:`udo_sock_tcp_send_data`

API Documentation
~~~~~~~~~~~~~~~~~

======================
udo_sock_tcp (private)
======================

| Structure defining UDO Socket TCP instance.

.. c:struct:: udo_sock_tcp

	.. c:member::
		struct udo_log_error_struct err;
		bool                        free;
		int                         fd;
		char                        ip_addr[INET6_ADDRSTRLEN];
		int                         port;
		struct sockaddr_in6         addr;

	:c:member:`err`
		| Stores information about the error that occured
		| for the given instance and may later be retrieved
		| by caller.

	:c:member:`free`
		| If structure allocated with `calloc(3)`_ member will be
		| set to true so that, we know to call `free(3)`_ when
		| destroying the instance.

	:c:member:`fd`
		| File descriptor to the open TCP socket.

	:c:member:`ip_addr`
		| Textual network IP address to `connect(2)`_ to or `accept(2)`_ with.

	:c:member:`port`
		| Network port number to `connect(2)`_ to or `accept(2)`_ with.

	:c:member:`addr`
		| Stores network byte information about the TCP socket context.
		| Is used for client `connect(2)`_ and server `accept(2)`_.

=========================================================================================================================================

===============================
udo_sock_tcp_server_create_info
===============================

| Structure passed to :c:func:`udo_sock_tcp_server_create`
| used to define how to create the server.

.. c:struct:: udo_sock_tcp_server_create_info

	.. c:member::
		unsigned char ipv6 : 1;
		const char    *ip_addr;
		int           port;
		int           connections;

	:c:member:`ipv6`
		| Boolean to determine if a socket is soley an ipv6
		| socket or requires an `ipv4-mapped-ipv6-address`_.

	:c:member:`ip_addr`
		| Textual network IP addresses to `accept(2)`_ with.

	:c:member:`port`
		| Network port to `accept(2)`_ with.

	:c:member:`connections`
		| Amount of connections that may be queued
		| at a given moment.

==========================
udo_sock_tcp_server_create
==========================

.. c:function:: struct udo_sock_tcp *udo_sock_tcp_server_create(struct udo_sock_tcp *sock, const void *sock_info);

| Creates a socket that may be utilized for server socket operations.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock
		  - | May be ``NULL`` or a pointer to a ``struct`` :c:struct:`udo_sock_tcp`.
		    | If ``NULL`` memory will be allocated and return to
		    | caller. If not ``NULL`` address passed will be used
		    | to store the newly created ``struct`` :c:struct:`udo_sock_tcp`
		    | instance.
		* - sock_info
		  - | Implementation uses a pointer to a
		    | ``struct`` :c:struct:`udo_sock_tcp_server_create_info`.
		    | no other implementation may be passed to
		    | this parameter.

	Returns:
		| **on success:** Pointer to a ``struct`` :c:struct:`udo_sock_tcp`
		| **on failure:** ``NULL``

=========================================================================================================================================

==========================
udo_sock_tcp_server_accept
==========================

.. c:function:: int udo_sock_tcp_server_accept(struct udo_sock_tcp *sock, struct sockaddr_in6 *addr);

| Accepts client connections returns file descriptor
| to the connected client.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_sock_tcp`.
		* - addr
		  - | May be ``NULL`` or a pointer to a ``struct`` `sockaddr_in6`_
		    | If not NULL ``addr`` is filled in via `accept(2)`_ call.

	Returns:
		| **on success:** File descriptor to accepted client
		| **on failure:** ``NULL``

=========================================================================================================================================

===============================
udo_sock_tcp_client_create_info
===============================

| Structure passed to :c:func:`udo_sock_tcp_client_create`
| used to define how to create the client.

.. c:struct:: udo_sock_tcp_client_create_info

	.. c:member::
		unsigned char ipv6 : 1;
		const char    *ip_addr;
		int           port;

	:c:member:`ipv6`
		| Boolean to determine if a socket is soley an ipv6
		| socket or requires an `ipv4-mapped-ipv6-address`_.

	:c:member:`ip_addr`
		| Textual network IP addresses to `connect(2)`_/`send(2)`_ to.

	:c:member:`port`
		| Network port to `connect(2)`_/`send(2)`_ to.

==========================
udo_sock_tcp_client_create
==========================

.. c:function:: struct udo_sock_tcp *udo_sock_tcp_client_create(struct udo_sock_tcp *sock, const void *sock_info);

| Creates a socket that may be utilized for client socket operations.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock
		  - | May be ``NULL`` or a pointer to a ``struct`` :c:struct:`udo_sock_tcp`.
		    | If ``NULL`` memory will be allocated and return to
		    | caller. If not ``NULL`` address passed will be used
		    | to store the newly created ``struct`` :c:struct:`udo_sock_tcp`
		    | instance.
		* - sock_info
		  - | Implementation uses a pointer to a
		    | ``struct`` :c:struct:`udo_sock_tcp_client_create_info`.
		    | no other implementation may be passed to
		    | this parameter.

	Returns:
		| **on success:** Pointer to a ``struct`` :c:struct:`udo_sock_tcp`
		| **on failure:** ``NULL``

=========================================================================================================================================

===========================
udo_sock_tcp_client_connect
===========================

.. c:function:: int udo_sock_tcp_client_connect(struct udo_sock_tcp *sock);

| Connects client socket to address provided via
| call to :c:func:`udo_sock_tcp_client_create`.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_sock_tcp`.

	Returns:
		| **on success:** 0
		| **on failure:** -1

=========================================================================================================================================

=============================
udo_sock_tcp_client_send_data
=============================

.. c:function:: ssize_t udo_sock_tcp_client_send_data(struct udo_sock_tcp *sock, const void *data, const size_t size, const void *sock_info);

| Send data to client socket address provided via
| call to :c:func:`udo_sock_tcp_client_create`.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_sock_tcp`.
		* - data
		  - | Pointer to buffer to send through socket.
		* - size
		  - | Size of data to send through socket.
		* - sock_info
		  - | Reserved for future usage. For now used
		    | to set the flag argument of `send(2)`_.

	Returns:
		| **on success:** Amount of bytes sent
		| **on failure:** # < 0

=========================================================================================================================================

===================
udo_sock_tcp_get_fd
===================

.. c:function:: int udo_sock_tcp_get_fd(struct udo_sock_tcp *sock);

| Acquire socket file descriptor associated with
| ``struct`` :c:struct:`udo_sock_tcp` instance.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_sock_tcp`.

	Returns:
		| **on success:** TCP socket file descriptor
		| **on failure:** -1

=========================================================================================================================================

========================
udo_sock_tcp_get_ip_addr
========================

.. c:function:: unsigned int udo_sock_tcp_get_ip_addr(struct udo_sock_tcp *sock);

| Acquire textual network address in string format
| associated with ``struct`` :c:struct:`udo_sock_tcp` instance.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_sock_tcp`.

	Returns:
		| **on success:** Textual network address
		| **on failure:** ``NULL``

=========================================================================================================================================

=====================
udo_sock_tcp_get_port
=====================

.. c:function:: int udo_sock_tcp_get_port(struct udo_sock_tcp *sock);

| Acquire network port associated with
| ``struct`` :c:struct:`udo_sock_tcp` instance.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_sock_tcp`.

	Returns:
		| **on success:** Network port connected to instance
		| **on failure:** -1

=========================================================================================================================================

====================
udo_sock_tcp_destroy
====================

.. c:function:: void udo_sock_tcp_destroy(struct udo_sock_tcp *sock);

| Frees any allocated memory and closes FD's (if open) created after
| :c:func:`udo_sock_tcp_server_create` or :c:func:`udo_sock_tcp_client_create` call.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock
		  - | Pointer to a valid ``struct`` :c:struct:`udo_sock_tcp`.

=========================================================================================================================================

=======================
udo_sock_tcp_get_sizeof
=======================

.. c:function:: int udo_sock_tcp_get_sizeof(void);

| Returns size of the internal structure. So,
| if caller decides to allocate memory outside
| of API interface they know the exact amount
| of bytes.

	Returns:
		| **on success:** sizeof(``struct`` :c:struct:`udo_sock_tcp`)
		| **on failure:** sizeof(``struct`` :c:struct:`udo_sock_tcp`)

=========================================================================================================================================

======================
udo_sock_tcp_recv_data
======================

.. c:function:: ssize_t udo_sock_tcp_recv_data(const int sock_fd, void *data, const size_t size, const void *sock_info);

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
		* - sock_info
		  - | Reserved for future usage. For now used
		    | to set the flag argument of `recv(2)`_.

	Returns:
		| **on success:** Amount of bytes received
		| **on failure:** # < 0

=========================================================================================================================================

======================
udo_sock_tcp_send_data
======================

.. c:function:: ssize_t udo_sock_tcp_send_data(const int sock_fd, const void *data, const size_t size, const void *sock_info);

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
		* - sock_info
		  - | Reserved for future usage. For now used
		    | to set the flag argument of `send(2)`_.

	Returns:
		| **on success:** Amount of bytes sent
		| **on failure:** # < 0

=========================================================================================================================================

.. _calloc(3): https://www.man7.org/linux/man-pages/man3/malloc.3.html
.. _free(3): https://www.man7.org/linux/man-pages/man3/free.3.html
.. _accept(2): https://www.man7.org/linux/man-pages/man2/accept.2.html
.. _connect(2): https://www.man7.org/linux/man-pages/man2/connect.2.html
.. _send(2): https://www.man7.org/linux/man-pages/man2/send.2.html
.. _recv(2): https://www.man7.org/linux/man-pages/man2/recv.2.html
.. _sockaddr_in6: https://www.man7.org/linux/man-pages/man7/sock.7.html
.. _ipv4-mapped-ipv6-address: https://en.wikipedia.org/wiki/IPv6#IPv4-mapped_IPv6_addresses
