/* unity-lock-background.c
 *
 * Copyright 2026 Muqtadir
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
      g_autofree gchar *path = g_build_filename (UNITY_LOCK_MIRROR_ROOT,
                                                 g_get_user_name (),
                                                 "background.png", NULL);
      g_autoptr (GError) error = NULL;

      texture = gdk_texture_new_from_filename (path, &error);

      if (texture == NULL)
        g_debug ("no published wallpaper at %s: %s", path, error->message);

      g_once_init_leave (&loaded, 1);
    }

  return texture != NULL ? GDK_PAINTABLE (texture) : NULL;
}
