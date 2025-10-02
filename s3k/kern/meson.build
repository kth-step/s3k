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
    '-ffreestanding',  # Required for bare-metal environments.
    '-flto',  # Enable link-time optimization.
    '-fno-stack-protector',  # Disable stack protection for kernel code.
    '-Wall',  # Enable all common warnings.
    '-Wextra',  # Enable extra warnings.
    '-Werror',  # Treat warnings as errors.
    '-fcall-saved-t4',
    '-fcall-saved-t5',
    '-fcall-saved-t6',
]

kern_link_args = [
    '-ffreestanding',  # Required for bare-metal environments.
    '-flto',  # Enable link-time optimization.
    '-nostartfiles',  # Do not use standard startup files.
    '-mrelax',  # Relax the architecture constraints.
    '-lgcc',  # Link against GCC's runtime library.
    '-Wl,--gc-sections',  # Remove unused sections.
    '-Wl,--no-warn-rwx-segments',  # Suppress RWX segment warnings.
    '-fcall-saved-t4',
    '-fcall-saved-t5',
    '-fcall-saved-t6',
]
