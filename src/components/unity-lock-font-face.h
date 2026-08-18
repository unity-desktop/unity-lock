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
 * These numbers are the stored form of the org.unity.lock style key and must
 * match the nicks in data/org.unity.lock.gschema.xml. Append only.
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
 * unity_lock_font_face_nick:
 * @style: a #UnityLockFontFaceStyle.
 *
 * Gets the nick for @style. The nick is the schema enum nick, the font filename
 * in data/fonts and the CSS class that styles the face.
 *
 * Returns: (nullable): the nick, owned by the enum class, or %NULL for a value
 *   outside the enum.
 */
const gchar *unity_lock_font_face_nick (UnityLockFontFaceStyle style);

/**
 * unity_lock_font_face_load:
 * @style: a #UnityLockFontFaceStyle.
 *
 * Loads the font for @style out of the gresource and registers it with the font
 * map from unity_lock_font_face_get_font_map(). Loading happens once per style,
 * and a style that failed once is not retried.
 *
 * Returns: %TRUE when the font is registered and can be used. %FALSE for
 *   %UNITY_LOCK_FONT_FACE_STYLE_DEFAULT, which has no font of its own, and for a
 *   font that could not be read.
 */
gboolean unity_lock_font_face_load (UnityLockFontFaceStyle style);

/**
 * unity_lock_font_face_get_font_map:
 *
 * Gets the font map the bundled faces are registered with. Apply it to a widget
 * with gtk_widget_set_font_map() so the widget can resolve them.
 *
 * Returns: (transfer none): the shared #PangoFontMap.
 */
PangoFontMap *unity_lock_font_face_get_font_map (void);

G_END_DECLS
