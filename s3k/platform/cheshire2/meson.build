kernel_opts = {
    'nproc': get_option('nproc').to_string(),
    'ntimeslot': get_option('ntimeslot').to_string(),
    'nmemoryfuel': get_option('nmemoryfuel').to_string(),
    'ntimefuel': get_option('ntimefuel').to_string(),
    'nmonitorfuel': get_option('nmonitorfuel').to_string(),
    'nipcfuel': get_option('nipcfuel').to_string(),
    'cspad': get_option('cspad').to_string(),
}

kern_dep = declare_dependency(
	sources: files('config.c'),
	compile_args: [
		'-D_MAX_PMP_SLOT=8',
		'-D_NUM_MEMORY_CAPS=3',
		'-D_NUM_HARTS=2',
		'-DSMP',
    		'-D_MAX_PID=' + kernel_opts['nproc'],
    		'-D_MAX_TIME_SLOT=' + kernel_opts['ntimeslot'],
    		'-D_MAX_MEMORY_FUEL=' + kernel_opts['nmemoryfuel'],
    		'-D_MAX_TIME_FUEL=' + kernel_opts['ntimefuel'],
    		'-D_MAX_MONITOR_FUEL=' + kernel_opts['nmonitorfuel'],
    		'-D_MAX_IPC_FUEL=' + kernel_opts['nipcfuel'],
		'-DCSPAD=' + kernel_opts['cspad'],
	],
	link_args: ['-T', meson.current_source_dir() / 'kernel.ld']
)
