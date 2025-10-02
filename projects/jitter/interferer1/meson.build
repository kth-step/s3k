interferer1_elf = executable(
	'interferer1.elf',
	sources: files(
		'head.S',
		'main.c',
	),
	c_args: [
		'-ffreestanding',
	],
	link_args: [
		'-nostartfiles',
		'-nostdlib',
		'-ffreestanding',
		'-lgcc',
		'-T', meson.current_source_dir() / 'cheshire.ld',
	],
	dependencies: [
		libs3k_dep,
	],
)
