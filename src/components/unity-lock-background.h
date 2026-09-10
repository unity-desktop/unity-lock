/* unity-lock-background.h
 *
 * Copyright 2026 Muqtadir
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
 * This is the PNG unity-shell publishes to
 * `$XDG_CACHE_HOME/unity-shell/background.png`: the session's current wallpaper,
 * already blurred and dimmed and already rendered at monitor size. It is read
 * once and shared, so a surface with several pages decodes it a single time no
 * matter how many pages draw it.
 *
 * Returns: (transfer none) (nullable): the wallpaper, or %NULL when unity-shell
 *   has not published one. A page with no wallpaper falls back to the flat
 *   backdrop from the stylesheet.
 */
GdkPaintable *unity_lock_background_get (void);

G_END_DECLS
