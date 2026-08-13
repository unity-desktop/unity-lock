/* unity-lock-datetime-page.c
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

#include "unity-lock-datetime-page.h"

#include "unity-lock-datetime.h"

/* On the scale setters in the template: the clock is sized by width, because the
 * widest bundled face draws "10:25 PM" at 72px per unit of scale and would
 * otherwise run off a narrow surface. The largest scale that still fits is
 * (width - 48) / 72, which gives 4.9 at 400sp, 7.7 at 600sp and 11.8 at 900sp.
 * Those are rounded down in the template for breathing room. The formula keeps
 * growing above 900sp, so the default is capped at 14 to stop the clock
 * swallowing a large screen. Remeasure with demo/ if a wider face is added. */

struct _UnityLockDatetimePage
{
  AdwBin parent_instance;

  UnityLockDatetime *datetime;
};

G_DEFINE_FINAL_TYPE (UnityLockDatetimePage, unity_lock_datetime_page, ADW_TYPE_BIN)

static void
unity_lock_datetime_page_class_init (UnityLockDatetimePageClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  g_type_ensure (UNITY_LOCK_TYPE_DATETIME);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/unity/Lock/unity-lock-datetime-page.ui");
  gtk_widget_class_bind_template_child (widget_class, UnityLockDatetimePage, datetime);
}

static void
unity_lock_datetime_page_init (UnityLockDatetimePage *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

GtkWidget *
unity_lock_datetime_page_new (void)
{
  return g_object_new (UNITY_LOCK_TYPE_DATETIME_PAGE, NULL);
}
