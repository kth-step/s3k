project('tutorial.05.timing', 'c',
	version: '0.1',
	meson_version: '>=1.1.0',
	default_options: [
		'buildtype=debugoptimized',
		'c_std=gnu11',
	]
)

s3k = subproject('s3k')
libs3k_dep = s3k.get_variable('lib_dep')
s3k_elf = s3k.get_variable('elf')

subdir('subprojects/s3k/projects/_mesonlib/tutorial_options')

if (get_option('platform') == 'qemu_virt') or (get_option('platform') == 'qemu_virt4')
  app_platform_uart = files('../_mesonlib/tutorial_platform/ns16550a.c')
  app_platform_ld_in = files('../_mesonlib/tutorial_platform/qemu_virt.ld.in')
  app_uart_base = '0x10000000'
  app_virtio_base = '0x10001000'
elif (get_option('platform') == 'cheshire') or (get_option('platform') == 'cheshire2')
  app_platform_uart = files('../_mesonlib/tutorial_platform/ti16750.c')
  app_platform_ld_in = files('../_mesonlib/tutorial_platform/cheshire.ld.in')
  app_uart_base = '0x03002000'
else
  error('Unknown platform: ' + get_option('platform'))
endif

subdir('app2')
subdir('app1')

tutorial_app_elves = [app1_elf, app2_elf]
tutorial_qemu_loader_cpus = [0, 1]

subdir('subprojects/s3k/projects/_mesonlib/tutorial_targets')
