
# Include platform-specific configuration
subdir('platform')

kernel_opts = {
    'nproc': get_option('nproc').to_string(),
    'ntimeslot': get_option('ntimeslot').to_string(),
    'nmemoryfuel': get_option('nmemoryfuel').to_string(),
    'ntimefuel': get_option('ntimefuel').to_string(),
    'nmonitorfuel': get_option('nmonitorfuel').to_string(),
    'nipcfuel': get_option('nipcfuel').to_string(),
    'cspad': get_option('cspad').to_string(),
}

kern_sources = files(
    'src/head.S',
    'src/trap.S',
    'src/exception.c',
    'src/interrupt.c',
    'src/ipc.c',
    'src/lock.c',
    'src/mem.c',
    'src/mon.c',
    'src/proc.c',
    'src/rtc.c',
    'src/sched.c',
    'src/syscall.c',
    'src/tsl.c',
    'src/ttas.c',
) + kernel_platform_sources

kern_incdir = include_directories('include')

# System configuration arguments
kern_system_args = [
    '-D_MAX_PID=' + kernel_opts['nproc'],
    '-D_MAX_TIME_SLOT=' + kernel_opts['ntimeslot'],
    '-D_MAX_MEMORY_FUEL=' + kernel_opts['nmemoryfuel'],
    '-D_MAX_TIME_FUEL=' + kernel_opts['ntimefuel'],
    '-D_MAX_MONITOR_FUEL=' + kernel_opts['nmonitorfuel'],
    '-D_MAX_IPC_FUEL=' + kernel_opts['nipcfuel'],
    '-D_CSPAD=' + kernel_opts['cspad'],
]

# Platform configuration arguments  
kern_platform_args = [
    '-D_MAX_PMP_SLOT=' + kernel_platform_opts['npmp'],
    '-D_NUM_MEMORY_CAPS=' + kernel_platform_opts['nmemcaps'],
    '-D_NUM_HARTS=' + kernel_platform_opts['nharts'],
]

kern_c_args = ['-ffreestanding'] + kern_system_args + kern_platform_args

kern_link_args = [
    '-nostdlib',        # Do not use standard startup or library files.
    '-lgcc',            # Link against GCC's runtime library.
    '-Wl,--gc-sections',  # Remove unused sections.
    '-Wl,--no-warn-rwx-segments',  # Suppress RWX segment warnings.
    '-T' + kernel_platform_ld
]
