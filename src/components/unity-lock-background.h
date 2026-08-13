/* unity-lock-background.h
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
 * unity_lock_background_get:
 *
 * Gets the wallpaper every page draws behind itself.
 *
 * This is the PNG unity-shell publishes for the greeter: the session's current
 * wallpaper, already blurred and dimmed and already rendered at monitor size.
 * It is read once and shared, so a surface with several pages decodes it a single
 * time no matter how many pages draw it.
 *
 * Returns: (transfer none) (nullable): the wallpaper, or %NULL when unity-shell
 *   has not published one. A page with no wallpaper falls back to the flat
 *   backdrop from the stylesheet.
 */
GdkPaintable *unity_lock_background_get (void);

G_END_DECLS
