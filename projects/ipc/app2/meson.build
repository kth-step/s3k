subdir('platform')
app2_elf = executable(
	'app2.elf',
	sources: files(
		'head.S',
		'main.c',
	) + app2_platform_uart,
	c_args: [
		'-specs=picolibc.specs',
		'-DUART_REG_STRIDE=' + get_option('uart_stride'),
	],
	link_args: [
		'-nostartfiles',
		'-specs=picolibc.specs',
		'-T', app2_platform_ld,
	],
	dependencies: [
		libs3k_dep,
	],
)
