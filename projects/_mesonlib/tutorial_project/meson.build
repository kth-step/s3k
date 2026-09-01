s3k = subproject('s3k')
libs3k_dep = s3k.get_variable('lib_dep')
s3k_elf = s3k.get_variable('elf')

tutorial_option_macros = {
  'nproc': ['NPROC'],
  'ntimeslot': ['MAX_TIME_SLOT'],
  'nmemoryfuel': ['MAX_MEMORY_FUEL'],
  'ntimefuel': ['MAX_TIME_FUEL'],
  'nmonitorfuel': ['MAX_MONITOR_FUEL'],
  'nipcfuel': ['MAX_IPC_FUEL'],
  'platform': ['PLATFORM'],
  'cspad': ['CSPAD'],
  'uart_stride': ['UART_REG_STRIDE'],
}

tutorial_app_c_args = ['-specs=picolibc.specs']
foreach option_name, macro_names : tutorial_option_macros
  option_value = '@0@'.format(get_option(option_name))
  foreach macro_name : macro_names
    tutorial_app_c_args += ['-D' + macro_name + '=' + option_value]
  endforeach
endforeach

if (get_option('platform') == 'qemu_virt') or (get_option('platform') == 'qemu_virt4')
  app_platform_uart = files('../tutorial_platform/ns16550a.c')
  app_platform_ld_in = files('../tutorial_platform/qemu_virt.ld.in')
  app_uart_base = '0x10000000'
  app_virtio_base = '0x10001000'
  tutorial_app_layouts = [
    {'origin': '0x80000000', 'length': '0x10000'},
    {'origin': '0x80020000', 'length': '0x10000'},
    {'origin': '0x80030000', 'length': '0x10000'},
  ]
elif (get_option('platform') == 'cheshire') or (get_option('platform') == 'cheshire2')
  app_platform_uart = files('../tutorial_platform/ti16750.c')
  app_platform_ld_in = files('../tutorial_platform/cheshire.ld.in')
  app_uart_base = '0x03002000'
  app_virtio_base = '0x0'
  tutorial_app_layouts = [
    {'origin': '0x10000000', 'length': '0x10000'},
    {'origin': '0x10020000', 'length': '0x10000'},
    {'origin': '0x10030000', 'length': '0x10000'},
  ]
else
  error('Unknown platform: ' + get_option('platform'))
endif

foreach app_slot : [1, 2, 3]
  tutorial_app_c_args += [
    '-DAPP@0@_RAM_ORIGIN=@1@'.format(app_slot, tutorial_app_layouts[app_slot - 1]['origin']),
    '-DAPP@0@_RAM_LENGTH=@1@'.format(app_slot, tutorial_app_layouts[app_slot - 1]['length']),
  ]
endforeach
