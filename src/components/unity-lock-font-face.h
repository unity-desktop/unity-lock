/* unity-lock-font-face.h
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

#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

/**
 * UnityLockFontFaceStyle:
 * @UNITY_LOCK_FONT_FACE_STYLE_DEFAULT: the session interface font.
 * @UNITY_LOCK_FONT_FACE_STYLE_FRAUNCES: Fraunces, a serif.
 * @UNITY_LOCK_FONT_FACE_STYLE_BITCOUNT_PROP_SINGLE: Bitcount, a dot matrix face.
 * @UNITY_LOCK_FONT_FACE_STYLE_PRESS_START_2P: Press Start 2P, a bitmap face.
 * @UNITY_LOCK_FONT_FACE_STYLE_SIXTYFOUR: Sixtyfour, a scanline face.
 * @UNITY_LOCK_FONT_FACE_STYLE_AUDIOWIDE: Audiowide.
 * @UNITY_LOCK_FONT_FACE_STYLE_MICHROMA: Michroma.
 * @UNITY_LOCK_FONT_FACE_STYLE_TOURNEY: Tourney.
 * @UNITY_LOCK_FONT_FACE_STYLE_BAGEL_FAT_ONE: Bagel Fat One.
 * @UNITY_LOCK_FONT_FACE_STYLE_DYNAPUFF: DynaPuff.
 * @UNITY_LOCK_FONT_FACE_STYLE_KABLAMMO: Kablammo.
 * @UNITY_LOCK_FONT_FACE_STYLE_RUBIK_GLITCH: Rubik Glitch.
 * @UNITY_LOCK_FONT_FACE_STYLE_EWERT: Ewert.
 * @UNITY_LOCK_FONT_FACE_STYLE_LIMELIGHT: Limelight.
 * @UNITY_LOCK_FONT_FACE_STYLE_CLIMATE_CRISIS: Climate Crisis.
 *
 * The available clock faces. Every value other than
 * %UNITY_LOCK_FONT_FACE_STYLE_DEFAULT names a font bundled in the gresource, so no
 * style depends on a font being installed on the host.
 *
 * These numbers are the stored form of the org.unity.Lock style key and must
 * match the nicks in data/org.unity.Lock.gschema.xml. Append only.
 */
typedef enum
{
  UNITY_LOCK_FONT_FACE_STYLE_DEFAULT = 0,
  UNITY_LOCK_FONT_FACE_STYLE_FRAUNCES,
  UNITY_LOCK_FONT_FACE_STYLE_BITCOUNT_PROP_SINGLE,
  UNITY_LOCK_FONT_FACE_STYLE_PRESS_START_2P,
  UNITY_LOCK_FONT_FACE_STYLE_SIXTYFOUR,
  UNITY_LOCK_FONT_FACE_STYLE_AUDIOWIDE,
  UNITY_LOCK_FONT_FACE_STYLE_MICHROMA,
  UNITY_LOCK_FONT_FACE_STYLE_TOURNEY,
  UNITY_LOCK_FONT_FACE_STYLE_BAGEL_FAT_ONE,
  UNITY_LOCK_FONT_FACE_STYLE_DYNAPUFF,
  UNITY_LOCK_FONT_FACE_STYLE_KABLAMMO,
  UNITY_LOCK_FONT_FACE_STYLE_RUBIK_GLITCH,
  UNITY_LOCK_FONT_FACE_STYLE_EWERT,
  UNITY_LOCK_FONT_FACE_STYLE_LIMELIGHT,
  UNITY_LOCK_FONT_FACE_STYLE_CLIMATE_CRISIS,
} UnityLockFontFaceStyle;

/**
 * UNITY_LOCK_TYPE_FONT_FACE_STYLE:
 *
 * The #GType for #UnityLockFontFaceStyle.
 */
#define UNITY_LOCK_TYPE_FONT_FACE_STYLE (unity_lock_font_face_style_get_type ())

GType unity_lock_font_face_style_get_type (void);

/**
 * UNITY_LOCK_TYPE_FONT_FACE:
 *
 * The #GType for #UnityLockFontFace.
 */
#define UNITY_LOCK_TYPE_FONT_FACE (unity_lock_font_face_get_type ())

/**
 * UnityLockFontFace:
 *
 * One clock face. The set is fixed at build time, so instances are read only.
 *
 * A face is identified by its nick, which is the schema enum nick, the font
 * filename in data/fonts and the CSS class that styles it. Everything
 * typographic lives in unity-lock-datetime.css rather than here.
 */
G_DECLARE_FINAL_TYPE (UnityLockFontFace, unity_lock_font_face, UNITY_LOCK, FONT_FACE, GObject)

/**
 * unity_lock_font_face_get_all:
 *
 * Gets every face, in #UnityLockFontFaceStyle order. Suitable as the model for a
 * settings drop down.
 *
 * Returns: (transfer none): a #GListModel of #UnityLockFontFace.
 */
GListModel *unity_lock_font_face_get_all (void);

/**
 * unity_lock_font_face_for_style:
 * @style: a #UnityLockFontFaceStyle.
 *
 * Looks up the face for @style, falling back to the default face when @style is
 * out of range.
 *
 * Returns: (transfer none): the matching #UnityLockFontFace.
 */
UnityLockFontFace *unity_lock_font_face_for_style (UnityLockFontFaceStyle style);

/**
 * unity_lock_font_face_get_style:
 * @self: a #UnityLockFontFace.
 *
 * Gets the style this face implements.
 *
 * Returns: a #UnityLockFontFaceStyle.
 */
UnityLockFontFaceStyle unity_lock_font_face_get_style (UnityLockFontFace *self);

/**
 * unity_lock_font_face_get_nick:
 * @self: a #UnityLockFontFace.
 *
 * Gets the stable identifier for this face. It doubles as the CSS class that
 * styles the face, so it can be passed to gtk_widget_add_css_class().
 *
 * Returns: (transfer none): the nick.
 */
const gchar *unity_lock_font_face_get_nick (UnityLockFontFace *self);

/**
 * unity_lock_font_face_get_label:
 * @self: a #UnityLockFontFace.
 *
 * Gets the name to show in a settings UI. Typeface names are not translated.
 *
 * Returns: (transfer none): the display name.
 */
const gchar *unity_lock_font_face_get_label (UnityLockFontFace *self);

/**
 * unity_lock_font_face_get_category:
 * @self: a #UnityLockFontFace.
 *
 * Gets the group this face belongs to in a settings UI, or %NULL for the
 * default face.
 *
 * Returns: (transfer none) (nullable): the category name.
 */
const gchar *unity_lock_font_face_get_category (UnityLockFontFace *self);

/**
 * unity_lock_font_face_load:
 * @self: a #UnityLockFontFace.
 *
 * Registers this face's bundled font with the shared font map, extracting it
 * from the gresource on the first call. Later calls are cheap.
 *
 * Returns: %TRUE once the font is usable. %FALSE for the default face, which has
 *   no bundled font, and on failure.
 */
gboolean unity_lock_font_face_load (UnityLockFontFace *self);

/**
 * unity_lock_font_face_get_font_map:
 *
 * Gets the font map holding the bundled faces. Set it on a widget with
 * gtk_widget_set_font_map() before selecting a face on that widget.
 *
 * Returns: (transfer none): the shared #PangoFontMap.
 */
PangoFontMap *unity_lock_font_face_get_font_map (void);

G_END_DECLS
