/* unity-lock-user-page.c
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

#include "unity-lock-user-page.h"

#include <string.h>

#include <act/act.h>
#include <glib/gi18n.h>

#include "unity-lock-background.h"
#include "unity-lock-conversation.h"

struct _UnityLockUserPage
{
  AdwBin parent_instance;

  GtkPicture *wallpaper;

  AdwAvatar           *avatar;
  GtkLabel            *name_label;
  GtkLabel            *message;
  AdwPasswordEntryRow *password;
  AdwPreferencesGroup *unlock_group;

  ActUser               *user;
  UnityLockConversation *conversation;
};

enum {
  SIGNAL_UNLOCKED,
  N_SIGNALS,
};

static guint signals[N_SIGNALS];

G_DEFINE_FINAL_TYPE (UnityLockUserPage, unity_lock_user_page, ADW_TYPE_BIN)

static gchar *
prompt_title (const gchar *message)
{
  g_autofree gchar *title = NULL;

  if (message == NULL)
    return g_strdup (_("Password"));

  title = g_strdup (message);
  g_strstrip (title);

  if (g_str_has_suffix (title, ":"))
    title[strlen (title) - 1] = '\0';

  g_strchomp (title);

  if (*title == '\0')
    return g_strdup (_("Password"));

  return g_steal_pointer (&title);
}

static void
clear_message (UnityLockUserPage *self)
{
  gtk_widget_set_visible (GTK_WIDGET (self->message), FALSE);
  gtk_widget_remove_css_class (GTK_WIDGET (self->message), "error");
  gtk_widget_remove_css_class (GTK_WIDGET (self->message), "dim-label");
}

static void
show_message (UnityLockUserPage *self,
              const gchar       *text,
              const gchar       *style_class)
{
  clear_message (self);

  if (text == NULL || *text == '\0')
    return;

  gtk_label_set_text (self->message, text);
  gtk_widget_add_css_class (GTK_WIDGET (self->message), style_class);
  gtk_widget_set_visible (GTK_WIDGET (self->message), TRUE);
}

static void
reset_entry (UnityLockUserPage *self)
{
  adw_preferences_row_set_title (ADW_PREFERENCES_ROW (self->password), _("Password"));
  gtk_editable_set_text (GTK_EDITABLE (self->password), "");
}

static void
on_busy_changed (UnityLockUserPage *self)
{
  gboolean busy = unity_lock_conversation_get_busy (self->conversation);
  gtk_widget_set_sensitive (GTK_WIDGET (self->password), !busy);
  gtk_widget_set_sensitive (GTK_WIDGET (self->unlock_group), !busy);
}

static void
on_prompt (UnityLockUserPage *self,
           const gchar       *message,
           gboolean           visible)
{
  g_autofree gchar *title = prompt_title (message);

  (void) visible;

  adw_preferences_row_set_title (ADW_PREFERENCES_ROW (self->password), title);
  gtk_editable_set_text (GTK_EDITABLE (self->password), "");
  gtk_widget_grab_focus (GTK_WIDGET (self->password));
}

static void
on_message (UnityLockUserPage *self,
            const gchar       *text,
            gboolean           is_error)
{
  show_message (self, text, is_error ? "error" : "dim-label");
}

static void
on_authenticated (UnityLockUserPage *self)
{
  g_signal_emit (self, signals[SIGNAL_UNLOCKED], 0);
}

static void
on_failed (UnityLockUserPage *self,
           const gchar       *message)
{
  show_message (self, message ? message : _("Authentication failed"), "error");
  reset_entry (self);
  gtk_widget_grab_focus (GTK_WIDGET (self->password));
}

static void
submit (UnityLockUserPage *self)
{
  clear_message (self);
  unity_lock_conversation_submit (self->conversation,
                                  gtk_editable_get_text (GTK_EDITABLE (self->password)));
}

static void
on_submit (GtkWidget *row,
           gpointer   user_data)
{
  (void) row;

  submit (UNITY_LOCK_USER_PAGE (user_data));
}

static const gchar *
display_name (ActUser *user)
{
  const gchar *real_name = act_user_get_real_name (user);

  if (real_name != NULL && *real_name != '\0')
    return real_name;

  return act_user_get_user_name (user);
}

static void
update_user (UnityLockUserPage *self)
{
  const gchar *name;
  const gchar *icon;

  if (self->user == NULL || !act_user_is_loaded (self->user))
    return;

  name = display_name (self->user);

  gtk_label_set_label (self->name_label, name);
  adw_avatar_set_text (self->avatar, name);
  adw_avatar_set_custom_image (self->avatar, NULL);

  icon = act_user_get_icon_file (self->user);
  if (icon == NULL || *icon == '\0')
    return;

  g_autoptr (GdkTexture) texture = gdk_texture_new_from_filename (icon, NULL);

  if (texture != NULL)
    adw_avatar_set_custom_image (self->avatar, GDK_PAINTABLE (texture));
}

void
unity_lock_user_page_type_into_entry (UnityLockUserPage *self,
                                      const gchar       *text)
{
  gint position = -1;

  g_return_if_fail (UNITY_LOCK_IS_USER_PAGE (self));

  if (text == NULL || *text == '\0')
    return;

  gtk_editable_insert_text (GTK_EDITABLE (self->password), text, -1, &position);
  gtk_editable_set_position (GTK_EDITABLE (self->password), -1);
}

void
unity_lock_user_page_set_active (UnityLockUserPage *self,
                                 gboolean           active)
{
  GtkRoot *root;

  g_return_if_fail (UNITY_LOCK_IS_USER_PAGE (self));

  if (active)
    {
      gtk_widget_grab_focus (GTK_WIDGET (self));
      return;
    }

  unity_lock_conversation_cancel (self->conversation);
  clear_message (self);
  reset_entry (self);

  root = gtk_widget_get_root (GTK_WIDGET (self));

  if (root != NULL)
    gtk_root_set_focus (root, NULL);
}

static gboolean
unity_lock_user_page_grab_focus (GtkWidget *widget)
{
  UnityLockUserPage *self = UNITY_LOCK_USER_PAGE (widget);

  return gtk_widget_grab_focus (GTK_WIDGET (self->password));
}

static void
unity_lock_user_page_constructed (GObject *object)
{
  UnityLockUserPage *self = UNITY_LOCK_USER_PAGE (object);
  ActUserManager *manager;

  G_OBJECT_CLASS (unity_lock_user_page_parent_class)->constructed (object);

  self->conversation = unity_lock_conversation_new ();

  g_signal_connect_object (self->conversation, "prompt",
                           G_CALLBACK (on_prompt), self, G_CONNECT_SWAPPED);
  g_signal_connect_object (self->conversation, "message",
                           G_CALLBACK (on_message), self, G_CONNECT_SWAPPED);
  g_signal_connect_object (self->conversation, "authenticated",
                           G_CALLBACK (on_authenticated), self, G_CONNECT_SWAPPED);
  g_signal_connect_object (self->conversation, "failed",
                           G_CALLBACK (on_failed), self, G_CONNECT_SWAPPED);
  g_signal_connect_object (self->conversation, "notify::busy",
                           G_CALLBACK (on_busy_changed), self, G_CONNECT_SWAPPED);

  manager = act_user_manager_get_default ();
  self->user = g_object_ref (act_user_manager_get_user (manager, g_get_user_name ()));

  g_signal_connect_object (self->user, "notify::is-loaded",
                           G_CALLBACK (update_user), self, G_CONNECT_SWAPPED);
  g_signal_connect_object (self->user, "changed",
                           G_CALLBACK (update_user), self, G_CONNECT_SWAPPED);

  update_user (self);
  on_busy_changed (self);
}

static void
unity_lock_user_page_dispose (GObject *object)
{
  UnityLockUserPage *self = UNITY_LOCK_USER_PAGE (object);

  g_clear_object (&self->conversation);
  g_clear_object (&self->user);

  G_OBJECT_CLASS (unity_lock_user_page_parent_class)->dispose (object);
}

static void
unity_lock_user_page_class_init (UnityLockUserPageClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->constructed = unity_lock_user_page_constructed;
  object_class->dispose = unity_lock_user_page_dispose;

  widget_class->grab_focus = unity_lock_user_page_grab_focus;

  /**
   * UnityLockUserPage::unlocked:
   * @self: a #UnityLockUserPage.
   *
   * PAM accepted the password. The session can be unlocked.
   */
  signals[SIGNAL_UNLOCKED] =
    g_signal_new ("unlocked", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
                  0, NULL, NULL, NULL, G_TYPE_NONE, 0);

  g_type_ensure (ADW_TYPE_BUTTON_ROW);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/unity/Lock/unity-lock-user-page.ui");
  gtk_widget_class_bind_template_child (widget_class, UnityLockUserPage, wallpaper);
  gtk_widget_class_bind_template_child (widget_class, UnityLockUserPage, avatar);
  gtk_widget_class_bind_template_child (widget_class, UnityLockUserPage, name_label);
  gtk_widget_class_bind_template_child (widget_class, UnityLockUserPage, message);
  gtk_widget_class_bind_template_child (widget_class, UnityLockUserPage, password);
  gtk_widget_class_bind_template_child (widget_class, UnityLockUserPage, unlock_group);
  gtk_widget_class_bind_template_callback (widget_class, on_submit);
}

static void
unity_lock_user_page_init (UnityLockUserPage *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  gtk_picture_set_paintable (self->wallpaper, unity_lock_background_get ());
}
