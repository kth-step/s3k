project('tutorial.11.network', 'c',
	version: '0.1',
	meson_version: '>=1.1.0',
	default_options: [
		'buildtype=debugoptimized',
		'c_std=gnu11',
	]
)

subdir('_mesonlib/tutorial_project')
subdir('app1')

tutorial_app_elves = [app1_elf]
tutorial_qemu_loader_cpus = [0]

tutorial_qemu_options = ['-global', 'virtio-mmio.force-legacy=false', '-netdev', 'user,id=n1,hostfwd=tcp::5555-:23', '-device', 'virtio-net-device,bus=virtio-mmio-bus.0,netdev=n1']

subdir('_mesonlib/tutorial_targets')
