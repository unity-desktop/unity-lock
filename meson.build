project(
  'unity-lock',
  'c',
  version: '26',
  license: 'GPL-3.0-or-later',
  meson_version: '>= 1.0',
  default_options: [
    'c_std=gnu11',
    'warning_level=2',
  ],
)

dep_gtk        = dependency('gtk4',            version: '>= 4.22')
dep_adw        = dependency('libadwaita-1',    version: '>= 1.9')
dep_gio        = dependency('gio-2.0',         version: '>= 2.80')
dep_glib       = dependency('glib-2.0',        version: '>= 2.80')
dep_act        = dependency('accountsservice', version: '>= 23.13.9')
dep_gnomedt    = dependency('gnome-desktop-4')
dep_pango      = dependency('pango',           version: '>= 1.56')
dep_pangocairo = dependency('pangocairo')
dep_astalauth  = dependency('astal-auth-0.1')
dep_layershell = dependency('gtk4-layer-shell-0', version: '>= 1.0')

dependency('gsettings-desktop-schemas')

gnome = import('gnome')
i18n  = import('i18n')

gettext_package = meson.project_name()

mirror_root = '/var/lib/unity-greeter'

add_project_arguments(
  '-DGNOME_DESKTOP_USE_UNSTABLE_API',
  '-DGETTEXT_PACKAGE="' + gettext_package + '"',
  '-DUNITY_LOCK_LOCALEDIR="' + get_option('prefix') / get_option('localedir') + '"',
  '-DUNITY_LOCK_VERSION="' + meson.project_version() + '"',
  '-DUNITY_LOCK_MIRROR_ROOT="' + mirror_root + '"',
  '-Wno-unused-parameter',
  language: 'c',
)

subdir('data')
subdir('po')
subdir('src')

gnome.post_install(glib_compile_schemas: true)
