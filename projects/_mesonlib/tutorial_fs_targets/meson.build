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
