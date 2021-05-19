project('args', 'c',
  version : '1.1.1',
  license : 'MIT',
  default_options : [
    'warning_level=3',
    'optimization=3',
    'strip=true',
    'c_std=c99',
  ]
)

c_warnings = [
  # warns
  '-Wall',
  '-Wextra',
  '-Werror',
  '-Wfatal-errors',
  '-Wpedantic',
  '-Wshadow',
  '-Wdouble-promotion',
  '-Wformat=2',
  '-Wformat-overflow=2',
  '-Wformat-truncation=1',
  '-Wundef',
  '-Wmissing-include-dirs',
  '-Wstrict-aliasing=2',
  '-Walloca',
  '-Wduplicated-branches',
  '-Wduplicated-cond',
  '-Wwrite-strings',
  '-Wdate-time',
  '-Wsizeof-pointer-memaccess',
  '-Wlogical-op',
  '-Wpacked',
  '-Wredundant-decls',
  '-Wnested-externs',
  '-Winline',
]

# These arguments are only used to build the shared library
# not the executables that use the library.
lib_args = [
  '-DBUILDING_ARGS',
]

shlib = shared_library(
  'args', 'args.c',
  install : true,
  c_args : [lib_args, c_warnings],
  gnu_symbol_visibility : 'hidden',
)

example_basic_exe = executable(
  'args_test', 'examples/basic.c',
  link_with : shlib
)

test('example', example_basic_exe)

# Make this library usable as a Meson subproject.
test_dep = declare_dependency(
  include_directories: include_directories('.'),
  link_with : shlib
)

# Make this library usable from the system's
# package manager.
install_headers('args.h', subdir : 'args')

pkg_mod = import('pkgconfig')
pkg_mod.generate(
  name : 'args',
  filebase : 'args',
  description : 'Simple args parsing',
  subdirs : 'args',
  libraries : shlib,
  version : '1.1.1',
)
