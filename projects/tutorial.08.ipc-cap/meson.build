project('tutorial.08.ipc-cap', 'c', 
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
tutorial_gdb_commands = [
	'-ex', 'set confirm off',
	'-ex', 'set pagination off',
	'-ex', 'target extended-remote 10.100.0.1:3333',
	'-ex', 'set *(int*)0x3001000 = 0x0f',
	'-ex', 'set *(int*)0x3001010 = 0x1',
	'-ex', 'load ' + app1_elf.full_path(),
	'-ex', 'load ' + app2_elf.full_path(),
	'-ex', 'load ' + s3k_elf.full_path(),
	'-ex', 'add-symbol-file ' + app1_elf.full_path(),
	'-ex', 'add-symbol-file ' + app2_elf.full_path(),
	'-ex', 'add-symbol-file ' + s3k_elf.full_path(),
	'-ex', 't 2',
	'-ex', 'set $pc=0x10000000',
	'-ex', 't 1',
	'-ex', 'set $pc=0x10000000',
	'-ex', 'continue',
]
tutorial_qemu_loader_cpus = [0]

subdir('subprojects/s3k/projects/_mesonlib/tutorial_targets')
