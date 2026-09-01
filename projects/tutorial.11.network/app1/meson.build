tutorial_app_name = 'app1'
tutorial_app_index = 1
tutorial_app_ram_length = '0x10000'
tutorial_app_sources = files(
  'head.S',
  'console.c',
  'virtio.c',
  'virtio_net.c',
  'net/net.c',
  'net/tcp.c',
  'net/tcp_app.c',
  'net/int_service.c',
  'net/telnet.c',
  'main.c',
)
tutorial_app_include_dirs = include_directories('.', 'net')

subdir('_mesonlib/tutorial_app')

app1_elf = tutorial_app_elf
