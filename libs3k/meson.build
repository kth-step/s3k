project('libs3k', 'c', version : '0.1')

subdir('src')
incdirs = include_directories('include')

s3k_lib = static_library(
  's3k',
  sources,
  include_directories : incdirs)

s3k_dep = declare_dependency(
  link_with : altc_lib,
  include_directories : incdirs)
