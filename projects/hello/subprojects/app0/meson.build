project('app0', 'c',
  version : '0.1',
  default_options : [
    'warning_level=2', 
    'c_std=gnu11', 
    'optimization=s']
)

altc = subproject('libaltc')
altc_dep = altc.get_variable('altc_dep')

subdir('src')
incdir = include_directories('include')

subdir('platform' / get_option('platform'))

cross_c_args = get_option('c_args')
c_args = [
  '-DNPROC=' + get_option('nproc').to_string(),
  '-DNHART=' + get_option('nhart').to_string(),
  '-DNSLOT=' + get_option('nslot').to_string(),
  '-DNTICK=' + get_option('ntick').to_string(),
  '-DNPMP=' + get_option('npmp').to_string(),
  '-DNCHAN=' + get_option('nchan').to_string(),
  '-DNCAP=' + get_option('ncap').to_string(),
]

cross_c_link_args = get_option('c_link_args')
c_link_args = [
  '-nostdlib',
  '-nostartfiles',
  '-T', linker_script
]

elf = executable('app0.elf', 
  sources : sources,
  include_directories : incdir,
  c_args : cross_c_args + c_args,
  link_args : cross_c_link_args + c_link_args,
  dependencies : [altc_dep]
)

hex = custom_target(
  'app0.hex',
  input : elf,
  output : 'app0.hex',
  command : [
    find_program('riscv64-unknown-elf-objcopy'), 
    '--output-target=ihex',
    '@INPUT@', 
    '@OUTPUT@'
  ],
  build_by_default : true
)

