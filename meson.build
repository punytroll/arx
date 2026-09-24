project(
  'arx',
  'cpp',
  version: '1.0.0',
  license: 'MIT',
  license_files: ['LICENSE'],
  meson_version: '>=1.11.0',
  default_options: {
    'cpp_std': 'c++23',
    'werror': true,
    'warning_level': '3'
  }
)

arx_library_cpp_args = []
arx_library_dependencies = []

zlib_dependency = dependency('zlib', required: get_option('zlib'))
if zlib_dependency.found() == true
  arx_library_cpp_args += '-DHAVE_ZLIB_H'
  arx_library_dependencies += zlib_dependency
endif
summary('zlib support', zlib_dependency.found(), bool_yn: true)

bzip2_dependency = dependency('bzip2', required: get_option('bzip2'))
if bzip2_dependency.found() == true
  arx_library_cpp_args += '-DHAVE_BZLIB_H'
  arx_library_dependencies += bzip2_dependency
endif
summary('BZip2 support', bzip2_dependency.found(), bool_yn: true)

arx_library_sources = files(
  'source/archive.cpp',
  'source/buffer.cpp',
  'source/buffer_reader.cpp',
  'source/buffer_writer.cpp',
  'source/common.cpp',
  'source/data.cpp',
  'source/item.cpp',
  'source/reference.cpp',
  'source/reference_core.cpp',
  'source/relation.cpp',
  'source/structure.cpp'
)
arx_library_include_directories = include_directories('include')

arx_library = library(
  'arx',
  cpp_args: arx_library_cpp_args,
  sources: arx_library_sources,
  dependencies: arx_library_dependencies,
  include_directories: arx_library_include_directories,
  version: meson.project_version(),
  soversion: '1',
  install: true
)

arx_library_dependency = declare_dependency(
  include_directories: arx_library_include_directories,
  link_with: arx_library,
  dependencies: arx_library_dependencies
)

meson.override_dependency(
  'arx',
  arx_library_dependency
)
