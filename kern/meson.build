# Include platform-specific configuration
subdir('platform')

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
)

incdir = include_directories('include')

# System configuration arguments
c_args = [
    '-ffreestanding',
    '-D_MAX_PID=' + get_option('nproc').to_string(),
    '-D_MAX_TIME_SLOT=' + get_option('ntimeslot').to_string(),
    '-D_MAX_MEMORY_FUEL=' + get_option('nmemoryfuel').to_string(),
    '-D_MAX_TIME_FUEL=' + get_option('ntimefuel').to_string(),
    '-D_MAX_MONITOR_FUEL=' + get_option('nmonitorfuel').to_string(),
    '-D_MAX_IPC_FUEL=' + get_option('nipcfuel').to_string(),
    '-D_CSPAD=' + get_option('cspad').to_string(),
    '-D_TIME_SLOT_US=' + get_option('timeslotus').to_string(),
]

link_args = [
    '-nostdlib',        # Do not use standard startup or library files.
    '-lgcc',            # Link against GCC's runtime library.
    '-Wl,--gc-sections',  # Remove unused sections.
    '-Wl,--no-warn-rwx-segments',  # Suppress RWX segment warnings.
]

elf = executable(
    's3k.elf',
    sources: sources + platform_sources,
    include_directories: incdir,
    c_args: c_args + c_platform_args,
    link_args: link_args + link_platform_args,
    build_by_default: true,
)
