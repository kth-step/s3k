app_platform_conf = configuration_data()
app_platform_conf.set('APP_RAM_ORIGIN', tutorial_app_ram_origin)
app_platform_conf.set('APP_RAM_LENGTH', tutorial_app_ram_length)
app_platform_conf.set('UART_BASE', app_uart_base)
app_platform_conf.set('VIRTIO_BASE', app_virtio_base)

configure_file(
  input: app_platform_ld_in,
  output: tutorial_app_name + '.ld',
  configuration: app_platform_conf,
)

tutorial_app_ld = meson.current_build_dir() / (tutorial_app_name + '.ld')
if is_variable('tutorial_app_include_dirs')
  tutorial_app_include_directories = tutorial_app_include_dirs
else
  tutorial_app_include_directories = []
endif

tutorial_app_elf = executable(
  tutorial_app_name + '.elf',
  sources: tutorial_app_sources + app_platform_uart,
  include_directories: tutorial_app_include_directories,
  c_args: tutorial_app_c_args,
  link_args: [
    '-nostartfiles',
    '-specs=picolibc.specs',
    '-T', tutorial_app_ld,
  ],
  dependencies: [
    libs3k_dep,
  ],
)
