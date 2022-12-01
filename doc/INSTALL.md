# S3K - INSTALL

Prerequisites:
- python3 with pyyaml
- riscv-gnu-toolchain : compiled with unknown-elf or similar (i.e., not linux)
    + Download from SiFive, 
    + Install with your package manager, or
    + Install from source https://github.com/riscv-collab/riscv-gnu-toolchain
        + Suggested config: `./configure --prefix=/opt/riscv --enable-multilib`

The following commands will compile the kernel, `s3k.elf`, with a dummy payload:
```bash
git clone git@github.com:kth-step/separation-kernel.git
cd separation-kernel
make
```

Custom payload, kernel config and platform:
+ Copy `config.h` and configure.
+ Configure the variables in your config file. 
+ Copy `bsp/virt/*.h` and configure it for your platform.
+ `make CONFIG=path/to/my/config.h PLATFORM=path/to/my/platform`

Check https://github.com/kth-step/s3k-poc (WIP) for sample application.
