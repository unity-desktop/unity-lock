/* unity-lock-conversation.c
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

#include "unity-lock-conversation.h"

#include <astal-auth.h>

/* The PAM service installed to /etc/pam.d. The astal-auth default is not
 * guaranteed to exist. */
#define PAM_SERVICE "unity-lock"

struct _UnityLockConversation
{
  GObject parent_instance;

  AstalAuthPam *pam;
  gchar        *secret;
  gboolean      awaiting;
  gboolean      busy;
};

enum {
  PROP_0,
  PROP_BUSY,
  N_PROPS,
};

enum {
  SIGNAL_PROMPT,
  SIGNAL_MESSAGE,
  SIGNAL_AUTHENTICATED,
  SIGNAL_FAILED,
  N_SIGNALS,
};

static GParamSpec *props[N_PROPS];
static guint signals[N_SIGNALS];

G_DEFINE_FINAL_TYPE (UnityLockConversation, unity_lock_conversation, G_TYPE_OBJECT)

static void
set_busy (UnityLockConversation *self,
          gboolean               busy)
{
  if (self->busy == busy)
    return;

  self->busy = busy;

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_BUSY]);
}

static void
report_prompt (UnityLockConversation *self,
               const gchar           *message,
               gboolean               visible)
{
  self->awaiting = TRUE;
  set_busy (self, FALSE);

  g_signal_emit (self, signals[SIGNAL_PROMPT], 0, message, visible);
}

static void
on_prompt_hidden (AstalAuthPam *pam,
                  const gchar  *message,
                  gpointer      user_data)
{
  UnityLockConversation *self = user_data;

  if (self->secret != NULL)
    {
      g_autofree gchar *secret = g_steal_pointer (&self->secret);

      astal_auth_pam_supply_secret (pam, secret);
      return;
    }

  report_prompt (self, message, FALSE);
}

static void
on_prompt_visible (AstalAuthPam *pam,
                   const gchar  *message,
                   gpointer      user_data)
{
  UnityLockConversation *self = user_data;

  (void) pam;

  /* A visible prompt is asking for something other than the password that was
   * typed, so the held secret is no longer an answer to anything. */
  g_clear_pointer (&self->secret, g_free);

  report_prompt (self, message, TRUE);
}

static void
on_info (AstalAuthPam *pam,
         const gchar  *message,
         gpointer      user_data)
{
  (void) pam;

  g_signal_emit (user_data, signals[SIGNAL_MESSAGE], 0, message, FALSE);
}

static void
on_error (AstalAuthPam *pam,
          const gchar  *message,
          gpointer      user_data)
{
  (void) pam;

  g_signal_emit (user_data, signals[SIGNAL_MESSAGE], 0, message, TRUE);
}

static void
on_fail (AstalAuthPam *pam,
         const gchar  *message,
         gpointer      user_data)
{
  UnityLockConversation *self = user_data;

  (void) pam;

  unity_lock_conversation_cancel (self);

  g_signal_emit (self, signals[SIGNAL_FAILED], 0, message);
}

static void
on_success (AstalAuthPam *pam,
            gpointer      user_data)
{
  UnityLockConversation *self = user_data;

  (void) pam;

  self->awaiting = FALSE;
  set_busy (self, FALSE);

  g_signal_emit (self, signals[SIGNAL_AUTHENTICATED], 0);
}

void
unity_lock_conversation_submit (UnityLockConversation *self,
                                const gchar           *text)
{
  g_return_if_fail (UNITY_LOCK_IS_CONVERSATION (self));

  if (self->busy)
    return;

  if (self->awaiting)
    {
      self->awaiting = FALSE;
      set_busy (self, TRUE);
      astal_auth_pam_supply_secret (self->pam, text);
      return;
    }

  g_free (self->secret);
  self->secret = g_strdup (text);

  set_busy (self, TRUE);
  astal_auth_pam_start_authenticate (self->pam);
}

void
unity_lock_conversation_cancel (UnityLockConversation *self)
{
  g_return_if_fail (UNITY_LOCK_IS_CONVERSATION (self));

  g_clear_pointer (&self->secret, g_free);
  self->awaiting = FALSE;

  set_busy (self, FALSE);
}

