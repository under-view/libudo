rt = cc.find_library('rt', required: shm.enabled())
libpthread = cc.find_library('pthread', required: jpool.enabled())

libudo_deps = [rt, libpthread]

fs = files([
  'futex.c',
  'log.c',
  'mm.c',
])

if file_ops.enabled()
  fs += files('file-ops.c')
endif

if shm.enabled()
  fs += files('shm.c')
endif

if sock_tcp.enabled()
  fs += files('sock-tcp.c')
endif

if sock_udp.enabled()
  fs += files('sock-udp.c')
endif

if jpool.enabled()
  fs += files('jpool.c')
endif

if csock_raw.enabled()
  fs += files('csock-raw.c')
endif

if usock_tcp.enabled()
  fs += files('usock-tcp.c')
endif

if usock_udp.enabled()
  fs += files('usock-udp.c')
endif

if vsock_tcp.enabled()
  fs += files('vsock-tcp.c')
endif

if vsock_udp.enabled()
  fs += files('vsock-udp.c')
endif
