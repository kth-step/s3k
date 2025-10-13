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
)

kern_incdir = include_directories('include')

kern_c_args = [
    '-ffreestanding',   # Required for bare-metal environments.
]

kern_link_args = [
    '-nostdlib',        # Do not use standard startup or library files.
    '-lgcc',            # Link against GCC's runtime library.
    '-Wl,--gc-sections',  # Remove unused sections.
    '-Wl,--no-warn-rwx-segments',  # Suppress RWX segment warnings.
]
