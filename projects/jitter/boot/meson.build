boot_elf = executable(
	'boot.elf',
	sources: files(
		'head.S',
		'main.c',
		'uart.c',
		'proc_init.c',
	),
	c_args: [
		'-specs=picolibc.specs',
	] + boot_config,
	link_args: [
		'-nostartfiles',
		'-specs=picolibc.specs',
		'-T', meson.current_source_dir() / 'cheshire.ld',
	],
	dependencies: [
		libs3k_dep,
	],
)
