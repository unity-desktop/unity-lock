/* unity-lock-conversation.h
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

#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * UNITY_LOCK_TYPE_CONVERSATION:
 *
 * The #GType for #UnityLockConversation.
 */
#define UNITY_LOCK_TYPE_CONVERSATION (unity_lock_conversation_get_type ())

/**
 * UnityLockConversation:
 *
 * Runs a PAM conversation against the unity-lock service to unlock the session
 * for the current user.
 *
 * The conversation holds no widgets and draws nothing. It reports what PAM says
 * through #UnityLockConversation::prompt, #UnityLockConversation::message,
 * #UnityLockConversation::authenticated and #UnityLockConversation::failed, and
 * exposes #UnityLockConversation:busy so a caller can show progress.
 *
 * PAM is only reachable one exchange at a time, so calls made while
 * #UnityLockConversation:busy is %TRUE are ignored.
 */
G_DECLARE_FINAL_TYPE (UnityLockConversation, unity_lock_conversation,
                      UNITY_LOCK, CONVERSATION, GObject)

/**
 * unity_lock_conversation_new:
 *
 * Creates a conversation for the current user. The PAM stack is not contacted
 * until unity_lock_conversation_submit() is called.
 *
 * Returns: (transfer full): a new #UnityLockConversation.
 */
UnityLockConversation *unity_lock_conversation_new (void);

/**
 * unity_lock_conversation_submit:
 * @self: a #UnityLockConversation.
 * @text: what the user typed.
 *
 * Sends @text to PAM, starting a new attempt when none is running and answering
 * the prompt last reported otherwise. Does nothing while
 * #UnityLockConversation:busy is %TRUE.
 *
 * The caller does not track which of the two is happening. The lock screen
 * collects a password before PAM has been contacted, so the first @text is held
 * and used to answer the first hidden prompt. Should PAM open with a visible
 * prompt instead, that held text is discarded and
 * #UnityLockConversation::prompt is emitted, because a visible prompt is not
 * asking for the password that was typed.
 */
void unity_lock_conversation_submit (UnityLockConversation *self,
                                     const gchar           *text);

/**
 * unity_lock_conversation_cancel:
 * @self: a #UnityLockConversation.
 *
 * Forgets any held secret and any waiting prompt, so the next
 * unity_lock_conversation_submit() starts a new attempt. Call this when the user leaves
 * the password prompt.
 *
 * This does not stop an exchange already in flight. AstalAuthPam offers no way to
 * cancel one, so PAM may still answer afterwards and the caller has to decide
 * whether that answer is still wanted.
 */
void unity_lock_conversation_cancel (UnityLockConversation *self);

/**
 * unity_lock_conversation_get_busy:
 * @self: a #UnityLockConversation.
 *
 * Gets whether PAM is working on an exchange.
 *
 * Returns: %TRUE while an exchange is in flight.
 */
gboolean unity_lock_conversation_get_busy (UnityLockConversation *self);

G_END_DECLS
