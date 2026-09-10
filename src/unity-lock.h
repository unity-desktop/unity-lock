/* unity-lock.h
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <unity-window.h>

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
 * This derives from #UnityWindow, so the surface is a `wlr-layer-shell`
 * overlay that carries the platform stylesheet.
 * unity_window_present_for_each_monitor() binds it to a monitor, presents it
 * and destroys it on unplug, so the caller must not present or size it.
 */
G_DECLARE_FINAL_TYPE (UnityLock, unity_lock, UNITY, LOCK, UnityWindow)

/**
 * unity_lock_new:
 * @app: the application the surface belongs to.
 *
 * Creates a lock surface. Every surface shows the clock, takes the password and
 * emits #UnityLock::unlocked once PAM accepts it, so removing a monitor never
 * takes the only prompt away with it.
 *
 * Returns: (transfer none): a new #UnityLock.
 */
UnityLock *unity_lock_new (GtkApplication *app);

G_END_DECLS
