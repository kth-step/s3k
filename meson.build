project('s3k', 'c',
  version : '1.1',
  meson_version : '>=1.1',
  default_options : [
    'warning_level=2', 
    'c_std=gnu11', 
    'optimization=s']
)

subdir('src')
incdir = include_directories('include')

subdir('platform' / get_option('platform'))

c_args = [
  '-ffreestanding',
  '-DNPROC=' + get_option('nproc').to_string(),
  '-DNHART=' + get_option('nhart').to_string(),
  '-DNSLOT=' + get_option('nslot').to_string(),
  '-DNTICK=' + get_option('ntick').to_string(),
  '-DNPMP=' + get_option('npmp').to_string(),
  '-DNCHAN=' + get_option('nchan').to_string(),
  '-DNCAP=' + get_option('ncap').to_string(),
]

link_args = [
  '-ffreestanding',
  '-nostdlib',
  '-lgcc',
  '-T', linker_script
]

elf = executable('s3k.elf', 
  sources : sources,
  include_directories : incdir,
  c_args : c_args,
  link_args : link_args,
)
