#pragma once

#if defined(PLATFORM_qemu_virt)
#include "plat/qemu_virt.h"
#elif defined(PLATFORM_sifive_unleashed)
#include "plat/sifive_unleashed.h"
#else
#error "Unsupported platform or platform not found"
#endif
