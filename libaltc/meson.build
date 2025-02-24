project('libaltc', 'c', version : '0.1')

subdir('src')
incdirs = include_directories('include')

altc_lib = static_library(
  'altc',
  sources,
  include_directories : incdirs)

altc_dep = declare_dependency(
  link_with : altc_lib,
  include_directories : incdirs)
