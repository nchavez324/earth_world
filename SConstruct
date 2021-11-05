AddOption(
  '--dbg',
  dest='debug',
  action='store_true',
  help='debug build',
  default=False)

if GetOption('debug'):
  variant_dir = 'build/debug'
  debug = True
else:
  variant_dir = 'build/release'
  debug = False

SConscript(
  'main.scons',
  variant_dir=variant_dir,
  exports=['debug'])