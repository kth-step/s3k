spy_elf = executable(
	'spy.elf',
	sources: files(
		'head.S',
		'main.c',
		'uart.c',
	),
	c_args: [
		'-specs=picolibc.specs',
	],
	link_args: [
		'-nostartfiles',
		'-specs=picolibc.specs',
		'-T', meson.current_source_dir() / measurer_ld,
	],
	dependencies: [
		libs3k_dep,
	],
)
