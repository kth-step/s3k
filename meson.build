project(
    's3k',
    'c',
    version: '25.10',
    meson_version: '>= 1.1.0',
    default_options: [
        'warning_level=2',      # -Wall -Wextra
        'werror=true',          # -Werror
        'c_std=gnu11',          # Use GNU11 standard for C.
        'b_lto=true',           # Enable link-time optimization.
        'b_ndebug=if-release',  # Disable assertions in release builds.
    ],
)


# Include platform-specific configuration
subdir('platform')

opts = {
    'nproc': get_option('nproc').to_string(),
    'ntimeslot': get_option('ntimeslot').to_string(),
    'nmemoryfuel': get_option('nmemoryfuel').to_string(),
    'ntimefuel': get_option('ntimefuel').to_string(),
    'nmonitorfuel': get_option('nmonitorfuel').to_string(),
    'nipcfuel': get_option('nipcfuel').to_string(),
    'cspad': get_option('cspad').to_string(),
}

sources = files(
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
) + platform_sources

incdir = include_directories('include')

# System configuration arguments
system_args = [
    '-D_MAX_PID=' + opts['nproc'],
    '-D_MAX_TIME_SLOT=' + opts['ntimeslot'],
    '-D_MAX_MEMORY_FUEL=' + opts['nmemoryfuel'],
    '-D_MAX_TIME_FUEL=' + opts['ntimefuel'],
    '-D_MAX_MONITOR_FUEL=' + opts['nmonitorfuel'],
    '-D_MAX_IPC_FUEL=' + opts['nipcfuel'],
    '-D_CSPAD=' + opts['cspad'],
]

# Platform configuration arguments  
platform_args = [
    '-D_MAX_PMP_SLOT=' + platform_opts['npmp'],
    '-D_NUM_MEMORY_CAPS=' + platform_opts['nmemcaps'],
    '-D_NUM_HARTS=' + platform_opts['nharts'],
]

c_args = ['-ffreestanding'] + system_args + platform_args

link_args = [
    '-nostdlib',        # Do not use standard startup or library files.
    '-lgcc',            # Link against GCC's runtime library.
    '-Wl,--gc-sections',  # Remove unused sections.
    '-Wl,--no-warn-rwx-segments',  # Suppress RWX segment warnings.
    '-T' + platform_ld
]

elf = executable(
    's3k.elf',
    sources: sources,
    include_directories: incdir,
    c_args: c_args,
    link_args: link_args,
    build_by_default: true,
)

lib_dep = declare_dependency(
    include_directories: include_directories('lib')
)
