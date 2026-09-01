app_platform_conf = configuration_data()
app_platform_conf.set('APP_RAM_ORIGIN', tutorial_app_ram_origins[2])
app_platform_conf.set('APP_RAM_LENGTH',    '0x10000')
app_platform_conf.set('UART_BASE', app_uart_base)
app_platform_conf.set('APP_LAYOUT_PREFIX', 'APP3')
app_platform_conf.set('VIRTIO_BASE', app_virtio_base)

configure_file(
  input: '../_mesonlib/tutorial_platform/app_layout.h.in',
  output: 'app3_layout.h',
  configuration: app_platform_conf,
)

app3_layout_inc = include_directories('.')

tutorial_app_name = 'app3'
tutorial_app_index = 3
tutorial_app_ram_length = '0x10000'
tutorial_app_sources = files(
  'head.S',
  'main.c',
)

subdir('_mesonlib/tutorial_app')

app3_elf = tutorial_app_elf
