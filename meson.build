project(
    's3k',
    'c',
    version: '2.0',
    meson_version: '>= 1.1.0',
    default_options: [
        'warning_level=2',      # -Wall -Wextra
        'werror=true',          # -Werror
        'c_std=gnu11',          # Use GNU11 standard for C.
        'b_lto=true',           # Enable link-time optimization.
        'b_ndebug=if-release',  # Disable assertions in release builds.
    ],
)

subdir('kern')
subdir('lib')
