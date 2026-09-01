project('tutorial.10.fs', 'c',
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

tutorial_qemu_options = ['-drive', 'file=fs.img,if=none,format=raw,id=x0', '-device', 'virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0']

subdir('_mesonlib/tutorial_targets')

run_target(
  'disk-image',
  command: [
    'sh',
    '-c',
    'qemu-img create fs.img 10M && mformat -i fs.img :: && echo "hello" > tmp.txt && mcopy -i fs.img tmp.txt ::/test.txt && rm tmp.txt',
  ],
)

run_target(
  'disk-read',
  command: [
    'sh',
    '-c',
    'mdir -i fs.img && mcopy -i fs.img ::/NEWFILE.TXT extracted.txt && cat < extracted.txt && rm extracted.txt',
  ],
)
