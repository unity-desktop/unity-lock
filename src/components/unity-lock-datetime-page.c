/* unity-lock-datetime-page.c
 *
 * Copyright 2026 Muqtadir
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
};

G_DEFINE_FINAL_TYPE (UnityLockDatetimePage, unity_lock_datetime_page, ADW_TYPE_BIN)

static void
unity_lock_datetime_page_dispose (GObject *object)
{
  gtk_widget_dispose_template (GTK_WIDGET (object), UNITY_LOCK_TYPE_DATETIME_PAGE);

  G_OBJECT_CLASS (unity_lock_datetime_page_parent_class)->dispose (object);
}

static void
unity_lock_datetime_page_class_init (UnityLockDatetimePageClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = unity_lock_datetime_page_dispose;

  g_type_ensure (UNITY_LOCK_TYPE_DATETIME);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/unity/lock/unity-lock-datetime-page.ui");
  gtk_widget_class_bind_template_child (widget_class, UnityLockDatetimePage, wallpaper);
}

static void
unity_lock_datetime_page_init (UnityLockDatetimePage *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  gtk_picture_set_paintable (self->wallpaper, unity_lock_background_get ());
}
