project(
    's3k',
    'c',
    version: '25.10',
    meson_version: '>= 1.1.0',
    default_options: [
        'warning_level=2',      # -Wall -Wextra
        'werror=true',          # -Werror
        'c_std=gnu11',          # Use GNU11 standard for C.
        'b_ndebug=if-release',  # Disable assertions in release builds.
    ],
)


subdir('kern')
subdir('lib')

elf = executable(
    's3k.elf',
    sources: kern_sources,
    include_directories: kern_incdir,
    c_args: kern_c_args,
    link_args: kern_link_args,
    build_by_default: true,
)

lib_dep = declare_dependency(include_directories: lib_includes)
