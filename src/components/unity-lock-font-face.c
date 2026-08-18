/* unity-lock-font-face.c
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

#include "unity-lock-font-face.h"

#include <glib/gstdio.h>
#include <pango/pangocairo.h>

#define FONT_RESOURCE_PREFIX "/org/unity/Lock/fonts/"
#define N_STYLES (UNITY_LOCK_FONT_FACE_STYLE_CLIMATE_CRISIS + 1)

G_DEFINE_ENUM_TYPE (
  UnityLockFontFaceStyle, unity_lock_font_face_style,
  G_DEFINE_ENUM_VALUE (UNITY_LOCK_FONT_FACE_STYLE_DEFAULT, "default"),
  G_DEFINE_ENUM_VALUE (UNITY_LOCK_FONT_FACE_STYLE_FRAUNCES, "fraunces"),
  G_DEFINE_ENUM_VALUE (UNITY_LOCK_FONT_FACE_STYLE_BITCOUNT_PROP_SINGLE, "bitcount-prop-single"),
  G_DEFINE_ENUM_VALUE (UNITY_LOCK_FONT_FACE_STYLE_PRESS_START_2P, "press-start-2p"),
  G_DEFINE_ENUM_VALUE (UNITY_LOCK_FONT_FACE_STYLE_SIXTYFOUR, "sixtyfour"),
  G_DEFINE_ENUM_VALUE (UNITY_LOCK_FONT_FACE_STYLE_AUDIOWIDE, "audiowide"),
  G_DEFINE_ENUM_VALUE (UNITY_LOCK_FONT_FACE_STYLE_MICHROMA, "michroma"),
  G_DEFINE_ENUM_VALUE (UNITY_LOCK_FONT_FACE_STYLE_TOURNEY, "tourney"),
  G_DEFINE_ENUM_VALUE (UNITY_LOCK_FONT_FACE_STYLE_BAGEL_FAT_ONE, "bagel-fat-one"),
  G_DEFINE_ENUM_VALUE (UNITY_LOCK_FONT_FACE_STYLE_DYNAPUFF, "dynapuff"),
  G_DEFINE_ENUM_VALUE (UNITY_LOCK_FONT_FACE_STYLE_KABLAMMO, "kablammo"),
  G_DEFINE_ENUM_VALUE (UNITY_LOCK_FONT_FACE_STYLE_RUBIK_GLITCH, "rubik-glitch"),
  G_DEFINE_ENUM_VALUE (UNITY_LOCK_FONT_FACE_STYLE_EWERT, "ewert"),
  G_DEFINE_ENUM_VALUE (UNITY_LOCK_FONT_FACE_STYLE_LIMELIGHT, "limelight"),
  G_DEFINE_ENUM_VALUE (UNITY_LOCK_FONT_FACE_STYLE_CLIMATE_CRISIS, "climate-crisis"))

const gchar *
unity_lock_font_face_nick (UnityLockFontFaceStyle style)
{
  static GEnumClass *styles;
  GEnumValue *value;

  if (g_once_init_enter_pointer (&styles))
    g_once_init_leave_pointer (&styles, g_type_class_ref (UNITY_LOCK_TYPE_FONT_FACE_STYLE));

  value = g_enum_get_value (styles, (gint) style);

  return value != NULL ? value->value_nick : NULL;
}

PangoFontMap *
unity_lock_font_face_get_font_map (void)
{
  static PangoFontMap *map;

  if (g_once_init_enter_pointer (&map))
    g_once_init_leave_pointer (&map, pango_cairo_font_map_new ());

  return map;
}

/* Pango takes a path rather than bytes, so each face is written to a temp file.
 *
 * The file has to survive for the life of the process. Cairo reopens it by path
 * every time it builds a scaled font for a size it has not cached yet, so
 * unlinking once the face is registered looks fine until the first resize and
 * then fails with "file not found" and renders nothing. Measured: a face still
 * resolves at an already used size after an unlink, but at a new size the ink
 * width drops to 0. The breakpoints change the clock size, so this would break
 * on any resize. Do not shorten the lifetime. */
static GPtrArray *staged_fonts;

static void
staged_font_free (gpointer path)
{
  g_unlink (path);
  g_free (path);
}

static void
staged_fonts_clear (void)
{
  g_clear_pointer (&staged_fonts, g_ptr_array_unref);
}

static gboolean
register_font (GBytes      *bytes,
               const gchar *resource)
{
  g_autoptr (GError) error = NULL;
  g_autoptr (GFileIOStream) stream = NULL;
  g_autoptr (GFile) file = g_file_new_tmp ("unity-lock-font-XXXXXX.ttf", &stream, &error);

  if (file == NULL)
    {
      g_warning ("Failed to stage %s: %s", resource, error->message);
      return FALSE;
    }

  if (g_output_stream_write_bytes (g_io_stream_get_output_stream (G_IO_STREAM (stream)),
                                   bytes, NULL, &error) < 0 ||
      !g_io_stream_close (G_IO_STREAM (stream), NULL, &error) ||
      !pango_font_map_add_font_file (unity_lock_font_face_get_font_map (),
                                     g_file_peek_path (file), &error))
    {
      g_warning ("Failed to register %s: %s", resource, error->message);
      g_file_delete (file, NULL, NULL);
      return FALSE;
    }

  if (staged_fonts == NULL)
    {
      staged_fonts = g_ptr_array_new_with_free_func (staged_font_free);
      atexit (staged_fonts_clear);
    }

  g_ptr_array_add (staged_fonts, g_strdup (g_file_peek_path (file)));

  return TRUE;
}

gboolean
unity_lock_font_face_load (UnityLockFontFaceStyle style)
{
  static gboolean loaded[N_STYLES];
  static gboolean failed[N_STYLES];

  const gchar *nick = unity_lock_font_face_nick (style);

  if (nick == NULL || style == UNITY_LOCK_FONT_FACE_STYLE_DEFAULT)
    return FALSE;

  if (loaded[style] || failed[style])
    return loaded[style];

  g_autofree gchar *resource = g_strconcat (FONT_RESOURCE_PREFIX, nick, ".ttf", NULL);
  g_autoptr (GError) error = NULL;
  g_autoptr (GBytes) bytes =
    g_resources_lookup_data (resource, G_RESOURCE_LOOKUP_FLAGS_NONE, &error);

  if (bytes == NULL)
    g_warning ("Failed to read %s: %s", resource, error->message);
  else
    loaded[style] = register_font (bytes, resource);

  failed[style] = !loaded[style];

  return loaded[style];
}
