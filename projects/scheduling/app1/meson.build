
if get_option('rand')
	app1_options = ['-DRAND']
else
	app1_options = []
endif

app1_elf = executable(
	'app1.elf',
	sources: files(
		'head.S',
		'main.c',
		'uart.c',
	),
	c_args: [
		'-specs=picolibc.specs', 
	] + app1_options,
	link_args: [
		'-nostartfiles',
		'-specs=picolibc.specs',
		'-fwhole-program',
		'-T', meson.current_source_dir() / 'cheshire.ld',
	],
	dependencies: [libs3k_dep],
)
