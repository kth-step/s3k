app1_elf = executable(
	'app1.elf',
	sources: files(
		'head.S',
		'main.c',
		'init2.c',
		'uart.c',
	),
	c_args: [
		'-specs=picolibc.specs',
	] + config,
	link_args: [
		'-nostartfiles',
		'-specs=picolibc.specs',
		'-T', meson.current_source_dir() / 'cheshire.ld',
	],
	dependencies: [
		libs3k_dep,
	],
)
