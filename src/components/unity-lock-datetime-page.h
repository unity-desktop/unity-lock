/* unity-lock-datetime-page.h
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <adwaita.h>

G_BEGIN_DECLS

/**
 * UNITY_LOCK_TYPE_DATETIME_PAGE:
 *
 * The #GType for #UnityLockDatetimePage.
 */
#define UNITY_LOCK_TYPE_DATETIME_PAGE (unity_lock_datetime_page_get_type ())

/**
 * UnityLockDatetimePage:
 *
 * The page the lock screen opens on, showing the time and date over the
 * wallpaper.
 *
 * An #AdwBreakpoint scales the clock down on narrow surfaces, because the widest
 * bundled face would otherwise overflow.
 */
G_DECLARE_FINAL_TYPE (UnityLockDatetimePage, unity_lock_datetime_page,
                      UNITY_LOCK, DATETIME_PAGE, AdwBin)

G_END_DECLS
