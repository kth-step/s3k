project('jitter', 'c', 
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

if get_option('spm')
	boot_config = ['-DSPM']
	measurer_ld = 'cheshire-spm.ld'
else
	boot_config = []
	measurer_ld = 'cheshire-dram.ld'
endif	

if get_option('dual')
	boot_config += ['-DDUAL']
endif

subdir('boot')
subdir('measurer')
subdir('dummy')
subdir('interferer1')
subdir('interferer2')

gdb_riscv64 = find_program('riscv64-unknown-elf-gdb', required: false)
run_target(
	'gdb-run',
	command: [
		gdb_riscv64,
		'-ex', 'set confirm off',
		'-ex', 'set pagination off',
		'-ex', 'target extended-remote 10.100.0.1:3333',
		'-ex', 'set *(int*)0x3001000 = 0x0f',
		'-ex', 'set *(int*)0x3001010 = 0x1',
		'-ex', 'load ' + boot_elf.full_path(),
		'-ex', 'load ' + measurer_elf.full_path(),
		'-ex', 'load ' + dummy_elf.full_path(),
		'-ex', 'load ' + interferer1_elf.full_path(),
		'-ex', 'load ' + interferer2_elf.full_path(),
		'-ex', 'load ' + s3k_elf.full_path(),
		'-ex', 'add-symbol-file ' + boot_elf.full_path(),
		'-ex', 'add-symbol-file ' + measurer_elf.full_path(),
		'-ex', 'add-symbol-file ' + dummy_elf.full_path(),
		'-ex', 'add-symbol-file ' + interferer1_elf.full_path(),
		'-ex', 'add-symbol-file ' + interferer2_elf.full_path(),
		'-ex', 'add-symbol-file ' + s3k_elf.full_path(),
		'-ex', 't 2',
		'-ex', 'set $mtvec=0x10000000',
		'-ex', 'set $pc=0x10000000',
		'-ex', 't 1',
		'-ex', 'set $mtvec=0x10000000',
		'-ex', 'set $pc=0x10000000',
		'-ex', 'continue',
	],
	depends : [s3k_elf, boot_elf, dummy_elf, measurer_elf, interferer1_elf, interferer2_elf],
)
