project('fcs', 'c', 
  version : '0.1',
  default_options : [
    'warning_level=2', 
    'c_std=gnu11', 
    'optimization=s'])

kernel = subproject('kernel')

hexes = [
  kernel.get_variable('hex')
]

hex = custom_target(
  'fcs.hex',
  input : hexes,
  output : 'fcs.hex',
  command : [
    find_program('srec_cat'), 
    '-O', '@OUTPUT@', '-Intel',
    '@INPUT0@', '-Intel',
  ],
  build_by_default : true
)