gboolean
unity_lock_conversation_get_busy (UnityLockConversation *self)
{
  g_return_val_if_fail (UNITY_LOCK_IS_CONVERSATION (self), FALSE);

  return self->busy;
}

static void
unity_lock_conversation_get_property (GObject    *object,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  UnityLockConversation *self = UNITY_LOCK_CONVERSATION (object);

  switch (prop_id)
    {
    case PROP_BUSY:
      g_value_set_boolean (value, self->busy);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
unity_lock_conversation_finalize (GObject *object)
{
  UnityLockConversation *self = UNITY_LOCK_CONVERSATION (object);

  g_clear_object (&self->pam);
  g_clear_pointer (&self->secret, g_free);

  G_OBJECT_CLASS (unity_lock_conversation_parent_class)->finalize (object);
}

static void
unity_lock_conversation_class_init (UnityLockConversationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = unity_lock_conversation_get_property;
  object_class->finalize = unity_lock_conversation_finalize;

  /**
   * UnityLockConversation:busy:
   *
   * Whether PAM is working on an exchange.
   */
  props[PROP_BUSY] =
    g_param_spec_boolean ("busy", NULL, NULL, FALSE,
                          G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPS, props);

  /**
   * UnityLockConversation::prompt:
   * @self: a #UnityLockConversation.
   * @message: text PAM wants shown against the input.
   * @visible: %TRUE when the answer should not be masked.
   *
   * PAM is waiting for an answer. Reply with
   * unity_lock_conversation_submit().
   */
  signals[SIGNAL_PROMPT] =
    g_signal_new ("prompt", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
                  0, NULL, NULL, NULL, G_TYPE_NONE, 2,
                  G_TYPE_STRING, G_TYPE_BOOLEAN);

  /**
   * UnityLockConversation::message:
   * @self: a #UnityLockConversation.
   * @text: what PAM said.
   * @is_error: %TRUE when PAM reported this as an error.
   *
   * PAM has something to tell the user, such as how many attempts are left.
   */
  signals[SIGNAL_MESSAGE] =
    g_signal_new ("message", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
                  0, NULL, NULL, NULL, G_TYPE_NONE, 2,
                  G_TYPE_STRING, G_TYPE_BOOLEAN);

  /**
   * UnityLockConversation::authenticated:
   * @self: a #UnityLockConversation.
   *
   * The user proved who they are. The session can be unlocked.
   */
  signals[SIGNAL_AUTHENTICATED] =
    g_signal_new ("authenticated", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
                  0, NULL, NULL, NULL, G_TYPE_NONE, 0);

  /**
   * UnityLockConversation::failed:
   * @self: a #UnityLockConversation.
   * @message: (nullable): why PAM refused.
   *
   * Authentication was refused. The conversation has reset itself, so
   * unity_lock_conversation_submit() may be called again.
   */
  signals[SIGNAL_FAILED] =
    g_signal_new ("failed", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
                  0, NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_STRING);
}

static void
unity_lock_conversation_init (UnityLockConversation *self)
{
  self->pam = g_object_new (ASTAL_AUTH_TYPE_PAM, NULL);

  astal_auth_pam_set_service (self->pam, PAM_SERVICE);

  g_signal_connect (self->pam, "auth-prompt-hidden", G_CALLBACK (on_prompt_hidden), self);
  g_signal_connect (self->pam, "auth-prompt-visible", G_CALLBACK (on_prompt_visible), self);
  g_signal_connect (self->pam, "auth-info", G_CALLBACK (on_info), self);
  g_signal_connect (self->pam, "auth-error", G_CALLBACK (on_error), self);
  g_signal_connect (self->pam, "fail", G_CALLBACK (on_fail), self);
  g_signal_connect (self->pam, "success", G_CALLBACK (on_success), self);
}

UnityLockConversation *
unity_lock_conversation_new (void)
{
  return g_object_new (UNITY_LOCK_TYPE_CONVERSATION, NULL);
}
