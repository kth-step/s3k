subdir('platform')

app1_elf = executable(
	'app1.elf',
	sources: files(
		'head.S',
		'main.c',
		'init2.c'
	) + app1_platform_uart,
	c_args: [
		'-specs=picolibc.specs',
		'-DUART_REG_STRIDE=' + get_option('uart_stride'),
	],
	link_args: [
		'-nostartfiles',
		'-specs=picolibc.specs',
		'-T', app1_platform_ld,
	],
	dependencies: [
		libs3k_dep,
	],
)
