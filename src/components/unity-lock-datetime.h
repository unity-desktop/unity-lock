/* unity-lock-datetime.h
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
 * UNITY_LOCK_TYPE_DATETIME:
 *
 * The #GType for #UnityLockDatetime.
 */
#define UNITY_LOCK_TYPE_DATETIME (unity_lock_datetime_get_type ())

/**
 * UnityLockDatetime:
 *
 * The time and date shown on the lock screen.
 *
 * Instances follow the org.unity.lock style and show-date keys, so changing a key
 * updates every instance, including one already on screen. Setting the matching
 * property overrides the key until it next changes.
 *
 * Type sizes come from unity-lock-datetime.ui, where the time carries a scale
 * attribute and the date carries the libadwaita .title-2 style class. A face only
 * contributes a font family, its variation axes and a relative size correction.
 */
G_DECLARE_FINAL_TYPE (UnityLockDatetime, unity_lock_datetime, UNITY_LOCK, DATETIME, AdwBin)

G_END_DECLS
