project(
  'udo',
  'c',
  version: '0.1.10',
  license: 'MIT',
  meson_version: '>=1.3.1',
  default_options: [
    'warning_level=2',
    'werror=true',
    'prefix=/usr/'
  ],
)

pargs = []
cc = meson.get_compiler('c')
so_version = '.'.join(meson.project_version())
project_name = meson.project_name()
src_root_dir = meson.current_source_dir()
build_root_dir = meson.current_build_dir()

file_offset_bits = get_option('file-offset-bits')
file_ops = get_option('file-ops')
shm = get_option('shm')
sock_tcp = get_option('sock-tcp')
sock_udp = get_option('sock-udp')
jpool = get_option('jpool')
csock_raw = get_option('csock-raw')
usock_tcp = get_option('usock-tcp')
usock_udp = get_option('usock-udp')
vsock_tcp = get_option('vsock-tcp')
vsock_udp = get_option('vsock-udp')

inc = include_directories('include')

pargs += ['-D_FILE_OFFSET_BITS=@0@'.format(file_offset_bits)]

subdir('include')
subdir('src')

libudo = library(
  project_name, fs,
  version: '.'.join(so_version),
  include_directories: inc,
  dependencies: libudo_deps,
  c_args: pargs,
  install: true
)

libudo_a = static_library(
  project_name, fs,
  include_directories: inc,
  dependencies: libudo_deps,
  c_args: pargs,
  install: true
)

pkg = import('pkgconfig')

pkglibs = [libudo]
pkgsubdirs = []
pkg.generate(libraries: pkglibs,
             subdirs : pkgsubdirs,
             version : so_version,
             name : project_name,
             filebase : 'udo',
             description : 'A common utility library for C programs/libraries.')

if get_option('tests')
  subdir('tests')
endif

if get_option('docs')
  docs_dir = src_root_dir + '/docs'
  docs_build_dir = build_root_dir + '/docs'
  sphinx = find_program('sphinx-build-5', 'sphinx-build', required: true)

  if sphinx.found()
    subdir('docs')
    custom_target('docs',
                  command: [sphinx, '-W', '-b', 'html', docs_dir, '@OUTPUT@'],
                  input: docs_src, output: 'docs', install: false, install_dir: '',
                  build_by_default: false, build_always_stale: true)
  endif
endif
