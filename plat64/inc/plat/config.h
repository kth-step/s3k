#pragma once

#if defined(PLATFORM_VIRT)
#include "plat/virt.h"
#elif defined(PLATFORM_HIFIVE_UNLEASHED)
#include "plat/hifive_unleashed.h"
#else
#error "Unsupported platform or platform not found"
#endif
