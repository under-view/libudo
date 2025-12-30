.. default-domain:: C

sock-udp (Socket UDP)
=====================

Header: cando/sock-udp.h

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

1. :c:struct:`cando_sock_udp`
#. :c:struct:`cando_sock_udp_server_create_info`
#. :c:struct:`cando_sock_udp_client_create_info`

=========
Functions
=========

1. :c:func:`cando_sock_udp_server_create`
#. :c:func:`cando_sock_udp_server_accept`
#. :c:func:`cando_sock_udp_server_recv_data`
#. :c:func:`cando_sock_udp_client_create`
#. :c:func:`cando_sock_udp_client_connect`
#. :c:func:`cando_sock_udp_client_send_data`
#. :c:func:`cando_sock_udp_get_fd`
#. :c:func:`cando_sock_udp_get_ip_addr`
#. :c:func:`cando_sock_udp_get_port`
#. :c:func:`cando_sock_udp_destroy`
#. :c:func:`cando_sock_udp_get_sizeof`
#. :c:func:`cando_sock_udp_recv_data`
#. :c:func:`cando_sock_udp_send_data`

API Documentation
~~~~~~~~~~~~~~~~~

========================
cando_sock_udp (private)
========================

| Structure defining Cando Socket UDP interface implementation.

.. c:struct:: cando_sock_udp

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
		| File descriptor to the open UDP socket

	:c:member:`ip_addr`
		| Textual network IP address to `sendto(2)`_/`recvfrom(2)`_.

	:c:member:`port`
		| Network port number to `sendto(2)`_/`recvfrom(2)`_ with.

	:c:member:`addr`
		| Stores IPV6 network byte information about the UDP socket context.
		| Is used for client `connect(2)`_ and server `bind(2)`_/`connect(2)`_.

=========================================================================================================================================

=================================
cando_sock_udp_server_create_info
=================================

| Structure passed to :c:func:`cando_sock_udp_server_create`
| used to define how to create the server.

.. c:struct:: cando_sock_udp_server_create_info

	.. c:member::
		unsigned char ipv6 : 1;
		const char    *ip_addr;
		int           port;

	:c:member:`ipv6`
		| Boolean to determine if a socket is soley an ipv6
		| socket or requires an `ipv4-mapped-ipv6-address`_.

	:c:member:`ip_addr`
		| Textual network IP address to `recvfrom(2)`_/`sendto(2)`_ data with.

	:c:member:`port`
		| Network port to `recvfrom(2)`_/`sendto(2)`_ data with.

============================
cando_sock_udp_server_create
============================

.. c:function:: struct cando_sock_udp *cando_sock_udp_server_create(struct cando_sock_udp *sock, const void *sock_info);

| Creates a socket that may be utilized for server socket operations.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock
		  - | May be ``NULL`` or a pointer to a ``struct`` :c:struct:`cando_sock_udp`.
		    | If ``NULL`` memory will be allocated and return to
		    | caller. If not ``NULL`` address passed will be used
		    | to store the newly created ``struct`` :c:struct:`cando_sock_udp`
		    | instance.
		* - sock_info
		  - | Implementation uses a pointer to a
		    | ``struct`` :c:struct:`cando_sock_udp_server_create_info`.
		    | no other implementation may be passed to
		    | this parameter.

	Returns:
		| **on success:** Pointer to a ``struct`` :c:struct:`cando_sock_udp`
		| **on failure:** ``NULL``

=========================================================================================================================================

============================
cando_sock_udp_server_accept
============================

.. c:function:: int cando_sock_udp_server_accept(struct cando_sock_udp *sock, const struct sockaddr_in6 *addr, const unsigned char ipv6);

| Creates file descriptor that can filter for
| ``addr`` data comming to server file descriptor.
| Useful to utilize in an `epoll(2)`_ event loop
| if caller wants to implement tcp like event
| handling with UDP sockets.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock
		  - | Must pass a pointer to a ``struct`` :c:struct:`cando_sock_udp`.
		* - addr
		  - | Must pass a pointer to a populated ``struct`` `sockaddr_in6`_.
		* - ipv6
		  - | Boolean determines if created socket is soley an ipv6
		    | socket or requires an `ipv4-mapped-ipv6-address`_.

	Returns:
		| **on success:** File descriptor to filtered socket
		| **on failure:** ``NULL``

