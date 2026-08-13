/* unity-lock-face.c
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

typedef struct
{
  const gchar *label;
  const gchar *category;   /* NULL for the default face */
} FaceSpec;

static const FaceSpec specs[] = {
  [UNITY_LOCK_FONT_FACE_STYLE_DEFAULT]              = { "Default", NULL },
  [UNITY_LOCK_FONT_FACE_STYLE_FRAUNCES]             = { "Fraunces", "Serif" },
  [UNITY_LOCK_FONT_FACE_STYLE_BITCOUNT_PROP_SINGLE] = { "Bitcount", "Pixel" },
  [UNITY_LOCK_FONT_FACE_STYLE_PRESS_START_2P]       = { "Press Start 2P", "Pixel" },
  [UNITY_LOCK_FONT_FACE_STYLE_SIXTYFOUR]            = { "Sixtyfour", "Pixel" },
  [UNITY_LOCK_FONT_FACE_STYLE_AUDIOWIDE]            = { "Audiowide", "Futuristic" },
  [UNITY_LOCK_FONT_FACE_STYLE_MICHROMA]             = { "Michroma", "Futuristic" },
  [UNITY_LOCK_FONT_FACE_STYLE_TOURNEY]              = { "Tourney", "Futuristic" },
  [UNITY_LOCK_FONT_FACE_STYLE_BAGEL_FAT_ONE]        = { "Bagel Fat One", "Playful" },
  [UNITY_LOCK_FONT_FACE_STYLE_DYNAPUFF]             = { "DynaPuff", "Playful" },
  [UNITY_LOCK_FONT_FACE_STYLE_KABLAMMO]             = { "Kablammo", "Playful" },
  [UNITY_LOCK_FONT_FACE_STYLE_RUBIK_GLITCH]         = { "Rubik Glitch", "Playful" },
  [UNITY_LOCK_FONT_FACE_STYLE_EWERT]                = { "Ewert", "Retro" },
  [UNITY_LOCK_FONT_FACE_STYLE_LIMELIGHT]            = { "Limelight", "Retro" },
  [UNITY_LOCK_FONT_FACE_STYLE_CLIMATE_CRISIS]       = { "Climate Crisis", "Expressive" },
};

G_STATIC_ASSERT (G_N_ELEMENTS (specs) == UNITY_LOCK_FONT_FACE_STYLE_CLIMATE_CRISIS + 1);

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

struct _UnityLockFontFace
{
  GObject parent_instance;

  const FaceSpec *spec;
  const gchar    *nick;
  gboolean        loaded;
  gboolean        failed;
};

enum {
  PROP_0,
  PROP_STYLE,
  PROP_NICK,
  PROP_LABEL,
  PROP_CATEGORY,
  N_PROPS,
};

static GParamSpec *props[N_PROPS];

G_DEFINE_FINAL_TYPE (UnityLockFontFace, unity_lock_font_face, G_TYPE_OBJECT)

UnityLockFontFaceStyle
unity_lock_font_face_get_style (UnityLockFontFace *self)
{
  g_return_val_if_fail (UNITY_LOCK_IS_FONT_FACE (self), UNITY_LOCK_FONT_FACE_STYLE_DEFAULT);

  return (UnityLockFontFaceStyle) (self->spec - specs);
}

const gchar *
unity_lock_font_face_get_nick (UnityLockFontFace *self)
{
  g_return_val_if_fail (UNITY_LOCK_IS_FONT_FACE (self), NULL);

  return self->nick;
}

const gchar *
unity_lock_font_face_get_label (UnityLockFontFace *self)
{
  g_return_val_if_fail (UNITY_LOCK_IS_FONT_FACE (self), NULL);

  return self->spec->label;
}

const gchar *
unity_lock_font_face_get_category (UnityLockFontFace *self)
{
  g_return_val_if_fail (UNITY_LOCK_IS_FONT_FACE (self), NULL);

  return self->spec->category;
}

