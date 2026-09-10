/* main.c
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <locale.h>

#include <adwaita.h>
#include <gio/gio.h>
#include <glib/gi18n.h>
#include <gtk4-layer-shell/gtk4-layer-shell.h>
#include <unity-window.h>

#include "unity-lock.h"

static gboolean presented;

static UnityWindow *
lock_factory (GdkMonitor *monitor, gpointer user_data)
{
  UnityLock *lock = unity_lock_new (GTK_APPLICATION (user_data));

  g_signal_connect_swapped (lock, "unlocked",
                            G_CALLBACK (g_application_quit), user_data);

  return UNITY_WINDOW (lock);
}

static void
on_activate (GApplication *app, gpointer user_data)
{
  if (presented)
    return;

  if (!gtk_layer_is_supported ())
    {
      g_warning ("the compositor does not support wlr-layer-shell");
      return;
    }

  presented = TRUE;
  unity_window_present_for_each_monitor (GTK_APPLICATION (app),
                                         lock_factory, app, NULL);
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

  g_autoptr (AdwApplication) app =
    adw_application_new ("org.unity.lock", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);

  return g_application_run (G_APPLICATION (app), argc, argv);
}
