/* unity-lock-datetime.c
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

#include "unity-lock-datetime.h"

#include <libgnome-desktop/gnome-wall-clock.h>

#include "unity-lock-font-face.h"

#define STYLESHEET       "/org/unity/Lock/unity-lock-datetime.css"
#define LOCK_SCHEMA      "org.unity.lock"
#define INTERFACE_SCHEMA "org.gnome.desktop.interface"

struct _UnityLockDatetime
{
  AdwBin parent_instance;

  GtkLabel *time_label;
  GtkLabel *date_label;

  UnityLockFontFaceStyle style;
  gdouble                scale;

  GnomeWallClock *clock;
  GSettings      *interface;
  GSettings      *lock;
};

typedef enum {
  PROP_SCALE = 1,
} UnityLockDatetimeProps;

static GParamSpec *props[PROP_SCALE + 1];

G_DEFINE_FINAL_TYPE (UnityLockDatetime, unity_lock_datetime, ADW_TYPE_BIN)

static void
ensure_stylesheet (GtkWidget *widget)
{
  static gsize loaded = 0;
  GdkDisplay *display = gtk_widget_get_display (widget);

  if (display == NULL || !g_once_init_enter (&loaded))
    return;

  g_autoptr (GtkCssProvider) provider = gtk_css_provider_new ();

  gtk_css_provider_load_from_resource (provider, STYLESHEET);
  gtk_style_context_add_provider_for_display (display,
                                              GTK_STYLE_PROVIDER (provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  g_once_init_leave (&loaded, 1);
}

static void
apply_style (UnityLockDatetime *self)
{
  UnityLockFontFace *face = unity_lock_font_face_for_style (self->style);
  GListModel *all = unity_lock_font_face_get_all ();

  for (guint i = 0; i < g_list_model_get_n_items (all); i++)
    {
      g_autoptr (UnityLockFontFace) other = g_list_model_get_item (all, i);

      gtk_widget_remove_css_class (GTK_WIDGET (self),
                                   unity_lock_font_face_get_nick (other));
    }

  if (!unity_lock_font_face_load (face))
    return;

  gtk_widget_set_font_map (GTK_WIDGET (self->time_label),
                           unity_lock_font_face_get_font_map ());
  gtk_widget_set_font_map (GTK_WIDGET (self->date_label),
                           unity_lock_font_face_get_font_map ());
  gtk_widget_add_css_class (GTK_WIDGET (self), unity_lock_font_face_get_nick (face));
}

static void
apply_scale (UnityLockDatetime *self)
{
  g_autoptr (PangoAttrList) attrs = pango_attr_list_new ();

  pango_attr_list_insert (attrs, pango_attr_scale_new (self->scale));
  gtk_label_set_attributes (self->time_label, attrs);
}

static void
update_time (UnityLockDatetime *self)
{
  g_autoptr (GDateTime) now = g_date_time_new_now_local ();
  g_autofree gchar *format = g_settings_get_string (self->interface, "clock-format");
  gboolean twelve_hour = g_strcmp0 (format, "12h") == 0;
  g_autofree gchar *time_text = g_date_time_format (now, twelve_hour ? "%I:%M %p" : "%H:%M");
  g_autofree gchar *date_text = g_date_time_format (now, "%A, %B %-e");

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

static void
set_scale (UnityLockDatetime *self,
           gdouble            scale)
{
  if (self->scale == scale)
    return;

  self->scale = scale;
  apply_scale (self);

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_SCALE]);
}

static void
unity_lock_datetime_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  UnityLockDatetime *self = UNITY_LOCK_DATETIME (object);

  (void) pspec;

  switch ((UnityLockDatetimeProps) prop_id)
    {
    case PROP_SCALE:
      g_value_set_double (value, self->scale);
      break;
    }
}

static void
unity_lock_datetime_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  UnityLockDatetime *self = UNITY_LOCK_DATETIME (object);

  (void) pspec;

  switch ((UnityLockDatetimeProps) prop_id)
    {
    case PROP_SCALE:
      set_scale (self, g_value_get_double (value));
      break;
    }
}

static void
unity_lock_datetime_constructed (GObject *object)
{
  UnityLockDatetime *self = UNITY_LOCK_DATETIME (object);
  GSettingsSchemaSource *source;

  G_OBJECT_CLASS (unity_lock_datetime_parent_class)->constructed (object);

  self->interface = g_settings_new (INTERFACE_SCHEMA);
  g_signal_connect_object (self->interface, "changed::clock-format",
                           G_CALLBACK (update_time), self, G_CONNECT_SWAPPED);

  source = g_settings_schema_source_get_default ();

  g_autoptr (GSettingsSchema) schema =
    source != NULL ? g_settings_schema_source_lookup (source, LOCK_SCHEMA, TRUE) : NULL;

  if (schema != NULL)
    {
      self->lock = g_settings_new_full (schema, NULL, NULL);

      g_signal_connect_object (self->lock, "changed::style",
                               G_CALLBACK (on_style_changed), self, G_CONNECT_SWAPPED);
      g_signal_connect_object (self->lock, "changed::show-date",
                               G_CALLBACK (on_show_date_changed), self, G_CONNECT_SWAPPED);

      self->style = g_settings_get_enum (self->lock, "style");

      gtk_widget_set_visible (GTK_WIDGET (self->date_label),
                              g_settings_get_boolean (self->lock, "show-date"));
    }
  else
    {
      g_debug ("%s is not installed, falling back to the session font", LOCK_SCHEMA);
    }

  self->clock = gnome_wall_clock_new ();
  g_signal_connect_object (self->clock, "notify::clock",
                           G_CALLBACK (update_time), self, G_CONNECT_SWAPPED);

  ensure_stylesheet (GTK_WIDGET (self));
  apply_style (self);
  apply_scale (self);
  update_time (self);
}

static void
unity_lock_datetime_dispose (GObject *object)
{
  UnityLockDatetime *self = UNITY_LOCK_DATETIME (object);

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

  object_class->get_property = unity_lock_datetime_get_property;
  object_class->set_property = unity_lock_datetime_set_property;
  object_class->constructed = unity_lock_datetime_constructed;
  object_class->dispose = unity_lock_datetime_dispose;

  /**
   * UnityLockDatetime:scale:
   *
   * How much larger than the theme font the time is drawn. Meant to be driven by
   * an #AdwBreakpoint so the clock never overflows a narrow surface.
   */
  props[PROP_SCALE] =
    g_param_spec_double ("scale", NULL, NULL,
                         0.1, 10.0, 10.0,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, G_N_ELEMENTS (props), props);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/unity/Lock/unity-lock-datetime.ui");
  gtk_widget_class_bind_template_child (widget_class, UnityLockDatetime, time_label);
  gtk_widget_class_bind_template_child (widget_class, UnityLockDatetime, date_label);
}

static void
unity_lock_datetime_init (UnityLockDatetime *self)
{
  self->scale = 10.0;

  gtk_widget_init_template (GTK_WIDGET (self));
}

GtkWidget *
unity_lock_datetime_new (void)
{
  return g_object_new (UNITY_LOCK_TYPE_DATETIME, NULL);
}
