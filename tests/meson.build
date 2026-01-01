libcmocka = dependency('cmocka', required: true)

progs = [ 'test-log.c', 'test-mm.c', 'test-futex.c' ]

if file_ops.enabled()
  progs += ['test-file-ops.c']
endif

if shm.enabled()
  progs += ['test-shm.c']
endif

if sock_tcp.enabled()
  progs += ['test-sock-tcp.c']
endif

if sock_udp.enabled()
  progs += ['test-sock-udp.c']
endif

if csock_raw.enabled()
  progs += ['test-csock-raw.c']
  csock_raw_iface = get_option('vcan-iface')
endif

if usock_tcp.enabled()
  progs += ['test-usock-tcp.c']
endif

if usock_udp.enabled()
  progs += ['test-usock-udp.c']
endif

if vsock_tcp.enabled()
  progs += ['test-vsock-tcp.c']
endif

if vsock_udp.enabled()
  progs += ['test-vsock-udp.c']
endif

original_args = pargs
foreach p : progs
  exec_name = p.substring(0,-2) # remove .c extension from name

  if exec_name == 'test-file-ops'
    pargs += ['-DTESTER_FILE_ONE="@0@/tests/tester-file-one.txt"'.format(src_root_dir)]
  endif

  if exec_name == 'test-csock-raw'
    pargs += ['-DTEST_VCAN_IFACE="@0@"'.format(csock_raw_iface)]
  endif

  exec = executable(exec_name, p,
                    link_with: libudo_a,
                    dependencies: [libcmocka],
                    include_directories: [inc],
                    c_args: pargs,
                    install: false)

  test(exec_name, exec)
  pargs = original_args
endforeach
