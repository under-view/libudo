.. default-domain:: C

vsock-tcp (VM Socket TCP)
=========================

Header: udo/vsock-tcp.h

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

1. :c:struct:`udo_vsock_tcp`
#. :c:struct:`udo_vsock_tcp_server_create_info`
#. :c:struct:`udo_vsock_tcp_client_create_info`

=========
Functions
=========

1. :c:func:`udo_vsock_tcp_server_create`
#. :c:func:`udo_vsock_tcp_server_accept`
#. :c:func:`udo_vsock_tcp_client_create`
#. :c:func:`udo_vsock_tcp_client_connect`
#. :c:func:`udo_vsock_tcp_client_send_data`
#. :c:func:`udo_vsock_tcp_get_fd`
#. :c:func:`udo_vsock_tcp_get_vcid`
#. :c:func:`udo_vsock_tcp_get_port`
#. :c:func:`udo_vsock_tcp_destroy`
#. :c:func:`udo_vsock_tcp_get_sizeof`
#. :c:func:`udo_vsock_tcp_get_local_vcid`
#. :c:func:`udo_vsock_tcp_recv_data`
#. :c:func:`udo_vsock_tcp_send_data`

API Documentation
~~~~~~~~~~~~~~~~~

==============
Kernel Modules
==============

**On Host Machine**

.. code-block:: sh

	sudo modprobe -a vhost_vsock vsock_loopback

**Guest Machine Kernel Config Symbols**

.. code-block:: sh

	CONFIG_PCI=Y
	CONFIG_VIRTIO_MENU=y
	CONFIG_VIRTIO_PCI=y
	CONFIG_VIRTIO_BALLON=y
	CONFIG_VSOCKETS_DIAG=y

	CONFIG_NET=y
	CONFIG_VSOCKETS=y
	CONFIG_VIRTIO_VSOCKETS=y

=======================
udo_vsock_tcp (private)
=======================

| Structure defining UDO VM Socket TCP instance.

.. c:struct:: udo_vsock_tcp

	.. c:member::
		struct udo_log_error_struct err;
		bool                        free;
		int                         fd;
		unsigned int                vcid;
		int                         port;
		struct sockaddr_vm          addr;

	:c:member:`err`
		| Stores information about the error that occured
		| for the given instance and may later be retrieved
		| by caller.

	:c:member:`free`
		| If structure allocated with `calloc(3)`_ member will be
		| set to true so that, we know to call `free(3)`_ when
		| destroying the instance.

	:c:member:`fd`
		| File descriptor to the open VM socket.

	:c:member:`vcid`
		| VM Context Identifier.

	:c:member:`port`
		| Network port number to `connect(2)`_ to or `accept(2)`_ from.

	:c:member:`addr`
		| Stores byte information about the VM socket context.
		| Is used for client `connect(2)`_ and server `accept(2)`_.

=========================================================================================================================================

================================
udo_vsock_tcp_server_create_info
================================

| Structure passed to :c:func:`udo_vsock_tcp_server_create`
| used to define how to create the server.

.. c:struct:: udo_vsock_tcp_server_create_info

	.. c:member::
		unsigned int vcid;
		int          port;
		int          connections;

	:c:member:`vcid`
		| VM Context Identifier to `accept(2)`_ with.

	:c:member:`port`
		| Network port to `accept(2)`_ with.

	:c:member:`connections`
		| Amount of connections that may be queued
		| at a given moment.

===========================
udo_vsock_tcp_server_create
===========================

.. c:function:: struct udo_vsock_tcp *udo_vsock_tcp_server_create(struct udo_vsock_tcp *vsock, const void *vsock_info);

| Creates a VM socket that may be utilized for server socket operations.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - vsock
		  - | May be ``NULL`` or a pointer to a ``struct`` :c:struct:`udo_vsock_tcp`.
		    | If ``NULL`` memory will be allocated and return to
		    | caller. If not ``NULL`` address passed will be used
		    | to store the newly created ``struct`` :c:struct:`udo_vsock_tcp`
		    | instance.
		* - vsock_info
		  - | Implementation uses a pointer to a
		    | ``struct`` :c:struct:`udo_vsock_tcp_server_create_info`.
		    | no other implementation may be passed to
		    | this parameter.

	Returns:
		| **on success:** Pointer to a ``struct`` :c:struct:`udo_vsock_tcp`
		| **on failure:** ``NULL``

=========================================================================================================================================

===========================
udo_vsock_tcp_server_accept
===========================

.. c:function:: int udo_vsock_tcp_server_accept(struct udo_vsock_tcp *vsock, struct sockaddr_vm *addr);