=========================================================================================================================================

===============================
cando_sock_udp_server_recv_data
===============================

.. c:function:: ssize_t cando_sock_udp_server_recv_data(struct cando_sock_udp *sock, void *data, const size_t size, struct sockaddr_in6 *addr, const void *sock_info);

| Receive data from server socket file descriptor.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock
		  - | Pointer to a ``struct`` :c:struct:`cando_sock_udp` instance.
		* - data
		  - | Pointer to buffer to store data received from a socket.
		* - size
		  - | Size of data to receive from a socket.
		* - addr
		  - | Pointer to ``struct`` `sockaddr_in6`_ which stores the
		    | address information of the socket that data
		    | was received from.
		* - sock_info
		  - | Reserved for future usage. For now used
		    | to set the flag argument of `recvfrom(2)`_.

	Returns:
		| **on success:** Amount of bytes received
		| **on failure:** # < 0

=========================================================================================================================================

=================================
cando_sock_udp_client_create_info
=================================

| Structure passed to :c:func:`cando_sock_udp_client_create`
| used to define how to create the client.

.. c:struct:: cando_sock_udp_client_create_info

	.. c:member::
		unsigned char ipv6 : 1;
		const char    *ip_addr;
		int           port;

	:c:member:`ipv6`
		| Boolean to determine if a socket is soley an ipv6
    		| socket or requires an `ipv4-mapped-ipv6-address`_.

	:c:member:`ip_addr`
		| Textual network IP address to `recvfrom(2)`_/`sendto(2)`_ data with.

	:c:member:`port`
		| Network port to `recvfrom(2)`_/`sendto(2)`_ data with.

============================
cando_sock_udp_client_create
============================

.. c:function:: struct cando_sock_udp *cando_sock_udp_client_create(struct cando_sock_udp *sock, const void *sock_info);

| Creates a socket that may be utilized for client socket operations.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock
		  - | May be ``NULL`` or a pointer to a ``struct`` :c:struct:`cando_sock_udp`.
		    | If ``NULL`` memory will be allocated and return to
		    | caller. If not ``NULL`` address passed will be used
		    | to store the newly created ``struct`` :c:struct:`cando_sock_udp`
		    | instance.
		* - sock_info
		  - | Implementation uses a pointer to a
		    | ``struct`` :c:struct:`cando_sock_udp_client_create_info`.
		    | no other implementation may be passed to
		    | this parameter.

	Returns:
		| **on success:** Pointer to a ``struct`` :c:struct:`cando_sock_udp`
		| **on failure:** ``NULL``

=========================================================================================================================================

=============================
cando_sock_udp_client_connect
=============================

.. c:function:: int cando_sock_udp_client_connect(struct cando_sock_udp *sock);

| Fliters client socket to allow sending data
| without passing a ``struct`` `sockaddr_in6`_ to `sendto(2)`_.
| Address is populated with a call to :c:func:`cando_sock_udp_client_create`.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock
		  - | Must pass a pointer to a ``struct`` :c:struct:`cando_sock_udp`.

	Returns:
		| **on success:** 0
		| **on failure:** -1

=========================================================================================================================================

===============================
cando_sock_udp_client_send_data
===============================

.. c:function:: ssize_t cando_sock_udp_client_send_data(struct cando_sock_udp *sock, const void *data, const size_t size, const void *sock_info);

| Send data to client socket address provided via
| call to :c:func:`cando_sock_udp_client_create`.

	.. list-table::
		:header-rows: 1

		* - Param
		  - | Must pass a pointer to a ``struct`` :c:struct:`cando_sock_udp`.
		* - data
		  - | Pointer to buffer to send through socket.
		* - size
		  - | Size of data to send through socket.
		* - sock_info
		  - | Reserved for future usage. For now used
		    | to set the flag argument of `sendto(2)`_.

	Returns:
		| **on success:** Amount of bytes sent
		| **on failure:** # < 0

=========================================================================================================================================

=====================
cando_sock_udp_get_fd
=====================

.. c:function:: int cando_sock_udp_get_fd(struct cando_sock_udp *sock);

