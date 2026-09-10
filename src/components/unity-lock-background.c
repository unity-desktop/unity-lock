/* unity-lock-background.c
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "unity-lock-background.h"

GdkPaintable *
unity_lock_background_get (void)
{
  static GdkTexture *texture;
  static gsize loaded = 0;

  if (g_once_init_enter (&loaded))
    {
      g_autofree gchar *path = g_build_filename (g_get_user_cache_dir (),
                                                 "unity-shell",
                                                 "background.png", NULL);
      g_autoptr (GError) error = NULL;

      texture = gdk_texture_new_from_filename (path, &error);

      if (texture == NULL)
        g_debug ("no published wallpaper at %s: %s", path, error->message);

      g_once_init_leave (&loaded, 1);
    }

  return texture != NULL ? GDK_PAINTABLE (texture) : NULL;
}
