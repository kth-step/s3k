app2_elf = executable(
	'app2.elf',
	sources: files(
		'head.S',
		'main.c',
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