| Acquire socket file descriptor associated with
| ``struct`` :c:struct:`cando_sock_udp` instance.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock
		  - | Must pass a pointer to a ``struct`` :c:struct:`cando_sock_udp`.

	Returns:
		| **on success:** UDP socket file descriptor
		| **on failure:** -1

=========================================================================================================================================

==========================
cando_sock_udp_get_ip_addr
==========================

.. c:function:: unsigned int cando_sock_udp_get_ip_addr(struct cando_sock_udp *sock);

| Acquire textual network ip address associated
| with ``struct`` :c:struct:`cando_sock_udp` instance.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock
		  - | Must pass a pointer to a ``struct`` :c:struct:`cando_sock_udp`.

	Returns:
		| **on success:** Textual network ip address
		| **on failure:** ``NULL``

=========================================================================================================================================

=======================
cando_sock_udp_get_port
=======================

.. c:function:: int cando_sock_udp_get_port(struct cando_sock_udp *sock);

| Acquire network port associated with
| ``struct`` :c:struct:`cando_sock_udp` instance.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock
		  - | Must pass a pointer to a ``struct`` :c:struct:`cando_sock_udp`.

	Returns:
		| **on success:** Network port connected to instance
		| **on failure:** -1

=========================================================================================================================================

======================
cando_sock_udp_destroy
======================

.. c:function:: void cando_sock_udp_destroy(struct cando_sock_udp *sock);

| Frees any allocated memory and closes FD's (if open) created after
| :c:func:`cando_sock_udp_server_create` or :c:func:`cando_sock_udp_client_create` call.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - sock
		  - | Pointer to a valid ``struct`` :c:struct:`cando_sock_udp`.

=========================================================================================================================================

=========================
cando_sock_udp_get_sizeof
=========================

.. c:function:: int cando_sock_udp_get_sizeof(void);

| Returns size of the internal structure. So,
| if caller decides to allocate memory outside
| of API interface they know the exact amount
| of bytes.

	Returns:
		| **on success:** sizeof(``struct`` :c:struct:`cando_sock_udp`)
		| **on failure:** sizeof(``struct`` :c:struct:`cando_sock_udp`)

=========================================================================================================================================

========================
cando_sock_udp_recv_data
========================

.. c:function:: ssize_t cando_sock_udp_recv_data(const int sock_fd, void *data, const size_t size, struct sockaddr_in6 *addr, const void *sock_info);

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
		* - addr
		  - | Pointer to ``struct`` `sockaddr_in6`_ which stores the
		    | address information of the socket that data
		    | was received from.
		* - sock_info
		  - | Reserved for future usage. For now used
		    | to set the flag argument of `recvfrom(2)`_.

	Returns:
		| **on success:** Amount of bytes received
		| **on failure:** # < 0

=========================================================================================================================================

========================
cando_sock_udp_send_data
========================

.. c:function:: ssize_t cando_sock_udp_send_data(const int sock_fd, const void *data, const size_t size, const struct sockaddr_in6 *addr, const void *sock_info);

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
		* - addr
		  - | Pointer to ``struct`` `sockaddr_in6`_ which stores the
		    | address information of a socket that data
		    | will be sent to.
		* - sock_info
		  - | Reserved for future usage. For now used
		    | to set the flag argument of `sendto(2)`_.

	Returns:
		| **on success:** Amount of bytes sent
		| **on failure:** # < 0

=========================================================================================================================================

.. _calloc(3): https://www.man7.org/linux/man-pages/man3/malloc.3.html
.. _free(3): https://www.man7.org/linux/man-pages/man3/free.3.html
.. _accept(2): https://www.man7.org/linux/man-pages/man2/accept.2.html
.. _connect(2): https://www.man7.org/linux/man-pages/man2/connect.2.html
.. _bind(2): https://www.man7.org/linux/man-pages/man2/bind.2.html
.. _epoll(2): https://www.man7.org/linux/man-pages/man2/epoll.2.html
.. _sendto(2): https://www.man7.org/linux/man-pages/man2/sendto.2.html
.. _recvfrom(2): https://www.man7.org/linux/man-pages/man2/recvfrom.2.html
.. _sockaddr_in6: https://www.man7.org/linux/man-pages/man7/sock.7.html
.. _ipv4-mapped-ipv6-address: https://en.wikipedia.org/wiki/IPv6#IPv4-mapped_IPv6_addresses
