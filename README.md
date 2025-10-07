# S3K

S3K is a capability-based partitining kernel designed to allow privilieged monitor processes to dynamically create and destroy partitions.

## Repository Organization

- `cross/` : Files related to cross compilation.
- `include/` : Kernel header files.
- `platform/` : Platform specific kernel configuration files.
- `src/` : Kernel source files.
- `Doxygen` : Doxygen configuration file.
- `LICENSE` : License file.
- `meson.build` : Meson build configuration.
- `README.md` : This README file.
- `meson.options` : Kernel configuration file.

## Compilation instructions

Requirements: Docker (rootless).

```bash
./docker.sh
meson setup builddir --cross-file=cross/rv64imac.toml
ninja -C builddir
```
