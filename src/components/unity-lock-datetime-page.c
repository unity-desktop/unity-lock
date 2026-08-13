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

#include "unity-lock-background.h"
#include "unity-lock-datetime.h"

struct _UnityLockDatetimePage
{
  AdwBin parent_instance;

  GtkPicture *wallpaper;

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
  gtk_widget_class_bind_template_child (widget_class, UnityLockDatetimePage, wallpaper);
  gtk_widget_class_bind_template_child (widget_class, UnityLockDatetimePage, datetime);
}

static void
unity_lock_datetime_page_init (UnityLockDatetimePage *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  gtk_picture_set_paintable (self->wallpaper, unity_lock_background_get ());
}

GtkWidget *
unity_lock_datetime_page_new (void)
{
  return g_object_new (UNITY_LOCK_TYPE_DATETIME_PAGE, NULL);
}
