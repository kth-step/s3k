lib_sources = files('src/syscall.c')

lib_includes = include_directories('include')

lib_c_args = [
    '-fno-stack-protector',
    '-fno-omit-frame-pointer',
    '-ffreestanding',
    '-fno-builtin',
    '-fno-strict-aliasing',
    '-fno-common',
    '-ffat-lto-objects',
    '-flto',
]
