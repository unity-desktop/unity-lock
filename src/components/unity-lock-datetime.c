/* unity-lock-datetime.c
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "unity-lock-datetime.h"

#include <glib/gi18n.h>
#include <libgnome-desktop/gnome-wall-clock.h>

#include "unity-lock-font-face.h"

#define LOCK_SCHEMA      "org.unity.lock"
#define INTERFACE_SCHEMA "org.gnome.desktop.interface"

struct _UnityLockDatetime
{
  AdwBin parent_instance;

  GtkLabel *time_label;
  GtkLabel *date_label;

  UnityLockFontFaceStyle style;
  UnityLockFontFaceStyle applied_style;

  GnomeWallClock *clock;
  GSettings      *interface;
  GSettings      *lock;
};

G_DEFINE_FINAL_TYPE (UnityLockDatetime, unity_lock_datetime, ADW_TYPE_BIN)

static void
apply_style (UnityLockDatetime *self)
{
  PangoFontMap *map = NULL;

  /* The default style carries no class of its own, so it doubles as the "nothing
     applied" state and the field needs no separate sentinel. */
  if (self->applied_style != UNITY_LOCK_FONT_FACE_STYLE_DEFAULT)
    gtk_widget_remove_css_class (GTK_WIDGET (self),
                                 unity_lock_font_face_nick (self->applied_style));

  self->applied_style = UNITY_LOCK_FONT_FACE_STYLE_DEFAULT;

  if (unity_lock_font_face_load (self->style))
    {
      self->applied_style = self->style;
      map = unity_lock_font_face_get_font_map ();
    }

  /* A NULL map restores the default one, so leaving a bundled face does not
     leave its map behind on the labels. */
  gtk_widget_set_font_map (GTK_WIDGET (self->time_label), map);
  gtk_widget_set_font_map (GTK_WIDGET (self->date_label), map);

  if (self->applied_style != UNITY_LOCK_FONT_FACE_STYLE_DEFAULT)
    gtk_widget_add_css_class (GTK_WIDGET (self),
                              unity_lock_font_face_nick (self->applied_style));
}

static void
update_time (UnityLockDatetime *self)
{
  g_autoptr (GDateTime) now = g_date_time_new_now_local ();
  gboolean twelve_hour = FALSE;

  if (self->interface != NULL)
    {
      g_autofree gchar *format = g_settings_get_string (self->interface, "clock-format");

      twelve_hour = g_strcmp0 (format, "12h") == 0;
    }

  g_autofree gchar *time_text = g_date_time_format (now, twelve_hour ? "%I:%M %p" : "%H:%M");
  /* Translators: this is a strftime format for the date under the clock.
     Reorder the fields for your locale. %A is the weekday, %B the month name
     and %-e the day of the month with no leading zero. */
  g_autofree gchar *date_text = g_date_time_format (now, _("%A, %B %-e"));

  gtk_label_set_text (self->time_label, time_text);
  gtk_label_set_text (self->date_label, date_text);
}

static void
on_style_changed (UnityLockDatetime *self)
{
  UnityLockFontFaceStyle style = g_settings_get_enum (self->lock, "style");

  if (self->style == style)
    return;

  self->style = style;
  apply_style (self);
}

static void
on_show_date_changed (UnityLockDatetime *self)
{
  gtk_widget_set_visible (GTK_WIDGET (self->date_label),
                          g_settings_get_boolean (self->lock, "show-date"));
}

/* g_settings_new aborts when a schema is missing, and an abort while the session
 * lock is held leaves the user with a blocked screen and no way in. So look every
 * schema up first and carry on without it. */
static GSettings *
settings_or_null (const gchar *schema_id)
{
  GSettingsSchemaSource *source = g_settings_schema_source_get_default ();
  g_autoptr (GSettingsSchema) schema =
    source != NULL ? g_settings_schema_source_lookup (source, schema_id, TRUE) : NULL;

  if (schema == NULL)
    {
      g_debug ("%s is not installed", schema_id);
      return NULL;
    }

  return g_settings_new_full (schema, NULL, NULL);
}

static void
unity_lock_datetime_constructed (GObject *object)
{
  UnityLockDatetime *self = UNITY_LOCK_DATETIME (object);

  G_OBJECT_CLASS (unity_lock_datetime_parent_class)->constructed (object);

  self->interface = settings_or_null (INTERFACE_SCHEMA);
  self->lock      = settings_or_null (LOCK_SCHEMA);

  if (self->interface != NULL)
    g_signal_connect_object (self->interface, "changed::clock-format",
                             G_CALLBACK (update_time), self, G_CONNECT_SWAPPED);

  if (self->lock != NULL)
    {
      g_signal_connect_object (self->lock, "changed::style",
                               G_CALLBACK (on_style_changed), self, G_CONNECT_SWAPPED);
      g_signal_connect_object (self->lock, "changed::show-date",
                               G_CALLBACK (on_show_date_changed), self, G_CONNECT_SWAPPED);

      self->style = g_settings_get_enum (self->lock, "style");

      gtk_widget_set_visible (GTK_WIDGET (self->date_label),
                              g_settings_get_boolean (self->lock, "show-date"));
    }

  self->clock = gnome_wall_clock_new ();
  g_signal_connect_object (self->clock, "notify::clock",
                           G_CALLBACK (update_time), self, G_CONNECT_SWAPPED);

  apply_style (self);
  update_time (self);
}

static void
unity_lock_datetime_dispose (GObject *object)
{
  UnityLockDatetime *self = UNITY_LOCK_DATETIME (object);

  gtk_widget_dispose_template (GTK_WIDGET (self), UNITY_LOCK_TYPE_DATETIME);

  g_clear_object (&self->clock);
  g_clear_object (&self->interface);
  g_clear_object (&self->lock);

  G_OBJECT_CLASS (unity_lock_datetime_parent_class)->dispose (object);
}

static void
unity_lock_datetime_class_init (UnityLockDatetimeClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->constructed = unity_lock_datetime_constructed;
  object_class->dispose = unity_lock_datetime_dispose;

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/unity/lock/unity-lock-datetime.ui");
  gtk_widget_class_bind_template_child (widget_class, UnityLockDatetime, time_label);
  gtk_widget_class_bind_template_child (widget_class, UnityLockDatetime, date_label);
}

static void
unity_lock_datetime_init (UnityLockDatetime *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}
