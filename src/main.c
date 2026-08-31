/* main.c
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

#include <locale.h>

#include <adwaita.h>
#include <gio/gio.h>
#include <glib/gi18n.h>
#include <gtk4-layer-shell/gtk4-session-lock.h>

#include "unity-lock.h"

static GtkSessionLockInstance *lock_instance;

static void
on_monitor (GtkSessionLockInstance *instance, GdkMonitor *monitor, gpointer user_data)
{
  GtkWindow *lock = unity_lock_new (GTK_APPLICATION (user_data));

  g_signal_connect_swapped (lock, "unlocked",
                            G_CALLBACK (gtk_session_lock_instance_unlock), instance);

  gtk_session_lock_instance_assign_window_to_monitor (instance, lock, monitor);
}

static void
on_failed (GtkSessionLockInstance *instance, gpointer user_data)
{
  g_warning ("could not acquire the session lock");
  g_application_release (G_APPLICATION (user_data));
}

static void
on_unlocked (GtkSessionLockInstance *instance, gpointer user_data)
{
  g_application_release (G_APPLICATION (user_data));
}

static void
on_activate (GApplication *app, gpointer user_data)
{
  if (lock_instance != NULL)
    return;

  if (!gtk_session_lock_is_supported ())
    {
      g_warning ("the compositor does not support ext-session-lock-v1");
      return;
    }

  lock_instance = gtk_session_lock_instance_new ();
  g_signal_connect (lock_instance, "monitor", G_CALLBACK (on_monitor), app);
  g_signal_connect (lock_instance, "failed", G_CALLBACK (on_failed), app);
  g_signal_connect (lock_instance, "unlocked", G_CALLBACK (on_unlocked), app);

  g_application_hold (app);
  gtk_session_lock_instance_lock (lock_instance);
}

gint
main (gint argc, gchar **argv)
{
  g_log_set_writer_func (g_log_writer_journald, NULL, NULL);

  setlocale (LC_ALL, "");
  bindtextdomain (GETTEXT_PACKAGE, UNITY_LOCK_LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

  g_set_application_name (_("Unity Lock"));

  g_autoptr (AdwApplication) app = g_object_new (ADW_TYPE_APPLICATION,
    "application-id",     "org.unity.Lock",
    "resource-base-path", "/org/unity/Lock",
    NULL);
  g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);

  return g_application_run (G_APPLICATION (app), argc, argv);
}
