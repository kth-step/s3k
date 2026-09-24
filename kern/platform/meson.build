# Platform-specific configuration for QEMU RISC-V Virt

platform_c_opt = '-O2'

if get_option('platform') == 'qemu_virt'
	platform_opts = {
	    'npmp': '8',
	    'nmemcaps': '2',
	    'nharts': '1',
	    'rtchz': '10000000',
	}
	platform_ld = meson.current_source_dir() / 'qemu_virt.ld'
	platform_sources = files('qemu_virt.c')
elif get_option('platform') == 'qemu_virt4'
	platform_opts = {
	    'npmp': '8',
	    'nmemcaps': '2',
	    'nharts': '2',
	    'rtchz': '10000000',
	}
	platform_ld = meson.current_source_dir() / 'qemu_virt.ld'
	platform_sources = files('qemu_virt.c')
elif get_option('platform') == 'cheshire'
	platform_opts = {
	    'npmp': '8',
	    'nmemcaps': '3',
	    'nharts': '1',
	    'rtchz': '1000000',
	}
	platform_ld = meson.current_source_dir() / 'cheshire.ld'
	platform_sources = files('cheshire.c')
elif get_option('platform') == 'cheshire2'
	platform_opts = {
	    'npmp': '8',
	    'nmemcaps': '4',
	    'nharts': '2',
	    'rtchz': '1000000',
	}
	platform_ld = meson.current_source_dir() / 'cheshire.ld'
	platform_sources = files('cheshire.c')
elif get_option('platform') == 'ch32v307'
	# CH32V307-EVT-R1 (QingKe V4F, RV32IMACF). 16 PMP slots per QingKe V4
	# processor manual v1.5. rtchz: the STK-backed RTC reads the core
	# system timer at HCLK/8 = 18 MHz in the 144 MHz PLL regime
	# (see platform/ch32v307.c).
	platform_opts = {
	    'npmp': '16',
	    'nmemcaps': '2',
	    'nharts': '1',
	    'rtchz': '18000000',
	}
	platform_ld = meson.current_source_dir() / 'ch32v307.ld'
	platform_sources = files('ch32v307.c', 'usbfs_cdc.c')
	# Size-constrained SRAM target: squeeze the kernel image.
	platform_c_opt = '-Os'
else
	error('Unknown platform: ' + get_option('platform'))
endif

# Platform configuration arguments  
c_platform_args = [
    platform_c_opt,
    '-D_MAX_PMP_SLOT=' + platform_opts['npmp'],
    '-D_NUM_MEMORY_CAPS=' + platform_opts['nmemcaps'],
    '-D_NUM_HARTS=' + platform_opts['nharts'],
    '-D_RTC_HZ=' + platform_opts['rtchz'],
]

if get_option('platform') == 'ch32v307'
    c_platform_args += ['-DPLATFORM_NO_SCOUNTEREN']
    # QingKe V4F routes SysTick through the PFIC: mip.MTIP never sets and
    # the COUNTIF flag cannot be cleared, so preemption is purely
    # poll-driven (see preempt.h).
    c_platform_args += ['-DPLATFORM_PREEMPT_STK']
endif

link_platform_args = [
    '-T' + platform_ld
]
