rt = cc.find_library('rt', required: shm.enabled())

libudo_deps = [rt]

fs = [
  'src/futex.c',
  'src/log.c',
  'src/mm.c',
]

if file_ops.enabled()
  fs += ['src/file-ops.c']
endif

if shm.enabled()
  fs += ['src/shm.c']
endif

if sock_tcp.enabled()
  fs += ['src/sock-tcp.c']
endif

if sock_udp.enabled()
  fs += ['src/sock-udp.c']
endif

if csock_raw.enabled()
  fs += ['src/csock-raw.c']
endif

if usock_tcp.enabled()
  fs += ['src/usock-tcp.c']
endif

if usock_udp.enabled()
  fs += ['src/usock-udp.c']
endif

if vsock_tcp.enabled()
  fs += ['src/vsock-tcp.c']
endif

if vsock_udp.enabled()
  fs += ['src/vsock-udp.c']
endif
