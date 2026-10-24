# Platform-specific configuration for QEMU RISC-V Virt

if get_option('platform') == 'qemu_virt'
	platform_opts = {
	    'npmp': '8',
	    'nmemcaps': '2',
	    'nharts': '1',
	    'rtchz': '10000000',
	    'cspad': '0',
	}
	platform_ld = meson.current_source_dir() / 'qemu_virt.ld'
	platform_sources = files('qemu_virt.c')
elif get_option('platform') == 'qemu_virt4'
	platform_opts = {
	    'npmp': '8',
	    'nmemcaps': '2',
	    'nharts': '2',
	    'rtchz': '10000000',
	    'cspad': '0',
	}
	platform_ld = meson.current_source_dir() / 'qemu_virt.ld'
	platform_sources = files('qemu_virt.c')
elif get_option('platform') == 'cheshire'
	platform_opts = {
	    'npmp': '8',
	    'nmemcaps': '3',
	    'nharts': '1',
	    'rtchz': '1000000',
	    'cspad': get_option('cspad').to_string(),
	}
	platform_ld = meson.current_source_dir() / 'cheshire.ld'
	platform_sources = files('cheshire.c')
elif get_option('platform') == 'cheshire2'
	platform_opts = {
	    'npmp': '8',
	    'nmemcaps': '4',
	    'nharts': '2',
	    'rtchz': '1000000',
	    'cspad': get_option('cspad').to_string(),
	}
	platform_ld = meson.current_source_dir() / 'cheshire.ld'
	platform_sources = files('cheshire.c')
else 
	error('Unknown platform: ' + get_option('platform'))
endif

if get_option('cspad') != 0 and platform_opts['cspad'] == '0'
	warning('cspad=' + get_option('cspad').to_string() + ' is ignored on platform ' + get_option('platform'))
endif

# Platform configuration arguments  
c_platform_args = [
    '-D_MAX_PMP_SLOT=' + platform_opts['npmp'],
    '-D_NUM_MEMORY_CAPS=' + platform_opts['nmemcaps'],
    '-D_NUM_HARTS=' + platform_opts['nharts'],
    '-D_RTC_HZ=' + platform_opts['rtchz'],
    '-D_CSPAD=' + platform_opts['cspad'],
]

link_platform_args = [
    '-T' + platform_ld
]
