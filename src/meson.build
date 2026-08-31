unity_lock_deps = [
  dep_gtk,
  dep_adw,
  dep_gio,
  dep_glib,
  dep_act,
  dep_astalauth,
  dep_gnomedt,
  dep_pango,
  dep_pangocairo,
  dep_layershell,
]

unity_lock_sources = files(
  'main.c',
  'unity-lock.c',
)

subdir('auth')
subdir('components')

unity_lock_resources = gnome.compile_resources(
  'unity-lock-resources',
  'unity-lock.gresource.xml',
  c_name: 'unity_lock',
  source_dir: ['.', meson.project_source_root() / 'data'],
)

unity_lock_link_args = [
  '-Wl,--no-as-needed',
  '-lgtk4-layer-shell',
  '-Wl,--as-needed',
]

executable(
  'unity-lock',
  unity_lock_sources,
  unity_lock_resources,
  dependencies:        unity_lock_deps,
  include_directories: include_directories('.', 'auth', 'components'),
  link_args:           unity_lock_link_args,
  install:             true,
)
