app_platform_conf = configuration_data()
app_platform_conf.set('APP_RAM_ORIGIN', tutorial_app_ram_origins[1])
app_platform_conf.set('APP_RAM_LENGTH', '0x10000')
app_platform_conf.set('UART_BASE', app_uart_base)
app_platform_conf.set('APP_LAYOUT_PREFIX', 'APP2')
app_platform_conf.set('VIRTIO_BASE', app_virtio_base)

configure_file(
  input: '../_mesonlib/tutorial_platform/app_layout.h.in',
  output: 'app2_layout.h',
  configuration: app_platform_conf,
)

app2_layout_inc = include_directories('.')

tutorial_app_name = 'app2'
tutorial_app_index = 2
tutorial_app_ram_length = '0x10000'
tutorial_app_sources = files(
  'head.S',
  'main.c',
)

subdir('_mesonlib/tutorial_app')

app2_elf = tutorial_app_elf
