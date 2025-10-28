
if (get_option('platform') == 'qemu_virt')
  app2_platform_uart = files('ns16550a.c')
  app2_platform_ld = meson.current_source_dir() / 'qemu_virt.ld'
elif (get_option('platform') == 'cheshire') or (get_option('platform') == 'cheshire2')
  app2_platform_uart = files('ti16750.c')
  app2_platform_ld = meson.current_source_dir() / 'cheshire.ld'
else
  error('Unknown platform: ' + get_option('platform'))
endif
