project('scheduling', 'c', 
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

subdir('app1')

gdb = find_program('riscv64-unknown-elf-gdb', required : true)

run_target(
	'gdb-run',
	command: [
		gdb,
		'-ex', 'set confirm off',
		'-ex', 'set pagination off',
		'-ex', 'target extended-remote 10.100.0.1:3333',
		'-ex', 'set *(int*)0x3001000 = 0x0f',
		'-ex', 'set *(int*)0x3001010 = 0x1',
		'-ex', 'load ' + app1_elf.full_path(),
		'-ex', 'load ' + s3k_elf.full_path(),
		'-ex', 'add-symbol-file ' + app1_elf.full_path(),
		'-ex', 'add-symbol-file ' + s3k_elf.full_path(),
		'-ex', 't 2',
		'-ex', 'set $pc=0x10000000',
		'-ex', 'set $mtvec=0x10000000',
		'-ex', 't 1',
		'-ex', 'set $pc=0x10000000',
		'-ex', 'set $mtvec=0x10000000',
		'-ex', 'continue',
	],
	depends : [app1_elf, s3k_elf],
)
