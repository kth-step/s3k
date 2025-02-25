project('hello_world', 'c', 
  version : '0.1',
  default_options : [
    'warning_level=2', 
    'c_std=gnu11', 
    'optimization=s'])

kernel = subproject('kernel')
app0 = subproject('app0')

hexes = [
  kernel.get_variable('hex'),
  app0.get_variable('hex'),
  ]

hex = custom_target(
  'hello.hex',
  input : hexes,
  output : 'hello.hex',
  command : [
    find_program('srec_cat'), 
    '-O', '@OUTPUT@', '-Intel',
    '@INPUT0@', '-Intel',
    '@INPUT1@', '-Intel',
  ],
  build_by_default : true
)
