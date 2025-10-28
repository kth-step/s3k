# Platform-specific configuration for QEMU RISC-V Virt

if get_option('platform') == 'qemu_virt'
	platform_opts = {
	    'npmp': '8',
	    'nmemcaps': '2',
	    'nharts': '1',
	    'rtchz': '100000',
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
else 
	error('Unknown platform: ' + get_option('platform'))
endif
