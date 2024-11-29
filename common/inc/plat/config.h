#pragma once

#if defined(PLATFORM_qemu_virt) || defined(PLATFORM_qemu_virt_norvc)
#include "plat/qemu_virt.h"
#elif defined(PLATFORM_qemu_virt4)
#include "plat/qemu_virt4.h"
#elif defined(PLATFORM_sifive_unleashed)
#include "plat/sifive_unleashed.h"
#elif defined(PLATFORM_sifive_unleashed4)
#include "plat/sifive_unleashed4.h"
#else
#error "Unsupported platform or platform not found"
#endif

#if S3K_HART_CNT > 1
#define SMP
#endif
