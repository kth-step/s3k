# Platform-specific configuration for QEMU RISC-V Virt

if get_option('platform') == 'qemu_virt'
	kernel_platform_opts = {
	    'npmp': '8',
	    'nmemcaps': '2',
	    'nharts': '1',
	}
	kernel_platform_ld = meson.current_source_dir() / 'qemu_virt.ld'
	kernel_platform_sources = files('qemu_virt.c')
elif get_option('platform') == 'cheshire'
	kernel_platform_opts = {
	    'npmp': '8',
	    'nmemcaps': '3',
	    'nharts': '1',
	}
	kernel_platform_ld = meson.current_source_dir() / 'cheshire.ld'
	kernel_platform_sources = files('cheshire.c')
elif get_option('platform') == 'cheshire2'
	kernel_platform_opts = {
	    'npmp': '8',
	    'nmemcaps': '4',
	    'nharts': '2',
	}
	kernel_platform_ld = meson.current_source_dir() / 'cheshire.ld'
	kernel_platform_sources = files('cheshire.c')
else 
	error('Unknown platform: ' + get_option('platform'))
endif
