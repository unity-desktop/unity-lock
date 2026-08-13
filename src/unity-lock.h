/* unity-lock.h
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

#include <adwaita.h>

G_BEGIN_DECLS

/**
 * UNITY_TYPE_LOCK:
 *
 * The #GType for #UnityLock.
 */
#define UNITY_TYPE_LOCK (unity_lock_get_type ())

/**
 * UnityLock:
 *
 * One lock surface, covering one monitor.
 *
 * This derives from #AdwApplicationWindow rather than a layer shell window.
 * gtk_session_lock_instance_assign_window_to_monitor() gives it the
 * ext_session_lock_surface_v1 role, a Wayland surface holds one role only, and
 * that library also presents and sizes the window, so it must not be presented
 * or given a fixed size by the caller.
 */
G_DECLARE_FINAL_TYPE (UnityLock, unity_lock, UNITY, LOCK, AdwApplicationWindow)

/**
 * unity_lock_new:
 * @app: the application the surface belongs to.
 * @primary: %TRUE for the monitor that carries the clock and the prompt.
 *
 * Creates a lock surface. A primary surface shows the clock and the password
 * prompt and emits #UnityLock::unlocked once PAM accepts the password. Every
 * other surface shows only the wallpaper.
 *
 * Returns: (transfer none): a new #UnityLock. Ownership passes to the session
 *   lock once the window is assigned to a monitor.
 */
GtkWindow *unity_lock_new (GtkApplication *app,
                           gboolean        primary);

G_END_DECLS
