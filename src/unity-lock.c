/* unity-lock.c
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

#include "unity-lock.h"

#include "unity-lock-datetime-page.h"
#include "unity-lock-user-page.h"

#define PAGE_CLOCK 0
#define PAGE_LOGIN 1

struct _UnityLock
{
  AdwApplicationWindow parent_instance;

  AdwCarousel *carousel;
};

enum {
  SIGNAL_UNLOCKED,
  N_SIGNALS,
};

static guint signals[N_SIGNALS];

G_DEFINE_FINAL_TYPE (UnityLock, unity_lock, ADW_TYPE_APPLICATION_WINDOW)

static UnityLockUserPage *
user_page (UnityLock *self)
{
  return UNITY_LOCK_USER_PAGE (adw_carousel_get_nth_page (self->carousel, PAGE_LOGIN));
}

static gboolean
on_login_page (UnityLock *self)
{
  return adw_carousel_get_position (self->carousel) > 0.5;
}

static void
scroll_to (UnityLock *self,
           guint      index)
{
  adw_carousel_scroll_to (self->carousel,
                          adw_carousel_get_nth_page (self->carousel, index),
                          TRUE);
}

static void
reveal (UnityLock *self)
{
  if (on_login_page (self))
    return;

  scroll_to (self, PAGE_LOGIN);
}

static void
back_cb (GtkWidget   *widget,
         const gchar *action_name,
         GVariant    *parameter)
{
  scroll_to (UNITY_LOCK (widget), PAGE_CLOCK);
}

static void
on_unlocked (UnityLock *self)
{
  g_signal_emit (self, signals[SIGNAL_UNLOCKED], 0);
}

static void
on_page_changed (UnityLock *self,
                 guint      index)
{
  unity_lock_user_page_set_active (user_page (self), index == PAGE_LOGIN);
}

static gboolean
on_key_pressed (GtkEventControllerKey *controller,
                guint                  keyval,
                guint                  keycode,
                GdkModifierType        state,
                gpointer               user_data)
{
  UnityLock *self = user_data;
  gboolean with_modifier;
  guint32 unicode;


  if (on_login_page (self))
    {
      if (keyval == GDK_KEY_Escape)
        {
          scroll_to (self, PAGE_CLOCK);
          return GDK_EVENT_STOP;
        }

      return GDK_EVENT_PROPAGATE;
    }

  if (keyval == GDK_KEY_Return || keyval == GDK_KEY_KP_Enter ||
      keyval == GDK_KEY_space || keyval == GDK_KEY_KP_Space)
    {
      reveal (self);
      return GDK_EVENT_STOP;
    }

  unicode = gdk_keyval_to_unicode (keyval);
  with_modifier = (state & (GDK_CONTROL_MASK | GDK_ALT_MASK)) != 0;

  if (unicode != 0 && !with_modifier && g_unichar_isprint (unicode))
    {
      g_autofree gchar *text = g_ucs4_to_utf8 (&unicode, 1, NULL, NULL, NULL);

      reveal (self);
      unity_lock_user_page_type_into_entry (user_page (self), text);

      return GDK_EVENT_STOP;
    }

  return GDK_EVENT_PROPAGATE;
}

static void
on_click_pressed (GtkGestureClick *gesture,
                  gint             n_press,
                  gdouble          x,
                  gdouble          y,
                  gpointer         user_data)
{
  reveal (UNITY_LOCK (user_data));
}

static void
unity_lock_class_init (UnityLockClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  /**
   * UnityLock::unlocked:
   * @self: a #UnityLock.
   *
   * PAM accepted the password. Forwarded from #UnityLockUserPage.
   */
  signals[SIGNAL_UNLOCKED] =
    g_signal_new ("unlocked", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
                  0, NULL, NULL, NULL, G_TYPE_NONE, 0);

  gtk_widget_class_install_action (widget_class, "lock.back", NULL, back_cb);

  g_type_ensure (UNITY_LOCK_TYPE_DATETIME_PAGE);
  g_type_ensure (UNITY_LOCK_TYPE_USER_PAGE);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/unity/Lock/unity-lock.ui");
  gtk_widget_class_bind_template_child (widget_class, UnityLock, carousel);
}

static void
unity_lock_init (UnityLock *self)
{
  GtkEventController *key;
  GtkGesture *click;

  gtk_widget_init_template (GTK_WIDGET (self));

  g_signal_connect_object (user_page (self), "unlocked",
                           G_CALLBACK (on_unlocked), self, G_CONNECT_SWAPPED);
  g_signal_connect_object (self->carousel, "page-changed",
                           G_CALLBACK (on_page_changed), self, G_CONNECT_SWAPPED);

  key = gtk_event_controller_key_new ();
  gtk_event_controller_set_propagation_phase (key, GTK_PHASE_CAPTURE);
  g_signal_connect (key, "key-pressed", G_CALLBACK (on_key_pressed), self);
  gtk_widget_add_controller (GTK_WIDGET (self), key);

  click = gtk_gesture_click_new ();
  g_signal_connect (click, "pressed", G_CALLBACK (on_click_pressed), self);
  gtk_widget_add_controller (GTK_WIDGET (self), GTK_EVENT_CONTROLLER (click));
}

GtkWindow *
unity_lock_new (GtkApplication *app)
{
  g_return_val_if_fail (GTK_IS_APPLICATION (app), NULL);

  return g_object_new (UNITY_TYPE_LOCK, "application", app, NULL);
}
