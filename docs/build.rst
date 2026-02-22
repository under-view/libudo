.. _build libudo:

Build libudo
============

Dependencies
~~~~~~~~~~~~

For better dependency version control underview builds all packages required from source.

=======================
Underview Build Depends
=======================

See `ubuild-depends (dependencies)`_ to get going. For this repo
you only need the system dependencies.

.. code-block::

	$ git clone https://github.com/under-view/ubuild-depends.git
	$ source setenvars.sh

=================
Yocto Project SDK
=================

Download SDK from `ubuild-depends (releases)`_

.. code-block::

	$ ./x86_64-0.1.0-underview.sh
	sdk # Folder to place libs
	$ source environment-setup-zen1-underview-linux

Meson
~~~~~

=======
Options
=======

All options/features are disabled by default.

.. code-block::
	:linenos:

	c_std=c11
	buildtype=release
	default_library=shared
	tests=true           # Default [false]
	docs=true            # Default [false]
	file-offset-bits=32  # Default [64]
	file-ops=enabled     # Default [disabled]
	jpool=enabled        # Default [disabled]
	shm=enabled          # Default [disabled]
	sock-tcp=enabled     # Default [disabled]
	sock-udp=enabled     # Default [disabled]
	csock-raw=enabled    # Default [disabled]
	usock-tcp=enabled    # Default [disabled]
	usock-udp=enabled    # Default [disabled]
	vsock-tcp=enabled    # Default [disabled]
	vsock-udp=enabled    # Default [disabled]

======================
Build/Install (Normal)
======================

.. code-block::

	$ meson setup [options] build
	$ ninja install -C build

.. code-block::

	$ meson setup \
		-Dtests="true" \
		-Ddocs="false" \
		-Dfile-offset-bits=64 \
		-Dfile-ops="enabled" \
		-Djpool="enabled" \
		-Dshm="enabled" \
		-Dsock-tcp="enabled" \
		-Dsock-udp="enabled" \
		-Dcsock-raw="enabled" \
		-Dusock-tcp="enabled" \
		-Dusock-udp="enabled" \
		-Dvsock-tcp="enabled" \
		-Dvsock-udp="enabled" \
		build
	$ ninja install -C build

===================
Build/Install (SDK)
===================

.. code-block::

	# Yocto Project SDK Builds
	$ meson setup \
		--prefix="${SDKTARGETSYSROOT}/usr" \
		--libdir="${SDKTARGETSYSROOT}/usr/lib64" \
		[options] \
		build
	$ ninja install -C build

.. code-block::

	$ meson setup \
		--prefix="${SDKTARGETSYSROOT}/usr" \
		--libdir="${SDKTARGETSYSROOT}/usr/lib64" \
		-Dtests="true" \
		-Ddocs="false" \
		-Dfile-offset-bits=64 \
		-Dfile-ops="enabled" \
		-Djpool="enabled" \
		-Dshm="enabled" \
		-Dsock-tcp="enabled" \
		-Dsock-udp="enabled" \
		-Dcsock-raw="enabled" \
		-Dusock-tcp="enabled" \
		-Dusock-udp="enabled" \
		-Dvsock-tcp="enabled" \
		-Dvsock-udp="enabled" \
		build
	$ ninja install -C build

=======
Include
=======

.. code-block::
	:linenos:

	# Clone libudo or create a udo.wrap under <source_root>/subprojects
	project('name', 'c')

	udo_dep = dependency('udo', required : true)

	executable('exe', 'src/main.c', dependencies : udo_dep)

.. code-block::
	:linenos:

	#include <udo/udo.h>

Documentation (Sphinx)
~~~~~~~~~~~~~~~~~~~~~~

libudo uses sphinx framework for documentation. Primarily utilizing `The C Domain`_.

https://www.sphinx-doc.org/en/master/man/sphinx-build.html

============
Dependencies
============

- python3-pip

==========
Build Docs
==========

.. code-block::

	$ git clone https://github.com/under-view/libudo.git
	$ cd libudo
	$ sudo pip3 install -r docs/requirements.txt

	# If no build directory exists
	$ meson setup -Ddocs=true build

	# If build directory exists
	$ meson configure -Ddocs=true build

	$ ninja docs -C build

.. _ubuild-depends: https://github.com/under-view/ubuild-depends
.. _ubuild-depends (dependencies): https://github.com/under-view/ubuild-depends#dependencies
.. _ubuild-depends (releases): https://github.com/under-view/ubuild-depends/releases
.. _The C Domain: https://www.sphinx-doc.org/en/master/usage/restructuredtext/domains.html#the-c-domain
