interferer2_elf = executable(
	'interferer2.elf',
	sources: files(
		'head.S',
		'main.c',
		'uart.c'
	),
	c_args: [
		'-specs=picolibc.specs'
	],
	link_args: [
		'-nostartfiles',
		'-specs=picolibc.specs',
		'-lgcc',
		'-T', meson.current_source_dir() / 'cheshire.ld',
	],
	dependencies: [
		libs3k_dep,
	],
)