static void
unity_lock_font_face_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  UnityLockFontFace *self = UNITY_LOCK_FONT_FACE (object);

  switch (prop_id)
    {
    case PROP_STYLE:
      g_value_set_enum (value, unity_lock_font_face_get_style (self));
      break;

    case PROP_NICK:
      g_value_set_string (value, self->nick);
      break;

    case PROP_LABEL:
      g_value_set_string (value, self->spec->label);
      break;

    case PROP_CATEGORY:
      g_value_set_string (value, self->spec->category);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
unity_lock_font_face_class_init (UnityLockFontFaceClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = unity_lock_font_face_get_property;

  /**
   * UnityLockFontFace:style:
   *
   * The style this face implements.
   */
  props[PROP_STYLE] =
    g_param_spec_enum ("style", NULL, NULL,
                       UNITY_LOCK_TYPE_FONT_FACE_STYLE,
                       UNITY_LOCK_FONT_FACE_STYLE_DEFAULT,
                       G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  /**
   * UnityLockFontFace:nick:
   *
   * The stable identifier, which is also the CSS class for this face.
   */
  props[PROP_NICK] =
    g_param_spec_string ("nick", NULL, NULL, NULL,
                         G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  /**
   * UnityLockFontFace:label:
   *
   * The name to show in a settings UI.
   */
  props[PROP_LABEL] =
    g_param_spec_string ("label", NULL, NULL, NULL,
                         G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  /**
   * UnityLockFontFace:category:
   *
   * The group this face belongs to in a settings UI.
   */
  props[PROP_CATEGORY] =
    g_param_spec_string ("category", NULL, NULL, NULL,
                         G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPS, props);
}

static void
unity_lock_font_face_init (UnityLockFontFace *self)
{
  self->spec = &specs[UNITY_LOCK_FONT_FACE_STYLE_DEFAULT];
}

GListModel *
unity_lock_font_face_get_all (void)
{
  static GListStore *store;

  if (g_once_init_enter (&store))
    {
      GListStore *faces = g_list_store_new (UNITY_LOCK_TYPE_FONT_FACE);
      GEnumClass *styles = g_type_class_ref (UNITY_LOCK_TYPE_FONT_FACE_STYLE);

      for (guint i = 0; i < G_N_ELEMENTS (specs); i++)
        {
          g_autoptr (UnityLockFontFace) face = g_object_new (UNITY_LOCK_TYPE_FONT_FACE, NULL);
          GEnumValue *style = g_enum_get_value (styles, (gint) i);

          face->spec = &specs[i];
          /* The enum is the only place a nick is written. It is also the CSS
           * class and the font filename, so taking it from here keeps those from
           * drifting apart. The string is a literal owned by the enum class. */
          face->nick = style->value_nick;
          g_list_store_append (faces, face);
        }

      g_type_class_unref (styles);

      g_once_init_leave (&store, faces);
    }

  return G_LIST_MODEL (store);
}

UnityLockFontFace *
unity_lock_font_face_for_style (UnityLockFontFaceStyle style)
{
  GListModel *model = unity_lock_font_face_get_all ();

  if ((guint) style >= g_list_model_get_n_items (model))
    style = UNITY_LOCK_FONT_FACE_STYLE_DEFAULT;

  /* The store keeps a reference for the process lifetime, so returning a
   * borrowed pointer is safe. */
  g_autoptr (UnityLockFontFace) face = g_list_model_get_item (model, style);

  return face;
}

PangoFontMap *
unity_lock_font_face_get_font_map (void)
{
  static PangoFontMap *map;

  if (g_once_init_enter (&map))
    g_once_init_leave (&map, pango_cairo_font_map_new ());

  return map;
}

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

/* Pango can only register a font from a path and fontconfig here has no call to
 * add one from memory, so a bundled font has to reach the disk first. GTK does
 * the same in gsk/gskrendernodeparser.c. Fontconfig may reread a font lazily, so
 * the files are only removed at process exit. */
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
unity_lock_font_face_load (UnityLockFontFace *self)
{
  g_return_val_if_fail (UNITY_LOCK_IS_FONT_FACE (self), FALSE);

  if (unity_lock_font_face_get_style (self) == UNITY_LOCK_FONT_FACE_STYLE_DEFAULT)
    return FALSE;

  if (self->loaded || self->failed)
    return self->loaded;

  g_autofree gchar *resource =
    g_strconcat (FONT_RESOURCE_PREFIX, self->nick, ".ttf", NULL);
  g_autoptr (GError) error = NULL;
  g_autoptr (GBytes) bytes =
    g_resources_lookup_data (resource, G_RESOURCE_LOOKUP_FLAGS_NONE, &error);

  if (bytes == NULL)
    g_warning ("Failed to read %s: %s", resource, error->message);
  else
    self->loaded = register_font (bytes, resource);

  self->failed = !self->loaded;

  return self->loaded;
}
