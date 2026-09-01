project('tutorial.04.yield', 'c',
	version: '0.1',
	meson_version: '>=1.1.0',
	default_options: [
		'buildtype=debugoptimized',
		'c_std=gnu11',
	]
)

subdir('_mesonlib/tutorial_project')

subdir('app2')
subdir('app1')

tutorial_app_elves = [app1_elf, app2_elf]
tutorial_qemu_loader_cpus = [0]

subdir('_mesonlib/tutorial_targets')