| Accepts client connections returns file descriptor
| to the connected client.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - vsock
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_vsock_tcp`.
		* - addr
		  - | May be ``NULL`` or a pointer to a ``struct`` `sockaddr_vm`_
		    | If not NULL ``addr`` is filled in via `accept(2)`_ call.

	Returns:
		| **on success:** File descriptor to accepted client
		| **on failure:** ``NULL``

=========================================================================================================================================

================================
udo_vsock_tcp_client_create_info
================================

| Structure passed to :c:func:`udo_vsock_tcp_client_create`
| used to define how to create the client.

.. c:struct:: udo_vsock_tcp_client_create_info

	.. c:member::
		unsigned int vcid;
		int          port;

	:c:member:`vcid`
		| VM Context Identifier to `connect(2)`_/`send(2)`_ to.

	:c:member:`port`
		| Network port to `connect(2)`_/`send(2)`_ to.

===========================
udo_vsock_tcp_client_create
===========================

.. c:function:: struct udo_vsock_tcp *udo_vsock_tcp_client_create(struct udo_vsock_tcp *vsock, const void *vsock_info);

| Creates a VM socket that may be utilized for client socket operations.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - vsock
		  - | May be ``NULL`` or a pointer to a ``struct`` :c:struct:`udo_vsock_tcp`.
		    | If ``NULL`` memory will be allocated and return to
		    | caller. If not ``NULL`` address passed will be used
		    | to store the newly created ``struct`` :c:struct:`udo_vsock_tcp`
		    | instance.
		* - vsock_info
		  - | Implementation uses a pointer to a
		    | ``struct`` :c:struct:`udo_vsock_tcp_client_create_info`.
		    | no other implementation may be passed to
		    | this parameter.

	Returns:
		| **on success:** Pointer to a ``struct`` :c:struct:`udo_vsock_tcp`
		| **on failure:** ``NULL``

=========================================================================================================================================

============================
udo_vsock_tcp_client_connect
============================

.. c:function:: int udo_vsock_tcp_client_connect(struct udo_vsock_tcp *vsock);

| Connects client socket to address provided via
| call to :c:func:`udo_vsock_tcp_client_create`.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - vsock
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_vsock_tcp`.

	Returns:
		| **on success:** 0
		| **on failure:** -1

=========================================================================================================================================

==============================
udo_vsock_tcp_client_send_data
==============================

.. c:function:: ssize_t udo_vsock_tcp_client_send_data(struct udo_vsock_tcp *vsock, const void *data, const size_t size, const void *vsock_info);

| Send data to client socket address provided via
| call to :c:func:`udo_vsock_tcp_client_create`.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - vsock
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_vsock_tcp`.
		* - data
		  - | Pointer to buffer to send through socket.
		* - size
		  - | Size of data to send through socket.
		* - vsock_info
		  - | Reserved for future usage. For now used
		    | to set the flag argument of `send(2)`_.

	Returns:
		| **on success:** Amount of bytes sent
		| **on failure:** # < 0

=========================================================================================================================================

====================
udo_vsock_tcp_get_fd
====================

.. c:function:: int udo_vsock_tcp_get_fd(struct udo_vsock_tcp *vsock);

| Acquire VM socket file descriptor associated with
| ``struct`` :c:struct:`udo_vsock_tcp` instance.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - vsock
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_vsock_tcp`.

	Returns:
		| **on success:** VM socket file descriptor
		| **on failure:** -1

=========================================================================================================================================

======================
udo_vsock_tcp_get_vcid
======================

.. c:function:: unsigned int udo_vsock_tcp_get_vcid(struct udo_vsock_tcp *vsock);

| Acquire VM socket context identifier associated with
| ``struct`` :c:struct:`udo_vsock_tcp` instance.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - vsock
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_vsock_tcp`.

	Returns:
		| **on success:** VM socket context identifier
		| **on failure:** ``UINT32_MAX``

=========================================================================================================================================

======================
udo_vsock_tcp_get_port
======================

.. c:function:: int udo_vsock_tcp_get_port(struct udo_vsock_tcp *vsock);

| Acquire network port associated with
| ``struct`` :c:struct:`udo_vsock_tcp` instance.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - vsock
		  - | Must pass a pointer to a ``struct`` :c:struct:`udo_vsock_tcp`.

	Returns:
		| **on success:** Network port connected to instance
		| **on failure:** -1

=========================================================================================================================================

=====================
udo_vsock_tcp_destroy
=====================

.. c:function:: void udo_vsock_tcp_destroy(struct udo_vsock_tcp *vsock);

| Frees any allocated memory and closes FD's (if open) created after
| :c:func:`udo_vsock_tcp_server_create` or :c:func:`udo_vsock_tcp_client_create` call.

	.. list-table::
		:header-rows: 1

		* - Param
	          - Decription
		* - vsock
		  - | Pointer to a valid ``struct`` :c:struct:`udo_vsock_tcp`.

=========================================================================================================================================

========================
udo_vsock_tcp_get_sizeof
========================

.. c:function:: int udo_vsock_tcp_get_sizeof(void);

| Returns size of the internal structure. So,
| if caller decides to allocate memory outside
| of API interface they know the exact amount
| of bytes.

	Returns:
		| **on success:** sizeof(``struct`` :c:struct:`udo_vsock_tcp`)
		| **on failure:** sizeof(``struct`` :c:struct:`udo_vsock_tcp`)

=========================================================================================================================================

============================
udo_vsock_tcp_get_local_vcid
============================

.. c:function:: unsigned int udo_vsock_tcp_get_local_vcid(void);

| Returns the local CID of the VM/Hypervisor after
| acquiring it from ``/dev/vsock``.

	Returns:
		| **on success:** Local VM context identifer
		| **on failure:** ``UINT32_MAX``

=========================================================================================================================================

=======================
udo_vsock_tcp_recv_data
=======================

.. c:function:: ssize_t udo_vsock_tcp_recv_data(const int sock_fd, void *data, const size_t size, const void *vsock_info);

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
		* - vsock_info
		  - | Reserved for future usage. For now used
		    | to set the flag argument of `recv(2)`_.

	Returns:
		| **on success:** Amount of bytes received
		| **on failure:** # < 0

=========================================================================================================================================

=======================
udo_vsock_tcp_send_data
=======================

.. c:function:: ssize_t udo_vsock_tcp_send_data(const int sock_fd, const void *data, const size_t size, const void *vsock_info);

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
		* - vsock_info
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
.. _sockaddr_vm: https://www.man7.org/linux/man-pages/man7/vsock.7.html
